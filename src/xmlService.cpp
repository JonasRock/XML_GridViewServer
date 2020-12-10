#include "xmlService.hpp"
#include "xmlParser.hpp"
#include "config.hpp"

#include <string>

void xmlServer::XmlService::start(std::string address, uint32_t port)
{
    ioHandler_ = std::make_shared<xmlServer::IOHandler>(address, port);
    messageParser_ = std::make_shared<xmlServer::MessageParser>();
    xmlParser_ = std::make_shared<xmlServer::XmlParser>();

    messageParser_->register_request_callback("init", xmlServer::XmlService::request_init);
    messageParser_->register_request_callback("getChildren", xmlServer::XmlService::request_getChildren);

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

jsonrpcpp::response_ptr xmlServer::XmlService::request_init(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params)
{
    xmlParser_->parse((params.to_json())["uri"].get<std::string>());
    json result = 0;
    return std::make_shared<jsonrpcpp::Response>(id, result);
}

jsonrpcpp::response_ptr xmlServer::XmlService::request_getChildren(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params)
{
    json jsonParams = params.to_json();
    bool arxml = false;
    if (jsonParams["options"]["arxml"].is_boolean())
    {
            arxml = jsonParams["options"]["arxml"].get<bool>();
    }
    json result = xmlParser_->getNodeData(jsonParams["xPath"].get<std::string>(), arxml);
    return std::make_shared<jsonrpcpp::Response>(id, result);
}