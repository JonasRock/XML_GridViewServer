#ifndef XMLSERVICE_H
#define XMLSERVICE_H

#include <string>
#include <memory>

#include "json.hpp"
#include "jsonrpcpp.hpp"

#include "ioHandler.hpp"
#include "messageParser.hpp"

namespace xmlServer
{


class xmlService
{
public:
    static void start(std::string address, uint32_t port);

private:
    static void run();
    static uint32_t getRequestID();
    static inline std::shared_ptr<xmlServer::IOHandler> ioHandler_ = nullptr;
    static inline std::shared_ptr<xmlServer::MessageParser> messageParser_ = nullptr;
}


}

#endif /* XMLSERVICE_H */