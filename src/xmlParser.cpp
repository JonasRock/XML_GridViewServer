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
        bool isArray = false;
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
                if(xPathRes.node().children().begin() != xPathRes.node().children().end())
                {
                    bool hasMultipleElements = xPathRes.node().first_child() != xPathRes.node().last_child();
                    if (hasMultipleElements)
                    {
                        std::string name1 = xPathRes.node().first_child().name();
                        std::string name2 = xPathRes.node().last_child().name();
                        if (!name1.compare(name2))
                        {
                            //For now just assume its either all the same or all different elements
                            isArray = true;
                        }
                    }
                    int index = 1;
                    for (auto element : xPathRes.node())
                    {
                        std::string name = element.name();
                        if (isArray)
                        {
                            name += "[" + std::to_string(index++) + "]";
                        }
                        //Empty element?
                        if ((element.first_child().empty()))
                        {
                            json elementJSON = {
                                {"name", name},
                                {"value", ""},
                                {"hasChildren", false}
                            };
                            result["elements"].push_back(elementJSON);
                        }
                        else if ((++element.children().begin() == element.children().end())
                            && (element.first_child().type() == pugi::node_pcdata))
                        {
                            json elementJSON = {
                                {"name", name},
                                {"value", element.child_value()},
                                {"hasChildren", false}
                            };
                            result["elements"].push_back(elementJSON);
                        } 
                        else
                        {
                            json elementJSON = {
                                {"name", name}, 
                                {"value", ""},
                                {"hasChildren", element.empty() ? false : element.children().begin() != element.children().end()},
                                {"fullPath", !xPathExpression.compare("/") ? xPathExpression + name : xPathExpression + "/" + name}
                            };
                            result["elements"].push_back(elementJSON);
                        }
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