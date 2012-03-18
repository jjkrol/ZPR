#include <sqlite3.h>
#include <iostream> //for testing only
#include "../include/dbConnector.hpp"

int SQLiteConnector::sendQuery(const char *query){
  sqlite3 *database;
  sqlite3_open("Database.sqlite",&database);
  sqlite3_stmt *statement;

  if(sqlite3_prepare_v2(database,query,-1,&statement,0)==SQLITE_OK){
    std::cout<<"WORKS!"<<std::endl;
    sqlite3_step(statement);
  }
  sqlite3_finalize(statement);
  sqlite3_close(database);
}




//int main() {
//  DBConnector *reference = SQLiteDBConnector::getInstance();
//
//  return 0;
//}
