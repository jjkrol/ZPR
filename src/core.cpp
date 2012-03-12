#include "../include/core.hpp"

using namespace boost::property_tree;
using namespace std;

CoreController::CoreController(){

}

void CoreController::startApplication(string forcedConfigPath=""){
string configurationPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
read_xml(configurationPath, configurationTree);
libraryDirectoryPath = boost::filesystem::path(configurationTree.get<string>("library.directory"));

}

Directory* CoreController::getDirectoryTree(){
  return new Directory(libraryDirectoryPath);
}

boost::filesystem::path CoreController::getLibraryDirectoryPath(){
  return libraryDirectoryPath;
}

ptree CoreController::getConfiguration(){
  return configurationTree;
}
