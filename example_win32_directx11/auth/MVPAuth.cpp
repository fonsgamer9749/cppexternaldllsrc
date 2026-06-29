#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include "MVPAuth.h"
#include "wnetwrap.h"
#include "json.hpp"

#pragma comment(lib, "advapi32.lib")

using json = nlohmann::json;

bool MVPAuth::Init() {
    std::string hwid = GetHwid();
    std::string post_data = "type=init&app_secret=" + secret + "&ownerid=" + ownerid + "&name=" + name + "&version=" + version + "&hwid=" + hwid;
    
    std::string response = wrap::Post(api_url, post_data);
    if (response.empty()) {
        LastMessage = "Server connection failed";
        return false;
    }

    try {
        auto j = json::parse(response);
        if (j["success"].get<bool>()) {
            sessionid = j["sessionid"].get<std::string>();
            LastMessage = j["message"].get<std::string>();
            return true;
        } else {
            LastMessage = j["message"].get<std::string>();
            return false;
        }
    } catch (...) {
        LastMessage = "Parse error. Server sent: " + (response.length() > 50 ? response.substr(0, 50) + "..." : response);
        return false;
    }
}

bool MVPAuth::Login(std::string user, std::string pass) {
    if (sessionid.empty()) {
        LastMessage = "Please initialize first";
        return false;
    }

    std::string hwid = GetHwid();
    std::string post_data = "type=login&username=" + user + "&password=" + pass + "&hwid=" + hwid + "&sessionid=" + sessionid + "&name=" + name + "&ownerid=" + ownerid;
    
    std::string response = wrap::Post(api_url, post_data);
    if (response.empty()) {
        LastMessage = "Server connection failed";
        return false;
    }

    try {
        auto j = json::parse(response);
        if (j["success"].get<bool>()) {
            User.Username = j["info"]["username"].get<std::string>();
            User.Expiry = j["info"]["expiry"].get<std::string>();
            LastMessage = j["message"].get<std::string>();
            return true;
        } else {
            LastMessage = j["message"].get<std::string>();
            return false;
        }
    } catch (...) {
        LastMessage = "Login Parse error. Server sent: " + (response.length() > 50 ? response.substr(0, 50) + "..." : response);
        return false;
    }
}

std::string MVPAuth::GetHwid() {
    HW_PROFILE_INFOA hwProfileInfo;
    if (GetCurrentHwProfileA(&hwProfileInfo)) {
        return std::string(hwProfileInfo.szHwProfileGuid);
    }
    return "UNKNOWN_HWID";
}
