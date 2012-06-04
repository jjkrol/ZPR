/**
 * @file dbConnector.hpp
 * @brief File containing definition of an DB connectors
 * @author Jacek Witkowski
 * @version 0.03
 * Basicly there are 3 types of classes defined in this file:
 * - DBConnector which is an interface for all connectors used
 *   to communicate with diffrent types of databases (eg.SQLite DB)
 * - DBConnectorFactory which is used to create DBConnectors
 *   (it ensures that there is no more than one object of each type
 *   of database connector
 * - concrete versions of DBConnector (eg. SQLiteConnector)
*/
#pragma once

#include <vector>
#include <set>
#include <string>
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include "disk.hpp"

class Disk;

class DBConnector;
class SQLiteConnector;

/*! @class DBConnectorFactory
  * Class used to provide instances of adequate concrete versions
  * of database connectors (eg. SQLiteConnector)
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
  /// @brief Flags that can be returned as a result of opening database
  enum Flags {
    FAILURE = 0x0,
    OPENED  = 0x1,
    CREATED = 0x2,
    CLOSED  = 0x4
  };

  /// @brief   opens connection with database and creates necessary structures.
  /// @param   filename   name of a database file
  /// @returns flags defined in DBConnector::Flags
  virtual int open(const std::string filename) = 0;
  
  /// @brief  closes the connection with database
  /// @return one of flags defined in Flags enum
  virtual int close() = 0;
  
  /// @brief  checks disk structure of scanned directories, deletes and adds
  ///         photos to database to synchronise it with a disk content
  /// @return true when database updated successfully
  virtual bool update() = 0; //TODO
  
  /// @brief  checks if all of the photos from the database exist on disk
  /// @return true if all photos from database exist on disk
  virtual bool checkCompatibility() const = 0;
  
  /// @brief  checks if database is empty
  /// @return true if the are no directories in database
  virtual bool isEmpty() const = 0;
  
  /// @brief  adds photos from specified directories to database
  /// @param  dirs   directories that should be added to database
  /// @return true if all photos added successfully
  virtual bool addPhotosFromDirectories(
    const std::vector<boost::filesystem::path> &dirs) = 0;

  /// @brief  adds photo from directory of specified path
  /// @param  dir  path of directory from which photos should be added
  /// @return true if photos where added successfully
  virtual bool addPhotosFromDirectory(const boost::filesystem::path &dir) = 0;
  
  /// @brief  adds the photo of specified path
  /// @param  photo   photo which should be added to database
  /// @return true if photo added successfully
  virtual bool addPhoto(const boost::filesystem::path &photo) = 0;
  
  /// @brief  deletes photo of specified path from database
  /// @param  photos_path   path of photo which should be deleted
  /// @return true if photo deleted successfully
  virtual bool deletePhoto(const boost::filesystem::path &photos_path) = 0;
  
  /// @brief  deletes directory with all subdirectories and contained photos
  /// @param  dir   path of directory that should be deleted
  /// @return true if directory deleted successfully
  virtual bool deleteDirectory(const boost::filesystem::path &dir) = 0;
  
  /// @brief  deletes specified directories (and their content recursively)
  /// @param  dirs  directories that should be deleted
  /// @return true if directories deleted successfully
  virtual bool deleteDirectories(
    const std::vector<boost::filesystem::path> &dirs) = 0;
  
  /// @brief adds tag to photo of specified path
  /// @param photo   path of photo to which tag should be added
  /// @param tag     text tag that should be added to photo
  virtual bool addTagToPhoto(
    const boost::filesystem::path &photo, const std::string &tag) = 0;
  
  /// @brief  adds tags to photo of specified path
  /// @param  photo  path of photo to which tags should be added
  /// @param  tags   set of tags which should be added to the photo
  /// @return true if tags added successfully
  virtual bool addTagsToPhoto(
    const boost::filesystem::path &photo,
    const std::set<std::string> &tags) = 0;
  
  /// @brief  delete tags from photo of specified path
  /// @param  photo  path of photo from which tags should be added
  /// @param  tags   set of tags that should be deleted from photo
  /// @return true if tags deleted successfully
  virtual bool deleteTagsFromPhoto(
    const boost::filesystem::path &photo,
    const std::set<std::string> &tags) = 0;
  
  /// @brief  deletes tag from photo of specified tag
  /// @param  photo path of photo from which tag should be deleted
  /// @param  tag   tag which should be deleted from the photo
  /// @return true if tag deleted successfully
  virtual bool deleteTagFromPhoto(
    const boost::filesystem::path &photo, const std::string &tag) = 0;
  
  /// @brief      gets photos which are tagged with all specified tags
  /// @param[in]  tags   tags that returned photos should contain
  /// @param[out] photos container which will be filled with photos 
  /// @return     true if getting photos having all specified tags run
  ///             successfully
  virtual bool getPhotosWithTags(
    const std::set<std::string> &tags,
    std::vector<boost::filesystem::path> &photos) = 0;
  
  /// @brief      gets tags assigned to photo of specified path
  /// @param[in]  photo  path of a photo from which tags should be taken
  /// @param[out] tags   container that will be filled with photo's tags
  /// @return     true if getting photo's tags ended successfully
  virtual bool getPhotosTags(
    const boost::filesystem::path &photo,
    std::set<std::string> &tags) = 0;
  
  /// @brief      gets all tags stored in the database
  /// @param[out] tags  container that should be filled with tags from database
  /// @return     true if getting tags ended successfully
  virtual bool getAllTags( std::set<std::string> &tags) = 0;
  
  /// @brief      gets paths of all directories stored in a database
  /// @param[out] dirs  container for path of directories stored in database
  /// @return     true if getting directories from database ended successfully
  virtual bool getDirectoriesFromDB(
    std::vector<boost::filesystem::path> &dirs) const = 0;
  
  /// @brief      gets paths of all photos from database contained
  ///             in the directory of specified path
  /// @param[in]  directory from which photos should be taken
  /// @param[out] container that should be filled with paths of photos
  virtual bool getPhotosFromDirectory(
    const boost::filesystem::path &directory,
    std::vector<boost::filesystem::path> &photos) const = 0; //tested

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
  * getInstance function which gives an access to the database
  * (every concrete connector is implemented as a Singleton)
*/
class SQLiteConnector : public DBConnector {
  friend class DBConnectorFactory;

public:
  //basic operations on database
  virtual int open(const std::string filename);
  virtual int close();
  virtual bool update();
  virtual bool checkCompatibility() const;
  virtual bool isEmpty() const;

  //adding photos and directories
  virtual bool addPhotosFromDirectories(
    const std::vector<boost::filesystem::path> &dirs);
  virtual bool addPhotosFromDirectory(const boost::filesystem::path &dir);
  virtual bool addPhoto(const boost::filesystem::path &photo);

  //deleting photos and directories
  virtual bool deletePhoto(const boost::filesystem::path &photos_path);
  virtual bool deleteDirectories(
    const std::vector<boost::filesystem::path> &dirs);
  virtual bool deleteDirectory(const boost::filesystem::path &dir);

  //getting photos and directories
  virtual bool getPhotosFromDirectory(
    const boost::filesystem::path &directory, 
    std::vector<boost::filesystem::path> &photos) const;
  virtual bool getDirectoriesFromDB(
    std::vector<boost::filesystem::path> &dirs) const;

  //adding tags
  virtual bool addTagsToPhoto(const boost::filesystem::path &photo,
    const std::set<std::string> &tags);
  virtual bool addTagToPhoto(
    const boost::filesystem::path &photo,
    const std::string &tag);

  //deleting tags
  virtual bool deleteTagFromPhoto(
    const boost::filesystem::path &, const std::string &);
  virtual bool deleteTagsFromPhoto(
    const boost::filesystem::path &photo, const std::set<std::string> &tags);

  //getting tags and photos taged with specified tags
  virtual bool getPhotosTags(
    const boost::filesystem::path &photo,
    std::set<std::string> &tags_output);
  virtual bool getAllTags( std::set<std::string> &tags_output);
  virtual bool getPhotosWithTags(
    const std::set<std::string> &tags,
    std::vector<boost::filesystem::path> &photos_output);

private:
  SQLiteConnector(){
    database = 0;
    disk = Disk::getInstance();
  };

  Disk * disk;

  /*!
   *  @fn bool createDB();
   *  @brief creates tables used in the database
   *  @returns a true value if tables has been created successfully
   *
   *  @fn inline bool reportErrors(const char *query) const;
   *  @brief Takes errors from the database and puts them in std::cerr stream
   *  @returns true if there were some errors and false if there weren't any
  */

  /// @brief holds a pointer to an opened database
  sqlite3 *database;

  /// holds a pointer to the only existing SQLiteConnector
  static DBConnector *instance;

  /// @brief  gives and access or creates the SQLiteConnector. Is private because
  ///         only the DBFactory (declared as a friend) should be able to run
  ///         this method
  /// @return a pointer to the instance of SQLiteConnector
  static DBConnector * getInstance();

  /// @brief  adds directory to directories table but included photos
  ///         are not added to photos table
  /// @param  dir  path od directory which should be added to database
  /// @return true if directory was successfully added to directories table
  bool addDirectoryToDB(const boost::filesystem::path &dir);

  /// @brief  creates all necessary tables
  /// @return true if tables created successfully
  bool createDB();

  /// @brief      gets subdirectories contained in the database for the directory
  ///             (also contained in the database)
  /// @param[in]  dir     path of directory whose subdirectories should be taken
  /// @param[out] subdirs container for subdirectories that will be filled
  ///                     with paths
  /// @return     true if subdirectories where got successfully
  bool getSubdirectoriesFromDB(
    const boost::filesystem::path &dir,
    std::vector<boost::filesystem::path> &subdirs) const;

  /// @brief      gets paths of all photos contained in the database
  /// @param[out] photos  container for paths of photos contained in database
  /// @return     true if photos' paths were got successfully
  bool getPhotosFromDB(std::vector<boost::filesystem::path> &photos) const;

  /// @brief  executes a simple query which returns no results
  /// @param  query  stores a content of a query
  /// @return true if query was executed successfully
  bool executeQuery(std::string query) const;

  /// @brief  reports errors which appeared after the last database operation
  ///         or does nothing if there were no errors
  /// @return true if there were some errors and false otherwise
  bool reportErrors(std::string query) const;
};
