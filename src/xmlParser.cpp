#include "xmlParser.hpp"
#include "pugixml.hpp"
#include "xmlExceptions.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include <set>
#include <map>
#include <boost/iostreams/device/mapped_file.hpp>

using namespace nlohmann; //json.hpp

xmlServer::types::Position xmlServer::XmlParser::getPositionFromOffset(const std::string uri, const uint32_t offset)
{
    auto offsets = newlineOffsets_.at(uri);
    xmlServer::types::Position ret;
    ret.line = std::lower_bound(
        offsets.begin(), offsets.end(), offset
    ) - offsets.begin() - 1;
    ret.character = offset - offsets[ret.line];
    return ret;
}

const std::string helper_sanitizeUri(std::string unsanitized)
{
    std::string sanitizedFilePath = unsanitized;
    auto colonPos = sanitizedFilePath.find("%3A");
    sanitizedFilePath.replace(colonPos, 3, ":");
    sanitizedFilePath = sanitizedFilePath.substr(colonPos - 1);
    return sanitizedFilePath;
}

bool xmlServer::XmlParser::parse(const std::string uri, pugi::xml_parse_result &res)
{
    //Prevent reparsing
    if(xmlRoots_.count(uri) < 1 && newlineOffsets_.count(uri) < 1)
    {
        std::cout << "Parsing: " << uri << "\n";
        auto t0 = std::chrono::high_resolution_clock::now();
        parseNewlines(uri, helper_sanitizeUri(uri));
        auto t1 = std::chrono::high_resolution_clock::now();
        uint32_t msNewline = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "Newline parsing finished: " << msNewline << "ms\n";

        auto t2 = std::chrono::high_resolution_clock::now();
        xmlRoots_.emplace(uri, pugi::xml_document());
        res = xmlRoots_.at(uri).load_file(helper_sanitizeUri(uri).c_str());
        auto t3 = std::chrono::high_resolution_clock::now();
        uint32_t msPugi = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
        std::cout << "PugiXML parsing finished: " << msPugi << "ms\n\n";
        return true;
    }
    else return false;
}

void xmlServer::XmlParser::parseNewlines(const std::string uri, const std::string filepath)
{
    boost::iostreams::mapped_file mmap(filepath, boost::iostreams::mapped_file::readonly);
    const char *const start = mmap.const_data();
    const char *current = start;
    const char *const end = current + mmap.size();

    if(current && current < end)
    {
        std::vector<uint32_t> offsets;
        uint32_t numLines = std::count(start, end, '\n');
        offsets.reserve(numLines + 1);
        offsets.push_back(0);
        
        while(current && current < end)
        {
            current = static_cast<const char *>(memchr(current, '\n', end - current));
            if(current)
            {
                offsets.push_back(current-start);
                ++current;
            }
        }
        newlineOffsets_.emplace(uri, offsets);
    }
    mmap.close();
}

nlohmann::json xmlServer::XmlParser::getNodeData(const std::string uri, std::string xPathExpression, bool arxml)
{
    try
    {
        json result = {
            {"attributes", nullptr},
            {"elements", nullptr}
        };
        pugi::xpath_node_set xPathResults = xmlRoots_.at(uri).select_nodes(xPathExpression.c_str());
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
                        {"value", element.value()},
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
                        {"value", !arxml ? "" : element.find_child(
                            [](pugi::xml_node node){
                                static const std::string sname = "SHORT-NAME";
                                if(!sname.compare(node.name()))
                                {
                                    return true;
                                }
                                return false;
                            }).child_value()
                        },
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

nlohmann::json xmlServer::XmlParser::getNodePosition(const std::string uri, const std::string xPathExpression)
{
    try
    {
        pugi::xpath_node_set xPathResults = xmlRoots_.at(uri).select_nodes(xPathExpression.c_str());
        if(xPathResults.size() != 1)
        {
            return nullptr;
        }
        else
        {
            pugi::xpath_node xPathRes = xPathResults.first();
            uint32_t offset = xPathRes.node().offset_debug();
            json result = getPositionFromOffset(uri, offset);
            return result;
        }
    }
    catch (const pugi::xpath_exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}