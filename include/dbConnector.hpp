/**
 * @file DBConnector
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.1
*/

#pragma once

/**
 * @todo Sending queries to database
 * @todo Adding photos to database
 * @todo Compatibility check ?
*/

/*! @class DBConnector
 *  Interface providing functions for communication with databases.
*/

class DBConnector {
/*! @fn virtual int sendQuery(const char*) = 0;
 *  @brief For testing purposes.
 *  @warning Should be commented or removed in final version!
 *  @returns 0 if something went wrong
*/
  virtual int sendQuery(const char *query) = 0;
};

class SQLiteConnector {
  int sendQuery(const char *query);
};













