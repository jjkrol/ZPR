/**
 * @file dbConnector.hpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.1
 * Basicly there are 3 types of classes defined in this file:
 * - DBConnector which is an interface for all singletons used
 *   to communicate with diffrent types of databases (eg.SQLite DB)
 * - DBConnectorFactory which is used to create DBConnectors
 *   (it ensures that there is no more than one object of each type
 *   of database connector
 * - concrete versions of DBConnector (eg. SQLiteConnector)
*/
#pragma once

#include <vector>
#include <string>
#include <sqlite3.h>
/**
 * @todo Adding photos to database
 * @todo Compatibility check ?
*/
using std::vector;
using std::string;

typedef vector< vector <string> > ResultTable;

class DBConnector;
class SQLiteConnector;

/*! @class DBConnectorFactory
 *  Class used to provide instances of adequate concrete versions
 *  of database connectors (eg. SQLiteConnector)
*/
class DBConnectorFactory {
public:
  static DBConnector* getInstance(const char *type);
};


/*! @interface DBConnector
 *  Interface providing functions for communication with databases.
*/
class DBConnector {
public:
  virtual bool open(const char *filename) = 0;
  virtual ResultTable sendQuery(char *query) = 0;
  virtual void close() = 0;

/*! @fn virtual int sendQuery(const char*) = 0;
   *  @brief For testing purposes.
   *  @warning Should be commented or removed in final version!
   *  @returns the result of the query
  */
};
////////////////////////////////////////////////////////////////////////
//Concrete versions of DBConnector Interface
////////////////////////////////////////////////////////////////////////
/*! @class SQLiteConnector
  * @implements DBConnector
  * @brief Concrete version of DBConnector. Uses SQLite Database.
  *
  * Must declare DBConnectorFactory as a friend. Must provide
  * getInstance function which gives access to the database
  * (every concrete connector is implemented as a Singleton)
*/
class SQLiteConnector : public DBConnector{
  friend class DBConnectorFactory;
public:
  ResultTable sendQuery(char *query);
  bool open(const char *filename);
  void close();
private:
  sqlite3 *database;
  static DBConnector *instance;
  static DBConnector* getInstance();
};


