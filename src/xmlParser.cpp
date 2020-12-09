#include "xmlParser.hpp"
#include "pugixml.hpp"
#include "xmlExceptions.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include <set>
#include <map>

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

nlohmann::json xmlServer::XmlParser::getNodeData(std::string xPathExpression)
{
    try
    {
        json result = {
            {"attributes", nullptr},
            {"elements", nullptr}
        };
        pugi::xpath_node_set xPathResults = xmlRoot_.select_nodes(xPathExpression.c_str());
        if(xPathResults.size() != 1)
        {
            return nullptr;
        }
        else
        {
            pugi::xpath_node xPathRes = xPathResults.first();
            
            //attributes
            for (auto attribute : xPathRes.node().attributes())
            {
                json attributeJSON = {
                    {"name", attribute.name()},
                    {"value", attribute.value()}
                };
                result["attributes"].push_back(attributeJSON);
            }

            //child elements
            //This works in two iterations.
            //First we loop over the elements once to determine which elements have duplicates, so are in an Array
            //Then we loop over again, and add them to the result, including the info for the arrays

            //contains all strings found so far
            std::set<std::string> foundSet;
            //contains all duplicates found so far
            std::map<std::string, uint32_t> duplicateMap;

            for (auto element : xPathRes.node().children())
            {
                auto res = foundSet.insert(element.name());
                if(!res.second)
                {
                    duplicateMap.insert_or_assign(element.name(), 1);
                }
            }
            //duplicateMap now only contains elements which occur more than once
            for (auto element: xPathRes.node().children())
            {
                std::string name = element.name();
                auto duplicateMapElem = duplicateMap.find(name);
                if(duplicateMapElem != duplicateMap.end())
                {
                    //This element has duplicates in this scope, so [x] is needed
                    name += "[" + std::to_string((duplicateMapElem->second)++) + "]";
                }
                
                //Empty element?
                if (element.first_child().empty() && element.first_attribute().empty())
                {
                    json elementJSON = {
                        {"name", name},
                        {"value", nullptr},
                        {"hasChildren", false}
                    };
                    result["elements"].push_back(elementJSON);
                }
                //Only a single pcdata element as child? -> show as value
                else if (element.first_attribute().empty()
                        && (element.first_child().type() == pugi::node_pcdata)
                        && (element.first_child().next_sibling().type() == pugi::node_null))
                {
                    json elementJSON = {
                        {"name", name},
                        {"value", element.child_value()},
                        {"hasChildren", false}
                    };
                    result["elements"].push_back(elementJSON);
                }
                //Has at least one attribute or child other than a single pcdata
                else
                {
                    json elementJSON = {
                        {"name", name},
                        {"value", ""},
                        {"hasChildren", true},
                        //If we are at root level ("/"), we dont want to have "//" after, because this has another meaning in xpath
                        {"fullPath", !xPathExpression.compare("/")
                            ? xPathExpression + name
                            : xPathExpression + "/" + name}
                    };
                    result["elements"].push_back(elementJSON);
                }
                        
            }
        }
        return result; 
    }
    catch(const pugi::xpath_exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}