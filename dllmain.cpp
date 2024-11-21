


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 头文件
#include <WinSock2.h>
#include <Windows.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

#include <detver.h>
#include <detours.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <regex>
#include <map>
#include "socket_manager.h"
#include "proxy_info.h"
#include "encoding_utils.h"
#include "base64.h"
#include "log_utils.h"

#pragma comment(lib, "ws2_32.lib")

#if _WIN64
extern "C" {
	FARPROC g_GetFileVersionInfoA;
	FARPROC g_GetFileVersionInfoByHandle;
	FARPROC g_GetFileVersionInfoExA;
	FARPROC g_GetFileVersionInfoExW;
	FARPROC g_GetFileVersionInfoSizeA;
	FARPROC g_GetFileVersionInfoSizeExA;
	FARPROC g_GetFileVersionInfoSizeExW;
	FARPROC g_GetFileVersionInfoSizeW;
	FARPROC g_GetFileVersionInfoW;
	FARPROC g_VerFindFileA;
	FARPROC g_VerFindFileW;
	FARPROC g_VerInstallFileA;
	FARPROC g_VerInstallFileW;
	FARPROC g_VerLanguageNameA;
	FARPROC g_VerLanguageNameW;
	FARPROC g_VerQueryValueA;
	FARPROC g_VerQueryValueW;
}
#else 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=_AheadLib_GetFileVersionInfoA,@1")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=_AheadLib_GetFileVersionInfoByHandle,@2")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=_AheadLib_GetFileVersionInfoExA,@3")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=_AheadLib_GetFileVersionInfoExW,@4")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=_AheadLib_GetFileVersionInfoSizeA,@5")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=_AheadLib_GetFileVersionInfoSizeExA,@6")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=_AheadLib_GetFileVersionInfoSizeExW,@7")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=_AheadLib_GetFileVersionInfoSizeW,@8")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=_AheadLib_GetFileVersionInfoW,@9")
#pragma comment(linker, "/EXPORT:VerFindFileA=_AheadLib_VerFindFileA,@10")
#pragma comment(linker, "/EXPORT:VerFindFileW=_AheadLib_VerFindFileW,@11")
#pragma comment(linker, "/EXPORT:VerInstallFileA=_AheadLib_VerInstallFileA,@12")
#pragma comment(linker, "/EXPORT:VerInstallFileW=_AheadLib_VerInstallFileW,@13")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=_AheadLib_VerLanguageNameA,@14")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=_AheadLib_VerLanguageNameW,@15")
#pragma comment(linker, "/EXPORT:VerQueryValueA=_AheadLib_VerQueryValueA,@16")
#pragma comment(linker, "/EXPORT:VerQueryValueW=_AheadLib_VerQueryValueW,@17")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 原函数地址指针
PVOID g_GetFileVersionInfoA;
PVOID g_GetFileVersionInfoByHandle;
PVOID g_GetFileVersionInfoExA;
PVOID g_GetFileVersionInfoExW;
PVOID g_GetFileVersionInfoSizeA;
PVOID g_GetFileVersionInfoSizeExA;
PVOID g_GetFileVersionInfoSizeExW;
PVOID g_GetFileVersionInfoSizeW;
PVOID g_GetFileVersionInfoW;
PVOID g_VerFindFileA;
PVOID g_VerFindFileW;
PVOID g_VerInstallFileA;
PVOID g_VerInstallFileW;
PVOID g_VerLanguageNameA;
PVOID g_VerLanguageNameW;
PVOID g_VerQueryValueA;
PVOID g_VerQueryValueW;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AheadLib
{
	HMODULE m_hModule = NULL;	// 原始模块句柄
	DWORD m_dwReturn[17] = { 0 };	// 原始函数返回地址


	// 获取原始函数地址
	FARPROC WINAPI GetAddress(PCSTR pszProcName, bool dontExit = false)
	{
		FARPROC fpAddress;
		CHAR szProcName[16];
		TCHAR tzTemp[MAX_PATH];

		fpAddress = GetProcAddress(m_hModule, pszProcName);
		if (fpAddress == NULL)
		{
			if (HIWORD(pszProcName) == 0)
			{
				wsprintfA(szProcName, "%d", pszProcName);
				pszProcName = szProcName;
			}

			if (!dontExit) {
				wsprintf(tzTemp, TEXT("无法找到函数 %hs，程序无法正常运行。"), pszProcName);
				MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
				ExitProcess(-2);
			}
		}

		return fpAddress;
	}

	// 初始化原始函数地址指针
	inline VOID WINAPI InitializeAddresses()
	{
		g_GetFileVersionInfoA = GetAddress("GetFileVersionInfoA");
		g_GetFileVersionInfoByHandle = GetAddress("GetFileVersionInfoByHandle");
		g_GetFileVersionInfoExA = GetAddress("GetFileVersionInfoExA", true); //win7下没有
		g_GetFileVersionInfoExW = GetAddress("GetFileVersionInfoExW");
		g_GetFileVersionInfoSizeA = GetAddress("GetFileVersionInfoSizeA");
		g_GetFileVersionInfoSizeExA = GetAddress("GetFileVersionInfoSizeExA", true); //win7下没有
		g_GetFileVersionInfoSizeExW = GetAddress("GetFileVersionInfoSizeExW");
		g_GetFileVersionInfoSizeW = GetAddress("GetFileVersionInfoSizeW");
		g_GetFileVersionInfoW = GetAddress("GetFileVersionInfoW");
		g_VerFindFileA = GetAddress("VerFindFileA");
		g_VerFindFileW = GetAddress("VerFindFileW");
		g_VerInstallFileA = GetAddress("VerInstallFileA");
		g_VerInstallFileW = GetAddress("VerInstallFileW");
		g_VerLanguageNameA = GetAddress("VerLanguageNameA");
		g_VerLanguageNameW = GetAddress("VerLanguageNameW");
		g_VerQueryValueA = GetAddress("VerQueryValueA");
		g_VerQueryValueW = GetAddress("VerQueryValueW");
	}

	// 加载原始模块
	inline BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		GetSystemDirectory(tzPath, MAX_PATH);
		lstrcat(tzPath, TEXT("\\version.dll"));
		m_hModule = LoadLibrary(tzPath);
		if (m_hModule == NULL)
		{
			wsprintf(tzTemp, TEXT("无法加载 %s，程序无法正常运行。"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}
		else
		{
			InitializeAddresses();
		}

		return (m_hModule != NULL);
	}

	// 释放原始模块
	inline VOID WINAPI Free()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}
}
using namespace AheadLib;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HMODULE(WINAPI* pMyGetModuleHandleW)(LPCWSTR lpModuleName) = GetModuleHandleW;
static HMODULE(WINAPI* pMyLoadLibraryW)(LPCWSTR lpLibFileName) = LoadLibraryW;
static HMODULE(WINAPI* pMyLoadLibraryExW)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) = LoadLibraryExW;

static DWORD(WINAPI* pMyGetEnvironmentVariableW)(LPCWSTR lpName,LPWSTR lpBuffer,DWORD nSize) = GetEnvironmentVariableW;
static SOCKET(WSAAPI* pMySocket)(int af, int type, int protocol) = socket;
static SOCKET(WSAAPI* pMyWSASocketW)(int af, int type, int protocol, LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags) = WSASocketW;

static int (WSAAPI* pMySend)(SOCKET s, const char* buf, int len, int flags) = send;
static int (WSAAPI* pMyRecv)(SOCKET s, char* buf, int len, int flags) = recv;
static int (WSAAPI* pMyConnect)(SOCKET s, const struct sockaddr* name, int namelen) = connect;

static int (WSAAPI* pMyWSASend)(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	) = WSASend;

static int (WSAAPI* pMyWSARecv)(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd,
	LPDWORD lpFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	) = WSARecv;


static BOOL(WINAPI* pMyCreateProcessW)(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	) = CreateProcessW;

wchar_t g_proxy[512];
bool g_hasProxy;
SocketManager g_socketManager;
ProxyInfo g_configProxy;
bool g_updaterProcess = false;

//updater.node的代理获取方法
DWORD WINAPI MyGetEnvironmentVariableW(
	LPCWSTR lpName,
	LPWSTR lpBuffer,
	DWORD nSize) {

	if (g_updaterProcess && g_hasProxy) {
		if (lstrcmpiW(lpName, L"HTTPS_PROXY") == 0 ||
			lstrcmpiW(lpName, L"https_proxy") == 0 ||
			lstrcmpiW(lpName, L"HTTP_PROXY") == 0 ||
			lstrcmpiW(lpName, L"http_proxy") == 0) {

			std::wstring wProxy = Utf8toWide(g_configProxy.GetProxyForceHttp().c_str());
			OutputDebugStringW(wProxy.c_str());
			wmemcpy(lpBuffer, wProxy.c_str(), lstrlenW(wProxy.c_str()));

			return lstrlenW(wProxy.c_str());
		}
	}

	return pMyGetEnvironmentVariableW(lpName, lpBuffer, nSize);
}


//discord.exe的代理获取方法
BOOL WINAPI MyCreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation) {

	auto cmd = std::wstring(lpCommandLine);
	std::string::size_type idx = cmd.find(L"\\Discord.exe");

	if (g_hasProxy && idx != std::string::npos) {
		std::string::size_type proxyServerIdx = cmd.find(L"--proxy-server");
		if (proxyServerIdx == std::string::npos) {
			//添加参数
			cmd.append(L" --proxy-server==");
			cmd.append(Utf8toWide(g_configProxy.GetProxyNoAuth().c_str()));
			return pMyCreateProcessW(lpApplicationName, (LPWSTR)cmd.c_str(), lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);;
		}
	}

	return pMyCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);;
}

bool ParseHttpConnect(const char* buf, int len, std::string& host, int& port) {
	std::string data(buf, len);
	std::regex connectRegex("CONNECT ([^:]*):(\\d+)");
	std::smatch match;

	if (std::regex_search(data, match, connectRegex)) {
		host = match[1].str();
		port = std::stoi(match[2].str());
		return true;
	}
	return false;
}

bool SendHttpToSocks(SOCKET sock, const char* buf, int len, int flags) {
	std::string host;
	int port;

	if (!ParseHttpConnect(buf, len, host, port)) {
		return false;
	}

	// SOCKS5握手，支持No Auth和Username/Password Auth
	char handshake[] = { 0x05, 0x02, 0x00, 0x02 };
	if (pMySend(sock, handshake, sizeof(handshake), flags) != sizeof(handshake)) {
		return false;
	}

	// 等待服务器响应
	fd_set readSet;
	timeval timeout = { 10, 0 };
	FD_ZERO(&readSet);
	FD_SET(sock, &readSet);

	if (select(0, &readSet, nullptr, nullptr, &timeout) != 1) {
		return false;
	}

	char response[2];
	if (pMyRecv(sock, response, 2, 0) != 2) {
		return false;
	}

	if (response[0] != 0x05) {
		return false;
	}

	if (response[1] == 0x00) {
		//LogDebug("Socks No Authentication Required");
	}
	else if (response[1] == 0x02) {

		// Chrome本身不支持认证，所以此实现无意义
		std::string username = g_configProxy.username;
		std::string password = g_configProxy.password;

		if (username.length() > 255 || password.length() > 255) {
			return false;
		}

		std::vector<char> authRequest;
		authRequest.push_back(0x01); // Version
		authRequest.push_back(static_cast<char>(username.length()));
		authRequest.insert(authRequest.end(), username.begin(), username.end());
		authRequest.push_back(static_cast<char>(password.length()));
		authRequest.insert(authRequest.end(), password.begin(), password.end());

		if (pMySend(sock, authRequest.data(), authRequest.size(), flags) != authRequest.size()) {
			return false;
		}

		char authResponse[2];
		if (pMyRecv(sock, authResponse, 2, 0) != 2) {
			return false;
		}
		
		if (authResponse[0] != 0x01 || authResponse[1] != 0x00) {
			// 认证失败
			return false;
		}
	}
	else {
		// 不支持的认证模式
		return false;
	}

	// 发送连接请求
	std::vector<char> request;
	request.push_back(0x05); // VER
	request.push_back(0x01); // CMD: CONNECT
	request.push_back(0x00); // RSV
	request.push_back(0x03); // ATYP: DOMAIN
	request.push_back(static_cast<char>(host.length())); // Domain length
	request.insert(request.end(), host.begin(), host.end()); // Domain
	request.push_back(static_cast<char>(port >> 8)); // Port high byte
	request.push_back(static_cast<char>(port & 0xFF)); // Port low byte

	if (pMySend(sock, request.data(), request.size(), flags) != request.size()) {
		return false;
	}

	LogDebug("SendHttpToSocks#7");

	g_socketManager.SetFakeHttpProxyFlag(sock);
	return true;
}

SOCKET WSAAPI MySocket(int af, int type, int protocol) {
	SOCKET sock = pMySocket(af, type, protocol);
	if (!g_updaterProcess) {
		return sock;
	}
	if (sock != INVALID_SOCKET) {
		g_socketManager.Add(sock, type, protocol);
	}
	return sock;
}

SOCKET WSAAPI MyWSASocketW(int af, int type, int protocol,
	LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags) {
	SOCKET sock = pMyWSASocketW(af, type, protocol, lpProtocolInfo, g, dwFlags);

	if (!g_updaterProcess) {
		return sock;
	}

	if (sock != INVALID_SOCKET) {
		g_socketManager.Add(sock, type, protocol);
	}
	return sock;
}


int WSAAPI MySend(SOCKET s, const char* buf, int len, int flags) {
	if (g_updaterProcess) {
		SocketManagerItem item;
		if (g_socketManager.IsFirstSend(s, item)) {
			if (item.isTcp && g_configProxy.isSocks5) {
				if (SendHttpToSocks(s, buf, len, flags)) {
					return len;
				}
			}
		}
	}

	return pMySend(s, buf, len, flags);
}

int WSAAPI MyRecv(SOCKET s, char* buf, int len, int flags) {
	int ret = pMyRecv(s, buf, len, flags);
	if (!g_updaterProcess) {
		return ret;
	}

	if (ret > 0 && g_socketManager.ClearFakeHttpProxyFlag(s)) {
		if (ret >= 10) {
			if (memcmp(buf, "\x05\x00\x00", 3) == 0) {
				const char* httpResponse =
					"HTTP/1.1 200 Connection Established\r\n\r\n";
				size_t respLen = strlen(httpResponse);
				if (respLen <= len) {
					memcpy(buf, httpResponse, respLen);
					return respLen;
				}
			}
		}
	}

	return ret;
}


void OnModuleLoaded(LPCWSTR moduleName) {
	//LogDebug(std::wstring(L"OnModuleLoaded " + std::wstring(moduleName)).c_str());
	
	std::wstring moduleNameStr(moduleName);
	const wchar_t* endsWith = L"\\updater.node";

	if (moduleNameStr.length() >= wcslen(endsWith) &&
		moduleNameStr.compare(moduleNameStr.length() - wcslen(endsWith), wcslen(endsWith), endsWith) == 0) {
		LogDebug(L"updater.node loaded");
		g_updaterProcess = true;
		//仅在updater中支持socks5，性能优化，chrome本身已支持，所以无需调用相关代码
	}
}

HMODULE WINAPI MyGetModuleHandleW(LPCWSTR lpModuleName) {
	HMODULE hModule = pMyGetModuleHandleW(lpModuleName);
	if (hModule && lpModuleName) {
		OnModuleLoaded(lpModuleName);
	}
	return hModule;
}

HMODULE WINAPI MyLoadLibraryW(LPCWSTR lpLibFileName) {
	HMODULE hModule = pMyLoadLibraryW(lpLibFileName);
	if (hModule && lpLibFileName) {
		OnModuleLoaded(lpLibFileName);
	}
	return hModule;
}

HMODULE WINAPI MyLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	HMODULE hModule = pMyLoadLibraryExW(lpLibFileName, hFile, dwFlags);
	if (hModule && lpLibFileName) {
		OnModuleLoaded(lpLibFileName);
	}
	return hModule;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY LoadProxyConfig() {

	auto commandLine = GetCommandLineA();
	auto cmd = std::string(commandLine);
	auto cmdTag = "--proxy-server=";
	std::string::size_type proxyServerIdx = cmd.find(cmdTag);

	if (cmd.find(cmdTag) != std::string::npos) {
		using namespace std;
		//OutputDebugStringA(cmd.c_str());
		std::string regexp_string(".*--proxy-server(={1,2})(.[^\\s]*)(\\s{0,1}|$|.*)"); // Nothing to be escaped here, right?
		std::regex  regexp_to_match(regexp_string);
		smatch matchResult;
		if (std::regex_match(cmd, matchResult, regexp_to_match)) {
			if (matchResult.size() == 4) {
				//OutputDebugStringA(matchResult[2].str().c_str());
				auto wide = Utf8toWide(matchResult[2].str());
				wmemcpy(g_proxy, wide.c_str(), wide.size());
				g_configProxy.Parse(matchResult[2].str());
				g_hasProxy = true;
			}
		}
		else {
			//未找到关键字
		}
		
	} else {

		//另一个配置方案，暂时不使用
		const wchar_t* lpPath = L".\\proxy.ini";
		auto result = GetPrivateProfileStringW(L"Config", L"Proxy", L"", g_proxy, 512, lpPath);
		g_hasProxy = result != 0;
	}

	return g_hasProxy;
}


BOOL APIENTRY InstallHook() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((void**)&pMyGetEnvironmentVariableW, MyGetEnvironmentVariableW);
	DetourAttach((void**)&pMyCreateProcessW, MyCreateProcessW);

	DetourAttach((void**)&pMySocket, MySocket);
	DetourAttach((void**)&pMyWSASocketW, MyWSASocketW);

	DetourAttach((void**)&pMySend, MySend);
	DetourAttach((void**)&pMyRecv, MyRecv);

	DetourAttach((void**)&pMyGetModuleHandleW, MyGetModuleHandleW);
	DetourAttach((void**)&pMyLoadLibraryW, MyLoadLibraryW);
	DetourAttach((void**)&pMyLoadLibraryExW, MyLoadLibraryExW);

	LONG ret = DetourTransactionCommit();
	return ret == NO_ERROR;
}

BOOL APIENTRY UnInstallHook() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((void**)&pMyGetEnvironmentVariableW, MyGetEnvironmentVariableW);
	DetourDetach((void**)&pMyCreateProcessW, MyCreateProcessW);

	DetourDetach((void**)&pMySocket, MySocket);
	DetourDetach((void**)&pMyWSASocketW, MyWSASocketW);

	DetourDetach((void**)&pMySend, MySend);
	DetourDetach((void**)&pMyRecv, MyRecv);

	DetourDetach((void**)&pMyGetModuleHandleW, MyGetModuleHandleW);
	DetourDetach((void**)&pMyLoadLibraryW, MyLoadLibraryW);
	DetourDetach((void**)&pMyLoadLibraryExW, MyLoadLibraryExW);

	LONG ret = DetourTransactionCommit();
	return ret == NO_ERROR;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入口函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		bool result = Load();
		LoadProxyConfig();
		InstallHook();
		return result;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !_WIN64
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoA(void)
{
	__asm JMP g_GetFileVersionInfoA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoByHandle(void)
{
	__asm JMP g_GetFileVersionInfoByHandle;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoExA(void)
{
	__asm JMP g_GetFileVersionInfoExA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoExW(void)
{
	__asm JMP g_GetFileVersionInfoExW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeA(void)
{
	__asm JMP g_GetFileVersionInfoSizeA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeExA(void)
{
	__asm JMP g_GetFileVersionInfoSizeExA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeExW(void)
{
	__asm JMP g_GetFileVersionInfoSizeExW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeW(void)
{
	__asm JMP g_GetFileVersionInfoSizeW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoW(void)
{
	__asm JMP g_GetFileVersionInfoW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerFindFileA(void)
{
	__asm JMP g_VerFindFileA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerFindFileW(void)
{
	__asm JMP g_VerFindFileW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerInstallFileA(void)
{
	__asm JMP g_VerInstallFileA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerInstallFileW(void)
{
	__asm JMP g_VerInstallFileW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerLanguageNameA(void)
{
	__asm JMP g_VerLanguageNameA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerLanguageNameW(void)
{
	__asm JMP g_VerLanguageNameW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerQueryValueA(void)
{
	__asm JMP g_VerQueryValueA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerQueryValueW(void)
{
	__asm JMP g_VerQueryValueW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


