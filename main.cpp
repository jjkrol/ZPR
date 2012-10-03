#include "include/core.hpp"

int main(int argc, char *argv[]) {
  CoreController* core = CoreController::getInstance();
  core->startApplication(argc, argv);
  core->destroy();
  return 0;
}
