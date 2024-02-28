#pragma once

#include <vector>
#include <string>

#include <cstdint>

class Base64 {
public:
    static std::string base64_encode(const std::vector<uint8_t>&);
    static std::vector<uint8_t> base64_decode(std::string const&);

private:
    static bool is_base64(uint8_t);

    static const std::string base64_chars;
};
