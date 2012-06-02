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
  if(type != "sqlite")
    cout << "Prosze sobie nie zartowac"<<endl;

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
  if( !database ||  !saveSettings() ) {
    //cout<< "Blad podczas zamykania: baza juz zamknieta" << endl
    //    << "lub nie udalo sie zapisac konfiguracji do bazy danych" <<endl;
    return FAILURE;
  }

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
    cout<<"Sprawdzanie czy istnieje sciezka: " << (*i).string() << endl;
    if(!disk->exists((*i))) {
      cout << "Sciezka " << i->string() << " nie istnieje" <<endl;
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
    cout << "Pobrano z BD sciezke:"<< path_string << endl;
    photos.push_back(path(path_string));
  }

  sqlite3_finalize(stmt);
  return !reportErrors(query);
}

bool SQLiteConnector::isEmpty() const{
  sqlite3_stmt *stmt;
  bool result;
  string query = "SELECT EXISTS (\n"
                 "  SELECT * FROM directories"
                 ");";
  
  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL))
      != SQLITE_OK) {
    reportErrors(query);
    return false;
  }

  if(sqlite3_step(stmt) != SQLITE_ROW)
    return false;
  
  result = static_cast<bool>(sqlite3_column_int(stmt,0));

  sqlite3_finalize(stmt);

  return result;
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
  string query = "DELETE FROM settings;";
  if(! executeQuery(query)) {
    cout <<"Nie udalo sie usunac poprzednich ustawien" << endl;
    return false;
  }
  query = "INSERT INTO settings VALUES ("
          + boost::lexical_cast<string>(calculateChecksum()) + ");";

  if(! executeQuery(query)) {
    cout << "Nie udalo sie zapisac nowych ustawien" << endl;
    return false;
  }

  return true;
}

bool SQLiteConnector::getChecksumFromDB(int &checksum) const {
  sqlite3_stmt *stmt;
  const char *query = "SELECT checksum FROM settings;";

  if((sqlite3_prepare_v2(database, query, -1, &stmt, NULL)) != SQLITE_OK)
    return false;

  if(sqlite3_step(stmt) != SQLITE_ROW)
    return false;
  
  //const void *blob = sqlite3_column_blob(stmt,0);
  //checksum = *(static_cast<const int *>(blob));
  checksum = sqlite3_column_int(stmt,0);

  sqlite3_finalize(stmt);

  return !reportErrors(query);
}

int SQLiteConnector::calculateChecksum() const{
  int checksum_tmp = 0;

  //vector with all directories stored in the database (even nested ones)
  vector<path> directories;
  if(!getDirectoriesFromDB(directories))
    return -1;
  
  //take every directory form database
  for(vector<path>::iterator i = directories.begin();
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
    if(! addPhotosFromDirectory(*i) ) {
      cout << "addPhotosFromDirectories: "
           <<"Blad przy dodawaniu zdjec do bazy danych" << endl;
      return false;
    }
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
  string query = "DELETE FROM photos WHERE parents_id\n"
                  " = (SELECT id FROM directories WHERE path=\'"
                  + dir.string() + "\');";
  
  if(! executeQuery(query)) {
    cout << "Nie udalo sie usunac zdjec bedacych w katalogu:"
         <<dir.string() <<endl;
    return false;
  }
  //getting subdirectories of directory which is being deleted
  vector<DirectoriesPath> subdirs;
  if( !getSubdirectoriesFromDB(dir, subdirs)) {
    cout<<"Nie udalo sie pobrac podkatalogow katalogu: " <<dir.string() << endl;
    return false;
  }
  //deleting each of subdirectories
  for(vector<DirectoriesPath>::iterator i = subdirs.begin();
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

bool SQLiteConnector::addPhotosFromDirectory(const DirectoriesPath &dir){

  //add directory's path to database
  if(!addDirectoryToDB(dir)) {
    cout << "Blad przy dodawaniu katalogu do bazy danych" << endl;
    return false;
  }
  //add photos which are located directly in this directory
  {
    //get photos which are located in this directory
    vector<PhotoPath> photos = disk->getPhotosPaths(dir);

    //add each photo to database
    for(vector<PhotoPath>::const_iterator i = photos.begin();
        i != photos.end() ; ++i) {
      if(! addPhoto(dir / (*i))) {
        cout << "Blad przy dodawaniu zdjecia do bazy danych" << endl;
        return false;
      }
    }
  }

  return true;
}

bool SQLiteConnector::addDirectoryToDB(const DirectoriesPath &dir) {
  //add subidirectory with values: autonum, dir.path, id_of_parental_directory 
  sqlite3_stmt *stmt;
  string parents_path, parents_id, query;

  parents_path = dir.parent_path().string();
  parents_id = "NULL";

  if( !parents_path.empty() )
    parents_id = "\n  (SELECT id FROM directories WHERE path=\'"
                 +parents_path+ "\')";

  query =
    "INSERT INTO directories VALUES(NULL, \'" + dir.string() + "\', "
    + parents_id + ");";

  if((sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, NULL)) != SQLITE_OK) {
    reportErrors(query);
    return false;
  }

  //sqlite3_bind_blob(stmt, 1, &dir, sizeof(path), SQLITE_STATIC);
  //path parent = dir.parent_path();
  //sqlite3_bind_blob(stmt, 2, &parent, sizeof(path), SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return !(reportErrors(query));
}

//TODO
bool SQLiteConnector::movePhoto(const path &old_path, const path &new_path) {
  return false;
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
const DirectoriesPath &dir, vector<DirectoriesPath> &directories) const {
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
vector<DirectoriesPath> &directories) const {
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

  string query = "SELECT path FROM photos p, photos_tags pt, tags t\n"
                 "WHERE p.id = pt.photos_id\n"
                 "AND pt.tags_id = t.id\n"
                 "AND tags.name = " + *(i++); //FIXME

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

bool SQLiteConnector::getAllTags(vector<path> &out) {
  return false;
}
