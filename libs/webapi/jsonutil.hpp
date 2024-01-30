#if !defined(JSONUTILITIES_HPP)
#define JSONUTILITIES_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

std::string toJson(std::unordered_map<std::string, std::string> record)
{
    boost::property_tree::ptree pt;

    // Populate the property tree with map elements
    for (const auto& entry : record) {
        pt.put(entry.first, entry.second);
    }

    // Convert the property tree to a JSON string
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pt);
    std::string jsonString = oss.str();

    return jsonString;
}

std::string toJson(std::vector<std::unordered_map<std::string, std::string>> records)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ptree arrayPt;

    // Iterate through the vector and add each unordered_map as a JSON object to the array
    for (const auto& myMap : records) {
        boost::property_tree::ptree objectPt;
        for (const auto& entry : myMap) {
            objectPt.put(entry.first, entry.second);
        }
        arrayPt.push_back(std::make_pair("", objectPt));
    }
    pt.add_child("data",arrayPt);
    // Convert the property tree to a JSON string
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pt);
    std::string jsonArrayString = oss.str();
    return jsonArrayString;
}

#endif // JSONUTILITIES_HPP
