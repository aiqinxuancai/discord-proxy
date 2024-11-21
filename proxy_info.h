#pragma once
#include <string>
#include <WinSock2.h>
#include "base64.h"


class ProxyInfo {
public:
    ProxyInfo();

    void Parse(const std::string& url);
    std::string GetProxyForceHttp() const;
    std::string GetProxyNoAuth() const;
    std::string ToString() const;

    bool isSpecified;
    std::string protocol;
    std::string username;
    std::string login;
    std::string password;
    std::string host;
    int port;
    bool isHttp;
    bool isSocks5;
    bool isAuth;


private:

    void Reset();
    int GetDefaultPort(const std::string& proto) const;
    static std::string ToLower(std::string s);
    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);
};

