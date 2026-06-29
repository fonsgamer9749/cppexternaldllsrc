#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>
#include <windows.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <include/MinHook.h>
#include <TlHelp32.h>
#include <src/adb/adb.hpp>
#include <src/Overlay/Overlay.hpp>
#include <src/Overlay/Render.hpp>
#include <src/ui/ui.hpp>
#include <src/Globals.hpp>
#include <EspLines/Data/Data.hpp>
#include <EspLines/Memory/Memory.hpp>
#include <EspLines/Offsets.hpp>
#include <EspLines/Visuals/Visual.hpp>


using namespace adb;

HMODULE g_hModule;
bool bShouldUnload = false;
FWork::Interface* g_pInterface = nullptr;

DWORD WINAPI Unload() {
	if (g_pInterface) {
		g_pInterface->ShutDown();
		delete g_pInterface;
		g_pInterface = nullptr;
	}

	if (MemoryUtils::ogPhysRead) {
		if (MH_DisableHook((LPVOID)MemoryUtils::ogPhysRead) != MH_OK) {
			std::cout << "Falha ao desativar o hook de PGMPhysRead!" << std::endl;
		}

		if (MH_RemoveHook((LPVOID)MemoryUtils::ogPhysRead) != MH_OK) {
			std::cout << "Falha ao remover o hook de PGMPhysRead!" << std::endl;
		}
	}

	MH_Uninitialize();

	FreeConsole();
	fclose(stdout);

	bShouldUnload = true;

	if (g_hModule) {
		FreeLibraryAndExitThread(g_hModule, 0);
	}

	return 0;
}

bool MemoryInit = false;

void Memory() {
	auto vmm = GetModuleHandleA("BstkVMM.dll");
	if (vmm == nullptr) {
		return;
	}

	auto readFunc = (MemoryUtils::PGMPhysReadFunc)GetProcAddress(vmm, "PGMPhysRead");
	if (readFunc == nullptr) {
		return;
	}

	MH_Initialize();
	if (MH_CreateHook((LPVOID)readFunc, MemoryUtils::HookedPGMPhysRead, (LPVOID*)&MemoryUtils::ogPhysRead) != MH_OK) {
		return;
	}

	if (MH_EnableHook((LPVOID)readFunc) != MH_OK) {
		return;
	}

	while (MemoryUtils::vmPtr == nullptr) {
		Sleep(10);
	}

	MemoryUtils::ogCPU = (MemoryUtils::VMMGetCpuByIdFunc)GetProcAddress(vmm, "VMMGetCpuById");
	if (MemoryUtils::ogCPU == nullptr) {
		return;
	}

	MemoryUtils::ogCast = (MemoryUtils::PGMPhysGCPtr2GCPhysFunc)GetProcAddress(vmm, "PGMPhysGCPtr2GCPhys");
	if (MemoryUtils::ogCast == nullptr) {
		return;
	}

	MemoryUtils::ogWrite = (MemoryUtils::PGMPhysSimpleWriteGCPhysFunc)GetProcAddress(vmm, "PGMPhysSimpleWriteGCPhys");
	if (MemoryUtils::ogWrite == nullptr) {
		return;
	}

	MemoryUtils::Initialize(MemoryUtils::vmPtr);
	std::cout << "Virt Memory: " << MemoryUtils::pVMAddr << std::endl;

	MemoryInit = true;
}







void adbInit() {
	TerminateAdbProcesses();
	ExecuteADBCommand("kill-server");
	ExecuteADBCommand("devices");
	adb::AdbRoot(); // Try to get root access via ADB first

	bool GetDir = ChangeDirectory(GetExecutableDirectory());
	if (!GetDir) {
		MessageBox(NULL, "Error: Could not set working directory to executable path.", "Dir Error", MB_ICONERROR);
	}

	std::string packageName = g_Globals.General.IsMaxSource ? "com.dts.freefiremax" : "com.dts.freefireth";
	
	// 1. Get all potential PIDs using multiple fallbacks
	std::vector<std::string> pidCandidates = {
		"pidof " + packageName,
		"ps -A | grep " + packageName + " | awk '{print $2}'",
		"ps | grep " + packageName + " | awk '{print $2}'",
		"ps -A | grep " + packageName, // Raw ps output if awk fails
		"ps | grep " + packageName      // Raw ps output if awk fails
	};

	std::string pidStr = "";
	for (const auto& cmd : pidCandidates) {
		std::string output = ExecuteShellCommandNoSuRaw(cmd);
		if (!output.empty() && output.find("not found") == std::string::npos) {
			pidStr += " " + output;
		}
	}
	
	std::vector<std::string> pids;
	std::stringstream ss(pidStr);
	std::string temp;
	while (ss >> temp) {
		if (!temp.empty() && std::all_of(temp.begin(), temp.end(), ::isdigit)) {
			// Basic validation: skip very small PIDs which are usually system/shell
			if (std::stoi(temp) > 100) {
				pids.push_back(temp);
			}
		}
	}

	if (pids.empty()) {
		MessageBox(NULL, ("Error: Game process not found for " + packageName + ".\nMake sure the game is running and and login to game first!").c_str(), "Process Error", MB_ICONERROR);
		return;
	}

	// 2. Loop through PIDs and try to find libil2cpp.so
	std::string il2cppLine = "";
	std::string lastPid = "";
	std::string lastError = "";

	std::vector<std::string> suCandidates = {
		"", // Try without su first
		"su -c ",
		"/system/xbin/su -c ",
		"/system/bin/su -c ",
		"/boot/android/android/system/xbin/bstk/su -c "
	};

	for (const std::string& pid : pids) {
		lastPid = pid;
		bool found = false;

		for (const std::string& suPrefix : suCandidates) {
			std::string mapCommand = "head -n 1";
			std::string cmd = suPrefix + "\"grep 'libil2cpp.so' /proc/" + pid + "/maps | " + mapCommand + "\"";
			
			if (suPrefix.empty()) {
				cmd = "grep 'libil2cpp.so' /proc/" + pid + "/maps | " + mapCommand;
			}

			il2cppLine = ExecuteShellCommandNoSuRaw(cmd);
			lastError = il2cppLine;

			if (!il2cppLine.empty() && 
				il2cppLine.find("libil2cpp.so") != std::string::npos && 
				il2cppLine.find("Permission denied") == std::string::npos &&
				il2cppLine.find("not found") == std::string::npos) {
				found = true;
				break;
			}
		}

		if (found) break;
	}

	if (il2cppLine.empty() || il2cppLine.find("libil2cpp.so") == std::string::npos) {
		std::string debugOutput = "Attempts for PID " + lastPid + " failed.\n\n"
								 "Last Output: " + lastError + "\n\n"
								 "Possible solutions:\n"
								 "1. Enable 'Root Access' in Emulator Settings.\n"
								 "2. Ensure the game is fully loaded into the lobby.\n"
								 "3. Try running the emulator as Administrator.";
		MessageBox(NULL, ("Error: Could not find libil2cpp.so in memory maps.\n\n" + debugOutput).c_str(), "Module Error", MB_ICONERROR);
		return; 
	}

	Offsets::Il2Cpp = ConvertToUintPtr(ExtractLibAddress(il2cppLine));

	if (Offsets::Il2Cpp == 0) {
		MessageBox(NULL, ("Error: Il2cpp Base address is 0.\nLine: " + il2cppLine).c_str(), "Base Error", MB_ICONERROR);
	}

	if (g_Globals.General.IsMaxSource) {
		Offsets::InitForMax();
	} else {
		Offsets::InitForNormal();
	}

	g_Globals.General.GameInitialized = true;
}


namespace Cheat {
	void Initialize() {

		Memory();
		if (!MemoryInit) {
			MessageBox(NULL, "Error Initialize Memory", "Error", NULL);
		}

		// adbInit will be called after login from UI
		FWork::Overlay::Setup(Render::FindRenderWindow());
		FWork::Overlay::Initialize();
		if (FWork::Overlay::IsInitialized())
		{
			FWork::Interface Interface(FWork::Overlay::GetOverlayWindow(), FWork::Overlay::GetTargetWindow(), FWork::Overlay::dxGetDevice(), FWork::Overlay::dxGetDeviceContext());
			Interface.UpdateStyle();
			FWork::Overlay::SetupWindowProcHook(std::bind(&FWork::Interface::WindowProc, &Interface, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

			MSG Message;
			ZeroMemory(&Message, sizeof(Message));
			while (Message.message != WM_QUIT)
			{
				HWND hOverlay = FWork::Overlay::GetOverlayWindow();
				if (hOverlay == nullptr) {
					break;
				}

				if (PeekMessage(&Message, hOverlay, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&Message); 
					DispatchMessage(&Message);
				}

				InvalidateRect(hOverlay, NULL, TRUE);
				UpdateWindow(hOverlay);

				bool menuOpen = Interface.GetMenuOpen();
				ImGui::GetIO().MouseDrawCursor = menuOpen;

				// Fix for input blocking: Mouse over menu -> interactive, Else -> transparent
				if (menuOpen) {
					POINT mousePos;
					GetCursorPos(&mousePos);
					ScreenToClient(hOverlay, &mousePos);

					float* rect = g_Globals.General.MenuRect; // X, Y, Width, Height
					bool hoveringMenu = (mousePos.x >= rect[0] && mousePos.x <= rect[0] + rect[2] &&
									   mousePos.y >= rect[1] && mousePos.y <= rect[1] + rect[3]);

					SetWindowLong(hOverlay, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | (hoveringMenu ? 0 : WS_EX_TRANSPARENT));
				} else {
					SetWindowLong(hOverlay, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
				}

				if (Interface.ResizeHeight != 0 || Interface.ResizeWidht != 0)
				{
					FWork::Overlay::dxCleanupRenderTarget();
					FWork::Overlay::dxGetSwapChain()->ResizeBuffers(0, Interface.ResizeWidht, Interface.ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
					Interface.ResizeHeight = Interface.ResizeWidht = 0;
					FWork::Overlay::dxCreateRenderTarget();
				}

				Interface.HandleMenuKey();
				FWork::Overlay::UpdateWindowPos();

				static bool CaptureBypassOn = false;
				if (g_Globals.General.Capture != CaptureBypassOn)
				{
					CaptureBypassOn = g_Globals.General.Capture;
					SetWindowDisplayAffinity(FWork::Overlay::GetOverlayWindow(), CaptureBypassOn ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
				}

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				{
					FWork::Data::Work();

					if (g_Globals.Visuals.Enable) {
						ESP::Players();
					}

					Interface.RenderGui();

					if (g_Globals.Misc.ShowAimbotFov && g_Globals.Visuals.Enable && g_Globals.EspConfig.Width > 0) {
						bool isForeground = (GetForegroundWindow() == FWork::Overlay::GetTargetWindow());
						if (isForeground || Interface.GetMenuOpen()) {
							ImDrawList* backgroundDrawList = ImGui::GetBackgroundDrawList();
							
							// Clip FOV to emulator area
							backgroundDrawList->PushClipRect(
								ImVec2(g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop),
								ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width, g_Globals.EspConfig.TargetTop + g_Globals.EspConfig.Height),
								true
							);

							ImVec2 center = ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2.0f, g_Globals.EspConfig.TargetTop + g_Globals.EspConfig.Height / 2.0f);
							float radius = g_Globals.AimBot.Fov;
							ImColor color = ImColor(g_Globals.Misc.AimbotFovColor[0], g_Globals.Misc.AimbotFovColor[1], g_Globals.Misc.AimbotFovColor[2], g_Globals.Misc.AimbotFovColor[3]);
							backgroundDrawList->AddCircle(center, radius, color, 360);

							backgroundDrawList->PopClipRect();
						}
					}
				}
				ImGui::EndFrame();
				ImGui::Render();
				FWork::Overlay::dxRefresh();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				FWork::Overlay::dxGetSwapChain()->Present(0, 0);

				if (g_Globals.General.ShutDown) {
					Unload();
					return;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(g_Globals.General.Delay));
			}
		}
	}
}

//int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
//#ifdef _DEBUG
//	AllocConsole();
//	SetConsoleOutputCP(CP_UTF8);
//	SetConsoleCP(CP_UTF8);
//	freopen("CONIN$", "r", stdin);
//	freopen("CONOUT$", "w", stdout);
//	freopen("CONOUT$", "w", stderr);
//	SetConsoleTitleA("Debug Console");
//#endif // _DEBUG
//
//	Cheat::Initialize();
//
//	while (!bShouldUnload || !g_Globals.General.ShutDown) {
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//
//	FreeConsole();
//	fclose(stdout);
//	return 0;
//}
//
//BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
//    switch (ul_reason_for_call) {
//    case DLL_PROCESS_ATTACH:
//        g_hModule = hModule;
//        DisableThreadLibraryCalls(hModule);
//
//        // Inicia primero el hilo del Start
//        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, hModule, 0, NULL);
//
//        // Crea un temporizador para iniciar el resto después de 3 segundos
//        CreateThread(NULL, 0, [](LPVOID lpParam) -> DWORD {
//            // Esperar 3 segundos (3000 milisegundos)
//            Sleep(3000);
//            
//            // Después del delay, iniciar el menú principal
//            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wWinMain, lpParam, 0, NULL);
//            
//            return 0;
//        }, hModule, 0, NULL);
//
//        break;
//    case DLL_PROCESS_DETACH:
//        FreeConsole();
//        fclose(stdout);
//        bShouldUnload = true;
//        break;
//    }
//    return TRUE;
//}



int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	Cheat::Initialize();

	while (!bShouldUnload || !g_Globals.General.ShutDown) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		AllocConsole();
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		SetConsoleTitleA("Debug Console");
		std::cout << "[DLL] Debug console initialized.\n";
#endif
		g_hModule = hModule;
		DisableThreadLibraryCalls(hModule);

		// Delay main menu (3 seconds)
		CreateThread(NULL, 0, [](LPVOID lpParam) -> DWORD {
			Sleep(1000);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wWinMain, lpParam, 0, NULL);
			return 0;
			}, hModule, 0, NULL);
		break;

	case DLL_PROCESS_DETACH:
		bShouldUnload = true;
#ifdef _DEBUG
		std::cout << "[DLL] Freeing debug console.\n";
		FreeConsole();
#endif
		break;
	}
	return TRUE;
}
