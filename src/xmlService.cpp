#include "xmlService.hpp"
#include "xmlParser.hpp"
#include "config.hpp"

void xmlServer::XmlService::start(std::string address, uint32_t port)
{
    ioHandler_ = std::make_shared<xmlServer::IOHandler>(address, port);
    messageParser_ = std::make_shared<xmlServer::MessageParser>();
    xmlParser_ = std::make_shared<xmlServer::XmlParser>("testfile");

    messageParser_->register_request_callback("initialize", xmlServer::XmlService::request_initialize);
    messageParser_->register_notification_callback("shutdown", xmlServer::XmlService::notification_shutdown);

    run();
}

void xmlServer::XmlService::run()
{
    while(1)
    {
        std::string message = ioHandler_->readNextMessage();
        jsonrpcpp::entity_ptr ret = messageParser_->parse(message);
        if(xmlServer::config::shutdown)
        {
            break;
        }
        if(ret)
        {
            if(ret->is_response())
            {
                ioHandler_->addMessageToSend(std::dynamic_pointer_cast<jsonrpcpp::Response>(ret)->to_json().dump());
            }
        }
        ioHandler_->writeAllMessages();
    }
}

void xmlServer::XmlService::notification_shutdown(const jsonrpcpp::Parameter &params __attribute__((unused))) {
    xmlServer::config::shutdown = true;
}

jsonrpcpp::response_ptr xmlServer::XmlService::request_initialize(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params)
{
    json result = params.to_json();
    return std::make_shared<jsonrpcpp::Response>(id, result);
}