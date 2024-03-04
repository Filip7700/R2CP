#include "RobotMotorControlModule.hpp"
#include "Logger.hpp"

#include <vector>
#include <cmath>
#include <wiringPi.h>



RobotMotorControlModule::RobotMotorControlModule(
    const unsigned ena,
    const unsigned in1,
    const unsigned in2,
    const unsigned in3,
    const unsigned in4,
    const unsigned enb) {

    this->ena = ena;
    this->in1 = in1;
    this->in2 = in2;
    this->in3 = in3;
    this->in4 = in4;
    this->enb = enb;

    int res = wiringPiSetup();

    if(res >= 0) {
        pinMode(ena, PWM_OUTPUT);
        pwmWrite(ena, MINPWMDUTYCYCLE);

        pinMode(in1, OUTPUT);
        digitalWrite(in1, LOW);

        pinMode(in2, OUTPUT);
        digitalWrite(in2, LOW);

        pinMode(in3, OUTPUT);
        digitalWrite(in3, LOW);

        pinMode(in4, OUTPUT);
        digitalWrite(in4, LOW);

        pinMode(enb, PWM_OUTPUT);
        pwmWrite(enb, MINPWMDUTYCYCLE);

        Logger::loginfo("Robot motor control module initialized...");
    }
    else {
        Logger::logerror("WiringPi setup failed!");
    }
}



RobotMotorControlModule::~RobotMotorControlModule() {
    // Shut down all active GPIO pins
    this->disablemotors();
}



std::vector<R2CP::R2CPCommand> RobotMotorControlModule::evaluatereactive(R2CP::R2CPCommand &r2cpcommand) {
    R2CPCommandCodeType commandcode = r2cpcommand.getcommandcode();
    std::vector<R2CP::R2CPParameter> &commandparameters = r2cpcommand.getcommandparameters();

    switch(commandcode) {
    case MOTORPWMENABLE:
        Logger::logwarning("Obsolete command...");
        break;

    case MOTORPWMDISABLE:
        this->disablemotors();
        break;

    case MOTORIDLE:
        this->idlemotors();
        break;

    case MOTORDUTYCYCLE:
        this->changedutycycle(commandparameters);
        break;

    default:
        Logger::logwarning(
            Logger::convertformattostring(
                "Command code %u not supported by robot motor control module. Ignoring command...",
                commandcode));
        break;
    }

    Logger::logdebug("Exiting function: evaluatereactive");

    std::vector<R2CP::R2CPCommand> noresponcecommands;
    return noresponcecommands;
}



std::vector<R2CP::R2CPCommand> RobotMotorControlModule::evaluateactive() {
    std::vector<R2CP::R2CPCommand> noresponcecommands;
    return noresponcecommands;
}



void RobotMotorControlModule::disablemotors() {
    pwmWrite(ena, MINPWMDUTYCYCLE);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    pwmWrite(enb, MINPWMDUTYCYCLE);
}



void RobotMotorControlModule::idlemotors() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}



void RobotMotorControlModule::changedutycycle(std::vector<R2CP::R2CPParameter> &r2cpcommandparameters) {
    for(std::vector<R2CP::R2CPParameter>::iterator it = r2cpcommandparameters.begin(); it != r2cpcommandparameters.end(); it++) {
        R2CPParameterCodeType parametercode = it->getparametercode();
        std::vector<uint8_t> &parameterdata = it->getparameterdata();
        unsigned parameterdatasize = parameterdata.size();

        if(parameterdatasize == 4U) {
            float motordutycyclepercentage = this->convertparameterdatatodutycyclepercentage(parameterdata);

            switch(parametercode) {
            case MOTORRIGHT:
                Logger::logwarning(
                    Logger::convertformattostring(
                        "Motor right PWM duty cycle: %f",
                        motordutycyclepercentage));
                this->updatemotorspeedanddirection(this->ena, this->in1, this->in2, motordutycyclepercentage);
                break;

            case MOTORLEFT:
                Logger::logwarning(
                    Logger::convertformattostring(
                        "Motor left PWM duty cycle: %f",
                        motordutycyclepercentage));
                this->updatemotorspeedanddirection(this->enb, this->in3, this->in4, motordutycyclepercentage);
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



float RobotMotorControlModule::convertparameterdatatodutycyclepercentage(const std::vector<uint8_t> &r2cpparameterdata) {
    uint32_t parameterdatabytes = r2cpparameterdata[0] | (r2cpparameterdata[1] << 8U) | (r2cpparameterdata[2] << 16U) | (r2cpparameterdata[3] << 24U);
    float dutycycle = *((float*)&parameterdatabytes);
    return dutycycle;
}



void RobotMotorControlModule::updatemotorspeedanddirection(
    const unsigned pwmpin,
    const unsigned inpin1,
    const unsigned inpin2,
    const float dutycyclepercentage) {

    const float TOLERANCE = 0.1F;

    float absdutycyclepercentage = dutycyclepercentage / 100.0F;
    if(absdutycyclepercentage < 0.0F) {
        absdutycyclepercentage = -absdutycyclepercentage;
    }

    unsigned dutycycle = (unsigned)((float)MAXPWMDUTYCYCLE * absdutycyclepercentage);

    if(dutycycle > MAXPWMDUTYCYCLE) {
        dutycycle = MAXPWMDUTYCYCLE;
    }

    Logger::logwarning(Logger::convertformattostring("Duty cycle: %u", dutycycle));

    if(dutycyclepercentage > TOLERANCE) {
        digitalWrite(inpin2, LOW);
        digitalWrite(inpin1, HIGH);
    }
    else if(dutycyclepercentage < TOLERANCE) {
        digitalWrite(inpin1, LOW);
        digitalWrite(inpin2, HIGH);
    }
    else {
        digitalWrite(inpin1, LOW);
        digitalWrite(inpin2, LOW);
    }

    pwmWrite(pwmpin, dutycycle);
}
