#include <cstdint>
#include <vector>

#include "R2CPCommand.hpp"



namespace R2CP {

class R2CPSerializer {
private:
    static const uint32_t MINR2CPPACKETSIZE = 8U;
    static const uint32_t R2CPMAGICNUMBER = 1379025744U;
    static const uint32_t RRCPMAGICNUMBER = 1381122896U;

    std::vector<uint8_t> serializingoffsetstack;

    std::vector<uint8_t> serializeparameterizedcommandcontent(std::vector<R2CPParameter>&);
    uint32_t precomputer2cppacketsize(std::vector<R2CPCommand>&);
    uint32_t precomputecommandcontentsize(std::vector<R2CPParameter>&);
    void writeuin32tor2cppacket(std::vector<uint8_t>&, const uint32_t);
    void writebytestor2cppacket(std::vector<uint8_t>&, const std::vector<uint8_t>&);

public:
    R2CPSerializer();
    std::vector<uint8_t> serializecommands(std::vector<R2CPCommand>&);
};

} // End of namespace: R2CP
