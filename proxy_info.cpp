#include "proxy_info.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iomanip>


ProxyInfo::ProxyInfo() {
    Reset();
}

void ProxyInfo::Parse(const std::string& url) {
    Reset();

    if (url.empty()) {
        return;
    }

    static const std::regex proxyRegex(
        R"(^(?:(?:([a-z][a-z0-9+\-.]*)):\/\/)?(?:([^:@\/]+)(?::([^@\/]+))?@)?([^:\/?#]+)(?::(\d+))?$)",
        std::regex::icase
    );

    std::smatch match;
    if (!std::regex_match(url, match, proxyRegex)) {
        return;
    }

    isSpecified = true;

    // 处理协议
    protocol = match[1].matched ? ToLower(match[1].str()) : "http";
    if (protocol == "https") {
        protocol = "http";
    }

    // 处理认证信息
    if (match[2].matched) {
        username = UrlDecode(match[2].str());
        login = username;  // 保持兼容性
        if (match[3].matched) {
            password = UrlDecode(match[3].str());
            isAuth = true;
        }
    }

    // 处理主机和端口
    host = match[4].str();
    port = match[5].matched ? std::stoi(match[5].str()) : GetDefaultPort(protocol);

    // 设置协议类型标志
    isHttp = (protocol == "http");
    isSocks5 = (protocol == "socks5");
}

std::string ProxyInfo::GetProxyForceHttp() const {
    if (!isSpecified) {
        return "";
    }
    std::ostringstream oss;
    if (isSocks5) {
        oss << "http://";
        oss << host << ":" << port;
    } else {
        oss << "http://";

        if (isAuth) {
            oss << UrlEncode(username) << ":" << UrlEncode(password) << "@";
        }
        oss << host << ":" << port;
    }

    return oss.str();
}

std::string ProxyInfo::GetProxyNoAuth() const {
    if (!isSpecified) {
        return "";
    }

    std::ostringstream oss;
    oss << protocol << "://";
    oss << host << ":" << port;
    return oss.str();
}

std::string ProxyInfo::ToString() const {
    if (!isSpecified) {
        return "";
    }

    std::ostringstream oss;
    oss << protocol << "://";

    if (isAuth) {
        oss << UrlEncode(username) << ":" << UrlEncode(password) << "@";
    }

    oss << host << ":" << port;
    return oss.str();
}

void ProxyInfo::Reset() {
    isSpecified = false;
    protocol.clear();
    username.clear();
    login.clear();
    password.clear();
    host.clear();
    port = 0;
    isHttp = false;
    isSocks5 = false;
    isAuth = false;
}

int ProxyInfo::GetDefaultPort(const std::string& proto) const {
    if (proto == "http") return 80;
    if (proto == "https") return 443;
    if (proto == "socks5") return 1080;
    return 0;
}

std::string ProxyInfo::ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string ProxyInfo::UrlEncode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        }
        else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }

    return escaped.str();
}

std::string ProxyInfo::UrlDecode(const std::string& str) {
    std::string result;
    std::string::size_type i;
    for (i = 0; i < str.length(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.length()) {
                int value;
                std::istringstream is(str.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    result += static_cast<char>(value);
                    i += 2;
                }
                else {
                    result += str[i];
                }
            }
            else {
                result += str[i];
            }
        }
        else if (str[i] == '+') {
            result += ' ';
        }
        else {
            result += str[i];
        }
    }
    return result;
}



