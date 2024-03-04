#include "R2CPRuntimeBroker.hpp"

#include "Logger.hpp"

#include "RobotMotorControlModule.hpp"



int main(int argc, char **argv) {
    const unsigned GPIO12PWM0WIRINGPICODE = 26U;
    const unsigned GPIO17WIRINGPICODE = 0U;
    const unsigned GPIO27WIRINGPICODE = 2U;
    const unsigned GPIO22WIRINGPICODE = 3U;
    const unsigned GPIO23WIRINGPICODE = 4U;
    const unsigned GPIO13PWM1WIRINGPICODE = 23U;

    //Logger::getinstance().addfilelogger("log.txt", Logger::LOGSEVERITYWARNING);

    R2CP::R2CPRuntimeBroker broker;
    R2CP::R2CPPacketProcessor &packetprocessorref = broker.getpacketprocessor();

    RobotMotorControlModule robotmotorcontrolobj(
        GPIO12PWM0WIRINGPICODE,
        GPIO17WIRINGPICODE,
        GPIO27WIRINGPICODE,
        GPIO22WIRINGPICODE,
        GPIO23WIRINGPICODE,
        GPIO13PWM1WIRINGPICODE);

    packetprocessorref.registerrobotmodule(MOTORPWMENABLE, &robotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORPWMDISABLE, &robotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORIDLE, &robotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORDUTYCYCLE, &robotmotorcontrolobj);

    broker.run();

    return 0;
}
