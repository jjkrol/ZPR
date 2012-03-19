/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.1
*/

#include <sqlite3.h>
#include <iostream> //for testing only
#include <vector>
#include <string>

#include "../include/dbConnector.hpp"

using std::vector;
using std::string;
//////////////////////////////////////////////////////////////////////
//Definitions of DBConnectorFactory methods
////////////////////////////////////////////////////////////////////////
DBConnector* DBConnectorFactory::Instance(const char *type) {
  /** @todo Concrete versions of DBConnector should
    * invoke the Register() function to register their types
    * in the register of types stored in the  DBConnectorGenerator
    * object (the register should be static). Then when
    * the DBConnectorFactory::Instance(type) is called,
    * Instance() should look for the pointer to adequate Instance()
    * function in its register and should call the function.
  */
  return SQLiteConnector::Instance();
}


//////////////////////////////////////////////////////////////////////
//Definitions of SQLiteConnector methods
////////////////////////////////////////////////////////////////////////
/** @warning Don't forget to initialize 'instance' pointer to 0
    in every concrete version of DBConnector
*/


/** Must be initialized with 0 before can be used*/
DBConnector* SQLiteConnector::instance = 0;

DBConnector* SQLiteConnector::Instance() {
  if(instance == 0)
    instance = new SQLiteConnector;  

  return instance;
}

bool SQLiteConnector::open(const char *filename) {
  if((sqlite3_open(filename,&database)) == SQLITE_OK)
    return true;

  return false;
}

void SQLiteConnector::close() {
  sqlite3_close(database);
}

int SQLiteConnector::sendQuery(char *query) {
  sqlite3_stmt *statement;
  vector<vector<string> > results;

  if(sqlite3_prepare_v2(database,query,-1,&statement,NULL)
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
  else
    std::cout << "OK" << std::endl;
}

int main(int argc, char *argv[]) {
  DBConnector *sqlconnector = DBConnectorFactory::Instance(" ");
  sqlconnector->open("DB.sqlite");

  if(argv[1][0] == 'a')
    sqlconnector->sendQuery("CREATE TABLE photos (id INTEGER,"
                            " INETEGR path );");
  if(argv[1][0] == 'b')
    sqlconnector->sendQuery("INSERT INTO photos VALUES(1,2);");
  //else {
    //sqlconnector->sendQuery(argv[1]);
  //}
  sqlconnector->close();
  return 0;
}

