#ifndef ADB_UTILS_HPP
#define ADB_UTILS_HPP

#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <sstream>
#include <string>

namespace adb {
    std::string GetExecutableDirectory();
    std::string GetExecutableDirectoryPID(DWORD processID);
    bool ChangeDirectory(const std::string& path);
    void TerminateAdbProcesses();
    std::string ExtractLibAddress(const std::string& input);
    uintptr_t ConvertToUintPtr(std::string str, int base = 16);
    std::string ExecuteShellCommand(const std::string& firstCommand, const std::string& secondCommand);
    std::string ExecuteShellCommandNoSu(const std::string& command);
    std::string ExecuteShellCommandNoSuRaw(const std::string& command);
    void ExecuteADBCommand(const std::string& command);
    void AdbRoot();




    
    void InjectLibIntoGame();
    void ConnectAndControlLib();


    //

    
    extern SOCKET g_LibSocket;

   
    void SendLibCommand(const char* cmd);



    bool IsLibConnected();
    bool IsLibConnectionFailed();
    bool IsLibConnecting();





    // struing store of lib 
    extern std::string g_LibGreeting;

    // for storing and showing status 

    void RequestLibStatus();

    extern std::string g_LibStatus;




}




#endif