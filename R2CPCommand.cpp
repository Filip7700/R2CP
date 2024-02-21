#include "R2CPCommand.hpp"



R2CP::R2CPCommand::R2CPCommand(const R2CPCommandCodeType commandcode, const std::vector<R2CP::R2CPParameter> &commandparameters) {
    this->commandcode = commandcode;
    this->commandparameters = commandparameters;
}



R2CPCommandCodeType R2CP::R2CPCommand::getcommandcode() {
    uint32_t cc = this->commandcode;
    return cc;
}



std::vector<R2CP::R2CPParameter>& R2CP::R2CPCommand::getcommandparameters() {
    std::vector<R2CP::R2CPParameter> &cp = this->commandparameters;
    return cp;
}



std::vector<R2CP::R2CPParameter> R2CP::R2CPCommand::getcommandparametersdeepcopy() {
    uint32_t commandparameterscount = this->commandparameters.size();

    std::vector<R2CP::R2CPParameter> commandparametersdeepcopy;
    commandparametersdeepcopy.reserve(commandparameterscount);

    for(uint32_t i = 0U; i < commandparameterscount; i++) {
        R2CP::R2CPParameter &commandparameter = commandparameters.at(i);

        uint32_t parametercode = commandparameter.getparametercode();
        std::vector<uint8_t> parameterdata = commandparameter.getparameterdatadeepcopy();

        R2CP::R2CPParameter commandparameterdeepcopy{parametercode, parameterdata};

        commandparametersdeepcopy[i] = commandparameterdeepcopy;
    }

    return commandparametersdeepcopy;
}



uint32_t R2CP::R2CPCommand::getnumberofcommandparameters() {
    uint32_t numberofcommandparameters = this->commandparameters.size();
    return numberofcommandparameters;
}
