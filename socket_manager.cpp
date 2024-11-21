#include "socket_manager.h"

int SocketManager::Find(SOCKET sock) {
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].sock == sock) {
            return (int)i;
        }
    }
    return -1;
}

void SocketManager::Delete(int index) {
    if (index < (int)items.size() - 1) {
        items[index] = items.back();
    }
    items.pop_back();
}

void SocketManager::ClearTimeoutSock() {
    DWORD tick = GetTickCount64() - 30000;
    for (int i = (int)items.size() - 1; i >= 0; i--) {
        if (items[i].createdAt < tick) {
            Delete(i);
        }
    }
}

void SocketManager::Add(SOCKET sock, int sockType, int sockProtocol) {
    SocketManagerItem item = {
        sock,
        (sockType == SOCK_STREAM) && (sockProtocol == IPPROTO_TCP || sockProtocol == 0),
        (sockType == SOCK_DGRAM) && (sockProtocol == IPPROTO_UDP || sockProtocol == 0),
        false,
        false,
        GetTickCount64()
    };

    std::lock_guard<std::mutex> lock(mtx);
    ClearTimeoutSock();

    int index = Find(sock);
    if (index == -1) {
        items.push_back(item);
    }
    else {
        items[index] = item;
    }
}

bool SocketManager::IsFirstSend(SOCKET sock, SocketManagerItem& item) {
    std::lock_guard<std::mutex> lock(mtx);
    int index = Find(sock);
    if (index == -1 || items[index].hasSent) {
        return false;
    }

    items[index].hasSent = true;
    item = items[index];
    return true;
}

void SocketManager::SetFakeHttpProxyFlag(SOCKET sock) {
    std::lock_guard<std::mutex> lock(mtx);
    int index = Find(sock);
    if (index >= 0) {
        items[index].fakeHttpProxyFlag = true;
    }
}

bool SocketManager::ClearFakeHttpProxyFlag(SOCKET sock) {
    std::lock_guard<std::mutex> lock(mtx);
    int index = Find(sock);
    if (index == -1 || !items[index].fakeHttpProxyFlag) {
        return false;
    }

    items[index].fakeHttpProxyFlag = false;
    return true;
}


