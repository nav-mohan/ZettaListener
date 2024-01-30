#include "ms_logger.hpp"
#include <sqlite3.h>
#include <unordered_map>
#include <vector>
#include <string>

class DbCon
{
public:
    DbCon(sqlite3 *db);
    bool insertRecord(std::unordered_map<std::string,std::string> record);
    std::unordered_map<std::string, std::string> getLatest();
    std::vector<std::unordered_map<std::string, std::string>> getByDate(std::string date);
    std::vector<std::unordered_map<std::string, std::string>> getByRange(std::string startTime,std::string endTime);

private:
    bool createTable();
    sqlite3* db_;

};