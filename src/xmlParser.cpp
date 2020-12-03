#include "xmlParser.hpp"
#include "pugixml.hpp"
#include "xmlExceptions.hpp"
#include <string>
#include <chrono>

const std::string helper_sanitizeUri(std::string unsanitized)
{
    std::string sanitizedFilePath = unsanitized;
    auto colonPos = sanitizedFilePath.find("%3A");
    sanitizedFilePath.replace(colonPos, 3, ":");
    sanitizedFilePath = sanitizedFilePath.substr(colonPos - 1);
    return sanitizedFilePath;
}

uint32_t xmlServer::XmlParser::parse(const std::string uri)
{
    auto t0 = std::chrono::high_resolution_clock::now();
    pugi::xml_parse_result result = xmlRoot_.load_file(helper_sanitizeUri(uri).c_str());
    auto t1 = std::chrono::high_resolution_clock::now();
    if (result) {
        uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        return ms;
    } else {
        throw xmlServer::parserErrorException();
    }
}