#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>

#include "Data.hpp"
#include <src/Globals.hpp>
#include <EspLines/Memory/Memory.hpp>
#include <EspLines/Offsets.hpp>
#include <EspLines/Math/TMatrix.hpp>
#include <map>
#include <EspLines/Math/Vector/Vector2.hpp>
#include <EspLines/Math/WordToScreen.hpp>
#include <EspLines/Math/AimB.hpp>

namespace FWork {

    static uint32_t savedLocalPlayer = 0;
    Player* FindClosestEnemy() {
        float closestDistance = FLT_MAX;
        Player* closestEntity = nullptr;

        Vector2 screenCenter(g_Globals.EspConfig.Width / 2.0f, g_Globals.EspConfig.Height / 2.0f);

        for (auto& pair : g_Globals.EspConfig.Entities) {
            Player* entity = &pair.second;

            if (entity->IsDead || (g_Globals.AimBot.IgnoreKnocked && entity->Pose == XPose::Knocked)) continue;

            ImVec2 head2D = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, entity->Head, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height);
            if (head2D.x < 1 || head2D.y < 1) continue;

            float distance = Vector3::Distance(g_Globals.EspConfig.MainCamera, entity->Head);
            if (distance > g_Globals.AimBot.DistanceAim) continue;

            float crosshairDist = std::sqrt(std::pow(head2D.x - screenCenter.X, 2) + std::pow(head2D.y - screenCenter.Y, 2));
            
            // Use FOV filtering if either FOV Circle or FOV is enabled
            bool fovEnabled = g_Globals.Misc.ShowAimbotFov || g_Globals.Misc.EnableFov;
            bool withinFov = !fovEnabled || crosshairDist <= g_Globals.AimBot.Fov;
            
            if (crosshairDist < closestDistance && withinFov) {
                closestDistance = crosshairDist;
                closestEntity = entity;
            }
        }

        return closestEntity;
    }
    std::vector<Player*> FindAllEnemies() {
        std::vector<Player*> enemies;

        for (auto& pair : g_Globals.EspConfig.Entities) {
            Player* entity = &pair.second;

            // ignore dead ya knocked (agar config me enable hai)
            if (entity->IsDead || (g_Globals.AimBot.IgnoreKnocked && entity->Pose == XPose::Knocked))
                continue;

            // screen par project hua position
            ImVec2 head2D = W2S::WorldToScreenImVec2(
                g_Globals.EspConfig.ViewMatrix,
                entity->Head,
                g_Globals.EspConfig.Width,
                g_Globals.EspConfig.Height
            );

            if (head2D.x < 1 || head2D.y < 1)
                continue;

            // distance check
            float distance = Vector3::Distance(g_Globals.EspConfig.MainCamera, entity->Head);
            if (distance > g_Globals.AimBot.DistanceAim)
                continue;

            // agar sab condition pass kar gaya to push karo list me
            enemies.push_back(entity);
        }

        return enemies;
    }
    void WriteLocation(uint32_t target, Vector3 loc)
    {
        uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target);   // RootTransform
        uint32_t enemyTransformVal = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
        uint32_t enemyTransformObj = Mem.ReadS<uint32_t>(enemyTransformVal + 0x8);
        uint32_t enemyMatrixVal = Mem.ReadS<uint32_t>(enemyTransformObj + 0x20);
        Mem.Write<Vector3>(enemyMatrixVal + 0x80, loc);
    }
    Vector3 ReadLocation(uint32_t target)
    {
        uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target);   // RootTransform
        uint32_t enemyTransformVal = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
        uint32_t enemyTransformObj = Mem.ReadS<uint32_t>(enemyTransformVal + 0x8);
        uint32_t enemyMatrixVal = Mem.ReadS<uint32_t>(enemyTransformObj + 0x20);
        Vector3 enemyPos = Mem.ReadS<Vector3>(enemyMatrixVal + 0x80);
        return enemyPos;
    }
    void LegitAimbot() {
        if (!g_Globals.AimBot.Enable) return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True) return;

        uint32_t headCollider = Mem.ReadS<uint32_t>(target->Address + Offsets::HeadCollider);
        if (headCollider == 0) return;

        const int repeatCount = 10;

        for (int i = 0; i < repeatCount; i++) {
            Mem.Write<uint32_t>(target->Address + Offsets::ReplaceCollider, headCollider);
        }
        Mem.Write<uint32_t>(target->Address + Offsets::ReplaceCollider, headCollider);
    }
    void ShakeKillV2() {
        if (!g_Globals.AimBot.ShakeKill) return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
            (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
            return;

        uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target->Address + Offsets::Bones::Root);
        uint32_t enemyTransformValue = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
        uint32_t enemyTransformObjPtr = Mem.ReadS<uint32_t>(enemyTransformValue + 0x8);
        uint32_t enemyMatrixValue = Mem.ReadS<uint32_t>(enemyTransformObjPtr + 0x20);
        Vector3 enemyPos = Mem.ReadS<Vector3>(enemyMatrixValue + 0x80);

        // Static angle so it increases each call
        static float spinAngle = 0.0f;
        spinAngle += 20.0f;  // rotation speed (increase value = faster spin)
        if (spinAngle >= 360.0f)
            spinAngle = 0.0f;

        // Center position (enemy original pos)
        Vector3 centerPos = enemyPos;

        // Radius of circular spin
        float radius = 1.0f;  // bada karne se jyada dur ghoomega

        // Apply spin around Y-axis (XZ plane)
        Vector3 newPos;
        newPos.X = centerPos.X + radius * cosf(spinAngle * 3.14159f / 180.0f);
        newPos.Z = centerPos.Z + radius * sinf(spinAngle * 3.14159f / 180.0f);
        newPos.Y = centerPos.Y + 0.04f; // thoda upar uthakar spin

        // Write back modified position
        Mem.Write<Vector3>(enemyMatrixValue + 0x80, newPos);
    }

  

    void UpPlayer() {
        if (!g_Globals.AimBot.UPPlayer) return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
            (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
            return;

        // Read bone transform hierarchy
        uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target->Address + Offsets::Bones::Head);   // RootTransform
        uint32_t enemyTransformVal = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
        uint32_t enemyTransformObj = Mem.ReadS<uint32_t>(enemyTransformVal + 0x8);
        uint32_t enemyMatrixVal = Mem.ReadS<uint32_t>(enemyTransformObj + 0x20);
        Vector3 enemyPos = Mem.ReadS<Vector3>(enemyMatrixVal + 0x80);

        enemyPos.Y += 0.0400f;
        Mem.Write<Vector3>(enemyMatrixVal + 0x80, enemyPos);
    }
    


    void TeleKill() {
        if (!g_Globals.AimBot.telekill) return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
            (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
            return;

        float playerDistance = Vector3::Distance(g_Globals.EspConfig.MainCamera, target->Head);
        if (playerDistance > 10) return;

        // Read bone transform hierarchy
        uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::Bones::Root);   // RootTransform
        Vector3 localRootTransform;
        TransformUtils::GetNodePosition(enemyRootBonePtr, localRootTransform);
        localRootTransform -= 1.0f;

        WriteLocation(target->Address + Offsets::Bones::Root, localRootTransform);
    }
    


    //void DownPlayer() {
    //    if (!g_Globals.AimBot.DownPlayer) return;

    //    Player* target = FindClosestEnemy();
    //    if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
    //        (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
    //        return;

    //    // Enemy position read
    //    uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target->Address + Offsets::Bones::Root);
    //    uint32_t enemyTransformVal = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
    //    uint32_t enemyTransformObj = Mem.ReadS<uint32_t>(enemyTransformVal + 0x8);
    //    uint32_t enemyMatrixVal = Mem.ReadS<uint32_t>(enemyTransformObj + 0x20);
    //    Vector3 enemyPos = Mem.ReadS<Vector3>(enemyMatrixVal + 0x80);
    //    enemyPos.Y -= 0.10f;
    //  
    //    Mem.Write<Vector3>(enemyMatrixVal + 0x80, enemyPos);
    //    

    //    uint32_t localRootBonePtr = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::Bones::Root);
    //    uint32_t localTransformVal = Mem.ReadS<uint32_t>(localRootBonePtr + 0x8);
    //    uint32_t localTransformObj = Mem.ReadS<uint32_t>(localTransformVal + 0x8);
    //    uint32_t localMatrixVal = Mem.ReadS<uint32_t>(localTransformObj + 0x20);
    //    Vector3 localPos = Mem.ReadS<Vector3>(localMatrixVal + 0x80);

    //    localPos.Y -= 0.08f;
    //    Mem.Write<Vector3>(localMatrixVal + 0x80, localPos);
    //}
    void DownPlayer()
    {
        if (!g_Globals.AimBot.DownPlayer)
            return;

        auto enemies = FindAllEnemies();

        for (auto* target : enemies)
        {
            if (!target || target->Address == 0 ||
                target->IsTeam == Bool3::True ||
                target->IsDead ||
                (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
                continue;

            // NEW ROOT OFFSET (0x3BC)
            uint32_t enemyRootBonePtr = Mem.ReadS<uint32_t>(target->Address + Offsets::Bones::Root);
            if (!enemyRootBonePtr) continue;

            uint32_t transform1 = Mem.ReadS<uint32_t>(enemyRootBonePtr + 0x8);
            if (!transform1) continue;

            uint32_t transform2 = Mem.ReadS<uint32_t>(transform1 + 0x8);
            if (!transform2) continue;

            uint32_t matrixPtr = Mem.ReadS<uint32_t>(transform2 + 0x20);
            if (!matrixPtr) continue;

            Vector3 pos = Mem.ReadS<Vector3>(matrixPtr + 0x80);

            // enemy ko thoda neeche
            pos.Y -= 0.10f;
            Mem.Write<Vector3>(matrixPtr + 0x80, pos);
        }


        // LOCAL PLAYER ↓
        uint32_t localRootBonePtr = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::Bones::Root);
        if (!localRootBonePtr) return;

        uint32_t Ltransform1 = Mem.ReadS<uint32_t>(localRootBonePtr + 0x8);
        if (!Ltransform1) return;

        uint32_t Ltransform2 = Mem.ReadS<uint32_t>(Ltransform1 + 0x8);
        if (!Ltransform2) return;

        uint32_t LmatrixPtr = Mem.ReadS<uint32_t>(Ltransform2 + 0x20);
        if (!LmatrixPtr) return;

        Vector3 localPos = Mem.ReadS<Vector3>(LmatrixPtr + 0x80);

        // local player ko neeche
        localPos.Y -= 0.08f;
        Mem.Write<Vector3>(LmatrixPtr + 0x80, localPos);
    }


    void Teleport() {
        if (!g_Globals.AimBot.teleport) return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
            (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
            return;

        Vector3 enemyPos = ReadLocation(target->Address + Offsets::Bones::Head);//target->Head;
        Vector3 myPos = ReadLocation(g_Globals.EspConfig.LocalPlayer + Offsets::Bones::Head);

        float distance = sqrt(
            pow(enemyPos.X - myPos.X, 2) +
            pow(enemyPos.Y - myPos.Y, 2) +
            pow(enemyPos.Z - myPos.Z, 2)
        );

        if (distance < 200.0f) {
            // Direction vector from you to enemy
            Vector3 direction;
            direction.X = enemyPos.X - myPos.X;
            direction.Y = enemyPos.Y - myPos.Y;
            direction.Z = enemyPos.Z - myPos.Z;

            // Normalize the direction
            float length = sqrt(direction.X * direction.X + direction.Y * direction.Y + direction.Z * direction.Z);
            if (length != 0) {
                direction.X /= length;
                direction.Y /= length;
                direction.Z /= length;
            }

            // Move you to a position 1 meter away from the enemy
            Vector3 newMyPos;
            newMyPos.X = enemyPos.X - direction.X * 1.0f;
            newMyPos.Y = enemyPos.Y - direction.Y * 1.0f;
            newMyPos.Z = enemyPos.Z - direction.Z * 1.0f;

            WriteLocation(g_Globals.EspConfig.LocalPlayer + Offsets::Bones::Head, newMyPos);
        }
    }

    void SilentAim()
    {
        if (!g_Globals.AimBot.silentaim)
            return;

        Player* target = FindClosestEnemy();
        if (!target || target->Address == 0 || target->IsTeam == Bool3::True || target->IsDead ||
            (target->Pose == XPose::Knocked && g_Globals.AimBot.IgnoreKnocked))
            return;


        bool isFiring = Mem.ReadS<bool>(g_Globals.EspConfig.LocalPlayer + Offsets::FiringSilent);
        if (isFiring)
        {
            uint32_t lastinfoweaponfired = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::SilentBulletDataPtrOffset);
            if (lastinfoweaponfired == 0)
                return;

            Vector3 headTarget = target->Head;
            headTarget.Y += 0.05f; // Small offset for much better headshots ("mast" headshot)

            Vector3 startPos = Mem.ReadS<Vector3>(lastinfoweaponfired + Offsets::arma);

            Vector3 newDir = headTarget - startPos;
            Mem.Write<Vector3>(lastinfoweaponfired + Offsets::tiro, newDir);
        }
    }

    
    void RapidFire() {
        if (!g_Globals.EspConfig.LocalPlayer) return;

        uint32_t attributesAddr = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::LocalPlayerAttributes);
        if (attributesAddr == 0) return;

        static bool lastState = false;
        static float originalValue = 1.0f;

        if (g_Globals.Misc.RapidFire) {
            float currentValue = Mem.ReadS<float>(attributesAddr + Offsets::m_FireIntervalScale);
            if (!lastState) {
                originalValue = currentValue;
                lastState = true;
            }
            float rapidValue = 0.5f; // From reference project
            if (abs(currentValue - rapidValue) > 0.001f) {
                Mem.Write<float>(attributesAddr + Offsets::m_FireIntervalScale, rapidValue);
            }
        }
        else if (lastState) {
            Mem.Write<float>(attributesAddr + Offsets::m_FireIntervalScale, originalValue);
            lastState = false;
        }
    }

    void NoRecoil() {
        if (!g_Globals.Misc.NoRecoil || !g_Globals.EspConfig.LocalPlayer) return;

        uint32_t weapon = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::Weapon);
        if (!weapon) return;

        uint32_t weaponData = Mem.ReadS<uint32_t>(weapon + Offsets::WeaponData);
        if (!weaponData) return;

        float recoil = Mem.ReadS<float>(weaponData + Offsets::WeaponRecoil);
        if (recoil != 0.0f) {
            Mem.Write<float>(weaponData + Offsets::WeaponRecoil, 0.0f);
        }
    }

    void FlyVertical() {
        if (!g_Globals.Misc.Speed || !g_Globals.EspConfig.LocalPlayer) return;

        uint32_t attributesAddr = Mem.ReadS<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::LocalPlayerAttributes);
        if (attributesAddr == 0) return;

        static bool lastState = false;
        static float originalValue = 1.0f;

        if (g_Globals.Misc.Speed) { // Using 'Speed' toggle for Fly Vertical if not separate
            if (!lastState) {
                originalValue = Mem.ReadS<float>(attributesAddr + Offsets::FallingSpeedUpScale);
                lastState = true;
            }
            Mem.Write<float>(attributesAddr + Offsets::FallingSpeedUpScale, 0.0f);
        }
        else if (lastState) {
            Mem.Write<float>(attributesAddr + Offsets::FallingSpeedUpScale, originalValue);
            lastState = false;
        }
    }

    // Magnet Pull variables
    static std::map<uint32_t, Vector3> originalPositions;
    static Player* currentMagnetTarget = nullptr;
    static uint32_t _stablePullEntity = 0;
    static Vector3 _smoothedCamPos;
    static Vector3 _smoothedFireDir;
    static float _smoothedProjLen = 0;
    static Vector3 _smoothWritePos;
    static bool _pullSmoothInited = false;

    const float CamSmoothAlpha = 0.45f; // Stickier
    const float DirSmoothAlpha = 0.35f;
    const float ProjSmoothAlpha = 0.40f;
    const float WriteSmoothAlpha = 0.82f; // Much stickier pull

    void RestoreMagnetPositions() {
        for (auto const& [addr, pos] : originalPositions) {
            uint32_t rootBonePtr = Mem.ReadS<uint32_t>(addr + Offsets::Bones::Root);
            if (!rootBonePtr) continue;
            uint32_t transformValue = Mem.ReadS<uint32_t>(rootBonePtr + 0x8);
            if (!transformValue) continue;
            uint32_t transformObjPtr = Mem.ReadS<uint32_t>(transformValue + 0x8);
            if (!transformObjPtr) continue;
            uint32_t matrixPtr = Mem.ReadS<uint32_t>(transformObjPtr + 0x20);
            if (!matrixPtr) continue;

            Mem.Write<Vector3>(matrixPtr + 0x60, pos);
        }
        originalPositions.clear();
        _pullSmoothInited = false;
        currentMagnetTarget = nullptr;
    }

    void MagnetPull() {
        bool pullHead = g_Globals.AimBot.MagnetHead;
        bool pullBody = g_Globals.AimBot.MagnetBody;
        
        if (!pullHead && !pullBody && !g_Globals.AimBot.MagnetPull) {
            if (!originalPositions.empty()) RestoreMagnetPositions();
            return;
        }

        bool isFiring = Mem.ReadS<bool>(g_Globals.EspConfig.LocalPlayer + Offsets::FiringSilent);
        if (!isFiring) {
            if (!originalPositions.empty()) RestoreMagnetPositions();
            return;
        }

        if (currentMagnetTarget == nullptr || currentMagnetTarget->IsDead) {
            currentMagnetTarget = FindClosestEnemy();
        }

        if (currentMagnetTarget == nullptr || currentMagnetTarget->Address == 0 || currentMagnetTarget->IsDead) return;

        uint32_t rootBonePtr = Mem.ReadS<uint32_t>(currentMagnetTarget->Address + Offsets::Bones::Root);
        if (!rootBonePtr) return;
        uint32_t transform1 = Mem.ReadS<uint32_t>(rootBonePtr + 0x8);
        if (!transform1) return;
        uint32_t transform2 = Mem.ReadS<uint32_t>(transform1 + 0x8);
        if (!transform2) return;
        uint32_t matrixPtr = Mem.ReadS<uint32_t>(transform2 + 0x20);
        if (!matrixPtr) return;

        Vector3 currentPos;
        TransformUtils::GetNodePosition(rootBonePtr, currentPos);

        if (originalPositions.find(currentMagnetTarget->Address) == originalPositions.end()) {
            originalPositions[currentMagnetTarget->Address] = currentPos;
        }

        Vector3 originalPos = originalPositions[currentMagnetTarget->Address];
        Vector3 camPos = g_Globals.EspConfig.MainCamera;

        Vector3 rawForward(
            g_Globals.EspConfig.ViewMatrix.m02,
            g_Globals.EspConfig.ViewMatrix.m12,
            g_Globals.EspConfig.ViewMatrix.m22
        );
        rawForward = Vector3::Normalized(rawForward);

        uint32_t eid = currentMagnetTarget->Address;
        if (!_pullSmoothInited || _stablePullEntity != eid) {
            _smoothedCamPos = camPos;
            _smoothedFireDir = rawForward;
            _smoothWritePos = currentPos;
            _stablePullEntity = eid;
            _pullSmoothInited = true;
        }
        else {
            _smoothedCamPos = Vector3::Lerp(_smoothedCamPos, camPos, CamSmoothAlpha);
            Vector3 blended = Vector3::Lerp(_smoothedFireDir, rawForward, DirSmoothAlpha);
            _smoothedFireDir = (Vector3::Magnitude(blended) > 0.0001f) ? Vector3::Normalized(blended) : rawForward;
        }

        // --- DIRECTIONAL & MODE LOGIC ---
        
        // 1. Calculate bone offset (to ensure crosshair hits bone)
        Vector3 boneOffset(0, 0, 0); 
        if (pullHead) {
             boneOffset = currentMagnetTarget->Head - currentMagnetTarget->Root;
        } else if (pullBody) {
             boneOffset = currentMagnetTarget->Hip - currentMagnetTarget->Root;
        } else {
             // Basic magnet pull (default body)
             boneOffset = currentMagnetTarget->Hip - currentMagnetTarget->Root;
        }

        float pullDist = Vector3::Distance(camPos, originalPos);
        
        // "jamiin mai fire karu tab 2 eya 3 m samne khich aye"
        if (_smoothedFireDir.Y < -0.65f) {
            pullDist = 2.8f; // Pull to ~3m in front
        }

        // Target bone position (Exactly on crosshair)
        Vector3 targetBonePos = _smoothedCamPos + (_smoothedFireDir * pullDist);
        
        // "1m Up" or dynamic Y logic
        if (_smoothedFireDir.Y < -0.65f) {
             targetBonePos.Y = originalPos.Y + 1.1f; // Ground stability
        }

        // Final Root Pos Calculation
        Vector3 targetRootPos = targetBonePos - boneOffset;

        // Horz Clamping (12m)
        float deltaX = targetRootPos.X - originalPos.X;
        float deltaZ = targetRootPos.Z - originalPos.Z;
        const float maxH = 11.0f;
        if (fabsf(deltaX) > maxH) targetRootPos.X = originalPos.X + (deltaX > 0 ? maxH : -maxH);
        if (fabsf(deltaZ) > maxH) targetRootPos.Z = originalPos.Z + (deltaZ > 0 ? maxH : -maxH);

        // Sticky smoothing
        _smoothWritePos = Vector3::Lerp(_smoothWritePos, targetRootPos, WriteSmoothAlpha);
        Mem.Write<Vector3>(matrixPtr + 0x60, _smoothWritePos);
    }

    void Data::Work() {
        g_Globals.EspConfig.Matrix = false;

        try {

            if (Offsets::Il2Cpp == 0) {
                //Reset();
                return;
            }

            uint32_t baseGameFacade = Mem.ReadS<uint32_t>(Offsets::Il2Cpp + Offsets::InitBase);
            if (!baseGameFacade)
            {
                Reset();
                return;
            }


            uint32_t gameFacade = Mem.ReadS<uint32_t>(baseGameFacade);
            if (!gameFacade) {
                Reset();
                return;
            }

            uint32_t staticGameFacade = Mem.ReadS<uint32_t>(gameFacade + Offsets::StaticClass);
            if (!staticGameFacade) {
                Reset();
                return;
            }

            uint32_t currentGame = Mem.ReadS<uint32_t>(staticGameFacade);
            if (!currentGame) {
                Reset();
                return;
            }

            uint32_t currentMatch = Mem.ReadS<uint32_t>(currentGame + Offsets::CurrentMatch);
            if (!currentMatch) {
                Reset();
                return;
            }

            uint32_t matchStatus = Mem.ReadS<uint32_t>(currentMatch + Offsets::MatchStatus);
            if (!matchStatus) {
                Reset();
                return;
            }

            if (matchStatus != 1) {
                Reset();
                return;
            }

            uint32_t localPlayer = 0;
            if (Mem.Read<uint32_t>(currentMatch + Offsets::LocalPlayer, localPlayer) && localPlayer != 0) {
                savedLocalPlayer = localPlayer;
            }
            else if (savedLocalPlayer != 0) {
                localPlayer = savedLocalPlayer;
            }
            else {
                localPlayer = 0;
                Reset();
                return;
            }

            g_Globals.EspConfig.LocalPlayer = localPlayer;

            uint32_t mainTransform = Mem.ReadS<uint32_t>(localPlayer + Offsets::MainCameraTransform);
            if (!mainTransform) {
                Reset();
                return;
            }

            Vector3 mainPos;
            TransformUtils::GetPosition(mainTransform, mainPos);
            g_Globals.EspConfig.MainCamera = mainPos;

            uint32_t followCamera = Mem.ReadS<uint32_t>(localPlayer + Offsets::FollowCamera);
            if (!followCamera) {
                Reset();
                return;
            }

            uint32_t camera = Mem.ReadS<uint32_t>(followCamera + Offsets::Camera);
            if (!camera) {
                Reset();
                return;
            }

            uint32_t cameraBase = Mem.ReadS<uint32_t>(camera + 0x8);
            if (!cameraBase) {
                Reset();
                return;
            }

            Matrix4x4 viewMatrix = Mem.ReadS<Matrix4x4>(cameraBase + Offsets::ViewMatrix);
            g_Globals.EspConfig.Matrix = true;
            g_Globals.EspConfig.ViewMatrix = viewMatrix;

            uint32_t entityDictionary = Mem.ReadS<uint32_t>(currentGame + Offsets::DictionaryEntities);
            if (!entityDictionary) {
                Reset();
                return;
            }

            int entitiesCount = Mem.ReadS<int>(entityDictionary + 0x10);
            if (entitiesCount < 1 || entitiesCount > 10000) {
                Reset();
                return;
            }

            uint32_t entriesPtr = Mem.ReadS<uint32_t>(entityDictionary + 0x0C);
            if (!entriesPtr) {
                Reset();
                return;
            }

            uint32_t startEntry = entriesPtr + 0x10;

            for (int i = 0; i < entitiesCount; ++i) {
                uint32_t entry = startEntry + (i * 0x10);

                int hash = Mem.ReadS<int>(entry + 0x0);
                if (hash < 0) continue;

                uint32_t entity = Mem.ReadS<uint32_t>(entry + 0x0C);
                if (entity == 0 || entity == localPlayer) continue;

                try {
                    auto it = g_Globals.EspConfig.Entities.find(entity);
                    Player* entityPtr = nullptr;

                    if (it != g_Globals.EspConfig.Entities.end()) {
                        entityPtr = &it->second;
                    }
                    else {
                        entityPtr = &g_Globals.EspConfig.Entities[entity];
                    }

                    if (!EntityData(entity, *entityPtr, mainPos)) {
                        g_Globals.EspConfig.Entities.erase(entity);
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Erro ao coletar dados da entidade " << entity << ": " << e.what() << std::endl;
                    g_Globals.EspConfig.Entities.erase(entity);
                }
            }

            LegitAimbot();
            ShakeKillV2();
            UpPlayer();
            SilentAim();
            TeleKill();
            Teleport();
            DownPlayer();
            RapidFire();
            NoRecoil();
            FlyVertical();
            MagnetPull();
        }
        catch (const std::exception& e) {
            //Reset();
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
        catch (...) {
            // Reset();
            std::cerr << "Unknown exception caught!" << std::endl;
        }
    }

    bool Data::EntityData(uint32_t entity, Player& player, Vector3& mainPos)
    {
        try {
            player.Address = entity;

            if (player.IsTeam == Bool3::True) return false;

            if (player.IsTeam == Bool3::Unknown) {
                uint32_t avatarManager = Mem.ReadS<uint32_t>(entity + Offsets::AvatarManager);
                if (avatarManager != 0) {
                    uint32_t avatar = Mem.ReadS<uint32_t>(avatarManager + Offsets::Avatar);
                    if (avatar != 0) {
                        bool isVisible = Mem.ReadS<bool>(avatar + Offsets::Avatar_IsVisible);
                        if (isVisible) {
                            player.IsVisible = isVisible;
                            uint32_t avatarData = Mem.ReadS<uint32_t>(avatar + Offsets::Avatar_Data);
                            if (avatarData != 0) {
                                bool isTeam = Mem.ReadS<bool>(avatarData + Offsets::Avatar_Data_IsTeam);
                                if (isTeam) {
                                    player.IsTeam = Bool3::True;
                                    player.IsKnown = false;
                                }
                                else {
                                    player.IsTeam = Bool3::False;
                                    player.IsKnown = true;
                                }
                            }
                        }
                    }
                }
            }

            if (player.IsTeam == Bool3::True || !player.IsKnown) {
                return false;
            }

            uint32_t shadowBase = Mem.ReadS<uint32_t>(entity + Offsets::ShadowState);
            if (shadowBase != 0) {
                int xpose = Mem.ReadS<int>(shadowBase + Offsets::XPose);
                player.Pose = static_cast<XPose>(xpose);
            }

            uint32_t dataPool = Mem.ReadS<uint32_t>(entity + Offsets::Player_Data);
            if (dataPool != 0) {
                uint32_t poolObj = Mem.ReadS<uint32_t>(dataPool + 0x8);
                if (poolObj != 0) {
                    // Gun (usually at index 4 or similar)
                    uint32_t poolGun = Mem.ReadS<uint32_t>(poolObj + 0x20);
                    if (poolGun != 0) {
                        player.Gun = Mem.ReadS<short>(poolGun + 0x10);
                    }

                    // Health logic from reference
                    uint32_t pool = Mem.ReadS<uint32_t>(poolObj + 0x10);
                    if (pool != 0) {
                        uint32_t healthAddr = Mem.ReadS<uint32_t>(pool + 0xC);
                        if (healthAddr != 0 && healthAddr > 0x10000) {
                            player.Health = Mem.ReadS<short>(healthAddr);
                        }
                        else {
                            player.Health = Mem.ReadS<short>(pool + 0x10);
                        }
                    }
                }
            }

            player.IsDead = Mem.ReadS<bool>(entity + Offsets::Player_IsDead);
            player.IsBot = Mem.ReadS<bool>(entity + Offsets::IsClientBot);

            std::vector<std::pair<uint32_t, Vector3*>> boneMap = {
                { (uint32_t)Offsets::Bones::Head, &player.Head },
                { (uint32_t)Offsets::Bones::Neck, &player.Neck },
                { (uint32_t)Offsets::Bones::LeftShoulder, &player.LeftShoulder },
                { (uint32_t)Offsets::Bones::RightShoulder, &player.RightShoulder },
                { (uint32_t)Offsets::Bones::LeftElbow, &player.LeftElbow },
                { (uint32_t)Offsets::Bones::RightElbow, &player.RightElbow },
                { (uint32_t)Offsets::Bones::LeftWrist, &player.LeftWrist },
                { (uint32_t)Offsets::Bones::RightWrist, &player.RightWrist },
                { (uint32_t)Offsets::Bones::LeftHand, &player.LeftHand },
                { (uint32_t)Offsets::Bones::RightHand, &player.RightHand },
                { (uint32_t)Offsets::Bones::Hip, &player.Hip },
                { (uint32_t)Offsets::Bones::Groin, &player.Groin },
                { (uint32_t)Offsets::Bones::Root, &player.Root },
                { (uint32_t)Offsets::Bones::RootBone, &player.RootBone },
                { (uint32_t)Offsets::Bones::LeftAnkle, &player.LeftAnkle },
                { (uint32_t)Offsets::Bones::RightAnkle, &player.RightAnkle },
                { (uint32_t)Offsets::Bones::LeftFoot, &player.LeftFoot },
                { (uint32_t)Offsets::Bones::RightFoot, &player.RightFoot }
            };

            std::vector<uint32_t> bonePointers(boneMap.size(), 0);

            int index = 0;
            for (const auto& [offset, boneVector] : boneMap) {
                Mem.Read<uint32_t>(entity + offset, bonePointers[index]);
                index++;
            }

            index = 0;
            for (const auto& [offset, boneVector] : boneMap) {
                if (bonePointers[index] != 0) {
                    TransformUtils::GetNodePosition(bonePointers[index], *boneVector);
                }
                index++;
            }

            if (player.Head != Vector3::Zero()) {
                player.Distance = Vector3::Distance(mainPos, player.Head);
            }

            uint32_t nameAddr = 0;
            bool rNameAddr = Mem.Read(entity + Offsets::Player_Name, nameAddr);
            if (rNameAddr && nameAddr != 0) {
                std::string name = Mem.String(nameAddr + 0xC, 128, true);
                if (!name.empty()) {
                    player.Name = name;
                }
            }

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception caught: " << std::string(e.what()) << std::endl;
            g_Globals.EspConfig.Entities.erase(entity);
            return false;
        }
    }

    void Data::Reset() {
        try {
            if (!g_Globals.EspConfig.Entities.empty()) {
                g_Globals.EspConfig.Entities.clear();
            }

            if (g_Globals.EspConfig.Matrix) {
                g_Globals.EspConfig.Matrix = false;
                g_Globals.EspConfig.ViewMatrix = Matrix4x4();
            }

            Mem.Cache.clear();
            g_Globals.EspConfig.MainCamera = Vector3(0, 0, 0);
        }
        catch (const std::exception& e) {
            std::cerr << "Error clearing game state: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown error while clearing game state." << std::endl;
        }
    }
}