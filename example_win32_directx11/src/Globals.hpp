#pragma once
#include <Windows.h> 
#include <EspLines/Player.h>
#include <EspLines/Math/Matrix4v4.hpp>
#include <unordered_map>

inline float color_edit[4] = { 255 / 255.f, 185 / 255.f, 84 / 255.f, 255 / 255.f };

class Globals {
public:
	struct AimBot {

		bool Enable;
		//int DownPlayer = 0;
		bool Prevision = false;
		bool IgnoreKnocked;
		bool IgnoreBots;
		int DistanceAim = 50;
		float Fov = 50.0f;
		float DelayToHead = 0.0f;
		bool ShakeKill = false;
		bool UPPlayer = false;
		bool DownPlayer = false;
		bool telekill = false;
		bool teleport = false;
		bool silentaim = false;
		bool MagnetPull = false;
		bool MagnetHead = false;
		bool MagnetBody = false;
		float EnemyPullMaxDistance1 = 150.0f;
		int EnemyPullTickMs1 = 2;






		bool ActivateAll; // Add this



	} AimBot;

	struct Visuals
	{
		int DistanceEsp = 120;
		bool Glows = false;
		bool Enable = true;
		bool Wukong = false;

		bool brazil;

		bool ChamsTexture;

		bool ChamsTransparente;


		bool VoiceAlerts = true;
		float VoiceAlertDistance = 50.0f;
		int VoiceAlertCooldown = 3; // segundos

		bool DobleGun;

		bool Tiro;

		bool Quick;

		bool jump;

		bool Celeste;

		bool Glow;

		bool Blanco;

		bool Frame;

		float TextureColor[4] = { 0.1176f, 0.3921f, 0.5882f, 0.6f };

		bool Negro;

		bool SnakeText = true;
		bool Lines = false;
		bool RGB = false;
		bool Watermark = false;
		bool FPS = false;
		float SkeletonThickness = 1.5f;
		int SkeletonStyle = 1; // 0 = Ninguno, 1 = Normal, 2 = Avanzado
		int EspLines = 0;
		float LinesColor[4] = { 1.f, 1.f, 1.f, 1.f };
		bool Box;
		int players_box = 0;
		float BoxColor[4] = { 1.f, 1.f, 1.f, 1.f };
		float FillColor[4] = { 1.0f, 1.0f, 1.0f, 0.2f };
		bool esparmas;
		float GunColor[4] = { 1.f, 1.f, 1.f, 1.f };
		bool showIcons;
		bool FillColorBox = false;
		bool Level = false;
		bool Pose = false;
		bool HealthBar = false;
		bool ShieldBar = false;
		int players_healthbar = 0;
		int players_shieldbar = 0;
		bool Skeleton = false;
		float SkeletonColor[4] = { 1.f, 1.f, 1.f, 1.f };
		bool Name = false;
		float NameColor[4] = { 1.f, 1.f, 1.f, 1.f };
		bool Distance;
		float DistColor[4] = { 1.f, 1.f, 1.f, 1.f };

	} Visuals;

	struct Misc
	{
		bool Speed = false;
		float SpeedValue = 1.0f;
        bool RapidFire = false;
        bool NoRecoil = false;
	//	bool DownPlayer;
		int KeyDown = 0;
		int KeyWall = 0;
		bool ShowAimbotFov;
		bool EnableFov;
		float AimbotFovColor[4] = { 1.f, 1.f, 1.f, 1.f };
	} Misc;

	struct General
	{
		bool ShutDown = false;
		bool Capture = false;
		bool IsMaxSource = false;
		bool GameInitialized = false;
		int KeyBindINT = 0x2D;
		int Delay = 1;
		char Username[255] = "";
		char Password[255] = "";
		char Key[255] = "";
		float MenuRect[4] = { 0, 0, 0, 0 }; // x, y, width, height
	} General;

	struct Esp {
		std::unordered_map<long, Player> Entities;
		Matrix4x4 ViewMatrix{};
		Vector3 MainCamera{};
		Player Local; // 👈 AGREGA ESTO
		uint32_t LocalPlayer = 0;
		uint32_t previousCount = 0;
		bool Matrix = false;
		int TargetLeft = 0;
		int TargetTop = 0;
		int Width = 0;
		int Height = 0;
		Vector3 playerpos;
	} EspConfig;

};

inline Globals g_Globals;