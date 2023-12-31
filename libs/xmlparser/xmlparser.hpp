#if !defined(XMLPARSER_HPP)
#define XMLPARSER_HPP


#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <vector>
#include <unordered_map>

#include <boost/regex.hpp> // boost::regex is a better library than std::regex, especially for multiline
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/optional.hpp>

#include "ms_logger.hpp"

#define REGEX_PATTERN "<\\?xml\\s.*?<\\/ZettaClipboard>"

const std::unordered_map<std::string,std::string> 
DEFAULT_RESULT = {
    {"LogEventID",      ""},
    {"AirDate",         ""},
    {"AirStartTime",    ""},
    {"AirStopTime",     ""},

    {"LogType",         ""},
    {"ShowType",        ""},

    {"Artist",          ""},
    {"Album",           ""},
    {"Title",           ""},

// Asset Specific Metadata
    {"AssetID",         ""},
    {"AssetType",       ""},
    {"AssetDuration",   ""},
    {"AssetProduct",    ""},
    {"AssetSponsor",    ""},
    {"AssetFilePath",   ""},

// Custom Attributes
    {"RWReleaseDate",     ""},
    {"RWLocal",           ""},
    {"RWCanCon",          ""},
    {"RWHit",             ""},
    {"RWExplicit",        ""},
    {"RWGenre",        ""}
};

struct ZettaFullXmlParser
{
private:
    boost::regex regexPattern_;
    std::mutex mut_;
    std::thread workerThread_;      // to pop strings from the queue_
    std::atomic_bool quit_ = ATOMIC_VAR_INIT(0);
    std::atomic_bool busy_ = ATOMIC_VAR_INIT(0);
    std::condition_variable cond_; 
    std::string buffer_;            
    std::queue<std::string> queue_; // a queue of regex-matching strings
    boost::property_tree::ptree ptree_;
    std::unordered_map<std::string,std::string> result_ = DEFAULT_RESULT;
    inline void resetResult(){result_ = DEFAULT_RESULT;}
    inline void printResult()
    {
        std::string serializedOutput = "\n";
        for(const auto [key,value]:  result_) 
            serializedOutput += key + "--->" + value + "|\n";
        basic_log(serializedOutput,DEBUG);
    }

public:
    void appendData(std::vector<char> &data, std::size_t dataSize); // to push strings into buffer_/queue_
    void parseXml(std::string &&xmlstring);
    ZettaFullXmlParser();
    void Handler(); // 
    ~ZettaFullXmlParser();

    bool isLiveTask(const boost::property_tree::ptree& logEvent);
    void parseLiveTask(const boost::property_tree::ptree& logEvent);

};

#endif // XMLPARSER_HPP
