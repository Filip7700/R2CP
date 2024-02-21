#include "R2CPParameter.hpp"



R2CP::R2CPParameter::R2CPParameter(const R2CPParameterCodeType parametercode, const std::vector<uint8_t> &parameterdata) {
    this->parametercode = parametercode;
    this->parameterdata = parameterdata;
}



R2CPParameterCodeType R2CP::R2CPParameter::getparametercode() {
    uint32_t pc = this->parametercode;
    return pc;
}



std::vector<uint8_t>& R2CP::R2CPParameter::getparameterdata() {
    std::vector<uint8_t> &pd = this->parameterdata;
    return pd;
}



std::vector<uint8_t> R2CP::R2CPParameter::getparameterdatadeepcopy() {
    uint32_t parameterdatasize = this->parameterdata.size();

    std::vector<uint8_t> parameterdatadeepcopy;
    parameterdatadeepcopy.reserve(parameterdatasize);

    for(uint32_t i = 0U; i < parameterdatasize; i++) {
        parameterdatadeepcopy[i] = this->parameterdata.at(i);
    }

    return parameterdatadeepcopy;
}



uint32_t R2CP::R2CPParameter::getparameterdatasize() {
    uint32_t parameterdatasize = this->parameterdata.size();
    return parameterdatasize;
}
