/** \class CoreController
 * A class acting as a main application controller
 * responsible for initialization
 */

#include "../include/core.hpp"

CoreController* CoreController::initialize(bool enableGui, std::string forcedConfigPath){
  static CoreController* instance;
  if(instance == NULL){
    instance = new CoreController(enableGui, forcedConfigPath);
  }

  return instance;
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

CoreController::CoreController(bool enableGui, std::string forcedConfigPath){

  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = ConfigurationManager::initialize(boost::filesystem::path(configPath));

  if(enableGui){
    int argc=0;
    char ** argv =NULL; 
    GUI gui(argc, argv);
    gui.createMainWindow();
  }
}

