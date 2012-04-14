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

/** @todo Concrete versions of DBConnector should
  * invoke the Register() function to register their types
  * in the register of types stored in the  DBConnectorGenerator
  * object (the register should be static). Then when
  * the DBConnectorFactory::getInstance(type) is called,
  * getInstance() should look for the pointer to adequate
  * getInstance() function in its register and should call
  * the function.
*/

////////////////////////////////////////////////////////////////////////////////
//Definitions of DBConnectorFactory methods
////////////////////////////////////////////////////////////////////////////////
DBConnector* DBConnectorFactory::getInstance(const string type) {
  /** @todo Factory should check the register of creators andn return the
   * desired one.
   */
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
//Public methods
////////////////////////////////////////////////////////////////////////////////
int SQLiteConnector::open(const string filename) {
  //if the name of the database is empty or database has been already opened
  //then the FAILURE flag should be returned at the very beginning
  if(filename.empty() || database)
    return FAILURE;

  //if the object exists, function tries to open the database
  if(Disk::exists(filename)) {
    if(sqlite3_open(filename.c_str(), &database) == SQLITE_OK)
      return OPENED;
    return FAILURE;
  }

  //if database file doesn't exist then it should be created
  else if(sqlite3_open(filename.c_str(),&database) == SQLITE_OK) {
    if(createDB())
      return (OPENED | CREATED);
    return FAILURE;      
  }

  //in case of failure in opening the database, a FAILURE value is returned
  return FAILURE;
}

int SQLiteConnector::close() {
  if(! (database && saveSettings()) )
    return FAILURE;

  sqlite3_close(database);
  database = 0;
  return CLOSED;
}

bool SQLiteConnector::hasChanged() const{
  int originalChecksum = 0;
  if(! getChecksumFromDB(originalChecksum))
    return true;
  return originalChecksum != calculateChecksum();
}

bool SQLiteConnector::isEmpty() {
  vector<DirectoriesPath> dirs;
  getDirectoriesFromDB(dirs);

  return (dirs.empty());
}

bool SQLiteConnector::addPhotosFromDirectories(
  const vector<DirectoriesPath> &dirs) {

  for(vector<DirectoriesPath>::const_iterator i = dirs.begin();
      i != dirs.end() ; ++i) {
    if(! addPhotosFromDirectory(*i) )
      return false;
  }
  return true;
}

bool SQLiteConnector::deleteDirectories(const vector<DirectoriesPath> &dirs) {
  for(vector<DirectoriesPath>::const_iterator i = dirs.begin() ;
      i != dirs.end() ; ++i) {
    if(!deleteDirectory(*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteDirectory(const DirectoriesPath &dir) {
  static Disk *disk = Disk::getInstance();

  //If necessary, all subdirectories should be removed as well (with their
  //photos ofcourse)

  if(dir.recursively) {
    vector<path> subdirectories = disk->getSubdirectoriesPaths(dir.path);
    
    for(vector<path>::iterator i = subdirectories.begin();
        i != subdirectories.end() ; ++i) {
    
      if(! deleteDirectory(DirectoriesPath(*i,true)))
        return false;
    }
  }

  //Directory should be removed from 'directories' table

  sqlite3_stmt *stmt;
  const char *query = "DELETE FROM directories WHERE path = ?";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &dir, sizeof(dir), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if(reportErrors(query))
    return false;
  
  //Finally, all which are directly in the directory should be removed

  {
    vector<PhotoPath> photos = disk->getPhotosPaths(dir.path);
    for(vector<PhotoPath>::iterator i = photos.begin() ; 
        i != photos.end() ; i++) {

      if(! deletePhoto(*i))
        return false;
    }
  }

  return true;
}

bool SQLiteConnector::addPhotosFromDirectory(const DirectoriesPath &dir){
  static Disk *disk = Disk::getInstance();
  if(dir.recursively) {
    vector<path> subdirectories = disk->getSubdirectoriesPaths(dir.path);

    for(vector<path>::iterator i = subdirectories.begin();
        i != subdirectories.end() ; ++i) {
      if(! addPhotosFromDirectory(DirectoriesPath(*i,true)))
        return false;  
    }
  }

  vector<PhotoPath> photos = disk->getPhotosPaths(dir.path);
  for(vector<PhotoPath>::const_iterator i = photos.begin();
      i != photos.end() ; ++i) {
    if(! addPhoto(*i))
      return false;
  }

  if(!addDirectoryToDB(dir))
    return false;

  return true;
}

bool SQLiteConnector::addDirectoryToDB(const DirectoriesPath &dir) {
  sqlite3_stmt *stmt;
  const char *query = "INSERT INTO directories VALUES (?);";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1, &dir, sizeof(dir), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !(reportErrors(query));
}

bool SQLiteConnector::movePhoto(const path &old_path, const path &new_path) {
  sqlite3_stmt *stmt;
  const char *query = "UPDATE photos SET path=? WHERE path=? ;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1, &old_path, sizeof(old_path), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, &new_path, sizeof(new_path), SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  return !(reportErrors(query));
}

bool SQLiteConnector::deletePhoto(const path &photos_path) {
  sqlite3_stmt *stmt;

  //Firstly, photo should be deleted from table of photos
  string query = "DELETE FROM photos WHERE path=? ;";

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK)
    return false;

  sqlite3_bind_blob(stmt, 1,&photos_path, sizeof(photos_path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if(reportErrors(query.c_str()))
    return false;

  //@todo
  //Secondly, all entries with adequate photo_ids in table linking
  //photos with corresponding tags should be removed.
  //Jak usuwać w SQLu wpisy z tabel gdzie wartość photo_id robi za klucz
  //obcy?
  /*query = "DELETE";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  */

  return !reportErrors(query.c_str());
}

////////////////////////////////////////////////////////////////////////////////
//Private methods
////////////////////////////////////////////////////////////////////////////////
bool SQLiteConnector::createDB() {
  string errmsg;
  const char *query = 
      "CREATE TABLE photos (id INTEGER PRIMARY KEY,path BLOB UNIQUE);"
      "CREATE TABLE directories (path BLOB);"
      "CREATE TABLE settings (key TEXT PRIMARY KEY, value BLOB);"
      "CREATE TABLE tags (id INTEGER PRIMARY KEY, name TEXT);"
      "CREATE TABLE photos_tags (photo_id INTEGER, tag_id INTEGER,"
        "FOREIGN KEY(photo_id) REFERENCES photos(id),"
        "FOREIGN KEY(tag_id) REFERENCES tags(id));";

  if(sqlite3_exec(database, query, NULL, NULL, NULL) == SQLITE_OK)
    return true;

  reportErrors(query);
  return false;
}

bool SQLiteConnector::saveSettings() {
  sqlite3_stmt *stmt;
  const char *query =
            "INSERT OR REPLACE INTO settings VALUES (\"checksum\",?);";
  int checksum = calculateChecksum();

  do {
    if((sqlite3_prepare_v2(database, query, -1, &stmt, 0) != SQLITE_OK))
      return false;

    sqlite3_bind_blob(stmt, 1, &checksum, sizeof(checksum), SQLITE_STATIC );
    if(sqlite3_step(stmt) == SQLITE_ROW)
      return false;
    
  } while (sqlite3_finalize(stmt) == SQLITE_SCHEMA);

  return !(reportErrors(query));
}

bool SQLiteConnector::addPhoto(const path &photo) {
  //inserting NULL as id value is used for autoincrementing id numbers
  const char *query = "INSERT INTO photos VALUES (NULL,?);";
  sqlite3_stmt *stmt;

  do {
    if(( sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK)
      return false;

    sqlite3_bind_blob(stmt, 1, &photo, sizeof(photo), SQLITE_STATIC );
    if(sqlite3_step(stmt) == SQLITE_ROW)
      return false;
    
  } while ((sqlite3_finalize(stmt)) == SQLITE_SCHEMA);

  return !reportErrors(query);
}

bool SQLiteConnector::getDirectoriesFromDB(vector<DirectoriesPath> &directories) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT path FROM directories;";
  directories.clear();

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) == SQLITE_OK) {
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      directories.push_back(
        *(static_cast<const DirectoriesPath *>(sqlite3_column_blob(stmt,0)))
      );
    }
    sqlite3_finalize(stmt);
  }

  return !reportErrors(query);
}

bool SQLiteConnector::getChecksumFromDB(int &checksum) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT value FROM settings WHERE key=\"checksum\";";
  int rc;
  do {
    if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
      return false;

    if((rc = sqlite3_step(stmt)) == SQLITE_ROW)
      checksum = *(static_cast<const int *>(sqlite3_column_blob(stmt,0)));
  } while (rc == SQLITE_SCHEMA);

  return !reportErrors(query);
}

int SQLiteConnector::calculateChecksum() const{
  int checksum_tmp = 0;
  vector<path> photo_paths;
  Disk * disk = Disk::getInstance();
  vector<DirectoriesPath> directories;
  if(!getDirectoriesFromDB(directories))
    return 0;
  
  for(vector<DirectoriesPath>::iterator i = directories.begin();
      i != directories.end(); ++i) {
    photo_paths = disk->getPhotosPaths(i->path);
    for(vector<path>::iterator j = photo_paths.begin();
        j != photo_paths.end() ; j++) {
      checksum_tmp += hash(j->string().c_str());
    }
  }

  return checksum_tmp;
}

bool SQLiteConnector::reportErrors(const char * query) const {
  string error = sqlite3_errmsg(database);
  if(error != "not an error"){
    std::cout << query << " " << error << std::endl;
    return true;
  }
  return false;
}

bool SQLiteConnector::addTags(
     const PhotoPath &photo, const vector<string> &tags) {
  for(vector<string>::const_iterator i = tags.begin();
      i != tags.end() ; ++i) {
    if(!addTag(photo,*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::addTag(const PhotoPath &photo, const string &tag) {
  //If a tag haven't existed before it should be created into tags table
  //first. Then entry linking photo with a tag should be inserterted
  //into photos_taggs table.
  sqlite3_stmt *stmt;

  //@todo
  //Is the following query correct? To be continued...
  string query = "IF NOT EXISTS (SELECT * FROM tags WHERE name=" + tag + ") "
                  "THEN INSERT INTO tags VALUES(NULL," + tag + ") ;";

  do {
    if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0))
        != SQLITE_OK)
      return false;

    if(sqlite3_step(stmt) != SQLITE_OK)
      return false;
    
  } while ((sqlite3_finalize(stmt)) == SQLITE_SCHEMA);

  //@todo
  //Now it's time to get photo_id and tag_id using some smart SQL query.
  //query = "INSERT OR REPLACE INTO photos_tags VALUES(?,?);";
  return true;
}

bool SQLiteConnector::getPhotosWithTags(
     const std::vector<std::string> &tags, std::vector<PhotoPath> &photos) {
  //@todo
  //Warnigs:
  //1. If vector of tags is empty, return false immediately.
  //2. Make vector of tag_ids corresponding to tags' names specified
  //   in the first argument.
  //3. Make a vector of photos that are linked with all of the specified tags.

  //query1: "SELECT paths FROM photos WHERE " + query2 + ";"
  //query2: "EXISTS (SELECT tag_id1 FROM photos_tags)"
  //        "AND EXISTS (SELECT tag_id2 FROM photos_tags);";
  //
  //vector<int>::iterator i = tag_ids.begin();
  //string query2 = "EXISTS (SELECT " + *(i++) + " FROM photos_tags)";
  //for(i ; i != tag_ids.end() ; ++i) {
  //  query2 += " AND EXISTS (SELECT " + *i + " FROM photos_tags)";
  //}

  return false;
}

bool SQLiteConnector::getPhotosTags(
  const PhotoPath &photo, vector<string> &tags) {
  //@todo
  return false;
}
