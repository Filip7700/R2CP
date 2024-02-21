#pragma once

#include <map>
#include <memory>
#include <set>
#include <thread>

#include <cstdint>

#include "IR2CPRobotModule.hpp"



namespace R2CP {

class R2CPPacketProcessor {
private:
    static const uint32_t DEFAULTPOLLTIMEOUTINMILLISECONDS = 10U;

    bool isrunning;
    uint32_t polltimeoutinmilliseconds;
    std::map<R2CPCommandCodeType, IR2CPRobotModule*> robotmodulesmap;
    std::set<IR2CPRobotModule*> robotmodulesset;
    std::thread *packetprocessorthread;

    void runthread();

protected:
    void executetask();

public:
    R2CPPacketProcessor();
    R2CPPacketProcessor(const uint32_t);
    ~R2CPPacketProcessor();

    void registerrobotmodule(const R2CPCommandCodeType, IR2CPRobotModule*);
    void registerrobotmodule(IR2CPRobotModule*);
    void unregisterrobotmodule(IR2CPRobotModule*);
    void unregisterrobotmodule(const R2CPCommandCodeType);
    void unregisterallrobotmodules();
    void run();
    void halt();
    void waituntilfinished();
};

} // End of namespace: R2CP
