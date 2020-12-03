#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include "pugixml.hpp"

namespace xmlServer
{


class XmlParser
{
public:
    uint32_t parse(const std::string filepath);

private:
    pugi::xml_document xmlRoot_;
};


}

#endif /* XMLPARSER_H */