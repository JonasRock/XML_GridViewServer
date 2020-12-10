#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>

#include "json.hpp"
#include "pugixml.hpp"

namespace xmlServer
{


class XmlParser
{
public:
    void parse(const std::string filepath);
    nlohmann::json getNodeData(const std::string xPathExpression, bool arxml = false);

private:
    pugi::xml_document xmlRoot_;
};


}

#endif /* XMLPARSER_H */