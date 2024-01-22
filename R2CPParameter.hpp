#pragma once

#include <cstdint>
#include <vector>



namespace R2CP {

class R2CPParameter {
private:
    uint32_t parametercode;
    std::vector<uint8_t> parameterdata;

public:
    R2CPParameter(const uint32_t, const std::vector<uint8_t>&);

    // Getters
    uint32_t getparametercode();
    std::vector<uint8_t>& getparameterdata();
    std::vector<uint8_t> getparameterdatadeepcopy();
    uint32_t getparameterdatasize();
};

} // End of namespace: R2CP
