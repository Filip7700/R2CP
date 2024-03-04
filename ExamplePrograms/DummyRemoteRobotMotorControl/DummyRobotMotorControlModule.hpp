#pragma once

#include "IR2CPRobotModule.hpp"



class DummyRobotMotorControlModule: public R2CP::IR2CPRobotModule {
public:
    DummyRobotMotorControlModule();
    ~DummyRobotMotorControlModule();

    std::vector<R2CP::R2CPCommand> evaluatereactive(R2CP::R2CPCommand&) override;
    std::vector<R2CP::R2CPCommand> evaluateactive() override;

private:
    void evaluatemotordutycycle(std::vector<R2CP::R2CPParameter>&);
    float convertparameterdatatopwmpercentage(const std::vector<uint8_t>&);
};
