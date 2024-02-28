#include <chrono>
#include <stdexcept>
#include <vector>

#include "Base64.hpp"
#include "Logger.hpp"

#include "R2CPTransceiverQueue.hpp"

#include "R2CPWebSocketServer.hpp"



R2CP::R2CPWebSocketServer::R2CPWebSocketServer() {
    this->isrunning = false;
    this->hasconnection = false;
    this->polltimeoutinmilliseconds = R2CP::R2CPWebSocketServer::DEFAULTPOLLTIMEOUTINMILLISECONDS;
}



R2CP::R2CPWebSocketServer::R2CPWebSocketServer(const uint32_t polltimeoutinmilliseconds) {
    this->isrunning = false;
    this->hasconnection = false;
    this->polltimeoutinmilliseconds = polltimeoutinmilliseconds;
}



R2CP::R2CPWebSocketServer::~R2CPWebSocketServer() {
    this->halt();
}



void R2CP::R2CPWebSocketServer::run() {
    this->isrunning = true;

    this->receptionthread = new std::thread(websocketpp::lib::bind(
        &R2CP::R2CPWebSocketServer::receiver2cppackettask,
        this));

    this->transmissionthread = new std::thread(websocketpp::lib::bind(
        &R2CP::R2CPWebSocketServer::transmitr2cppackettask,
        this));
}



void R2CP::R2CPWebSocketServer::halt() {
    this->isrunning = false;

    if(this->transmissionthread != nullptr) {
        this->waituntilfinished(this->transmissionthread);
        delete this->transmissionthread;
        this->transmissionthread = nullptr;
    }

    if(this->receptionthread != nullptr) {
        this->wsserver.stop_listening();
        this->closewsserverconnections();
        this->waituntilfinished(this->receptionthread);
        delete this->receptionthread;
        this->receptionthread = nullptr;
    }
}



void R2CP::R2CPWebSocketServer::waituntilfinished() {
    this->receptionthread->join();
    this->transmissionthread->join();
}



void R2CP::R2CPWebSocketServer::onmessage(
    websocketpp::server<websocketpp::config::asio> *pwsserver,
    websocketpp::connection_hdl hdl,
    websocketpp::server<websocketpp::config::asio>::message_ptr msg) {

    R2CP::R2CPTransceiverQueue &tqref = R2CP::R2CPTransceiverQueue::getinstance();

    std::string &r2cppacketbase64 = msg->get_raw_payload();
    std::vector<uint8_t> r2cppacket = Base64::base64_decode(r2cppacketbase64);

    try {
        tqref.pushtoreceptionqueue(r2cppacket);
    }
    catch(std::overflow_error &e) {
        Logger::logwarning(e.what());
    }
}



void R2CP::R2CPWebSocketServer::onopen(
    websocketpp::server<websocketpp::config::asio> *pwsserver,
    websocketpp::connection_hdl hdl) {

    if(this->hasconnection) {
        websocketpp::server<websocketpp::config::asio>::connection_ptr con = this->wsserver.get_con_from_hdl(hdl);
        con->close(websocketpp::close::status::try_again_later, "Other operator already connected!");
    }
    else {
        this->operatorconnectionhandle = hdl;
        this->hasconnection = true;
    }
}



void R2CP::R2CPWebSocketServer::onclose(
    websocketpp::server<websocketpp::config::asio> *pwsserver,
    websocketpp::connection_hdl hdl) {

    this->hasconnection = false;
    Logger::loginfo("Operator has closed connection.");
}



void R2CP::R2CPWebSocketServer::receiver2cppackettask() {
    try {
        this->wsserver.init_asio();

        /* Information on how to setup events and bind event function to event handler
        https://docs.websocketpp.org/md_tutorials_utility_client_utility_client.html */

        this->wsserver.set_message_handler(
            websocketpp::lib::bind(
                &R2CP::R2CPWebSocketServer::onmessage,
                this,
                &this->wsserver,
                websocketpp::lib::placeholders::_1,
                websocketpp::lib::placeholders::_2));

        this->wsserver.set_open_handler(
            websocketpp::lib::bind(
                &R2CP::R2CPWebSocketServer::onopen,
                this,
                &this->wsserver,
                websocketpp::lib::placeholders::_1));

        this->wsserver.set_close_handler(
            websocketpp::lib::bind(
                &R2CP::R2CPWebSocketServer::onclose,
                this,
                &this->wsserver,
                websocketpp::lib::placeholders::_1));

        this->wsserver.listen(4600);
        this->wsserver.start_accept();

        Logger::loginfo("Starting R2CP WebSocket server...");
        this->wsserver.run();
    }
    catch(websocketpp::exception &e) {
        Logger::logerror(e.what());
    }
    catch(...) {
        Logger::logerror("Unknown exception occured during R2CP WebSocket server setup.");
    }

    Logger::loginfo("Exiting R2CP packet reception thread.");
}



void R2CP::R2CPWebSocketServer::transmitr2cppackettask() {
    while(this->isrunning) {
        try {
            R2CP::R2CPTransceiverQueue &tqref = R2CP::R2CPTransceiverQueue::getinstance();
            std::vector<uint8_t> r2cppacket = tqref.popfromtransmissionqueue();
            std::string r2cppacketbase64 = Base64::base64_encode(r2cppacket);

            if(this->hasconnection) {
                this->wsserver.send(this->operatorconnectionhandle, r2cppacketbase64, websocketpp::frame::opcode::binary);
            }
        }
        catch(std::underflow_error &e) {
            Logger::loginfo(e.what());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->polltimeoutinmilliseconds));
    }

    Logger::loginfo("Exiting R2CP packet transmission thread.");
}



void R2CP::R2CPWebSocketServer::waituntilfinished(std::thread *const workingthread) {
    workingthread->join();
}



void R2CP::R2CPWebSocketServer::closewsserverconnections() {
    if(this->hasconnection) {
        websocketpp::server<websocketpp::config::asio>::connection_ptr con = this->wsserver.get_con_from_hdl(this->operatorconnectionhandle);
        con->close(websocketpp::close::status::going_away, "Goodbye!");
        this->hasconnection = false;
    }
}
