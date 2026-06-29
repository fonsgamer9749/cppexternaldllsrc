#pragma once
#include <cstdint>

class Offsets {
public:
    // Base Addresses
    static inline uintptr_t Il2Cpp = 0x0;
    static inline uintptr_t UnityCpp = 0x0;
    static inline uintptr_t InitBase = 0x0;
    static inline uintptr_t StaticClass = 0x0;

    // Match
    static inline uintptr_t CurrentMatch = 0x0;
    static inline uintptr_t MatchStatus = 0x0;
    static inline uintptr_t DictionaryEntities = 0x0;

    // Local Player
    static inline uintptr_t LocalPlayer = 0x0;
    static inline uintptr_t Player_IsDead = 0x0;
    static inline uintptr_t Player_Name = 0x0;
    static inline uintptr_t Player_Data = 0x0;
    static inline uintptr_t ShadowState = 0x0;
    static inline uintptr_t LocalPlayerAttributes = 0x0;
    static inline uintptr_t HeadCollider = 0x0;
    static inline uintptr_t ReplaceCollider = 0x0;
    static inline uintptr_t PlayerVitals = 0x44; 
    static inline uintptr_t XPose = 0x0;

    // Avatar
    static inline uintptr_t AvatarManager = 0x0;
    static inline uintptr_t Avatar = 0x0;
    static inline uintptr_t Avatar_IsVisible = 0x0;
    static inline uintptr_t Avatar_Data = 0x0;
    static inline uintptr_t Avatar_Data_IsTeam = 0x0;

    // Camera
    static inline uintptr_t FollowCamera = 0x0;
    static inline uintptr_t Camera = 0x0;
    static inline uintptr_t AimRotation = 0x0;
    static inline uintptr_t MainCameraTransform = 0x0;
    static inline uintptr_t ViewMatrix = 0x0;

    // Weapon
    static inline uintptr_t Weapon = 0x0;
    static inline uintptr_t WeaponData = 0x0;
    static inline uintptr_t WeaponRecoil = 0x0;

    
    static inline uintptr_t FireRate = 0x0;
    static inline uintptr_t NoReload = 0x0;


    static inline uintptr_t SilentBulletDataPtrOffset = 0x0;
    static inline uintptr_t FiringSilent = 0x0;
    static inline uintptr_t arma = 0x0;
    static inline uintptr_t tiro = 0x0;

    static inline uintptr_t m_FireIntervalScale = 0x0;
    static inline uintptr_t FallingSpeedUpScale = 0x0;
    static inline uintptr_t MatrixList = 0x0;
    static inline uintptr_t MatrixIndices = 0x0;
    static inline uintptr_t IsClientBot = 0x2EC;
    static inline uintptr_t FastSwitch = 0x4AC;
    static inline uintptr_t FastMedkit = 0x88;
    static inline uintptr_t InfiniteAmmo = 0x90;
    static inline uintptr_t GhostHack = 0x4B0;

    // Game
    static inline uintptr_t GameTimer = 0x10;
    static inline uintptr_t FixedDeltaTime = 0x24;

    class Bones {
    public:
        static inline uintptr_t Head = 0x0;
        static inline uintptr_t Neck = 0x0;
        static inline uintptr_t LeftShoulder = 0x0;
        static inline uintptr_t RightShoulder = 0x0;
        static inline uintptr_t LeftElbow = 0x0;
        static inline uintptr_t RightElbow = 0x0;
        static inline uintptr_t LeftWrist = 0x0;
        static inline uintptr_t RightWrist = 0x0;
        static inline uintptr_t LeftHand = 0x0;
        static inline uintptr_t RightHand = 0x0;
        static inline uintptr_t Hip = 0x0;
        static inline uintptr_t Groin = 0x0;
        static inline uintptr_t LeftAnkle = 0x0;
        static inline uintptr_t RightAnkle = 0x0;
        static inline uintptr_t LeftFoot = 0x0;
        static inline uintptr_t RightFoot = 0x0;
        static inline uintptr_t Root = 0x0;
        static inline uintptr_t RootBone = 0x0;
        static inline uintptr_t Spine = 0x0;
    };

    static void InitForMax() {
        InitBase = 0xAD23C38; // GameFacade in Offsets_FFMAX
        StaticClass = 0x5C;
        LocalPlayerAttributes = 0x4C4;
        CurrentMatch = 0x50;
        MatchStatus = 0x8C;
        LocalPlayer = 0x94;
        DictionaryEntities = 0x68;
        Player_IsDead = 0x50;
        Player_Name = 0x2E4;
        Player_Data = 0x48;
        ShadowState = 0x18C8;
        XPose = 0x78;
        AvatarManager = 0x4C8;
        Avatar = 0xA8;
        Avatar_IsVisible = 0x95;
        Avatar_Data = 0x14;
        Avatar_Data_IsTeam = 0x59;
        FollowCamera = 0x458;
        Camera = 0x18;
        AimRotation = 0x408;
        MainCameraTransform = 0x250;
        Weapon = 0x3FC;
        WeaponData = 0x58;
        WeaponRecoil = 0x0C;
        ViewMatrix = 0xE4;
        FireRate = 0x18C;
        NoReload = 0x99;
        SilentBulletDataPtrOffset = 0x980; // sAim2 / SilentBulletDataPtrOffset in Offsets_FFMAX
        FiringSilent = 0x548; // sAim1 / FiringSilent in Offsets_FFMAX
        arma = 0x38; // sAim3 / arma
        tiro = 0x2C; // sAim4 / tiro
        m_FireIntervalScale = 0x18C;
        FallingSpeedUpScale = 0x1D4;
        MatrixList = 0x18; // Transform::MatrixList
        MatrixIndices = 0x1C; // Transform::MatrixIndices
        HeadCollider = 0x49C;
        ReplaceCollider = 0x54;

        Bones::Head = 0x460;
        Bones::Neck = 0x468;
        Bones::LeftShoulder = 0x494;
        Bones::RightShoulder = 0x498;
        Bones::LeftElbow = 0x4A8;
        Bones::RightElbow = 0x4A4;
        Bones::LeftWrist = 0x4A0;
        Bones::RightWrist = 0x49C;
        Bones::LeftHand = 0x48C;
        Bones::RightHand = 0x45C;
        Bones::Hip = 0x464;
        Bones::Groin = 0x474;
        Bones::LeftAnkle = 0x47C;
        Bones::RightAnkle = 0x480;
        Bones::LeftFoot = 0x484;
        Bones::RightFoot = 0x488;
        Bones::Root = 0x474;
        Bones::RootBone = 0x474;
        Bones::Spine = 0x46C;
    }

    static void InitForNormal() {
        InitBase = 0xABFF3C0; // GameFacade in Offsets_FF
        StaticClass = 0x5C;
        LocalPlayerAttributes = 0x4BC;
        CurrentMatch = 0x50;
        MatchStatus = 0x8C;
        LocalPlayer = 0x94;
        DictionaryEntities = 0x68;
        Player_IsDead = 0x50;
        Player_Name = 0x2DC;
        Player_Data = 0x48;
        ShadowState = 0x18B8;
        XPose = 0x78;
        AvatarManager = 0x4C0;
        Avatar = 0xA8;
        Avatar_IsVisible = 0x95;
        Avatar_Data = 0x14;
        Avatar_Data_IsTeam = 0x59;
        FollowCamera = 0x450;
        Camera = 0x18;
        AimRotation = 0x400;
        MainCameraTransform = 0x24C;
        Weapon = 0x3F4;
        WeaponData = 0x58;
        WeaponRecoil = 0x0C;
        ViewMatrix = 0xE8;
        FireRate = 0x18C;
        NoReload = 0x99;
        SilentBulletDataPtrOffset = 0x978; // sAim2 / SilentBulletDataPtrOffset in Offsets_FF
        FiringSilent = 0x540; // sAim1 / FiringSilent in Offsets_FF
        arma = 0x38; // sAim3 / arma
        tiro = 0x2C; // sAim4 / tiro
        m_FireIntervalScale = 0x18C;
        FallingSpeedUpScale = 0x1D4;
        MatrixList = 0x18; // Transform::MatrixList
        MatrixIndices = 0x1C; // Transform::MatrixIndices
        HeadCollider = 0x494;
        ReplaceCollider = 0x54;

        Bones::Head = 0x458;
        Bones::Neck = 0x460;
        Bones::LeftShoulder = 0x48C;
        Bones::RightShoulder = 0x490;
        Bones::LeftElbow = 0x4A0;
        Bones::RightElbow = 0x49C;
        Bones::LeftWrist = 0x498;
        Bones::RightWrist = 0x494;
        Bones::LeftHand = 0x484;
        Bones::RightHand = 0x454;
        Bones::Hip = 0x45C;
        Bones::Groin = 0x46C;
        Bones::LeftAnkle = 0x474;
        Bones::RightAnkle = 0x478;
        Bones::LeftFoot = 0x47C;
        Bones::RightFoot = 0x480;
        Bones::Root = 0x46C;
        Bones::RootBone = 0x46C;
        Bones::Spine = 0x464;
    }
};