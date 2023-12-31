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

#include <boost/regex.hpp> // boost::regex is a better library than std::regex, especially for multiline
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "ms_logger.hpp"

#define REGEX_PATTERN "<\\?xml\\s.*?<\\/ZettaClipboard>"

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

public:
    void appendData(std::vector<char> &data, std::size_t dataSize); // to push strings into buffer_/queue_
    void parseXml(std::string &&xmlstring);
    ZettaFullXmlParser();
    void Handler(); // 
    ~ZettaFullXmlParser();

    bool isLiveXml(const boost::property_tree::ptree& logEvent);

};

#endif // XMLPARSER_HPP
