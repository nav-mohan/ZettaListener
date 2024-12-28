# ZettaListener
`ZettaListener` is an [RCS-Zetta](https://www.rcsworks.com/zetta/) compliant metadata logger and distributor. It has two components 
* a TCP server for receiving and parsing streaming XML data 
* a SQLite database for data-persistence
* a HTTP(S) server for querying and serving JSON data

## Third-Pary Libraries 
* `Boost::ASIO`
* `Boost::Regex`
* `Boost::PropertyTree`
* `sqlite3`

## Custom Libraries
* `ms_logger` for thread-safe logging
* `server` for client-server TCP connection


## Work in progress
* `ZettaFullXmlParser` accepts custom attributes (`RW Release Date`,`Local`,`Cancon`,etc.)
* `DbCon` accepts custom names for `MainTable` and `AttributeTable`.
* `DbCon` accepts custom schema for `MainTable` and `AttributeTable`.
* Save configurations (`ZettaFullXML/ZettaCleanXML`,`PortNumber`, `MainTableName`,`AttributeTableName`, `AutoConnect`) to a separate table `ConfigurationTable`.

## ToDo
* Fewer Log statements
