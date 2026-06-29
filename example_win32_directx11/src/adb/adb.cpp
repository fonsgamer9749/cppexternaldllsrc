
#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "adb.hpp"
#include "ProcessUtils.hpp"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//#include <atomic>

using namespace ProcessUtils;

namespace adb {


    SOCKET g_LibSocket = INVALID_SOCKET;
 //   static std::atomic<bool> g_LibGreetingReceived{ false };
    static std::atomic<int> g_LibConnectionStatus{ 0 };
    // for storing lib greet
    std::string g_LibGreeting;


    // show status for all current functions 
    std::string g_LibStatus;



    

    std::string GetExecutableDirectory() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        std::string fullPath(path);
        size_t lastSlashIndex = fullPath.find_last_of("\\/");
        if (lastSlashIndex != std::string::npos) {
            return fullPath.substr(0, lastSlashIndex);
        }
        return "";
    }

    std::string GetExecutableDirectoryPID(DWORD processID) {
        char path[MAX_PATH];
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

        if (hProcess) {
            if (GetModuleFileNameExA(hProcess, NULL, path, MAX_PATH)) {
                std::string fullPath(path);
                size_t lastSlashIndex = fullPath.find_last_of("\\/");
                CloseHandle(hProcess);
                if (lastSlashIndex != std::string::npos) {
                    return fullPath.substr(0, lastSlashIndex);
                }
            }
            CloseHandle(hProcess);
        }

        return "";
    }

    bool ChangeDirectory(const std::string& path) {
        return SetCurrentDirectoryA(path.c_str());
    }

    void TerminateAdbProcesses() {
        DWORD ProcIdAdb = GetProc(L"adb.exe");
        DWORD ProcIdAdb2 = 0;
        KillProc(ProcIdAdb);
        if (IsProcRun(L"adb.exe", ProcIdAdb2)) {
            ForgeKillProc(ProcIdAdb2);
        }

        DWORD ProcIdAdbHD = GetProc(L"HD-Adb.exe");
        DWORD ProcIdAdbHD2 = 0;
        KillProc(ProcIdAdbHD);
        if (IsProcRun(L"HD-Adb.exe", ProcIdAdbHD2)) {
            ForgeKillProc(ProcIdAdbHD2);
        }
    }

    std::vector<DWORD> GetAllProcessIds() {
        std::vector<DWORD> pids(1024);
        DWORD bytesReturned;
        if (EnumProcesses(pids.data(), static_cast<DWORD>(pids.size() * sizeof(DWORD)), &bytesReturned)) {
            pids.resize(bytesReturned / sizeof(DWORD));
        }
        else {
            pids.clear();
        }
        return pids;
    }

    std::wstring GetProcessPath(DWORD pid) {
        wchar_t buffer[MAX_PATH] = { 0 };
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess) {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                GetModuleFileNameExW(hProcess, hMod, buffer, MAX_PATH);
            }
            CloseHandle(hProcess);
        }
        return std::wstring(buffer);
    }

    bool IsProcessRunning(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess == NULL) {
            return false; // El proceso ya no existe o no se puede acceder
        }

        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            CloseHandle(hProcess);
            return (exitCode == STILL_ACTIVE);
        }

        CloseHandle(hProcess);
        return false;
    }

    void KillBlueStacksAdbProcesses() {
        std::vector<DWORD> pids = GetAllProcessIds();

        for (DWORD pid : pids) {
            std::wstring exePath = GetProcessPath(pid);
            std::transform(exePath.begin(), exePath.end(), exePath.begin(), ::towlower);

            // Lista de patrones para detectar BlueStacks (todas versiones)
            const std::vector<std::wstring> bluestacksPatterns = {
                L"BlueStacks_msi5",       // Todas las versiones
                L"bstk",             // Algunas versiones usan este prefijo
                L"hd-adb",           // ADB de BlueStacks
                L"BlueStacks_nxt",    // BlueStacks 5
                L"bluestack_x"       // BlueStacks X
            };

            // Lista de procesos ADB a cerrar
            const std::vector<std::wstring> adbProcesses = {
                L"adb.exe",
                L"hd-adb.exe",
                L"bstkadb.exe"
            };

            bool isBluestacks = false;
            for (const auto& pattern : bluestacksPatterns) {
                if (exePath.find(pattern) != std::wstring::npos) {
                    isBluestacks = true;
                    break;
                }
            }

            bool isAdbProcess = false;
            for (const auto& adbProc : adbProcesses) {
                if (exePath.find(adbProc) != std::wstring::npos) {
                    isAdbProcess = true;
                    break;
                }
            }

            if (isBluestacks && isAdbProcess) {
                KillProc(pid);

                // Verificación adicional para asegurar que el proceso se cerró
                if (IsProcessRunning(pid)) {
                    ForgeKillProc(pid); // Método más agresivo si el proceso persiste
                }
            }
        }

        // Cierre adicional de procesos conocidos por nombre
        DWORD adbPid = GetProc(L"adb.exe");
        if (adbPid != 0) {
            std::wstring path = GetProcessPath(adbPid);
            std::transform(path.begin(), path.end(), path.begin(), ::towlower);
            if (path.find(L"bluestacks") != std::wstring::npos) {
                KillProc(adbPid);
            }
        }

        DWORD hdAdbPid = GetProc(L"HD-Adb.exe");
        if (hdAdbPid != 0) {
            KillProc(hdAdbPid);
        }
    }

    std::string ExtractLibAddress(const std::string& input) {
        // Robust address extraction for 32/64-bit addresses
        // Example: aa8c2000-aa9c1000 r-xp ...
        // Example: 7f8c200000-7f8c300000 r-xp ...
        
        std::size_t end_pos = 0;
        while (end_pos < input.length()) {
            std::size_t dash_pos = input.find('-', end_pos);
            if (dash_pos == std::string::npos) break;

            // Address should be at the start of the line or after a newline
            std::size_t start_pos = dash_pos;
            while (start_pos > 0 && input[start_pos - 1] != '\n' && input[start_pos - 1] != '\r' && !isspace(static_cast<unsigned char>(input[start_pos - 1]))) {
                start_pos--;
            }

            // Extract potential address
            std::string potentialAddr = input.substr(start_pos, dash_pos - start_pos);
            
            // Validate it's hex and not too long/short
            if (!potentialAddr.empty() && potentialAddr.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
                return potentialAddr;
            }
            
            end_pos = dash_pos + 1;
        }
        return "";
    }

    uintptr_t ConvertToUintPtr(std::string str, int base) {
        if (sizeof(uintptr_t) == sizeof(unsigned long)) {
            return strtoul(str.c_str(), nullptr, base);
        }
        return strtoull(str.c_str(), nullptr, base);
    }

    std::string ExecuteShellCommand(const std::string& firstCommand, const std::string& secondCommand) {
        HANDLE hStdOutRead, hStdOutWrite;
        HANDLE hStdInRead, hStdInWrite;

        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
            std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
            return "";
        }

        if (!SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "SetHandleInformation failed: " << GetLastError() << std::endl;
            return "";
        }

        if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
            std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
            return "";
        }

        if (!SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "SetHandleInformation failed: " << GetLastError() << std::endl;
            return "";
        }

        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hStdOutWrite;
        si.hStdOutput = hStdOutWrite;
        si.hStdInput = hStdInRead;
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcessA(NULL, (LPSTR)".\\HD-Adb shell", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
            return "";
        }

        CloseHandle(hStdOutWrite);
        CloseHandle(hStdInRead);

        DWORD written;
        std::string commands = firstCommand + "\n" + secondCommand + "\n";
        if (!WriteFile(hStdInWrite, commands.c_str(), commands.length(), &written, NULL)) {
            std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
            return "";
        }
        CloseHandle(hStdInWrite);

        CHAR buffer[128];
        DWORD read;
        std::string output;
        while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0) {
            buffer[read] = '\0';
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutRead);

        KillBlueStacksAdbProcesses();

        return ExtractLibAddress(output);
    }

    std::string ExecuteShellCommandNoSu(const std::string& command) {
        HANDLE hStdOutRead, hStdOutWrite;
        HANDLE hStdInRead, hStdInWrite;

        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) return "";
        SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);

        if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) return "";
        SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hStdOutWrite;
        si.hStdOutput = hStdOutWrite;
        si.hStdInput = hStdInRead;
        si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        // Use standard adb shell "command" format
        std::string fullCommand = ".\\HD-Adb shell \"" + command + "\"";

        if (!CreateProcessA(NULL, (LPSTR)fullCommand.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            CloseHandle(hStdInRead);
            CloseHandle(hStdInWrite);
            return "";
        }

        CloseHandle(hStdOutWrite);
        CloseHandle(hStdInRead);
        CloseHandle(hStdInWrite);

        CHAR buffer[4096]; // Larger buffer for maps
        DWORD readBytes;
        std::string output;
        while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &readBytes, NULL) && readBytes > 0) {
            buffer[readBytes] = '\0';
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, 5000);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutRead);

        return ExtractLibAddress(output);
    }

    std::string ExecuteShellCommandNoSuRaw(const std::string& command) {
        HANDLE hStdOutRead, hStdOutWrite;
        HANDLE hStdInRead, hStdInWrite;

        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) return "";
        SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);

        if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) return "";
        SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hStdOutWrite;
        si.hStdOutput = hStdOutWrite;
        si.hStdInput = hStdInRead;
        si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::string fullCommand = ".\\HD-Adb shell \"" + command + "\"";

        if (!CreateProcessA(NULL, (LPSTR)fullCommand.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            CloseHandle(hStdInRead);
            CloseHandle(hStdInWrite);
            return "CreateProcess Error: " + std::to_string(GetLastError());
        }

        CloseHandle(hStdOutWrite);
        CloseHandle(hStdInRead);
        CloseHandle(hStdInWrite);

        CHAR buffer[4096];
        DWORD readBytes;
        std::string output;
        while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &readBytes, NULL) && readBytes > 0) {
            buffer[readBytes] = '\0';
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, 5000);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutRead);

        return output;
    }

    void ExecuteADBCommand(const std::string& command) {
        std::string fullCommand = ".\\HD-Adb shell " + command;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.wShowWindow = SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcessA(NULL, const_cast<LPSTR>(fullCommand.c_str()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            std::cerr << "Failed to start process. Error: " << GetLastError() << std::endl;
            return;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        KillBlueStacksAdbProcesses();
    }

    void ExecuteAdb(const std::string& args) {
        std::string fullCommand = ".\\HD-Adb " + args;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.wShowWindow = SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcessA(NULL, const_cast<LPSTR>(fullCommand.c_str()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            std::cerr << "Failed to start process. Error: " << GetLastError() << std::endl;
            return;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

       // KillBlueStacksAdbProcesses();
    }

    void AdbRoot() {
        ExecuteAdb("root");
    }

  

    void adb::InjectLibIntoGame() {
        std::string su_path = "/boot/android/android/system/xbin/bstk/su";

        ExecuteAdb("push C:\\Windows\\emuinj32 /sdcard/");
        ExecuteAdb("push C:\\Windows\\libaimkillob50.so /sdcard/");

        ExecuteAdb("shell \"" + su_path + " -c 'cp /sdcard/emuinj32 /data/local/'\"");
        ExecuteAdb("shell \"" + su_path + " -c 'cp /sdcard/libaimkillob50.so /data/local/'\"");

        ExecuteAdb("shell \"" + su_path + " -c 'chmod 777 /data/local/emuinj32'\"");
        ExecuteAdb("shell \"" + su_path + " -c 'chmod 777 /data/local/libaimkillob50.so'\"");

        ExecuteAdb("shell \"" + su_path + " -c '/data/local/emuinj32 com.dts.freefireth/data/local/libaimkillob50.so'\"");

        ExecuteAdb("shell \"" + su_path + " -c 'rm /data/local/emuinj32'\"");
        ExecuteAdb("shell \"" + su_path + " -c 'rm /sdcard/emuinj32'\"");
        ExecuteAdb("shell \"" + su_path + " -c 'rm /sdcard/libaimkillob50.so'\"");

        Sleep (1000);

        KillBlueStacksAdbProcesses();
    }





    //void adb::ConnectAndControlLib() {
    //    system("cls");
    //    std::cout << "[DLL] === LIB.SO CONTROL PANEL ===\n";

    //    // 1. Check device
    //    system(".\\HD-Adb devices");

    //    // 2. Forward port (PC 21405 -> Device 21405)
    //    system(".\\HD-Adb forward tcp:21405 tcp:21405");

    //    // 3. Init Winsock
    //    WSADATA wsa;
    //    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    //        std::cout << "[DLL] WSAStartup failed!\n";
    //        return;
    //    }

    //    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    //    if (sock == INVALID_SOCKET) {
    //        std::cout << "[DLL] Socket creation failed!\n";
    //        WSACleanup();
    //        return;
    //    }

    //    sockaddr_in addr{};
    //    addr.sin_family = AF_INET;
    //    addr.sin_port = htons(21405);
    //    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    //    std::cout << "[DLL] Connecting...\n";
    //    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    //        std::cout << "[DLL] Connection failed!\n";
    //        closesocket(sock);
    //        WSACleanup();
    //        return;
    //    }

    //    g_LibSocket = sock; 


    //    std::cout << "[DLL] Connected to lib.so\n";

    //    char buffer[256];
    //    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    //    if (bytes > 0)
    //    {
    //        buffer[bytes] = '\0';
    //        std::cout << "[LIB] Says: " << buffer << "\n";
    //        g_LibConnectionStatus.store(1); // ✅ Connected
    //    }
    //    else
    //    {
    //        std::cout << "[DLL] No greeting received!\n";
    //        g_LibConnectionStatus.store(-1); // ❌ Failed
    //    }


    //    //
    //    while (true) {
    //        std::cout << "\n=== Select Command ===\n";
    //        std::cout << "1 - Toggle Aimbot\n";
    //        std::cout << "2 - Toggle ActivateAll\n";
    //        std::cout << "3 - Set FOV\n";
    //        std::cout << "4 - Check Status\n";
    //        std::cout << "0 - Exit\n> ";

    //        int choice;
    //        std::cin >> choice;
    //        if (choice == 0) break;

    //        std::string cmd;
    //        switch (choice) {
    //        case 1: {
    //            std::cout << "[1=ON / 0=OFF]: ";
    //            int state; std::cin >> state;
    //            cmd = state ? "AIMBOT_ON" : "AIMBOT_OFF";
    //            break;
    //        }
    //        case 2: {
    //            std::cout << "[1=ON / 0=OFF]: ";
    //            int state; std::cin >> state;
    //            cmd = state ? "ACTIVATEALL_ON" : "ACTIVATEALL_OFF";
    //            break;
    //        }
    //        case 3: {
    //            std::cout << "Enter FOV value: ";
    //            float fov; std::cin >> fov;
    //            cmd = "SETFOV " + std::to_string(fov);
    //            break;
    //        }
    //        case 4: {
    //            cmd = "GETSTATUS";
    //            break;
    //        }
    //        default:
    //            std::cout << "[DLL] Invalid choice!\n";
    //            continue;
    //        }

    //        // Send command to lib
    //        send(sock, cmd.c_str(), (int)cmd.size(), 0);
    //        std::cout << "[DLL] Sent: " << cmd << "\n";

    //        // If GETSTATUS, read and display reply
    //        if (cmd == "GETSTATUS") {
    //            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    //            if (bytes > 0) {
    //                buffer[bytes] = '\0';
    //                std::cout << "[LIB] Status: " << buffer << "\n";
    //            }
    //            else {
    //                std::cout << "[DLL] Failed to receive status!\n";
    //            }
    //        }
    //    }

    //    closesocket(sock);
    //    WSACleanup();
    //}




void adb::ConnectAndControlLib() {
    // 1. Check device
    ExecuteAdb("devices");

    // 2. Forward port (PC 21405 -> Device 21405)
    ExecuteAdb("forward tcp:21405 tcp:21405");

    // 3. Init Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        g_LibConnectionStatus.store(-1);
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        g_LibConnectionStatus.store(-1);
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(21405);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        g_LibConnectionStatus.store(-1);
        return;
    }

    g_LibSocket = sock;

    // Greeting
    char buffer[256];
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        g_LibGreeting = buffer;  // store greeting
        g_LibConnectionStatus.store(1); // ✅ Connected
    }
    else {
        g_LibGreeting = "No greeting received!";
        g_LibConnectionStatus.store(-1); // ❌ Failed
    }

}






             




    void SendLibCommand(const char* cmd) {
        if (g_LibSocket != INVALID_SOCKET) {
            send(g_LibSocket, cmd, (int)strlen(cmd), 0);
            std::cout << "[DLL] Sent: " << cmd << "\n";
        }
    }

    // chk status

    bool adb::IsLibConnected()
    {
        return g_LibConnectionStatus.load() == 1;
    }

    bool adb::IsLibConnectionFailed()
    {
        return g_LibConnectionStatus.load() == -1;
    }

    bool adb::IsLibConnecting()
    {
        return g_LibConnectionStatus.load() == 0;
    }




    void adb::RequestLibStatus()
    {
        if (g_LibSocket != INVALID_SOCKET)
        {
            const char* cmd = "GETSTATUS";
            send(g_LibSocket, cmd, (int)strlen(cmd), 0);

            char buffer[256];
            int bytes = recv(g_LibSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                g_LibStatus = buffer;
            }
            else {
                g_LibStatus = "Failed to receive status";
            }
        }
        else
        {
            g_LibStatus = "Not connected to lib";
        }
    }













}