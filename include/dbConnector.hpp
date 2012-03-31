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

/*! @typedef typedef std::vector< std::vector <std::string> > ResultTable;
  * ResultTable is a type of objects storing results of queries.
  * Each line of a result is represented by a vector of strings.
  * Results may be composed of more than a line, that's why
  * a ResultTable is of type vector<vector<string> >
  *
  * @typedef typedef DBConnector* (*Creator)();
  * Creator is a static function called by the DBConnectorFactory
  * to give access to different DBConnectors
  *
  * @class DBConnectorFactory
  * Class used to provide instances of adequate concrete versions
  * of database connectors (eg. SQLiteConnector)
*/
typedef std::vector< std::vector <std::string> > ResultTable;
typedef DBConnector*(*Creator)(void);

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

  virtual ResultTable sendQuery(std::string query) = 0;
  virtual int open(const std::string filename) = 0;
  virtual inline bool hasChanged() = 0;
  virtual inline void addDirectories(
    const std::vector<boost::filesystem::path> &directories) = 0;
  virtual bool addPhotos(
    const std::vector<boost::filesystem::path> &photos) = 0;
  virtual void close() = 0;

  virtual bool movePhoto(
    boost::filesystem::path old_path, boost::filesystem::path new_path) = 0;
  virtual bool deletePhoto(boost::filesystem::path photos_path) = 0;

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
class SQLiteConnector : public DBConnector {
  friend class DBConnectorFactory;

public:
/*! @fn ResultTable sendQuery(std::string query);
  * @brief Function used for test. Allows to send static queries (without
  * variables) to database.
  * @returns the vector of vectors which represents the result of the query
  *
  * @fn int open(const std::string filename)
  * @brief Opens a database stored in a file of name specified in the function's
  * argument. It creates a new DB if it doesn't exist.
  * @returns falgs defined in DBConnector::Flags
  *
*/
  ResultTable sendQuery(std::string query);

  int open(const std::string filename);
  inline bool hasChanged();
  inline void addDirectories(
    const std::vector<boost::filesystem::path> &directories);
  bool addPhotos(const std::vector<boost::filesystem::path> &photos);
  bool addPhoto(const boost::filesystem::path &photo);
  void close();

  bool movePhoto(
    boost::filesystem::path old_path, boost::filesystem::path new_path);
  bool deletePhoto(boost::filesystem::path photos_path);

private:
  sqlite3 *database;
  std::string filename;
  std::vector<boost::filesystem::path> directories;

  static DBConnector *instance;
  static DBConnector * getInstance();

  bool createDB();

  bool saveSettings();
  bool saveDirectories();

  bool getDirectoriesFromDB();
  bool getChecksumFromDB(int &checksum);

  int calculateChecksum();
  inline bool reportErrors(const char *query);
};
