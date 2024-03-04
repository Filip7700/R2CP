#include "R2CPRuntimeBroker.hpp"

#include "Logger.hpp"

#include "DummyRobotMotorControlModule.hpp"



int main(int argc, char **argv) {
    //Logger::getinstance().addfilelogger("log.txt", Logger::LOGSEVERITYWARNING);
    //Logger &l = Logger::getinstance();
    //l.setstdoutlogseverity(Logger::LOGSEVERITYWARNING);

    R2CP::R2CPRuntimeBroker broker;
    R2CP::R2CPPacketProcessor &packetprocessorref = broker.getpacketprocessor();

    DummyRobotMotorControlModule dummyrobotmotorcontrolobj;
    packetprocessorref.registerrobotmodule(MOTORPWMENABLE, &dummyrobotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORPWMDISABLE, &dummyrobotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORIDLE, &dummyrobotmotorcontrolobj);
    packetprocessorref.registerrobotmodule(MOTORDUTYCYCLE, &dummyrobotmotorcontrolobj);

    broker.run();

    return 0;
}
