#pragma once
#include <vector>
#include <mutex>
#include <windows.h>

struct SocketManagerItem {
    SOCKET sock;
    bool isTcp;
    bool isUdp;
    bool hasSent;
    bool fakeHttpProxyFlag;
    DWORD createdAt;
};

class SocketManager {
private:

    struct UdpAssociate {
        SOCKET tcpControl;
        sockaddr_in bindAddr;
        bool authenticated;
    };

    std::vector<SocketManagerItem> items;
    std::mutex mtx;

    int Find(SOCKET sock);
    void Delete(int index);
    void ClearTimeoutSock();

public:
    void Add(SOCKET sock, int sockType, int sockProtocol);
    bool IsFirstSend(SOCKET sock, SocketManagerItem& item);
    void SetFakeHttpProxyFlag(SOCKET sock);
    bool ClearFakeHttpProxyFlag(SOCKET sock);

};