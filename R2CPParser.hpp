#pragma once

#include <cstdint>
#include <vector>

#include "R2CPCommand.hpp"



namespace R2CP {

class R2CPParser {
private:
    static const uint32_t MINR2CPPACKETSIZE = 8U;
    static const uint32_t R2CPMAGICNUMBER = 1379025744U;
    static const uint32_t RRCPMAGICNUMBER = 1381122896U;

    std::vector<uint32_t> parsingoffsetstack;

    std::vector<R2CPParameter> parsecommandcontent(const uint32_t, const std::vector<uint8_t>&);
    std::vector<R2CPParameter> parseparameterizedcommandcontent(const uint32_t, const std::vector<uint8_t>&);
    bool validatecommandsfromr2cppacket(const std::vector<uint8_t>&);
    bool validatecommandcontent(const uint32_t, const std::vector<uint8_t>&);
    bool validateparameterizedcommandcontent(const std::vector<uint8_t>&);
    bool checkifr2cppacketendisreached(const std::vector<uint8_t>&);
    uint32_t readr2cppacketasuint32(const std::vector<uint8_t>&);
    std::vector<uint8_t> readbytesfromr2cppacket(const std::vector<uint8_t>&, const uint32_t);
    void skipbytesfromr2cppacket(const uint32_t);

public:
    R2CPParser();
    std::vector<R2CPCommand> parser2cppacket(const std::vector<uint8_t>&);
    bool validater2cppacket(const std::vector<uint8_t>&);
};

} // End of namespace: R2CP
