#pragma once

#include <cstdint>

#include "IR2CPRobotModule.hpp"



class RobotMotorControlModule: public R2CP::IR2CPRobotModule {
public:
    RobotMotorControlModule(const unsigned, const unsigned, const unsigned, const unsigned, const unsigned, const unsigned);
    ~RobotMotorControlModule();

    std::vector<R2CP::R2CPCommand> evaluatereactive(R2CP::R2CPCommand&) override;
    std::vector<R2CP::R2CPCommand> evaluateactive() override;

private:
    static const unsigned MAXPWMDUTYCYCLE = 1024U;
    static const unsigned MINPWMDUTYCYCLE = 0U;

    void disablemotors();
    void idlemotors();
    void changedutycycle(std::vector<R2CP::R2CPParameter>&);
    float convertparameterdatatodutycyclepercentage(const std::vector<uint8_t>&);
    void updatemotorspeedanddirection(const unsigned, const unsigned, const unsigned, const float);

    // Raspberry Pi's GPIO pin codes to L298N pins
    unsigned ena, in1, in2, in3, in4, enb;
};
