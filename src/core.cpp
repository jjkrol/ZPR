#include "../include/core.hpp"
#include "../include/gui.hpp"

CoreController* CoreController::instance = NULL;
CoreController* CoreController::initialize(int argc, char ** argv, bool enableGui, std::string forcedConfigPath){
  if(instance == NULL){
    instance = new CoreController(argc, argv, enableGui, forcedConfigPath);
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

CoreController::CoreController(int argc, char ** argv, bool enableGui, std::string forcedConfigPath){

  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = ConfigurationManager::initialize(boost::filesystem::path(configPath));

  if(enableGui){
    GUI gui(argc, argv);
    gui.createMainWindow();
  }
}

