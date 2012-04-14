#include "include/core.hpp"

#define DATABASE_TESTING false
#if DATABASE_TESTING == true
#include "include/dbConnector.hpp"
#endif

int main(int argc, char *argv[]) {
  CoreController* core = CoreController::getInstance();
  core->startApplication(argc, argv);
  core->destroy();

// JUST FOR TESTS
  #if DATABASE_TESTING == true
  DBConnector *sqlconnector = DBConnectorFactory::getInstance(" ");
  sqlconnector->open("DB.sqlite");

  if(argc == 1)
    return 0;
  sqlconnector->sendQuery(argv[1]);

  sqlconnector->close();
  #endif

  return 0;
}
