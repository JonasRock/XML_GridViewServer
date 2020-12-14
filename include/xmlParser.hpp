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
    pugi::xml_parse_result parse(const std::string filepath);
    nlohmann::json getNodeData(const std::string xPathExpression, bool arxml = false);
    nlohmann::json getNodePosition(const std::string xPathExpression);
    xmlServer::types::Position getPositionFromOffset(const uint32_t offset);

private:
    pugi::xml_document xmlRoot_;
    std::vector<uint32_t> newlineOffsets_;
    void parseNewlines(const std::string filepath);
};


}

#endif /* XMLPARSER_H */