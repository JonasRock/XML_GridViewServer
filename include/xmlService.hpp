#ifndef XMLSERVICE_H
#define XMLSERVICE_H

#include <string>
#include <memory>

#include "json.hpp"
#include "jsonrpcpp.hpp"

#include "ioHandler.hpp"
#include "messageParser.hpp"
#include "xmlParser.hpp"

namespace xmlServer
{


class XmlService
{
public:
    static void start(std::string address, uint32_t port);

private:
    static void run();
    static uint32_t getRequestID();
    static inline std::shared_ptr<xmlServer::IOHandler> ioHandler_ = nullptr;
    static inline std::shared_ptr<xmlServer::MessageParser> messageParser_ = nullptr;
    static inline std::shared_ptr<xmlServer::XmlParser> xmlParser_ = nullptr;

    static jsonrpcpp::response_ptr request_init(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params);
    static jsonrpcpp::response_ptr request_getChildren(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params);
    
    static void notification_shutdown(const jsonrpcpp::Parameter &params);
};


}

#endif /* XMLSERVICE_H */