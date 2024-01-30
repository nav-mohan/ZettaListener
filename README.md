# ZettaListener

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