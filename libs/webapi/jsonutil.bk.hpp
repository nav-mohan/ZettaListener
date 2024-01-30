#if !defined(UTILITIES_HPP)
#define UTILITIES_HPP

#include <string>
#include <unordered_map>
#include <vector>

std::string toJson(std::unordered_map<std::string, std::string> record)
{
    std::string resBody;
    resBody += "{";
    for (auto it = record.begin(); it != record.end(); it++)
    {
        if (it != record.begin()) resBody += ",";
        resBody += "\"" + it->first + "\"" + ":" + "\"" + it->second + "\"";
    }
    resBody += "}";
    return resBody;
}

std::string toJson(std::vector<std::unordered_map<std::string, std::string>> records)
{
    std::string resBody;
    resBody += "[";
    for(auto it = records.begin(); it!= records.end(); it++)
    {
        if(it != records.begin()) resBody += ",";
        resBody += toJson(*it);
    }
    resBody += "]";
    return resBody;
}

#endif // UTILITIES_HPP
