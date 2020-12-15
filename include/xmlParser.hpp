#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>

#include "json.hpp"
#include "pugixml.hpp"

#include "types.hpp"

namespace xmlServer
{


class XmlParser
{
public:
    bool parse(const std::string uri, pugi::xml_parse_result &result);
    nlohmann::json getNodeData(const std::string uri, const std::string xPathExpression, bool arxml = false);
    nlohmann::json getNodePosition(const std::string uri, const std::string xPathExpression);
    xmlServer::types::Position getPositionFromOffset(const std::string uri, const uint32_t offset);

private:
    std::map<std::string, pugi::xml_document> xmlRoots_;
    std::map<std::string, std::vector<uint32_t>> newlineOffsets_;
    void parseNewlines(const std::string uri, const std::string filepath);
};


}

#endif /* XMLPARSER_H */