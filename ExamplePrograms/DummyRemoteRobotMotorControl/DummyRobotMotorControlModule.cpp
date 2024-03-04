#include "DummyRobotMotorControlModule.hpp"

#include "Logger.hpp"

#include <string>
#include <vector>

#include <cstdint>



DummyRobotMotorControlModule::DummyRobotMotorControlModule() {}



DummyRobotMotorControlModule::~DummyRobotMotorControlModule() {}



std::vector<R2CP::R2CPCommand> DummyRobotMotorControlModule::evaluatereactive(R2CP::R2CPCommand &r2cpcommand) {
    R2CPCommandCodeType commandcode = r2cpcommand.getcommandcode();
    std::vector<R2CP::R2CPParameter> &commandparameters = r2cpcommand.getcommandparameters();

    switch(commandcode) {
    case MOTORPWMENABLE:
        Logger::logwarning("Motors enabled...");
        break;

    case MOTORPWMDISABLE:
        Logger::logwarning("Motors disabled...");
        break;

    case MOTORIDLE:
        Logger::logwarning("Motors are set to idle...");
        break;

    case MOTORDUTYCYCLE:
        this->evaluatemotordutycycle(commandparameters);
        break;

    default:
        Logger::logwarning(
            Logger::convertformattostring(
                "Command code %u not supported by robot motor control module. Ignoring command...",
                commandcode));
        break;
    }

    std::vector<R2CP::R2CPCommand> noresponcecommands;
    return noresponcecommands;
}



std::vector<R2CP::R2CPCommand> DummyRobotMotorControlModule::evaluateactive() {
    std::vector<R2CP::R2CPCommand> noresponcecommands;
    return noresponcecommands;
}



void DummyRobotMotorControlModule::evaluatemotordutycycle(std::vector<R2CP::R2CPParameter> &r2cpcommandparameters) {
    for(std::vector<R2CP::R2CPParameter>::iterator it = r2cpcommandparameters.begin(); it != r2cpcommandparameters.end(); it++) {
        R2CPParameterCodeType parametercode = it->getparametercode();
        std::vector<uint8_t> &parameterdata = it->getparameterdata();
        unsigned parameterdatasize = parameterdata.size();

        if(parameterdatasize == 4U) {
            float motorpwm = this->convertparameterdatatopwmpercentage(parameterdata);

            switch(parametercode) {
            case MOTORRIGHT:
                Logger::logwarning(
                    Logger::convertformattostring(
                        "Motor right PWM duty cycle: %f",
                        motorpwm));
                break;

            case MOTORLEFT:
                Logger::logwarning(
                    Logger::convertformattostring(
                        "Motor left PWM duty cycle: %f",
                        motorpwm));
                break;

            default:
                Logger::logwarning(
                    Logger::convertformattostring(
                        "Parameter code %u not supported by robot motor control module. Ignoring parameter...",
                        parametercode));
                break;
            }
        }
        else {
            Logger::logerror(
                Logger::convertformattostring(
                    "Invalid size of parameter data. Expected: 4 bytes, actual: %u. Ignoring parameter",
                    parameterdatasize));
        }
    }
}



float DummyRobotMotorControlModule::convertparameterdatatopwmpercentage(const std::vector<uint8_t> &parameterdata) {
    uint32_t parameterdatabytes;

    parameterdatabytes = parameterdata[0] | (parameterdata[1] << 8U) | (parameterdata[2] << 16U) | (parameterdata[3] << 24U);

    float pwm = *((float*)&parameterdatabytes);

    return pwm;
}
