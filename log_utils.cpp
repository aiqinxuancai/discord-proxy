
#include "log_utils.h"

void LogDebug(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

void LogDebug(const wchar_t* format, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);
    OutputDebugStringW(buffer);
}