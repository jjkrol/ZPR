/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.01
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
int SQLiteConnector::open(const string f) {
  //first the name of a database file stored in this object is changed
  if(filename.empty())
    filename=f;
  else
    return FAILURE;

  //if the object exists, function tries to open the database
  if(Disk::exists(filename)) {
    if(sqlite3_open(filename.c_str(),&database) != SQLITE_OK)
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

void SQLiteConnector::close() {
  saveSettings();
  sqlite3_close(database);

  database = 0;
  filename.erase();
}

bool SQLiteConnector::hasChanged() const{
  int originalChecksum = 0;
  if(! getChecksumFromDB(originalChecksum))
    return true;
  return originalChecksum != calculateChecksum();
}

bool SQLiteConnector::addPhotosFromDirectories(
  //add every photo from each of directories stored in a class member vector
  //directories to the database (non-recursively)
  const path &main_dir,
  const vector<DirectoryPath> &excluded_dirs) {
  static Disk *disk = Disk::getInstance();

  //1. Add photos from the main directory
  //2. Get subdirectories from main dir, which are not specified in excluded
  //   directories.
  //3. For each of subdirecories call addPhotosFromDirectories(vector).

  vector<DirectoryPath> subdirs = disk->getSubdirectoriesPaths(main_dir);

  //Firstly, excluded dirs should be removed from the subdirectories
  //of the main directory
  for(vector<DirectoryPath>::const_iterator i = excluded_dirs.begin();
      i != excluded_dirs.end() ; i++) {
    vector<DirectoryPath>::iterator excluded =
      find(subdirs.begin(), subdirs.end(), *i);
    if(excluded != subdirs.end()) {
      subdirs.erase(excluded);
    }
  }

  //Secondly, photos from the main folder should be added to database
  vector<PhotoPath> photos = disk->getPhotosPaths(main_dir);
  for(vector<PhotoPath>::const_iterator i = photos.begin() ;
      i != photos.end() ; i++){
    addPhoto(*i);
  }

  //Thirdly, photos from the rest of directories should be added recursively
  return addPhotosFromDirectories(subdirs);
}

bool SQLiteConnector::addPhotosFromDirectories(
  const vector<DirectoryPath> &dirs) {

  for(vector<DirectoryPath>::const_iterator i = dirs.begin();
      i != dirs.end() ; i++) {
    if(! addPhotosFromDirectory(*i) )
      return false;
  }
  return true;
}

bool SQLiteConnector::addPhotosFromDirectory(const DirectoryPath &dir){
  static Disk *disk = Disk::getInstance();
  vector<DirectoryPath> subdirectories = disk->getSubdirectoriesPaths(dir);

  for(vector<DirectoryPath>::iterator i = subdirectories.begin();
      i != subdirectories.end() ; i++) {
    if(! addPhotosFromDirectory(*i))
      return false;  
  }

  vector<PhotoPath> photos = disk->getPhotosPaths(dir);
  for(vector<PhotoPath>::const_iterator i = photos.begin();
      i != photos.end() ; i++) {
    if(! addPhoto(*i))
      return false;
  }

  return true;
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
  //TODO deleting adequate row from tags_photos table
  sqlite3_stmt *stmt;
  const char *query = "DELETE FROM photos WHERE path=? ;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1,&photos_path, sizeof(photos_path), SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  return !(reportErrors(query));
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
  int rc, checksum = calculateChecksum();

  do {
    rc = sqlite3_prepare_v2(database, query, -1, &stmt, 0);
    if(rc != SQLITE_OK)
      return false;

    sqlite3_bind_blob(stmt, 1, &checksum, sizeof(checksum), SQLITE_STATIC );
    rc = sqlite3_step(stmt);
    
    if(rc == SQLITE_ROW)
      return false;
    
    rc = sqlite3_finalize(stmt);

  } while (rc==SQLITE_SCHEMA);

  return !(reportErrors(query));
}
/*bool SQLiteConnector::saveDirectories() {
  sqlite3_stmt *stmt;
  const char *query = "INSERT INTO directories VALUES(?);";
  
  unique(directories.begin(),directories.end());

  if((sqlite3_prepare_v2(database, query, -1, &stmt, 0)) != SQLITE_OK) {
    for(vector<path>::const_iterator i = directories.begin() ;
        i != directories.end() ; i++) {
      sqlite3_bind_blob(stmt, 1, &(*i), sizeof(path), SQLITE_STATIC);
      sqlite3_step(stmt);

      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
    }
    sqlite3_finalize(stmt);
  }

  return reportErrors(query);
}*/

bool SQLiteConnector::addPhoto(const path &photo) {
  //inserting NULL as id value is used for autoincrementing id numbers
  const char *query = "INSERT INTO photos VALUES (NULL,?);";
  sqlite3_stmt *stmt;
  int rc;

  do {
    rc = sqlite3_prepare_v2(database, query, -1, &stmt, 0);
    if(rc != SQLITE_OK)
      return false;

    sqlite3_bind_blob(stmt, 1, &photo, sizeof(photo), SQLITE_STATIC );
    rc = sqlite3_step(stmt);
    
    if(rc == SQLITE_ROW)
      return false;
    
    rc = sqlite3_finalize(stmt);

  } while (rc==SQLITE_SCHEMA);

  return !reportErrors(query);
}

bool SQLiteConnector::getDirectoriesFromDB(
    path &main_dir, vector<path> &directories) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT path FROM directories;";
  directories.clear();

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) == SQLITE_OK) {
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      directories.push_back(
        *(static_cast<const path *>(sqlite3_column_blob(stmt,0)))
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
    rc=sqlite3_prepare_v2(database, query, -1, &stmt, NULL);
    if(rc != SQLITE_OK)
      return false;

    rc=sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
      checksum = *(static_cast<const int *>(sqlite3_column_blob(stmt,0)));
    }
  } while (rc==SQLITE_SCHEMA);

  return !reportErrors(query);
}

int SQLiteConnector::calculateChecksum() const{
  int checksum_tmp = 0;
  vector<path> photo_paths;
  Disk * disk = Disk::getInstance();
  
  for(vector<path>::const_iterator i = directories.begin();
      i != directories.end(); i++) {
    photo_paths = disk->getPhotosPaths(*i);
    for(vector<path>::const_iterator j = photo_paths.begin();
        j != photo_paths.end() ; j++) {
      checksum_tmp += hash((j->string()).c_str());
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

//Methods defined beneath this line are used only for tests and should not
//be included into final version of the program
/*ResultTable SQLiteConnector::sendQuery(string query) {
  sqlite3_stmt *statement;
  vector<vector<string> > results;

  if(sqlite3_prepare_v2(database,query.c_str(),-1,&statement,0) == SQLITE_OK) {
    int columns = sqlite3_column_count(statement);

    while(sqlite3_step(statement) == SQLITE_ROW) {
      //if result_state equals SQLITE_ROWS that means that there is
      //result_state = sqlite3_step(statement);
      vector<string> values;
      for(int i=0 ; i<columns ; ++i) {
        //NULL can be returned as a result what would cause
        //a runtime error, if not proceeded appropriately.
        //That's why if NULL is returned the empty string is pushed
        //to the vector (line)
        string s;
        char *tmp = (char*)sqlite3_column_text(statement,i);
        if(tmp) s=tmp;
        values.push_back(s);
      }
      results.push_back(values);
    }
    sqlite3_finalize(statement);
  }
  string error = sqlite3_errmsg(database);
  if(error != "not an error")
    std::cout << query << " " << error << std::endl;

  return results;
}*/

