/**
 * @file dbConnector.cpp
 * @brief File containing definition of an DB connectors
 * @author Jack Witkowski
 * @version 0.1
*/

#include <sqlite3.h>
#include <iostream> //for testing only
#include "../include/dbConnector.hpp"

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

int SQLiteConnector::sendQuery(const char *query) {
  sqlite3 *database;
  sqlite3_open("Database.sqlite",&database);
  sqlite3_stmt *statement;

  if(sqlite3_prepare_v2(database,query,-1,&statement,0)==SQLITE_OK) {
    std::cout<<"WORKS!"<<std::endl;
    sqlite3_step(statement);
  }
  sqlite3_finalize(statement);
  sqlite3_close(database);
}

int main() {
  //DBConnector *reference = SQLiteConnector::Instance();
  DBConnector *sqlconnector = DBConnectorFactory::Instance("dupa");
  sqlconnector->sendQuery("CREATE TABLE a (b INTEGER, c INTEGER);");
  return 0;
}
