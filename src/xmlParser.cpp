#include "xmlParser.hpp"
#include "pugixml.hpp"
#include "xmlExceptions.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <chrono>

using namespace nlohmann; //json.hpp

const std::string helper_sanitizeUri(std::string unsanitized)
{
    std::string sanitizedFilePath = unsanitized;
    auto colonPos = sanitizedFilePath.find("%3A");
    sanitizedFilePath.replace(colonPos, 3, ":");
    sanitizedFilePath = sanitizedFilePath.substr(colonPos - 1);
    return sanitizedFilePath;
}

void xmlServer::XmlParser::parse(const std::string uri)
{
    std::cout << "Parsing: " << uri << "\n";
    auto t0 = std::chrono::high_resolution_clock::now();
    pugi::xml_parse_result result = xmlRoot_.load_file(helper_sanitizeUri(uri).c_str());
    auto t1 = std::chrono::high_resolution_clock::now();
    if (result) {
        uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "Parsing finished: " << ms << "ms\n";
        return;
    } else {
        throw xmlServer::parserErrorException();
    }
}

//xPathExpression points to the node whose info we want

nlohmann::json xmlServer::XmlParser::getNodeData(std::string xPathExpression)
{
    try 
    {
        pugi::xpath_node_set xPathResults = xmlRoot_.select_nodes(xPathExpression.c_str());
        json result = {
            {"attributes", nullptr},
            {"elements", nullptr}
        };
        for(auto xPathRes : xPathResults)
        {
            if(xPathRes)
            {
                for (auto attribute : xPathRes.node().attributes())
                {
                    json attributeJSON = {
                        {"name", attribute.name()},
                        {"value", attribute.value()}
                    };
                    result["attributes"].push_back(attributeJSON);
                }
                for (auto element : xPathRes.node())
                {
                    if(++element.children().begin() == element.children().end()
                        && element.first_child().type() == pugi::node_pcdata)
                    {
                        json elementJSON = {
                            {"name", element.name()},
                            {"value", element.child_value()},
                            {"hasChildren", false}
                        };
                        result["elements"].push_back(elementJSON);
                    } else
                    {
                        json elementJSON = {
                            {"name", element.name()}, 
                            {"value", ""},
                            {"hasChildren", element.children().begin() != element.children().end()}
                        };
                        result["elements"].push_back(elementJSON);
                    }
                    
                }
            }
        }
        return result;
    }
    catch (const pugi::xpath_exception &e)
    {
        std::cout << "XPath error";
        return nullptr;
    }
}