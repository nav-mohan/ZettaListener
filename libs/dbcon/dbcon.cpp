#include "dbcon.hpp"

DbCon::DbCon(sqlite3 *db) : db_(db)
{
    basic_log("ESTABLISHING DATABASE CONNECTION...");
    bool retval;
    retval = createTable();
}

bool DbCon::createTable()
{
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS ZettaLogger (\
    id INTEGER PRIMARY KEY, \
    LogEventID INTEGER, \
    AirDate DATE, \
    AirStartTime TIME, \
    AirStopTime TIME, \
    LogType TEXT, \
    ShowType TEXT, \
    Artist TEXT, \
    Album TEXT, \
    Title TEXT, \
    AssetID INTEGER, \
    AssetType TEXT, \
    AssetDuration FLOAT, \
    AssetProduct TEXT, \
    AssetSponsor TEXT, \
    AssetFilePath TEXT, \
    RWReleaseDate TEXT, \
    RWGenre TEXT, \
    RWLocal INTEGER, \
    RWCanCon INTEGER, \
    RWHit INTEGER, \
    RWExplicit INTEGER \
    );";
    // LogEventID AirDate AirStartTime AirStopTime LogType ShowType Artist Album Title 
    // AssetID AssetType AssetDuration AssetProduct AssetSponsor AssetFilePath 
    // RWReleaseDate RWLocal RWCanCon RWHit RWExplicit RWGenre

    int retval;
    retval = sqlite3_exec(db_, create_table_sql, 0, 0, NULL);
    if (retval != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db_));
        basic_log("FAILED TO CREATE TABLE",ERROR);
        sqlite3_close(db_);
        return false;
    }
    return true;
}

bool DbCon::insertRecord(std::unordered_map<std::string,std::string> record)
{
    // std::cout << "INSERTING YOUR CUSTOM RECORD" << std::endl;
    sqlite3_stmt *stmt;
    const char *insert_sql = "INSERT INTO ZettaLogger \
    (LogEventID, AirDate, AirStartTime, AirStopTime, LogType, ShowType, Artist, Album, Title, AssetID, AssetType, AssetDuration, AssetProduct, AssetSponsor, AssetFilePath, RWReleaseDate, RWGenre, RWLocal, RWCanCon, RWHit, RWExplicit) \
    VALUES \
    (?, ?,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,? ,?) \
    ";
    int retval;
    retval = sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, NULL);
    if (retval != SQLITE_OK)
    {
        fprintf(stderr, "Prepare statement error: %s\n", sqlite3_errmsg(db_));
        sqlite3_close(db_);
        return false;
    }

    int         logEventID      = atoi( record["LogEventID"]    .c_str());
    const char  *airDate        =       record["AirDate"]       .c_str();
    const char  *airStartTime   =       record["AirStartTime"]  .c_str();
    const char  *airStopTime    =       record["AirStopTime"]   .c_str();
    const char  *logType        =       record["LogType"]       .c_str();
    const char  *showType       =       record["ShowType"]      .c_str();
    const char  *artist         =       record["Artist"]        .c_str();
    const char  *album          =       record["Album"]         .c_str();
    const char  *title          =       record["Title"]         .c_str();
    int         assetID         = atoi( record["AssetID"]       .c_str());
    const char  *assetType      =       record["AssetType"]     .c_str();
    float       assetDuration   = atof( record["AssetDuration"] .c_str());
    const char  *assetProduct   =       record["AssetProduct"]  .c_str();
    const char  *assetSponsor   =       record["AssetSponsor"]  .c_str();
    const char  *assetFilePath  =       record["AssetFilePath"] .c_str();
    const char  *rwReleaseDate  =       record["RWReleaseDate"] .c_str();
    const char  *rwGenre        =       record["RWGenre"]       .c_str();
    int         rwLocal         = atoi( record["RWLocal"]       .c_str());
    int         rwCanCon        = atoi( record["RWCanCon"]      .c_str());
    int         rwHit           = atoi( record["RWHit"]         .c_str());
    int         rwExplicit      = atoi( record["RWExplicit"]    .c_str());

    sqlite3_bind_int(stmt,  1, logEventID);
    sqlite3_bind_text(stmt, 2, airDate,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, airStartTime,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, airStopTime,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, logType,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, showType,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, artist,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, album,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, title,-1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, assetID);
    sqlite3_bind_text(stmt,11, assetType,-1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 12, assetDuration);
    sqlite3_bind_text(stmt,13, assetProduct,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,14, assetSponsor,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,15, assetFilePath,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,16, rwReleaseDate,-1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,17, rwGenre,-1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 18, rwLocal);
    sqlite3_bind_int(stmt, 19, rwCanCon);
    sqlite3_bind_int(stmt, 20, rwHit);
    sqlite3_bind_int(stmt, 21, rwExplicit);

    retval = sqlite3_step(stmt);
    if (retval != SQLITE_DONE)
    {
        basic_log("FAILED TO INSERT RECORD " + std::to_string(logEventID),ERROR);
        fprintf(stderr, "Execution error: %s\n", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        // sqlite3_close(db_);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::unordered_map<std::string, std::string> DbCon::getLatest()
{
    basic_log("GET LATEST",DEBUG);
    const char *sql = "SELECT * FROM ZettaLogger ORDER BY AirStartTime DESC LIMIT 1;";
    sqlite3_stmt *stmt;
    int retval;
    retval = sqlite3_prepare_v2(db_, sql, -1, &stmt, NULL);
    if (retval != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        return {};
    }

    std::unordered_map<std::string,std::string> latestRecord;
    // Execute the statement step by step
    retval = sqlite3_step(stmt);
    if (retval == SQLITE_ROW) 
    {
        // Fetch and print columns
        int id = sqlite3_column_int(stmt, 0); // Assuming the first column is an integer
        const unsigned char *artist         = sqlite3_column_text(stmt, 7);
        const unsigned char *album          = sqlite3_column_text(stmt, 8);
        const unsigned char *title          = sqlite3_column_text(stmt, 9);
        const unsigned char *assetType      = sqlite3_column_text(stmt, 11);
        const unsigned char *rwGenre        = sqlite3_column_text(stmt, 17);
        
        std::string artistStr       = reinterpret_cast<const char*>(artist);
        std::string albumStr        = reinterpret_cast<const char*>(album);
        std::string titleStr        = reinterpret_cast<const char*>(title);
        std::string rwGenreStr      = reinterpret_cast<const char*>(rwGenre);
        std::string assetTypeStr    = reinterpret_cast<const char*>(assetType);
        
        latestRecord = {
            {"Artist",  artistStr},
            {"Album",   albumStr},
            {"Title",   titleStr},
            {"RWGenre" , rwGenreStr}, // for pre-rec shows
            {"AssetType" , assetTypeStr} // for links/call-IDs
        };
    } 
    else 
    {
        fprintf(stderr, "No records found or execution error: %s\n", sqlite3_errmsg(db_));
        latestRecord = {};
    }
    sqlite3_finalize(stmt);
    return latestRecord;

}

std::vector<std::unordered_map<std::string,std::string>> DbCon::getByRange(std::string startTime, std::string endTime)
{
    basic_log("GET BY RANGE [" + startTime + "," + endTime + "]" ,DEBUG);
    // "SELECT * FROM ZettaLogger WHERE AirStartTime BETWEEN "2024-01-20T00:00:00Z" and "2024-01-21T00:00:00Z ORDER BY AirStartTime DESC;";
    const std::string query = "SELECT * FROM ZettaLogger WHERE AirStartTime BETWEEN '" + startTime + "' AND '" + endTime + "' ORDER BY AirStartTime DESC;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        return {};
    }

    std::vector<std::unordered_map<std::string,std::string>> records;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        // Fetch and print columns
        int id = sqlite3_column_int(stmt, 0); // Assuming the first column is an integer
        const unsigned char *airDate        = sqlite3_column_text(stmt, 2);
        const unsigned char *airStartTime   = sqlite3_column_text(stmt, 3);
        const unsigned char *airStopTime    = sqlite3_column_text(stmt, 4);
        const unsigned char *logType        = sqlite3_column_text(stmt, 5);
        const unsigned char *showType       = sqlite3_column_text(stmt, 6);
        const unsigned char *artist         = sqlite3_column_text(stmt, 7);
        const unsigned char *album          = sqlite3_column_text(stmt, 8);
        const unsigned char *title          = sqlite3_column_text(stmt, 9);
        const unsigned char *assetType      = sqlite3_column_text(stmt, 11);
        const unsigned char *rwGenre        = sqlite3_column_text(stmt, 17);
        
        std::string airDateStr = reinterpret_cast<const char*>(airDate);
        std::string airStartTimeStr = reinterpret_cast<const char*>(airStartTime);
        std::string airStopTimeStr = reinterpret_cast<const char*>(airStopTime);
        std::string logTypeStr = reinterpret_cast<const char*>(logType);
        std::string showTypeStr = reinterpret_cast<const char*>(showType);
        std::string artistStr = reinterpret_cast<const char*>(artist);
        std::string albumStr = reinterpret_cast<const char*>(album);
        std::string titleStr = reinterpret_cast<const char*>(title);
        std::string assetTypeStr = reinterpret_cast<const char*>(assetType);
        std::string rwGenreStr = reinterpret_cast<const char*>(rwGenre);

        records.push_back(
        {
            {"AirDate" , airDateStr},
            {"AirStartTime" , airStartTimeStr},
            {"AirStopTime" , airStopTimeStr},
            {"LogType" , logTypeStr},
            {"ShowType" , showTypeStr},
            {"Artist" , artistStr},
            {"Album" , albumStr},
            {"Title" , titleStr},
            {"AssetType" , assetTypeStr},
            {"RWGenre" , rwGenreStr}
        }
        );
    } 
    sqlite3_finalize(stmt);
    return records;

}

std::vector<std::unordered_map<std::string,std::string>> DbCon::getByDate(std::string date)
{
    basic_log("GET BY DATE " + date ,DEBUG);
    const std::string query = "SELECT * FROM ZettaLogger WHERE AirDate='" + date + "' ORDER BY AirStartTime DESC;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        return {};
    }
    
    std::vector<std::unordered_map<std::string,std::string>> records;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        // Fetch and print columns
        int id = sqlite3_column_int(stmt, 0); // Assuming the first column is an integer
        const unsigned char *airDate        = sqlite3_column_text(stmt, 2);
        const unsigned char *airStartTime   = sqlite3_column_text(stmt, 3);
        const unsigned char *airStopTime    = sqlite3_column_text(stmt, 4);
        const unsigned char *logType        = sqlite3_column_text(stmt, 5);
        const unsigned char *showType       = sqlite3_column_text(stmt, 6);
        const unsigned char *artist         = sqlite3_column_text(stmt, 7);
        const unsigned char *album          = sqlite3_column_text(stmt, 8);
        const unsigned char *title          = sqlite3_column_text(stmt, 9);
        const unsigned char *assetType      = sqlite3_column_text(stmt, 11);
        const unsigned char *rwGenre        = sqlite3_column_text(stmt, 17);
        
        std::string airDateStr = reinterpret_cast<const char*>(airDate);
        std::string airStartTimeStr = reinterpret_cast<const char*>(airStartTime);
        std::string airStopTimeStr = reinterpret_cast<const char*>(airStopTime);
        std::string logTypeStr = reinterpret_cast<const char*>(logType);
        std::string showTypeStr = reinterpret_cast<const char*>(showType);
        std::string artistStr = reinterpret_cast<const char*>(artist);
        std::string albumStr = reinterpret_cast<const char*>(album);
        std::string titleStr = reinterpret_cast<const char*>(title);
        std::string assetTypeStr = reinterpret_cast<const char*>(assetType);
        std::string rwGenreStr = reinterpret_cast<const char*>(rwGenre);

        records.push_back(
        {
            {"AirDate" , airDateStr},
            {"AirStartTime" , airStartTimeStr},
            {"AirStopTime" , airStopTimeStr},
            {"LogType" , logTypeStr},
            {"ShowType" , showTypeStr},
            {"Artist" , artistStr},
            {"Album" , albumStr},
            {"Title" , titleStr},
            {"AssetType" , assetTypeStr},
            {"RWGenre" , rwGenreStr}
        }
        );
    } 
    sqlite3_finalize(stmt);
    return records;
}