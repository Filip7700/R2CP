#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

#include "Logger.hpp"
#include "R2CPTransceiverQueue.hpp"
#include "R2CPParser.hpp"
#include "R2CPSerializer.hpp"

#include "R2CPPacketProcessor.hpp"



R2CP::R2CPPacketProcessor::R2CPPacketProcessor() {
    this->polltimeoutinmilliseconds = R2CP::R2CPPacketProcessor::DEFAULTPOLLTIMEOUTINMILLISECONDS;
    this->isrunning = false;
}



R2CP::R2CPPacketProcessor::R2CPPacketProcessor(const uint32_t polltimeoutinmilliseconds) {
    this->polltimeoutinmilliseconds = polltimeoutinmilliseconds;
    this->isrunning = false;
}



R2CP::R2CPPacketProcessor::~R2CPPacketProcessor() {
    this->halt();
    this->unregisterallrobotmodules();
}



void R2CP::R2CPPacketProcessor::registerrobotmodule(const R2CPCommandCodeType reactcommandcode, R2CP::IR2CPRobotModule *robotmodule) {
    if(this->robotmodulesmap.find(reactcommandcode) == this->robotmodulesmap.end()) {
        this->robotmodulesset.insert(robotmodule);
        this->robotmodulesmap[reactcommandcode] = robotmodule;
    }
    else {
        std::string s = Logger::convertformattostring("Robot module not registered. There is already registered robot module to reaction code %u.", reactcommandcode);
        Logger::logwarning(s);
    }
}



void R2CP::R2CPPacketProcessor::registerrobotmodule(R2CP::IR2CPRobotModule *robotmodule) {
    this->robotmodulesset.insert(robotmodule);
}



void R2CP::R2CPPacketProcessor::unregisterrobotmodule(IR2CPRobotModule *robotmodule) {
    for(std::map<R2CPCommandCodeType, IR2CPRobotModule*>::iterator it = this->robotmodulesmap.begin();
        it != this->robotmodulesmap.end();
        it++) {

        R2CPCommandCodeType reactcommandcode = it->first;
        IR2CPRobotModule *registeredrobotmodule = it->second;

        if(robotmodule == registeredrobotmodule) {
            this->robotmodulesmap.erase(reactcommandcode);
        }
    }

    this->robotmodulesset.erase(robotmodule);
}



void R2CP::R2CPPacketProcessor::unregisterrobotmodule(const R2CPCommandCodeType reactcommandcode) {
    try {
        IR2CPRobotModule *registeredrobotmodule = this->robotmodulesmap.at(reactcommandcode);

        this->robotmodulesmap.erase(reactcommandcode);
        this->robotmodulesset.erase(registeredrobotmodule);
    }
    catch(std::out_of_range &e) {
        Logger::logwarning("Robot module cannot be unregistered, because it has not even been registered.");
    }
}



void R2CP::R2CPPacketProcessor::unregisterallrobotmodules() {
    this->robotmodulesmap.clear();
    this->robotmodulesset.clear();
}



void R2CP::R2CPPacketProcessor::run() {
    this->isrunning = true;
    this->packetprocessorthread = new std::thread(&R2CP::R2CPPacketProcessor::runthread, this);
}



void R2CP::R2CPPacketProcessor::halt() {
    this->isrunning = false;

    if(this->packetprocessorthread != nullptr) {
        this->waituntilfinished();
        delete this->packetprocessorthread;
        this->packetprocessorthread = nullptr;
    }
}



void R2CP::R2CPPacketProcessor::runthread() {
    Logger::loginfo("Started packet processing...");
    while(this->isrunning) {
        this->executetask();
        std::this_thread::sleep_for(std::chrono::milliseconds(this->polltimeoutinmilliseconds));
    }
}



void R2CP::R2CPPacketProcessor::executetask() {
    R2CP::R2CPTransceiverQueue &transceiverqueueinstance = R2CP::R2CPTransceiverQueue::getinstance();

    try {
        std::vector<uint8_t> r2cppacket = transceiverqueueinstance.popfromreceptionqueue();

        R2CP::R2CPParser r2cpparserobj;
        R2CP::R2CPSerializer r2cpserializerobj;

        std::vector<R2CP::R2CPCommand> responsecommands;

        // Parse R2CP packet
        try {
            std::vector<R2CP::R2CPCommand> r2cpcommands = r2cpparserobj.parser2cppacket(r2cppacket);

            // Evaluate reactive robot modules functions
            for(std::vector<R2CP::R2CPCommand>::iterator it = r2cpcommands.begin(); it != r2cpcommands.end(); it++) {
                R2CPCommandCodeType commandcode = it->getcommandcode();

                try {
                    R2CP::IR2CPRobotModule *robotmodule = this->robotmodulesmap.at(commandcode);
                    std::vector<R2CPCommand> reactiveresponsecommands = robotmodule->evaluatereactive(*it);

                    uint32_t reactiveresponsecommandscount = reactiveresponsecommands.size();

                    if(reactiveresponsecommandscount > 0U) {
                        responsecommands.insert(responsecommands.end(), reactiveresponsecommands.begin(), reactiveresponsecommands.end());
                    }
                }
                catch(std::out_of_range &e) {
                    std::string s = Logger::convertformattostring("No robot modules assigned to command code %u. Ignoring the command code.", commandcode);
                    Logger::logwarning(s);
                }
            }

            // Evaluate active robot modules functions
            for(std::set<IR2CPRobotModule*>::iterator it = this->robotmodulesset.begin(); it != this->robotmodulesset.end(); it++) {
                std::vector<R2CP::R2CPCommand> activeresultcommands = (*it)->evaluateactive();

                uint32_t activeresultcommandscount = activeresultcommands.size();

                if(activeresultcommandscount > 0U) {
                    responsecommands.insert(responsecommands.end(), activeresultcommands.begin(), activeresultcommands.end());
                }
            }

            size_t responsecommandscount = responsecommands.size();
            if(responsecommandscount > 0U) {
                // Serialize response R2CP commands to R2CP packet
                std::vector<uint8_t> responser2cppacket = r2cpserializerobj.serializecommands(responsecommands);
    
                try {
                    transceiverqueueinstance.pushtotransmissionqueue(responser2cppacket);
                }
                catch(std::overflow_error &e) {
                    Logger::logwarning(e.what());
                }
            }
        }
        catch(std::invalid_argument &e) {
            Logger::logwarning(e.what());
        }
    }
    catch(std::underflow_error &e) {
        Logger::logwarning(e.what());
    }
}



void R2CP::R2CPPacketProcessor::waituntilfinished() {
    this->packetprocessorthread->join();
}
