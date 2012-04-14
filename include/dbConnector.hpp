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
struct DirectoriesPath;

typedef std::vector< std::vector <std::string> > ResultTable;
typedef DBConnector*(*Creator)(void);
typedef boost::filesystem::path PhotoPath;

class DBConnectorFactory {
public:
  static DBConnector* getInstance(std::string type);

};

/*! @interface DBConnector
 *  Interface providing functions for communication with databases.
*/
class DBConnector {
public:
/*! @fn virtual int open(const string filename) = 0;
 *  @brief Opens connection with database and creates necessary
 *  structures.
 *  @returns flags defined in DBConnector::Flags
 *
 *  @fn virtual virtual void close() = 0;
 *  @brief closes the connection with database
 * 
 *  @fn virtual inline bool hasChanged() const = 0;
 *  @brief Tells if database was changed since last usage of imgview
 *  @returns true if database has changed 
 *  
 *  @fn virtual bool addPhotosFromDirectories(
 *        const std::vector<DirectoriesPath> &dirs
 *      ) = 0;
 *  @brief adds photos from the directories included in the vector
 *  (recursively).
 *  @returns true value when run successfully and false value otherwise
 *
 *  @fn virtual bool addPhoto(const PhotoPath &photo) = 0;
 *  @brief adds a single photo to database
 *  @returns a true value when run successfully and false otherwise
 *
 *  @fn virtual bool movePhoto(
 *        const PhotoPath &old_path,
 *        const PhotoPath &new_path) = 0;
 *  @brief moves photo from old path to new path
 *  @returns true if photo has been moved successfully and false otherwise
 *
 *  @fn virtual bool deletePhoto(const PhotoPath &photos_path) = 0;
 *  @brief removes photo from database
 *  @returns true if photo has been removed successfully and false otherwise
 *
 *  @fn virtual bool addTags(
 *        const PhotoPath &photo, const std::vector<std::string> &tags) = 0;
 *  @brief add tags to photo specified in the first argument.
 *  @returns true value if run successfully and false value otherwise
 *
 *  @fn virtual bool getPhotosWithTags(
 *        const std::vector<std::string> &tags,
 *        std::vector<PhotoPath> &photos_output) = 0;
 *  @brief takes tags specified in a first argument and puts photos
 *  that are tagged with all specified tags in result vector: photos_output
 *  @returns true value when run successfully and false value otherwise
 *
 *  @fn virtual bool getPhotosTags(
 *        const PhotoPath &photo, std::vector<std::string> &tags_output) = 0;
 *  @brief takes a path of photo and puts all corresponding tags in a vector
 *  specified in a second argument (tags_output)
 *  @returns a true value when run successfully and a false value otherwise
*/
  enum Flags {
    FAILURE = 0x0,
    OPENED  = 0x1,
    CREATED = 0x2,
    CLOSED  = 0x4
  };

  virtual int open(const std::string filename) = 0;
  virtual int close() = 0;
  virtual inline bool hasChanged() const = 0;
  virtual bool isEmpty() = 0;

  virtual bool addPhotosFromDirectories(
    const std::vector<DirectoriesPath> &dirs) = 0;
  virtual bool addPhoto(const PhotoPath &photo) = 0;

  virtual bool movePhoto(
    const PhotoPath &old_path, const PhotoPath &new_path) = 0;
  virtual bool deletePhoto(const PhotoPath &photos_path) = 0;
  virtual bool addTags(
    const PhotoPath &photo, const std::vector<std::string> &tags) = 0;
  virtual bool getPhotosWithTags(
    const std::vector<std::string> &tags,
    std::vector<PhotoPath> &photos_output) = 0;
  virtual bool getPhotosTags(
    const PhotoPath &photo, std::vector<std::string> &tags_output) = 0;

protected:
  virtual ~DBConnector(){};
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
  int open(const std::string filename);
  int close();
  inline bool hasChanged() const;
  bool isEmpty();

  //bool addPhotosFromDirectories(
  //  const boost::filesystem::path &main_dir,
  //  const std::vector<boost::filesystem::path> &excluded_dirs);
  bool addPhotosFromDirectories(
    const std::vector<DirectoriesPath> &dirs);
  bool addPhoto(const PhotoPath &photo);
  bool movePhoto(
    const PhotoPath &old_path,
    const PhotoPath &new_path);
  bool deletePhoto(const PhotoPath &photos_path);
  bool addTags(const PhotoPath &photo, const std::vector<std::string> &tags);
  bool getPhotosWithTags(
    const std::vector<std::string> &tags,
    std::vector<PhotoPath> &photos_output);
  bool getPhotosTags(
    const PhotoPath &photo, std::vector<std::string> &tags_output);

private:
  SQLiteConnector(){
    database = 0;
  }; //private constructor is a part of singleton pattern

  /*! @var sqlite3 *database;
   *  @brief holds a pointer to opened database
   *
   *  @var std::string filename;
   *  @brief holds a name of database
   *
   *  @var static DBConnector *instance;
   *  @brief holds a pointer to the only existing SQLiteConnector
   *
   *  @fn static DBConnector * getInstance();
   *  @brief Gives access or creates the SQLiteConnector. Is private because
   *  only the DBFactory should be able to run this method
   *  @returns a pointer to the instance of SQLiteConnector
   *
   *  @fn bool createDB();
   *  @brief creates tables used in the database
   *  @returns a true value if tables has been created successfully
   *
   *  @fn bool saveSettings();
   *  @brief Saves every single setting in a database. Actually the only
   *  setting hold in a database is a checksum, so nothing more is saved.
   *
   *  @fn bool getDirectoriesFromDB(
   *        boost::filesystem::path &main_directory,
   *        std::vector<boost::filesystem::path> &dirs) const;
   *  @brief Gets a main directory from the database and the vector
   *  of directories that should be excluded form the database.
   *  @returns true if was executed successfully and false otherwise
   *
   *  @fn bool getChecksumFromDB(int &checksum) const;
   *  @brief gets a checksum saved in database
   *  @returns true if executed successfully and false otherwise
   *
   *  @fn int calculateChecksum(
   *        const boost::filesystem::path main_path
   *        const std::vector<boost::filesystem::path> &dir_paths) const;
   *  @brief Takes a path of main directory and a vector of directories that
   *  should be excluded.
   *  @returns a checksum
   *
   *  @fn inline bool reportErrors(const char *query) const;
   *  @brief Takes errors from the database and puts them in std::cerr stream
   *  @returns true if there were some errors and false if there weren't any
  */

  sqlite3 *database;

  static DBConnector *instance;
  static DBConnector * getInstance();

  bool addPhotosFromDirectory(const DirectoriesPath &dir);
  bool addDirectoryToDB(const DirectoriesPath &dir);

  bool createDB();
  bool saveSettings();

  bool getDirectoriesFromDB(std::vector<DirectoriesPath> &dirs) const;
  bool getChecksumFromDB(int &checksum) const;
  bool addTag(const PhotoPath &photo, const std::string &tag);

  int calculateChecksum() const;
  inline bool reportErrors(const char *query) const;
};
