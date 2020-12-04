#include "ioHandler.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "boost/asio.hpp"
#include "boost/lexical_cast.hpp"

using namespace boost;

xmlServer::IOHandler::IOHandler(const std::string &address, uint32_t port)
    : ioc_(), endpoint_(asio::ip::address::from_string(address), port), socket_(ioc_, endpoint_.protocol())
{
    std::cout << "Connecting to " << address << ":" << port << "...\n";
    socket_.connect(endpoint_);
    std::cout << "Connection established\n";
}

void xmlServer::IOHandler::addMessageToSend(const std::string &message)
{
    sendStack_.emplace(message);
}

std::string xmlServer::IOHandler::readNextMessage()
{
    std::string ret;
    if (read_(ret))
    {
#ifndef NO_TERMINAL_OUTPUT
        std::cout << " >> Receiving Message:\n" << ret << "\n\n";
#endif
        return ret;
    }
    else
    {
        return "";
    }
}

void xmlServer::IOHandler::writeAllMessages()
{
    while(!sendStack_.empty())
    {
        std::string toSend = sendStack_.top();
        sendStack_.pop();
        write_(toSend);
#ifndef NO_TERMINAL_OUTPUT
        if(toSend.size() > (1024*5)) //5kb write limit to console
            std::cout << " >> Sending Message:\n" << toSend.substr(0, 1024*5) << "\n>> Console Write limit reached. The write to the socket was unaffected, this is to prevent the console from crashing.\n\n";
        else
            std::cout << " >> Sending Message:\n" << toSend << "\n\n";
#endif
    }
}

std::size_t xmlServer::IOHandler::read_(std::string &message)
{
    static asio::streambuf headerbuf(5000);
    while(socket_.available() < 5 && !headerbuf.size())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    boost::system::error_code ec;

    std::size_t length = asio::read_until(socket_, headerbuf, "\r\n\r\n", ec);
    if (ec) { std::cerr << ec.message() << std::endl; }

    message = std::string(
        asio::buffers_begin(headerbuf.data()),
        // Remove the "\r\n\r\n"
        asio::buffers_begin(headerbuf.data()) + length - 4
    );
    headerbuf.consume(length);
    return length;
}

std::size_t xmlServer::IOHandler::write_(const std::string &message)
{
    asio::streambuf sendBuf;
    std::ostream sendStream(&sendBuf);

    sendStream << message;
    sendStream << "\r\n\r\n";

    size_t sentBytes = asio::write(socket_, sendBuf);
    return sentBytes;
}