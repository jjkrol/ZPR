/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.01
*/

#include <iostream> //for testing only

#include "../include/dbConnector.hpp"
#include "../include/disk.hpp"
#include "../include/hashFunctions.hpp"

using namespace boost::filesystem;
//////////////////////////////////////////////////////////////////////
//Definitions of DBConnectorFactory methods
////////////////////////////////////////////////////////////////////////
DBConnector* DBConnectorFactory::getInstance(const string type) {
  /** @todo Concrete versions of DBConnector should
    * invoke the Register() function to register their types
    * in the register of types stored in the  DBConnectorGenerator
    * object (the register should be static). Then when
    * the DBConnectorFactory::getInstance(type) is called,
    * getInstance() should look for the pointer to adequate
    * getInstance() function in its register and should call
    * the function.
  */
  return SQLiteConnector::getInstance();
}

////////////////////////////////////////////////////////////////////////////////
//Definitions of SQLiteConnector methods
////////////////////////////////////////////////////////////////////////////////
/** @warning Don't forget to initialize 'instance' pointer to 0
    in every concrete version of DBConnector
*/

/** Must be initialized with 0 before can be used*/
DBConnector* SQLiteConnector::instance = 0;
DBConnector* SQLiteConnector::getInstance() {
  if(instance == 0)
    instance = new SQLiteConnector;  

  return instance;
}

int SQLiteConnector::open(string f) {
  if(filename.empty())
    filename=f;
  else
    return FAILURE;

  if(Disk::exists(filename)) {
    if(sqlite3_open(filename.c_str(),&database) != SQLITE_OK)
      return FAILURE;
    else {
     //if(hasChanged())
      if(0)
        return (OPENED | CHANGED);
      else
        return OPENED;
    }
  }
  else if(sqlite3_open(filename.c_str(),&database) == SQLITE_OK) {
    createDB();
    return (OPENED | CREATED);
  }

  return FAILURE;
}
void SQLiteConnector::createDB() {
  const char *query = 
      "CREATE TABLE photos (id INTEGER PRIMARY KEY, path BLOB);"
      "CREATE TABLE settings (key TEXT PRIMARY KEY, value BLOB);";
  sendQuery(query);
}

bool SQLiteConnector::loadDB(vector<path> dirs) {
  //add every photo from each of directories stored in this vector
  //to the database (non-recursively)
  Disk *disk_space = Disk::getInstance();

  for(vector<path>::iterator i = dirs.begin();i!=dirs.end() ; i++) {
    vector<path> photos = disk_space->getPhotosPaths(*i);
    for(vector<path>::iterator i = photos.begin() ; i != photos.end() ; i++) {
      if(!addPhoto(*i))
        return false;
    }
  }
  return true;
}

bool SQLiteConnector::addPhoto(path photo) {
  const char *query = "INSERT INTO photos VALUES (NULL,?);";
  sqlite3_stmt *stmt;
  int rc;

  do {
    rc = sqlite3_prepare(database, query, -1, &stmt, 0);
    if(rc != SQLITE_OK)
      return false;

    sqlite3_bind_blob(stmt, 1, &photo, sizeof(photo), SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if(rc == SQLITE_ROW)
      return false;
    
    rc = sqlite3_finalize(stmt);

  } while (rc==SQLITE_SCHEMA);

  return true;
}

void SQLiteConnector::close() {
  sqlite3_stmt *stmt;
  const char *query = "INSERT INTO settings VALUES(?,?);";
  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, "checksum", -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, &checksum, sizeof(checksum), SQLITE_STATIC);
  }
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  sqlite3_close(database);
  database = NULL;
  filename.erase();
}

ResultTable SQLiteConnector::sendQuery(string query) {
  sqlite3_stmt *statement;
  vector<vector<string> > results;

  if(sqlite3_prepare_v2(database,query.c_str(),-1,&statement,NULL)
     == SQLITE_OK) {
    int columns = sqlite3_column_count(statement);
    int result_state = 0;

    while(true) {
      //if result_state equals SQLITE_ROWS that means that there is
      result_state = sqlite3_step(statement);

      if(result_state == SQLITE_ROW) {
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
      else
        break;
    }
    sqlite3_finalize(statement);
  }
  string error = sqlite3_errmsg(database);
  if(error != "not an error")
    std::cout << query << " " << error << std::endl;

  return results;
}

bool SQLiteConnector::hasChanged (boost::filesystem::path directory = "/") {
  //Only photos should affect the checksum_tmp.
  Disk *disk_space = Disk::getInstance();
  vector<boost::filesystem::path>
  paths = disk_space->getPhotosPaths(directory);

  static unsigned int checksum_tmp = 0; 

  for(vector<boost::filesystem::path>::iterator i = paths.begin();
      i != paths.end() ; i++ ) {
    //% (unsigned int)(-1) opperation assures that max unsigned int value (-1)
    //is never exceeded.
    checksum_tmp = (checksum_tmp + hash((i->string()).c_str()))
                   % (unsigned int)(-1);
  }

  //Now we're scanning all subdirectories included in the directory
  paths=disk_space->getSubdirectoriesPaths(directory);

  for(vector<boost::filesystem::path>::iterator i = paths.begin();
      i != paths.end() ; i++) {
    hasChanged(*i);
  }

  return (checksum == checksum_tmp);
}

