#pragma once
#include <string>
#include <vector>

struct UserData {
    std::string Username;
    std::string Expiry;
    std::string Subscription;
};

struct AppData {
    std::string Version;
    std::string Status;
};

class MVPAuth {
public:
    std::string name, ownerid, secret, version, api_url;
    std::string LastMessage = "";
    UserData User;
    AppData App;

    MVPAuth(std::string name, std::string ownerid, std::string secret, std::string version, std::string url)
        : name(name), ownerid(ownerid), secret(secret), version(version), api_url(url) {}

    bool Init();
    bool Login(std::string user, std::string pass);

private:
    std::string sessionid;
    std::string GetHwid();
};

inline MVPAuth MVPAuthApp(
    "mvp",
    "1265242758914773074",
    "jQ0H0dkD4ruFJ9Mvm1j4FWZuwmvBaFnq",
    "1.0",
    "http://paid.mvpcheats.online:19107/api/1.0"
);

inline std::string KeyAuth_message = "";
inline std::string KeyAuth_expiry = "";
inline bool authed = false;
