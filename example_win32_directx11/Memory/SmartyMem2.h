#pragma once
#include <Windows.h>
#include <vector>
#include <string> 
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>
#include <unordered_map>




extern std::string MemoryLogs;

class SmartyMemory
{

public:


    const char* GetEmulatorRunning()
    {
        if (GetPid("HD-Player.exe") != 0)
            return "HD-Player.exe";

        else if
            (GetPid("HD-Player") != 0)
            return "HD-Player";

        else if
            (GetPid("MEmuHeadless.exe") != 0)
            return "MEmuHeadless.exe";

        else if
            (GetPid("LdVBoxHeadless.exe") != 0)
            return "LdVBoxHeadless.exe";

        else if
            (GetPid("AndroidProcess.exe") != 0)
            return "AndroidProcess.exe";

        else if
            (GetPid("Nox.exe") != 0)
            return "Nox.exe";
    }

    void ReWrite(std::string type, DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* Search, BYTE* Replace)
    {
        if (!AttackProcess(GetEmulatorRunning()))
            MemoryLogs = "Panel : Error!";

        bool Status = ReplacePattern(dwStartRange, dwEndRange, Search, Replace, true);
        if (Status)

            MemoryLogs = "Panel : Error!";

        else

            MemoryLogs = "Panel : Error!";

        CloseHandle(ProcessHandle);
    }

    void OnlyRed()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;


        if (!AttackProcess(GetEmulatorRunning()))
        {
            return;
        }


        std::vector<BYTE> scan = { 0x3F, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x40, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x40, 0x3F };

        std::vector<BYTE> replace = { 0x3F, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        bool status = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());





        // Log results
        if (status)
        {

        }

        else
        {

        }


    }
    void Activating()
    {

    }

    void Activado()
    {

    }

    

    




  

    void SniperSwitch()
    {
        if (!AttackProcess(GetEmulatorRunning()))
        {
            
            MemoryLogs = "Emulator Not Found!";
            return;
        }


        Activating();
        MemoryLogs = "Applying";

        bool st = ReplacePattern(0x0000000000000000, 0x00007fffffffffff,
            new BYTE[]{ 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x34, 0x42, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x8F, 0xC2, 0x35, 0x3F, 0x9A, 0x99, 0x99, 0x3F, 0x00, 0x00, 0x80, 0x3F },
            new BYTE[]{ 0x1A, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x34, 0x42, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x8F, 0xC2, 0x35, 0x3F, 0x9A, 0x99, 0x99, 0x3F, 0x00, 0x00, 0x80, 0x3F },
            true);


        if (st)
        {
            Activado();
            MemoryLogs = "Sniper Scope : Successfully applied !";
           
        }

        else
        {
            MemoryLogs = "Failed To Apply!";
            
        }

        CloseHandle(ProcessHandle);
    }


    void SniperScope()
    {
        if (!AttackProcess(GetEmulatorRunning()))
        {

            MemoryLogs = "Emulator Not Found!";
            return;
        }


        Activating();
        MemoryLogs = "Applying";

        bool st = ReplacePattern(0x0000000000000000, 0x00007fffffffffff,
            new BYTE[]{ 0xFF, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x40, 0xCD, 0xCC, 0x8C, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xB0, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x01 },
            new BYTE[]{ 0xFF, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x40, 0xCD, 0xCC, 0x8C, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xB0, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x01 },
            true);


        if (st)
        {
            Activado();
            MemoryLogs = "Sniper Scope : Successfully applied !";

        }

        else
        {
            MemoryLogs = "Failed To Apply!";

        }

        CloseHandle(ProcessHandle);
    }

    struct EntitySpeedHere
    {
        DWORD_PTR addressSpeed;
        std::vector<BYTE> patternSpeed;
    };

    std::vector<EntitySpeedHere> OldSpeed;
    std::vector<DWORD_PTR> NewSpeed;

    struct EntityHere
    {
        std::vector<EntityHere> SpeedoriginalBytesMap;
        DWORD_PTR addressSpeed;

    };



    std::unordered_map<uintptr_t, std::vector<BYTE>> SpeedoriginalBytesMap;


    bool loadsniper()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = reinterpret_cast<DWORD_PTR>(si.lpMinimumApplicationAddress);
        DWORD_PTR endAddress = reinterpret_cast<DWORD_PTR>(si.lpMaximumApplicationAddress);


        std::vector<BYTE> SearchSpeed = { 0xCC, 0x3D, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xB0, 0x3F, 0x00, 0x00, 0x80, 0x3F };


        if (!AttackProcess("HD-Player.exe"))
        {
            MemoryLogs = "Emulator Not Found !";
            
            return false;
        }

        
        MemoryLogs = "Pattern : Scanning!";

        NewSpeed.clear();
        OldSpeed.clear();


        if (!FindPattern(startAddress, endAddress, SearchSpeed.data(), NewSpeed))
        {
            
            MemoryLogs = "Pattern search failed.";
            return false;
        }


        if (NewSpeed.empty())
        {
            
            MemoryLogs = "Address Not Found!";
            return false;
        }

       
        MemoryLogs = "Scan results found!";
        return true;
    }




    bool onscope()
    {
        std::vector<BYTE> replace = { 0xCC, 0x3D, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xB0, 0x3F, 0x00, 0x00, 0x80, 0x3F };


        if (NewSpeed.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }


        for (const auto& address : NewSpeed)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }

            
        }

        return true;
    }


    bool offscope()
    {
        std::vector<BYTE> replace = { 0xCC, 0x3D, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xB0, 0x3F, 0x00, 0x00, 0x80, 0x3F };


        if (NewSpeed.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }

        for (const auto& address : NewSpeed)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }
        }

        return true;
    }

    bool SaveSpeedAoB()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = reinterpret_cast<DWORD_PTR>(si.lpMinimumApplicationAddress);
        DWORD_PTR endAddress = reinterpret_cast<DWORD_PTR>(si.lpMaximumApplicationAddress);


        std::vector<BYTE> SearchSpeed = { 0x02, 0x2B, 0x07, 0x3D, 0x02, 0x2B, 0x07, 0x3D, 0x02, 0x2B, 0x07, 0x3D };


        if (!AttackProcess("HD-Player.exe"))
        {
            MemoryLogs = "Emulator Not Found !";
            return false;
        }

        MemoryLogs = "Pattern : Scanning!";

        NewSpeed.clear();
        OldSpeed.clear();


        if (!FindPattern(startAddress, endAddress, SearchSpeed.data(), NewSpeed))
        {
            MemoryLogs = "Pattern search failed.";
            return false;
        }


        if (NewSpeed.empty())
        {
            MemoryLogs = "Address Not Found!";
            return false;
        }

        MemoryLogs = "Scan results found!";
        return true;
    }

    bool ActivateSpeed()
    {
        std::vector<BYTE> replace = { 0x02, 0x2B, 0x90, 0x3C, 0x02, 0x2B, 0x90, 0x3C, 0x02, 0x2B, 0x07, 0x3D };


        if (NewSpeed.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }

        MemoryLogs = "Scan results found. Replacing values...";

        for (const auto& address : NewSpeed)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }

            MemoryLogs = "Speed Hack Active";
        }

        return true;
    }


    bool OFFSpeed()
    {
        std::vector<BYTE> replace = { 0x02, 0x2B, 0x07, 0x3D, 0x02, 0x2B, 0x07, 0x3D, 0x02, 0x2B, 0x07, 0x3D };


        if (NewSpeed.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }

        MemoryLogs = "Scan results found. Replacing values...";

        for (const auto& address : NewSpeed)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }

            MemoryLogs = "Speed Hack Deactive";
        }

        return true;
    }

    struct EntitySpeedHere1
    {
        DWORD_PTR addressSpeed;
        std::vector<BYTE> patternSpeed1;
    };

    std::vector<EntitySpeedHere1> OldSpeed1;
    std::vector<DWORD_PTR> NewSpeed1;

    struct EntityHere1
    {
        std::vector<EntityHere> SpeedoriginalBytesMap1;
        DWORD_PTR addressSpeed1;

    };



    std::unordered_map<uintptr_t, std::vector<BYTE>> SpeedoriginalBytesMap1;

    bool SaveWallAoB()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = reinterpret_cast<DWORD_PTR>(si.lpMinimumApplicationAddress);
        DWORD_PTR endAddress = reinterpret_cast<DWORD_PTR>(si.lpMaximumApplicationAddress);


        std::vector<BYTE> SearchSpeed1 = { 0x3F, 0xAE, 0x47, 0x81, 0x3F, 0x00, 0x1A, 0xB7, 0xEE, 0xDC, 0x3A, 0x9F, 0xED, 0x30 };


        if (!AttackProcess("HD-Player.exe"))
        {
            MemoryLogs = "Emulator Not Found !";
            return false;
        }

        MemoryLogs = "Pattern : Scanning!";

        NewSpeed1.clear();
        OldSpeed1.clear();


        if (!FindPattern(startAddress, endAddress, SearchSpeed1.data(), NewSpeed))
        {
            MemoryLogs = "Pattern search failed.";
            return false;
        }


        if (NewSpeed1.empty())
        {
            MemoryLogs = "Address Not Found!";
            return false;
        }

        MemoryLogs = "Scan results found!";
        return true;
    }

    bool ActivateWall()
    {
        std::vector<BYTE> replace = { 0xBF, 0xAE, 0x47, 0x81, 0x3F, 0x00, 0x1A, 0xB7, 0xEE, 0xDC, 0x3A, 0x9F, 0xED, 0x30 };


        if (NewSpeed1.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }

        MemoryLogs = "Scan results found. Replacing values...";

        for (const auto& address : NewSpeed1)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }

            MemoryLogs = "Speed Hack Active";
        }

        return true;
    }

    bool OFFWall()
    {
        std::vector<BYTE> replace = { 0x3F, 0xAE, 0x47, 0x81, 0x3F, 0x00, 0x1A, 0xB7, 0xEE, 0xDC, 0x3A, 0x9F, 0xED, 0x30 };


        if (NewSpeed1.empty())
        {
            MemoryLogs = "No addresses found!";
            return false;
        }

        MemoryLogs = "Scan results found. Replacing values...";

        for (const auto& address : NewSpeed1)
        {
            if (!WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(address), replace.data(), replace.size(), nullptr))
            {
                MemoryLogs += "Failed to write memory at address";
                continue;
            }

            MemoryLogs = "Speed Hack Deactive";
        }

        return true;
    }

    void deWrite(std::string type, DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* Search, BYTE* Replace)
    {
        if (!AttackProcess(GetEmulatorRunning()))
            MemoryLogs = "Panel : Error";

        bool Status = ReplacePattern(dwStartRange, dwEndRange, Search, Replace, true);
        if (Status)
            MemoryLogs = "Panel : Error";
        else
            MemoryLogs = "Panel : Error";

        CloseHandle(ProcessHandle);
    }

    DWORD ProcessId = 0;
    HANDLE ProcessHandle;

    typedef struct _MEMORY_REGION
    {
        DWORD_PTR dwBaseAddr;
        DWORD_PTR dwMemorySize;
    }MEMORY_REGION;

    int GetPid(const char* procname)
    {

        if (procname == NULL)
            return 0;
        DWORD pid = 0;
        DWORD threadCount = 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;

        pe.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnap, &pe);
        while (Process32Next(hSnap, &pe)) {
            if (_tcsicmp(pe.szExeFile, procname) == 0)
            {
                if ((int)pe.cntThreads > threadCount)
                {
                    threadCount = pe.cntThreads;

                    pid = pe.th32ProcessID;

                }
            }
        }
        return pid;
    }
    BOOL AttackProcess(const char* procname)
    {
        DWORD ProcId = GetPid(procname);
        if (ProcId == 0)
            return false;

        ProcessId = ProcId;
        ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
        return ProcessHandle != nullptr;
    }

    bool ChangeProtection(ULONG Address, size_t size, DWORD NewProtect, DWORD& OldProtect)
    {
        return VirtualProtectEx(ProcessHandle, (LPVOID)Address, size, NewProtect, &OldProtect);;
    }

    bool ReplacePattern(DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* SearchAob, BYTE* ReplaceAob, bool ForceWrite = false)
    {
        int RepByteSize = _msize(ReplaceAob);
        if (RepByteSize <= 0) return false;
        std::vector<DWORD_PTR> foundedAddress;
        FindPattern(dwStartRange, dwEndRange, SearchAob, foundedAddress);
        if (foundedAddress.empty())
            return false;

        OutputDebugStringA(std::to_string(foundedAddress.size()).c_str());

        DWORD OldProtect;
        for (int i = 0; i < foundedAddress.size(); i++)
        {
            ChangeProtection(foundedAddress[i], RepByteSize, PAGE_EXECUTE_READWRITE, OldProtect);
            WriteProcessMemory(ProcessHandle, (LPVOID)foundedAddress[i], ReplaceAob, RepByteSize, 0);
        }

        return true;
    }


    bool FindPattern(DWORD_PTR StartRange, DWORD_PTR EndRange, BYTE* SearchBytes, std::vector<DWORD_PTR>& AddressRet)
    {

        BYTE* pCurrMemoryData = NULL;
        MEMORY_BASIC_INFORMATION	mbi;
        std::vector<MEMORY_REGION> m_vMemoryRegion;
        mbi.RegionSize = 0x1000;
        DWORD_PTR dwAddress = StartRange;
        DWORD_PTR nSearchSize = _msize(SearchBytes);


        while (VirtualQueryEx(ProcessHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < EndRange) && ((dwAddress + mbi.RegionSize) > dwAddress))
        {

            if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE))
            {

                MEMORY_REGION mData = { 0 };
                mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
                mData.dwMemorySize = mbi.RegionSize;
                m_vMemoryRegion.push_back(mData);

            }
            dwAddress = (DWORD_PTR)mbi.BaseAddress + mbi.RegionSize;

        }

        std::vector<MEMORY_REGION>::iterator it;
        for (it = m_vMemoryRegion.begin(); it != m_vMemoryRegion.end(); it++)
        {
            MEMORY_REGION mData = *it;


            DWORD_PTR dwNumberOfBytesRead = 0;
            pCurrMemoryData = new BYTE[mData.dwMemorySize];
            ZeroMemory(pCurrMemoryData, mData.dwMemorySize);
            ReadProcessMemory(ProcessHandle, (LPCVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);
            if ((int)dwNumberOfBytesRead <= 0)
            {
                delete[] pCurrMemoryData;
                continue;
            }
            DWORD_PTR dwOffset = 0;
            int iOffset = Memfind(pCurrMemoryData, dwNumberOfBytesRead, SearchBytes, nSearchSize);
            while (iOffset != -1)
            {
                dwOffset += iOffset;
                AddressRet.push_back(dwOffset + mData.dwBaseAddr);
                dwOffset += nSearchSize;
                iOffset = Memfind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, SearchBytes, nSearchSize);
            }

            if (pCurrMemoryData != NULL)
            {
                delete[] pCurrMemoryData;
                pCurrMemoryData = NULL;
            }
        }
        return TRUE;
    }

    int Memfind(BYTE* buffer, DWORD_PTR dwBufferSize, BYTE* bstr, DWORD_PTR dwStrLen)
    {
        if (dwBufferSize < 0)
        {
            return -1;
        }
        DWORD_PTR  i, j;
        for (i = 0; i < dwBufferSize; i++)
        {
            for (j = 0; j < dwStrLen; j++)
            {
                if (buffer[i + j] != bstr[j] && bstr[j] != '?')
                    break;

            }
            if (j == dwStrLen)
                return i;
        }
        return -1;
    }

    static int findMyProc(const char* procname) {
        if (procname == NULL)
            return 0;
        DWORD pid = 0;
        DWORD threadCount = 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;

        pe.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnap, &pe);
        while (Process32Next(hSnap, &pe)) {
            if (_tcsicmp(pe.szExeFile, procname) == 0) {
                if ((int)pe.cntThreads > threadCount) {
                    threadCount = pe.cntThreads;

                    pid = pe.th32ProcessID;

                }
            }
        }
        return pid;
    }
};
