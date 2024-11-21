

#include "encoding_utils.h"
#include <stdexcept>


std::string WideToUtf8(const std::wstring& wide) {
	if (wide.empty()) {
		return std::string();
	}

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(),
		static_cast<int>(wide.length()), NULL, 0, NULL, NULL);

	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.length()),
		&strTo[0], size_needed, NULL, NULL);

	return strTo;
}

std::wstring Utf8toWide(const std::string& str) {
	if (str.empty())
		return std::wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	return std::wstring(&buffer[0], charsConverted);
}
