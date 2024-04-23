extern g_GetFileVersionInfoA: DQ
extern g_GetFileVersionInfoByHandle: DQ
extern g_GetFileVersionInfoExA: DQ
extern g_GetFileVersionInfoExW: DQ
extern g_GetFileVersionInfoSizeA: DQ
extern g_GetFileVersionInfoSizeExA: DQ
extern g_GetFileVersionInfoSizeExW: DQ
extern g_GetFileVersionInfoSizeW: DQ
extern g_GetFileVersionInfoW: DQ
extern g_VerFindFileA: DQ
extern g_VerFindFileW: DQ
extern g_VerInstallFileA: DQ
extern g_VerInstallFileW: DQ
extern g_VerLanguageNameA: DQ
extern g_VerLanguageNameW: DQ
extern g_VerQueryValueA: DQ
extern g_VerQueryValueW: DQ

.code
FakeGetFileVersionInfoA proc
        jmp g_GetFileVersionInfoA
FakeGetFileVersionInfoA endp
FakeGetFileVersionInfoByHandle proc
        jmp g_GetFileVersionInfoByHandle
FakeGetFileVersionInfoByHandle endp
FakeGetFileVersionInfoExA proc
        jmp g_GetFileVersionInfoExA
FakeGetFileVersionInfoExA endp
FakeGetFileVersionInfoExW proc
        jmp g_GetFileVersionInfoExW
FakeGetFileVersionInfoExW endp
FakeGetFileVersionInfoSizeA proc
        jmp g_GetFileVersionInfoSizeA
FakeGetFileVersionInfoSizeA endp
FakeGetFileVersionInfoSizeExA proc
        jmp g_GetFileVersionInfoSizeExA
FakeGetFileVersionInfoSizeExA endp
FakeGetFileVersionInfoSizeExW proc
        jmp g_GetFileVersionInfoSizeExW
FakeGetFileVersionInfoSizeExW endp
FakeGetFileVersionInfoSizeW proc
        jmp g_GetFileVersionInfoSizeW
FakeGetFileVersionInfoSizeW endp
FakeGetFileVersionInfoW proc
        jmp g_GetFileVersionInfoW
FakeGetFileVersionInfoW endp
FakeVerFindFileA proc
        jmp g_VerFindFileA
FakeVerFindFileA endp
FakeVerFindFileW proc
        jmp g_VerFindFileW
FakeVerFindFileW endp
FakeVerInstallFileA proc
        jmp g_VerInstallFileA
FakeVerInstallFileA endp
FakeVerInstallFileW proc
        jmp g_VerInstallFileW
FakeVerInstallFileW endp
FakeVerLanguageNameA proc
        jmp g_VerLanguageNameA
FakeVerLanguageNameA endp
FakeVerLanguageNameW proc
        jmp g_VerLanguageNameW
FakeVerLanguageNameW endp
FakeVerQueryValueA proc
        jmp g_VerQueryValueA
FakeVerQueryValueA endp
FakeVerQueryValueW proc
        jmp g_VerQueryValueW
FakeVerQueryValueW endp
end