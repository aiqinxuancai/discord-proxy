#pragma once

#include <string>
#include <vector>
#include <windows.h>

std::string WideToUtf8(const std::wstring& wide);
std::wstring Utf8toWide(const std::string& str);