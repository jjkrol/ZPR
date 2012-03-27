/**
 * @file dbConnector.hpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.01
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
#include <boost/filesystem.hpp>
#include <sqlite3.h>

/**
 * @todo Adding photos to database
 * @todo Removing photos from database
*/
class Disk;

class DBConnector;
class SQLiteConnector;

using std::vector;
using std::string;

/** @typedef typedef vector< vector <string> > ResultTable;
  * ResultTable is a type of objects storing results of queries.
  * Each line of a result is represented by a vector of strings.
  * Results may be composed of more than a line, that's why
  * a ResultTable is of type vector<vector<string> >
  *
  * @typedef typedef DBConnector* (*Creator)();
  * Creator is a static function called by the DBConnectorFactory
  * to give access to different DBConnectors
*/
typedef vector< vector <string> > ResultTable;
typedef DBConnector*(*Creator)(void);

/*! @class DBConnectorFactory
 *  Class used to provide instances of adequate concrete versions
 *  of database connectors (eg. SQLiteConnector)
*/
class DBConnectorFactory {
public:
  static DBConnector* getInstance(std::string type);

};

/*! @interface DBConnector
 *  Interface providing functions for communication with databases.
*/
class DBConnector {
public:
/*! @fn virtual ResultTable sendQuery(string query) = 0;
 *  @brief For testing purposes.
 *  @warning Should be commented or removed in final version!
 *  @returns the result of the query
 *
 *  @fn virtual bool open(const string filename) = 0;
 *  @brief Opens connection with database and creates necessary
 *  structures.
 *  @returns `true` when database openned succesfully and `false`
 *  otherwise.
*/
  enum Flags {
    FAILURE = 0x0,
    OPENED  = 0x1,
    CREATED = 0x2,
    CHANGED = 0x4
  };

  virtual int open(const std::string filename) = 0;
  virtual ResultTable sendQuery(std::string query) = 0;
  virtual void close() = 0;

protected:
  virtual ~DBConnector(){ }
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
  ResultTable sendQuery(string query);
  int open(const string filename);
  void close();
  bool addPhoto(boost::filesystem::path photo);
private:
  static DBConnector *instance;
  static DBConnector * getInstance();

  sqlite3 *database;

  unsigned int checksum;
  string filename;

  bool hasChanged(boost::filesystem::path directory);
  inline void createDB();
  bool loadDB(std::vector<boost::filesystem::path> directories);
};
