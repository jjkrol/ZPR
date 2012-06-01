/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.03
*/

#include <iostream> //for testing end error reporting

#include "../include/dbConnector.hpp"
#include "../include/disk.hpp"
#include "../include/hashFunctions.hpp"

using namespace boost::filesystem;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::set;

/** @todo Concrete versions of DBConnector should invoke the Register()
  * function to register their types in the register of types stored
  * in the  DBConnectorGenerator object (the register should be static).
  * Then when the DBConnectorFactory::getInstance(type) is called,
  * getInstance() should look for the pointer to adequate getInstance()
  * function in its register and should call it.
*/

////////////////////////////////////////////////////////////////////////////////
//Definitions of DBConnectorFactory methods
////////////////////////////////////////////////////////////////////////////////
DBConnector* DBConnectorFactory::getInstance(const string type) {
  /** @todo Factory should check the register of creators andn return the
   * desired one.
   */
  if(type != "SQLiteConnector")
    cout << "Proszę sobie nie żartować"<<endl;

  return SQLiteConnector::getInstance();
}

////////////////////////////////////////////////////////////////////////////////
//Definitions of SQLiteConnector methods
////////////////////////////////////////////////////////////////////////////////
/** @warning Don't forget to initialize 'instance' pointer to 0
  * in every concrete version of DBConnector
*/
DBConnector* SQLiteConnector::instance = 0;
DBConnector* SQLiteConnector::getInstance() {
  if(instance == 0)
    instance = new SQLiteConnector;  

  return instance;
}

////////////////////////////////////////////////////////////////////////////////
//Methods for maintainig a database: opening, closing, checking if is empty,
//creating, saving settings, checking if has since last opening with imagine,
//saving and loading checksum
////////////////////////////////////////////////////////////////////////////////

int SQLiteConnector::open(const string filename) {
  //return a FAILURE flag if the database is opened or a filename is empty
  if(filename.empty() || database)
    return FAILURE;

  //open a database, if it exists
  if(disk->absoluteExists(filename)) {
    if(sqlite3_open(filename.c_str(), &database) == SQLITE_OK) {
      return OPENED;
    }
    return FAILURE;
  }

  //create a new database, if it doesn't exist
  else if(sqlite3_open(filename.c_str(),&database) == SQLITE_OK) {
    if(createDB())
      return (OPENED | CREATED);
  }

  //return a FAILURE if nothig worked
  return FAILURE;
}

int SQLiteConnector::close() {
  //return a FAILURE flag if a connection is closed or you can't save settings
  if(! (database /*&& saveSettings()*/) )
    return FAILURE;

  sqlite3_close(database);
  database = 0;

  return CLOSED;
}

bool SQLiteConnector::hasChanged() const{
  int originalChecksum;

  //try to get checksum from a database
  if(! getChecksumFromDB(originalChecksum)) {
    cout<<"SQLiteConnector failed to get checksum from DB" <<endl;
    return true;
  }

  //calculate a checksum and comapre it with the checksum stored in a database
  return (originalChecksum != calculateChecksum());
}

bool SQLiteConnector::checkCompatibility() const{
  vector<path> photos;
  if(!getPhotosFromDB(photos))
    return false;

  for(vector<path>::const_iterator i = photos.begin() ;
      i != photos.end() ; ++i) {
    if(!disk->exists(*i))
      return false;
  }

  return true;
}
bool SQLiteConnector::getPhotosFromDB(vector<path> &photos) const{
  const char *query = "SELECT path FROM photos;";
  sqlite3_stmt *stmt;

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return false;
  }

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    photos.push_back(*static_cast<const path *>(sqlite3_column_blob(stmt,1)));
  }

  sqlite3_finalize(stmt);
  return !reportErrors(query);
}

bool SQLiteConnector::isEmpty() const{
  //TODO Getting all directories from the database is not necessary.
  //Only one directory existing in the database is enough
  vector<DirectoriesPath> dirs;
  getDirectoriesFromDB(dirs);

  return (dirs.empty());
}

bool SQLiteConnector::createDB() {
  const char *query = 
      "CREATE TABLE photos ("
        "id INTEGER PRIMARY KEY,"
        "path BLOB UNIQUE,"
        "parents_id INTEGER"
      ");"

      "CREATE TABLE directories ("
        "id INTEGER PRIMARY KEY,"
        "path BLOB UNIQUE,"
        "parents_id INT"
      ");"

      "CREATE TABLE settings ("
        "checksum INTEGER"
      ");"

      "CREATE TABLE tags ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT UNIQUE"
      ");"

      "CREATE TABLE photos_tags ("
        "photos_id INTEGER,"
        "tags_id INTEGER,"
        "FOREIGN KEY(photos_id) REFERENCES photos(id),"
        "FOREIGN KEY(tags_id) REFERENCES tags(id)"
        "PRIMARY KEY(photos_id, tags_id)"
      ");";

  if(sqlite3_exec(database, query, NULL, NULL, NULL) == SQLITE_OK)
    return true;

  reportErrors(query);
  return false;
}

bool SQLiteConnector::saveSettings() {
  //@todo is it ok to delete from an empty table?
  string query = "DELETE FROM settings;";
  if(sqlite3_exec(database, query.c_str(), NULL, NULL, NULL) != SQLITE_OK)
    return !reportErrors(query.c_str());
 
  query = "INSERT INTO settings VALUES (0);"; //quick fix
  //@todo how to make a string from an integer
  //query = "INSERT INTO settings VALUES (" + calculateChecksum()
  //        + string(");") ;

  if(sqlite3_exec(database, query.c_str(), NULL, NULL, NULL) == SQLITE_OK)
    return true;
    
  return !reportErrors(query.c_str());
}

bool SQLiteConnector::getChecksumFromDB(int &checksum) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT checksum FROM settings;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  if(sqlite3_step(stmt) != SQLITE_ROW)
    return false;
  
  const void *blob = sqlite3_column_blob(stmt,0);
  checksum = *(static_cast<const int *>(blob));

  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

int SQLiteConnector::calculateChecksum() const{
  int checksum_tmp = 0;

  //vector with all directories stored in the database (even nested ones)
  vector<DirectoriesPath> directories;
  if(!getDirectoriesFromDB(directories))
    return -1;
  
  //take every directory form database
  for(vector<DirectoriesPath>::iterator i = directories.begin();
      i != directories.end(); ++i) {
  
    //add make a hash from every photo from directory to checksum
    vector<path> photo_paths = disk->getPhotosPaths(*i);
    for(vector<path>::iterator j = photo_paths.begin();
        j != photo_paths.end() ; j++) {
      checksum_tmp += hash(j->string().c_str());
    }
  }

  return checksum_tmp;
}
////////////////////////////////////////////////////////////////////////////////
//Methods for adding and removing photos and directories
////////////////////////////////////////////////////////////////////////////////

bool SQLiteConnector::addPhotosFromDirectories(
const vector<path> &dirs) {
  //add photos from each directory from database
  for(vector<path>::const_iterator i = dirs.begin();
      i != dirs.end() ; ++i) {
    //in case of failure in adding photos from directory, stop adding photos
    if(! addPhotosFromDirectory(*i) )
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteDirectories(const vector<DirectoriesPath> &dirs) {
  //delete each directory from the vector
  for(vector<DirectoriesPath>::const_iterator i = dirs.begin() ;
      i != dirs.end() ; ++i) {
    //in case of failure in deleting directories, stop
    if(!deleteDirectory(*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteDirectory(const DirectoriesPath &dir) {
  //1. Delete photos which are directly in this directory.
  //2. Get all subdirectories from the DB.
  //3. Call deleteDirectory for each subdirecory.
  //4. Delete directory from directories table.

  //deleting photos from this folder
  sqlite3_stmt *stmt;
  const char *query = "DELETE FROM photos WHERE parent_dir"
                      "IN (SELECT id FROM directories WHERE path=?);";
  
  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &dir, sizeof(path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  //getting subdirectories of directory which is being deleted
  vector<DirectoriesPath> subdirs;
  if( !getSubdirectoriesFromDB(dir, subdirs))
    return false;

  //deleting each of subdirectories
  for(vector<DirectoriesPath>::iterator i = subdirs.begin();
      i != subdirs.end() ; ++i) {
    if(!deleteDirectory(*i))
      return false;
  }

  //deleting directory
  query = "DELETE FROM directories WHERE path=? ;";
  stmt=0;

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &(dir), sizeof(path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return true;
}

bool SQLiteConnector::addPhotosFromDirectory(const DirectoriesPath &dir){

  //add directory's path to database
  if(!addDirectoryToDB(dir))
    return false;

  //add photos which are located directly in this directory
  {
    //get photos which are located in this directory
    vector<PhotoPath> photos = disk->getPhotosPaths(dir);

    //add each photo to database
    for(vector<PhotoPath>::const_iterator i = photos.begin();
        i != photos.end() ; ++i) {
      if(! addPhoto(*i))
        return false;
    }
  }

  return true;
}

bool SQLiteConnector::addDirectoryToDB(const DirectoriesPath &dir) {
  //add subidirectory with values: autonum, dir.path, id_of_parental_directory 
  sqlite3_stmt *stmt;
  const char *query =
    "IF EXISTS"
      "(SELECT NULL, dir.path, directories.id FROM directories "
      " WHERE path = dir.path.parent_path()) "
    "THEN "
      "INSERT INTO directories "
      "SELECT NULL, dir.path, directories.id FROM directories "
      "WHERE path = dir.path.parent_path() "
    "ELSE "
      "INSERT INTO directories VALUES(NULL, dir.path, NULL); ";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &dir, sizeof(path), SQLITE_STATIC);
  path parent = dir.parent_path();
  sqlite3_bind_blob(stmt, 2, &parent, sizeof(path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !(reportErrors(query));
}

//TODO
bool SQLiteConnector::movePhoto(const path &old_path, const path &new_path) {
  return false;
}

bool SQLiteConnector::deletePhoto(const path &photos_path) {
  sqlite3_stmt *stmt;

  //Firstly, connection between the photo and corresponding tags is deleted
  string query = "DELETE FROM photos_tags WHERE photos_id IN ("
                 "SELECT id FROM photos WHERE path=? )";

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &photos_path, sizeof(photos_path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if(reportErrors(query.c_str()))
    return false;

  //Secondly, photo should be deleted from the table of photos
  query = "DELETE FROM photos WHERE path=? ;";
  stmt = 0;

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &photos_path, sizeof(photos_path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !reportErrors(query.c_str());
}

bool SQLiteConnector::addPhoto(const path &photo) {
  //inserting NULL as an id value is used for autoincrementing id numbers
  //add a photo with an appropriate parent's id
  const char *query = "INSERT INTO photos"
                      "SELECT NULL, ?, directories.id FROM directories"
                      "WHERE path=?;";
  path parent = photo.parent_path();
  sqlite3_stmt *stmt;

  if(( sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK)
    return !reportErrors(query);

  sqlite3_bind_blob(stmt, 1, &photo, sizeof(photo), SQLITE_STATIC );
  sqlite3_bind_blob(stmt, 2, &parent, sizeof(parent), SQLITE_STATIC );
  sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::getSubdirectoriesFromDB(
const DirectoriesPath &dir, vector<DirectoriesPath> &directories) const {

  const char *query = "SELECT path FROM directories WHERE parents_id"
                      "IN (SELECT id FROM directories WHERE path = ?);";
  sqlite3_stmt *stmt;

  if(( sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK)
    return !reportErrors(query);

  sqlite3_bind_blob(stmt, 1, &dir, sizeof(path), SQLITE_STATIC );

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    const void *blob = sqlite3_column_blob(stmt,0);
    directories.push_back( *static_cast<const path *>(blob) );
  }

  sqlite3_finalize(stmt);

  return !reportErrors(query);
 
}

bool SQLiteConnector::getDirectoriesFromDB (
vector<DirectoriesPath> &directories) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT path FROM directories;";
  directories.clear();

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return !reportErrors(query);

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    const void *blob = sqlite3_column_blob(stmt,0);
    directories.push_back( *static_cast<const path *>(blob) );
  }
  
  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

////////////////////////////////////////////////////////////////////////////////
//Other methods: reporting errors
////////////////////////////////////////////////////////////////////////////////

bool SQLiteConnector::reportErrors(const char * query) const {
  string error = sqlite3_errmsg(database);

  if(error != "not an error"){
    cout << query << " " << error << std::endl;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
//Methods for managing tags: adding, removing, geting photos with a set of tags,
//getting tags of a photo, getting all tags stored in a database
////////////////////////////////////////////////////////////////////////////////

bool SQLiteConnector::addTagsToPhoto(
const PhotoPath &photo, const set<string> &tags) {
  for(set<string>::const_iterator i = tags.begin() ; i != tags.end() ; ++i) {
    if(!addTagToPhoto(photo,*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::addTagToPhoto(const PhotoPath &photo, const string &tag) {
  sqlite3_stmt *stmt;

  //inserting a tag into a tags table if a tag has not existed before
  string query = "IF NOT EXISTS (SELECT * FROM tags WHERE name='" + tag + "') "
                  "THEN INSERT INTO tags VALUES(NULL,'" + tag + "') ;";

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK)
    return false;

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if(reportErrors(query.c_str()))
    return false;

  //making a connection between the tag and a corresponding photo
  query = "INSERT INTO photos_tags "
          "SELECT photos.id, tags.id FROM photos, tags "
          "WHERE photos.path = ? AND tags.name = '" + tag  + "';";

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK)
    return false;
    
  sqlite3_bind_blob(stmt, 1, &photo, sizeof(path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !reportErrors(query.c_str());
}

bool SQLiteConnector::getPhotosTags(
const PhotoPath &photo, set<string> &tags) {
  //Get id of a given photo. Then get ids of tags connected with it.
  //Finally, get names of those tags.
  sqlite3_stmt *stmt;
  const char *query = "SELECT name FROM tags WHERE tags.id IN ("
                        "SELECT tags_id FROM photos_tags WHERE photos_id IN ("
                          "SELECT id FROM photos WHERE path = ?"
                        ")"
                      ");";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK)
    return false;
    
  sqlite3_bind_blob(stmt, 1, &photo, sizeof(path), SQLITE_STATIC);
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    const void *blob = sqlite3_column_text(stmt,0);
    tags.insert(* static_cast<const string *>(blob));
  }
  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::deleteTagsFromPhoto(
const PhotoPath &photo, const std::set<std::string> &tags) {
  for(std::set<std::string>::const_iterator i = tags.begin() ;
      i != tags.end() ; ++i) {
    if(!deleteTagFromPhoto(photo,*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteTagFromPhoto(
const PhotoPath &photo, const string &tag) {
  string query = "DELETE FROM photos_tags WHERE tags_id IN ("
                    "SELECT id FROM tags WHERE name=" + tag +
                  ");"

                 "IF NOT EXISTS ("
                   "SELECT * FROM photos_tags WHERE tags_id IN ("
                     "SELECT id FROM tags WHERE name=?"
                   ")"
                 ")"
                 "THEN "
                   "DELETE FROM tags WHERE name=" + tag + " ;";

  if(sqlite3_exec(database, query.c_str(), NULL, NULL, NULL) == SQLITE_OK)
    return true;

  reportErrors(query.c_str());
  return false;
}
bool SQLiteConnector::getPhotosWithTags(
const set<string> &tags, std::vector<PhotoPath> &photos) {
//  SELECT path FROM photos
//    INNER JOIN photos_tags
//      ON photos.id = photos_tags.photos_id
//    INNER JOIN tags
//      ON photos_tags.tags_id = tags.id
//  WHERE (tags.name = 'tag1' AND tags.name = 'tag2' AND ...);
  if(tags.empty())
    return false;

  //constructing a query
  set<string>::const_iterator i = tags.begin();

  string query = "SELECT path FROM photos "
                   "INNER JOIN photos_tags "
                     "ON photos.id = photos_tags.photos_id "
                   "INNER JOIN tags "
                     "ON photos_tags.tags_id = tags.id "
                 "WHERE (tags.name = " + *(i++);

  for(; i != tags.end() ; ++i) {
    query += " AND tags.name = " + (*i);
  }
  query += ";";

  //executing a query
  sqlite3_stmt *stmt;
  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query.c_str());
    return false;
  }

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    photos.push_back( *static_cast<const path *>(sqlite3_column_blob(stmt,0)) );
  }

  sqlite3_finalize(stmt);
  return !reportErrors(query.c_str());
}

////////////////////////////////////////////////////////////////////////////////
//  Methods and functions used for testig purposes
////////////////////////////////////////////////////////////////////////////////
void SQLiteConnector::showTags(std::ostream &out) {
  sqlite3_stmt *stmt;
  const char* query = "SELECT * FROM tags;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK)
    return;

  out << "id\t| name" <<std::endl;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    out << static_cast<const int>(sqlite3_column_int(stmt,0)) << "\t  "
        << static_cast<const unsigned char *>(sqlite3_column_text(stmt,1))
        << std::endl;
  }
  sqlite3_finalize(stmt);
}

void SQLiteConnector::showPhotosTags(std::ostream &out) {
  sqlite3_stmt *stmt;
  const char* query = "SELECT * FROM photos_tags;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return;
  }

  out << "photos_id\t| tags_id" <<std::endl;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    out << static_cast<const int>(sqlite3_column_int(stmt,0)) << "\t  "
        << static_cast<const int>(sqlite3_column_int(stmt,1)) << std::endl;
  }
  sqlite3_finalize(stmt);
}

void SQLiteConnector::showPhotos(std::ostream &out) {
  sqlite3_stmt *stmt;
  const char* query = "SELECT * FROM photos;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return;
  }

  out << "id\t| path\t\t\t| parents_id " <<std::endl;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    out << static_cast<const int>(sqlite3_column_int(stmt,0)) << "\t  "
        << *static_cast<const path *>(sqlite3_column_blob(stmt,1)) << "\t\t\t  "
        << static_cast<const int>(sqlite3_column_int(stmt,2)) << std::endl;
  }
  sqlite3_finalize(stmt);
}

//funkcja testowa
void SQLiteConnector::showDirectories(std::ostream &out) {
  sqlite3_stmt *stmt;
  const char* query = "SELECT * FROM directories;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return;
  }

  out << "id\t|" << " path\t\t\t| parents_id " <<std::endl;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    out << static_cast<const int>(sqlite3_column_int(stmt,0)) << "\t  "
        << *static_cast<const path *>(sqlite3_column_blob(stmt,1)) << "\t\t\t  "
        << static_cast<const int>(sqlite3_column_int(stmt,2)) << std::endl;
  }
  sqlite3_finalize(stmt);
}
