/** \class CoreController
 * A class acting as a main application controller
 * responsible for initialization
 */

#include "../include/core.hpp"

CoreController::CoreController(){

}

void CoreController::startApplication(std::string forcedConfigPath=""){
  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = new ConfigurationManager(boost::filesystem::path(configPath));
}

Directory* CoreController::getDirectoryTree(){
  return new Directory("");
}

boost::filesystem::path CoreController::getLibraryDirectoryPath(){
  return boost::filesystem::path(configManager->getStringValue("library.directory"));
}

boost::property_tree::ptree CoreController::getConfiguration(){
  return configManager->getConfigurationTree();
}
