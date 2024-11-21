#pragma once

#include <string>
#include <vector>

class Base64 {
public:
    static std::string Encode(const std::string& input);
    static std::string Encode(const unsigned char* input, size_t length);

    static std::string Decode(const std::string& input);

private:
    static const std::string base64_chars;
    static inline bool is_base64(unsigned char c);
};

