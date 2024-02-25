#include "R2CPRuntimeBroker.hpp"

#include <iostream>
#include <string>



R2CP::R2CPRuntimeBroker::R2CPRuntimeBroker() {}



R2CP::R2CPRuntimeBroker::R2CPRuntimeBroker(
    const uint32_t robotserverpolltimeoutinmilliseconds,
    const uint32_t packetprocessorpolltimeoutinmilliseconds):
        robotserver{robotserverpolltimeoutinmilliseconds},
        packetprocessor{packetprocessorpolltimeoutinmilliseconds} {}



R2CP::R2CPRuntimeBroker::~R2CPRuntimeBroker() {
    this->robotserver.halt();
    this->packetprocessor.halt();
}



void R2CP::R2CPRuntimeBroker::run() {
    this->robotserver.run();
    this->packetprocessor.run();

    std::string userinput;

    while(userinput != "q") {
        std::cin >> userinput;
    }
}



R2CP::R2CPWebSocketServer& R2CP::R2CPRuntimeBroker::getrobotserver() {
    return this->robotserver;
}



R2CP::R2CPPacketProcessor& R2CP::R2CPRuntimeBroker::getpacketprocessor() {
    return this->packetprocessor;
}
