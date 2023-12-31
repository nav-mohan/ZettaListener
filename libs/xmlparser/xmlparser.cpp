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
        if(key != "LogEvent") continue;
        resetResult();
        parseDefaults(logEvent);
        if(isLiveTask(logEvent))
        {
            basic_log("LIVE XML!",DEBUG);
            parseLiveTask(logEvent);
        }
        else if (isWeirdAsset(logEvent))
        {
            basic_log("WEIRD ASSET XML!",DEBUG);
            parseWeirdAsset(logEvent);
        }
        else 
        {
            basic_log("UNKNONW!",ERROR);
                    }
        printResult();
    }
}

// check if there is a <Task> tag within <LogEvent> tag. The <Task> tag contains attributes such as [Show-Name, Show-Host, Show-Type], or [Sequencer-Mode]
bool ZettaFullXmlParser::isLiveTask(const boost::property_tree::ptree& logEvent)
{
    boost::optional<const boost::property_tree::ptree&> optTask = logEvent.get_child_optional("Task");
    if(optTask) return 1;
    else return 0;
}

// WeirdAsset is the individual song metadata that Steve Kopp uses during his show
// LogEventID==0, AirStopTimeLocal does not exist, and <LogEvent.Asset> exists
bool ZettaFullXmlParser::isWeirdAsset(const boost::property_tree::ptree& logEvent)
{
    const std::string logEventID = logEvent.get<std::string>("<xmlattr>.LogEventID");
    if(logEventID != "0") return 0;

    boost::optional<std::string> optAirStopTime = logEvent.get_optional<std::string>("<xmlattr>.AirStoptimeLocal");
    if(optAirStopTime) return 0;

    boost::optional<const boost::property_tree::ptree&> optAsset = logEvent.get_child_optional("AssetEvent");
    if(!optAsset) return 0;

    return 1;
}

// parse LiveTask should return Show Name, Show Host, and Show Type(Live Music or Live Talk)
// or it should return what type of Sequencer.SetMode Auto, LiveAssist, Manual, etc.
// so it should return LogType, ShowHost, ShowName, ShowType
void ZettaFullXmlParser::parseLiveTask(const boost::property_tree::ptree& logEvent)
{
    const boost::property_tree::ptree& task = logEvent.get_child("Task");
    std::string comment = task.get<std::string>("<xmlattr>.Comment");

    // this means it's a Sequencer.SetMode
    if(task.get_optional<std::string>("<xmlattr>.ParamA"))
    {
        result_["LogType"] = "Sequencer SetMode";
        // now find the "Mode" within the Comment - LiveAssist, Manual, or Auto
        std::size_t modeStartIndex = comment.find("Mode: ") + 6;
        std::size_t modeEndIndex = comment.find(" ]");
        result_["ShowType"] = comment.substr(modeStartIndex,modeEndIndex-modeStartIndex);
    }

    // this means it's a Live Show
    else if(task.get_optional<std::string>("<xmlattr>.ParamB"))
    {
        result_["LogType"] = "Live Show";
        // now find the Artist, Title, Composer within the Comment
        // its arranged like "Title: History of Us, Artist: Steve Kopp, Composer: Live Music Show ]"
        const std::size_t titleStartIndex = comment.find("Title: ") + 7;
        const std::size_t titleEndIndex = comment.find(", Artist: ");
        
        const std::size_t artistStartIndex = titleEndIndex + 10;
        const std::size_t artistEndIndex = comment.find(", Composer: ");
        
        const std::size_t composerStartIndex = artistEndIndex + 12;
        const std::size_t composerEndIndex = comment.find(" ]");

        result_["Title"]    = comment.substr(titleStartIndex, titleEndIndex-titleStartIndex);
        result_["Artist"]   = comment.substr(artistStartIndex, artistEndIndex-artistStartIndex);
        result_["ShowType"] = comment.substr(composerStartIndex, composerEndIndex-composerStartIndex);
    }
}

void ZettaFullXmlParser::parseWeirdAsset(const boost::property_tree::ptree& logEvent)
{
    result_["LogType"] = "Weird Asset";

    const boost::property_tree::ptree& asset = logEvent.get_child("AssetEvent.Asset");

    result_["Title"] = asset.get<std::string>("<xmlattr>.Title");

    boost::optional<const boost::property_tree::ptree&> optionalArtist = asset.get_child_optional("Artist");
    boost::optional<const boost::property_tree::ptree&> optionalParticipant = asset.get_child_optional("Participant");

    if(optionalArtist)
        result_["Artist"] = optionalArtist.get().get<std::string>("<xmlattr>.Name");

    if(optionalParticipant)
        result_["Show Type"] = optionalParticipant.get().get<std::string>("<xmlattr>.Name");

}

// this populates the LogEventID, AirDate, AirStartTime, AirStopTime
void ZettaFullXmlParser::parseDefaults(const boost::property_tree::ptree& logEvent)
{

    result_["LogEventID"] = logEvent.get<std::string>("<xmlattr>.LogEventID");
    result_["AirStartTime"] = logEvent.get<std::string>("<xmlattr>.AirStarttimeLocal");

    const boost::optional<std::string> airStopTimeLocal = logEvent.get_optional<std::string>("<xmlattr>.AirStoptimeLocal");
    if(airStopTimeLocal)
        result_["AirStopTime"] = std::move(airStopTimeLocal.get());
    else
        result_["AirStopTime"] = result_["AirStopTime"];

    const std::size_t airDateEndIndex = result_["AirStartTime"].find(" ");
    result_["AirDate"] = result_["AirStartTime"].substr(0,airDateEndIndex);
}