#pragma once

#include <cstdint>
#include <vector>

#include "R2CPCommandDictionary.hpp"



namespace R2CP {

class R2CPParameter {
private:
    R2CPParameterCodeType parametercode;
    std::vector<uint8_t> parameterdata;

public:
    R2CPParameter(const R2CPParameterCodeType, const std::vector<uint8_t>&);

    // Getters
    R2CPParameterCodeType getparametercode();
    std::vector<uint8_t>& getparameterdata();
    std::vector<uint8_t> getparameterdatadeepcopy();
    uint32_t getparameterdatasize();
};

} // End of namespace: R2CP
