/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.03
*/

#include <iostream> //for testing end error reporting
#include <boost/lexical_cast.hpp>

#include "../include/dbConnector.hpp"
#include "../include/disk.hpp"
#include "../include/hashFunctions.hpp"

using namespace boost::filesystem;
using boost::lexical_cast;
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
//creating, saving settings, checking if has changed since last opening with
//imagine
////////////////////////////////////////////////////////////////////////////////

int SQLiteConnector::open(const string filename) {
  //return a FAILURE flag if the database is opened or a filename is empty
  if(filename.empty() || database)
    return FAILURE;

  //open a database, if it exists
  if(disk->absoluteExists(path(string("./")+filename))) {
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
  if( !database /* ||  !saveSettings()*/ ) {
    //cout<< "Blad podczas zamykania: baza juz zamknieta" << endl
    //    << "lub nie udalo sie zapisac konfiguracji do bazy danych" <<endl;
    return FAILURE;
  }

  sqlite3_close(database);
  database = 0;

  return CLOSED;
}
bool SQLiteConnector::update() {
  //add those photos
  //get paths from databasePaths which are not contained in the diskPaths
  //delete those photos

  //get directories paths
  vector<path> paths;
  if( !getDirectoriesFromDB(paths))
    return false;

  //get photos from every directory from disk to diskPhotos
  vector<path> diskPaths;
  for(vector<path>::const_iterator i=paths.begin() ; i!=paths.end() ; ++i) {
    vector<path> photos_tmp = disk->getAbsolutePhotosPaths(*i);
    diskPaths.insert(diskPaths.end(), photos_tmp.begin(), photos_tmp.end());
  }

  //get all paths from database to databasePaths
  vector<path> databasePaths;
  if(!getPhotosFromDB(databasePaths))
    return false;

  //get paths from diskPaths which are not contained in the databasePaths
  paths.clear();
  for(vector<path>::const_iterator i = diskPaths.begin();
      i != diskPaths.end() ; ++i) {
    if(find(databasePaths.begin(), databasePaths.end(), *i)
       == databasePaths.end()) {
      paths.push_back(*i);
    }
  }

  return false;
}
bool SQLiteConnector::checkCompatibility() const{
  vector<path> photos;
  if(!getPhotosFromDB(photos))
    return false;

  for(vector<path>::const_iterator i = photos.begin() ;
      i != photos.end() ; ++i) {
    if(!disk->absoluteExists((*i))) {
      return false;
    }
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
    //photos.push_back(*static_cast<const path *>(sqlite3_column_blob(stmt,1)));
    string path_string = reinterpret_cast<const char*>
                         (sqlite3_column_text(stmt,0));
    photos.push_back(path(path_string));
  }

  sqlite3_finalize(stmt);
  return !reportErrors(query);
}

bool SQLiteConnector::isEmpty() const{
  sqlite3_stmt *stmt;
  bool result;
  string query = "SELECT EXISTS (\n"
                 "  SELECT 1 FROM directories"
                 ");";
  
  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK) {
    reportErrors(query);
    return false; //TODO exceptions
  }

  if(sqlite3_step(stmt) != SQLITE_ROW)
    return false; //TODO exceptions
  
  result = static_cast<bool>(sqlite3_column_int(stmt,0));

  sqlite3_finalize(stmt);

  return !result;
}

bool SQLiteConnector::createDB() {
  const char *query = 
      "CREATE TABLE photos ("
        "id INTEGER PRIMARY KEY,"
        "path TEXT UNIQUE,"
        "parents_id INTEGER"
      ");"

      "CREATE TABLE directories ("
        "id INTEGER PRIMARY KEY,"
        "path TEXT UNIQUE,"
        "parents_id INT"
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

////////////////////////////////////////////////////////////////////////////////
//Methods for adding and removing photos and directories
////////////////////////////////////////////////////////////////////////////////

bool SQLiteConnector::addPhotosFromDirectories(
const vector<path> &dirs) {
  //add photos from each directory from database
  for(vector<path>::const_iterator i = dirs.begin();
      i != dirs.end() ; ++i) {
    //in case of failure in adding photos from directory, stop adding photos
    if(! addPhotosFromDirectory(*i) ) {
      cout << "addPhotosFromDirectories: "
           <<"Blad przy dodawaniu zdjec do bazy danych" << endl;
      return false;
    }
  }

  return true;
}

bool SQLiteConnector::deleteDirectories(const vector<path> &dirs) {
  //delete each directory from the vector
  for(vector<path>::const_iterator i = dirs.begin() ;
      i != dirs.end() ; ++i) {
    //in case of failure in deleting directories, stop
    if(!deleteDirectory(*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteDirectory(const path &dir) {
  //1. Delete photos which are directly in this directory.
  //2. Get all subdirectories from the DB.
  //3. Call deleteDirectory for each subdirecory.
  //4. Delete directory from directories table.

  //deleting photos from this folder
  string query = "DELETE FROM photos WHERE parents_id\n"
                  " = (SELECT id FROM directories WHERE path=\'"
                  + dir.string() + "\');";
  
  if(! executeQuery(query)) {
    cout << "Nie udalo sie usunac zdjec bedacych w katalogu:"
         <<dir.string() <<endl;
    return false;
  }
  //getting subdirectories of directory which is being deleted
  vector<path> subdirs;
  if( !getSubdirectoriesFromDB(dir, subdirs)) {
    cout<<"Nie udalo sie pobrac podkatalogow katalogu: " <<dir.string() << endl;
    return false;
  }
  //deleting each of subdirectories
  for(vector<path>::iterator i = subdirs.begin();
      i != subdirs.end() ; ++i) {
    if(!deleteDirectory(*i))
      return false;
  }

  //deleting directory
  query = "DELETE FROM directories WHERE path=\'" + dir.string() +"\';";

  if(! executeQuery(query)) {
    cout << "Nie udalo sie usunac katalogu:"<< dir.string() << endl;
    return false;
  }
  return true;
}

bool SQLiteConnector::addPhotosFromDirectory(const path &dir){

  //add directory's path to database
  if(!addDirectoryToDB(dir)) {
    cout << "Blad przy dodawaniu katalogu do bazy danych" << endl;
    return false;
  }
  //add photos which are located directly in this directory
  {
    //get photos which are located in this directory
    vector<path> photos = disk->getAbsolutePhotosPaths(dir);

    //add each photo to database
    for(vector<path>::const_iterator i = photos.begin();
        i != photos.end() ; ++i) {
      if(! addPhoto(dir / (*i))) {
        cout << "Blad przy dodawaniu zdjecia do bazy danych" << endl;
        return false;
      }
    }
  }

  return true;
}

bool SQLiteConnector::addDirectoryToDB(const path &dir) {
  //add subidirectory with values: autonum, dir.path, id_of_parental_directory 
  string parents_path, parents_id, query;

  parents_path = dir.parent_path().string();
  parents_id = "NULL";

  if( !parents_path.empty() )
    parents_id = "\n  (SELECT id FROM directories WHERE path=\'"
                 +parents_path+ "\')";

  query =
    "INSERT INTO directories VALUES(NULL, \'" + dir.string() + "\', "
    + parents_id + ");";

  return executeQuery(query);
}

bool SQLiteConnector::deletePhoto(const path &photos_path) {
  //Firstly, connection between the photo and corresponding tags is deleted
  string query = "DELETE FROM photos_tags WHERE photos_id IN (\n"
                 "  SELECT id FROM photos WHERE path=\'" + photos_path.string()+
                 "\'\n)";

  if(!executeQuery(query)) {
    cout << "Nie udalo sie usunac zdjecia z tabeli powiazan" << endl;
    return false;
  }

  //Secondly, photo should be deleted from the table of photos
  query = "DELETE FROM photos WHERE path=\'" + photos_path.string() + "\' ;";

  if(!executeQuery(query)) {
    cout << "Nie udalo sie usunac zdjecia z tabeli zdjec" << endl;
    return false;
  }
  return true;
}

bool SQLiteConnector::addPhoto(const path &photo) {
  //inserting NULL as an id value is used for autoincrementing id numbers
  //add a photo with an appropriate parent's id
  string parents_id, parents_path;

  parents_path = photo.parent_path().string();
  parents_id = "NULL";

  if( !parents_path.empty() ) {
    parents_id = "\n  (SELECT id FROM directories\n"
                 "i\n  WHERE path=\'" + parents_path + "\')";
  }


  string query = "INSERT INTO photos VALUES("
                  "NULL, \'" + photo.string() +"\', " + parents_id + ");";
  //path parent = photo.parent_path();
  sqlite3_stmt *stmt;

  if(( sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK)
    return !reportErrors(query);

  //sqlite3_bind_blob(stmt, 1, &photo, sizeof(photo), SQLITE_STATIC );
  //sqlite3_bind_blob(stmt, 2, &parent, sizeof(parent), SQLITE_STATIC );
  sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::getSubdirectoriesFromDB(
const path &dir, vector<path> &directories) const {
  sqlite3_stmt *stmt;
  directories.clear();
  string query = "SELECT path FROM directories WHERE parents_id\n"
                      "IN (SELECT id FROM directories WHERE path =\'"
                      + dir.string() + "\');";

  if(( sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK)
    return !reportErrors(query);

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string path_string = reinterpret_cast<const char *>
                         (sqlite3_column_text(stmt,0));
    directories.push_back( path(path_string));
  }

  sqlite3_finalize(stmt);

  return !reportErrors(query);
 
}

bool SQLiteConnector::getDirectoriesFromDB (
vector<path> &directories) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT path FROM directories;";
  directories.clear();

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return !reportErrors(query);

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string path_string = reinterpret_cast<const char*>
                        (sqlite3_column_text(stmt,0));
    directories.push_back( path(path_string) );
  }
  
  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::getPhotosFromDirectory(
  const path &directory, vector<path> &out) const{
  //FIXME this query should work if specified directory doesn't exist
  //however it will propably work for non-exisiting paths (result will be the
  //same as for "/" path). Not 100% about that one
  sqlite3_stmt *stmt;
  string query = "SELECT path FROM photos\n"
                 "WHERE parents_id = (\n"
                 "  SELECT id FROM directories\n"
                 "  WHERE path=\'" + directory.string() + "\'"
                 ");";
  out.clear();

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK) {
    cout << "Blad przy odczycie sciezki zdjecia z bazy danych" <<endl;
    return !reportErrors(query);
  }

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string path_string = reinterpret_cast<const char*>
                        (sqlite3_column_text(stmt,0));
    out.push_back(path(path_string));
  }
  sqlite3_finalize(stmt);

  return !reportErrors(query);

}

////////////////////////////////////////////////////////////////////////////////
//Other methods: reporting errors
////////////////////////////////////////////////////////////////////////////////

bool SQLiteConnector::reportErrors(string query) const {
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
const path &photo, const set<string> &tags) {
  for(set<string>::const_iterator i = tags.begin() ; i != tags.end() ; ++i) {
    if(!addTagToPhoto(photo,*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::addTagToPhoto(const path &photo, const string &tag) {
  //inserting a tag into a tags table if a tag has not existed before
  string query = "INSERT OR IGNORE INTO tags VALUES (NULL, \'" + tag + "\');\n";

  if(! executeQuery(query))
    return false;

  //making a connection between the tag and a corresponding photo
  query = "INSERT OR IGNORE INTO photos_tags\n"
          "  SELECT p.id, t.id\n"
          "  FROM photos p, tags t\n"
          "  WHERE p.path = \'" + photo.string() + "\'\n" +
          "    AND t.name = \'" + tag  + "\'\n;";

  if(!executeQuery(query)) {
    cout << "Nie udalo sie dodac polaczenia photo-tag" <<endl;
    return false;
  }
  return true;
}

bool SQLiteConnector::getPhotosTags(
const path &photo, set<string> &tags) {
  //Get id of a given photo. Then get ids of tags connected with it.
  //Finally, get names of those tags.
  sqlite3_stmt *stmt;
  string query = "SELECT t.name\n"
                 "FROM tags t, photos_tags pt, photos p\n"
                 "WHERE t.id = pt.tags_id\n"
                 "  AND pt.photos_id = p.id\n"
                 "  AND p.path =\'" + photo.string() + "\';";
  tags.clear();

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return false;
  }
    
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string tag = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
    tags.insert(tag);
  }
  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::deleteTagsFromPhoto(
const path &photo, const std::set<std::string> &tags) {
  for(std::set<std::string>::const_iterator i = tags.begin() ;
      i != tags.end() ; ++i) {
    if(!deleteTagFromPhoto(photo,*i))
      return false;
  }

  return true;
}

bool SQLiteConnector::deleteTagFromPhoto(
const path &photo, const string &tag) {
  string query = "DELETE FROM photos_tags WHERE tags_id = (\n"
                    "SELECT id FROM tags WHERE name=\'" + tag +
                  "\'\n);";

  if( !executeQuery(query) )
    return false;
 
  query = "DELETE FROM tags\n"
          "WHERE name=\'" + tag + "\'\n"
          "  AND NOT EXISTS (\n"
          "    SELECT 1 FROM photos_tags\n"
          "    WHERE tags_id = (\n"
          "      SELECT id FROM tags WHERE name=\' +  tag+ \'\n"
          "    )"
          ");";

  return executeQuery(query);
}
bool SQLiteConnector::getPhotosWithTags(
const set<string> &tags, std::vector<path> &photos) {
  if(tags.empty())
    return false;

  string tags_names, query;

  {
    set<string>::const_iterator i = tags.begin();
    tags_names+='\''+ *i +'\'';
    while(++i != tags.end()) {
      ;//tags //TODO
    }
  }
  
  query = "SELECT p.path\n"
          "FROM photos p, photos_tags pt, tags t\n"
          "WHERE p.id = pt.photos_id\n"
          "  AND pt.tags_id = t.id\n"
          "  AND t.name IN (" + tags_names + ")\n"
          "GROUP BY p.path\n"
          "HAVING count(*) =" + "1" /*tags.size()*/ + ";"; //TODO

  photos.clear();

  //executing a query
  sqlite3_stmt *stmt;
  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query.c_str());
    return false;
  }

  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string path_string = reinterpret_cast<const char*>
                         (sqlite3_column_text(stmt,0));
    photos.push_back(path(path_string));
  }

  sqlite3_finalize(stmt);
  return !reportErrors(query.c_str());
}

bool SQLiteConnector::executeQuery(string query) const{
  sqlite3_stmt *stmt;
  if(sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0) != SQLITE_OK){
    reportErrors(query);
    return false;
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

bool SQLiteConnector::getAllTags(set<string> &tags) {
  sqlite3_stmt *stmt;
  string query = "SELECT name FROM tags;";

  tags.clear();

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0)) != SQLITE_OK) {
    reportErrors(query);
    return false;
  }
    
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    string tag = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
    tags.insert(tag);
  }
  sqlite3_finalize(stmt);

  return !reportErrors(query);
 
}
