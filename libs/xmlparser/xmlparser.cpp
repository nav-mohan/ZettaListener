#include "xmlparser.hpp"
ZettaFullXmlParser::ZettaFullXmlParser() 
    : regexPattern_(REGEX_PATTERN), 
    workerThread_(&ZettaFullXmlParser::Handler, this) 
    {
        basic_log("ZettaFullXmlParser::ZettaFullXmlParser()",DEBUG);
    };

ZettaFullXmlParser::~ZettaFullXmlParser()
{
    basic_log("ZettaFullXmlParser::~ZettaFullXmlParser()",DEBUG);
    std::unique_lock<std::mutex> lock(mut_);
    quit_.store(1);
    cond_.notify_one();
    lock.unlock();
    if(workerThread_.joinable()) workerThread_.join();
}

void ZettaFullXmlParser::Handler()
{
    std::unique_lock<std::mutex> lock(mut_);

    do
    {
        basic_log("ZettaFullXmlParser::Handler waiting...");
        cond_.wait(lock, [this](){
            return (busy_.load()==0 && (queue_.size()||quit_.load()));
        });
        if(queue_.size() && quit_.load()==0 && busy_.load()==0)
        {
            basic_log("ZettaFullXmlParser::Handler LETS GO!!!");
            std::string xmlstring = std::move(queue_.front());
            queue_.pop();
            busy_.store(1);
            lock.unlock();                  // socket is free to call ZettaFullXmlParser::appendData
            parseXml(std::move(xmlstring)); // blocking call. 
            lock.lock();
            busy_.store(0);
        }
    } while (quit_.load() == 0);
    
}

void ZettaFullXmlParser::appendData(std::vector<char>& data, std::size_t dataSize)
{
    buffer_.insert(buffer_.end(), data.begin(), data.begin() + dataSize);

    boost::sregex_iterator iter(buffer_.begin(), buffer_.end(), regexPattern_);
    boost::sregex_iterator end;
    while(iter != end)
    {
        {
            std::lock_guard<std::mutex> lock(mut_);
            queue_.emplace(buffer_.substr(iter->position(),iter->length()));
        }
        buffer_.erase(0, iter->position() + iter->length());
        boost::algorithm::trim(buffer_);
        iter++;
        cond_.notify_one();
    }
}

void ZettaFullXmlParser::parseXml(std::string&& xmlstring)
{
    std::stringstream ss(xmlstring);
    try
    {
        boost::property_tree::read_xml(ss,ptree_);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    boost::property_tree::ptree& logEvents = ptree_.get_child("ZettaClipboard.LogEvents");
    for(const auto& [key,logEvent] : logEvents)
    {
        if(key == "LogEvent")
            if(isLiveXml(logEvent))
                basic_log("LIVE XML!",INFO);
    }

}

bool ZettaFullXmlParser::isLiveXml(const boost::property_tree::ptree& logEvent)
{
    std::string logEventID = logEvent.get<std::string>("<xmlattr>.LogEventID");
    basic_log(logEventID,ERROR);
    return 1;
}
