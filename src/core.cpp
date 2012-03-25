#include "../include/core.hpp"

CoreController* CoreController::instance = NULL;

CoreController* CoreController::getInstance(std::string forcedConfigPath){
  if(instance == NULL){
    instance = new CoreController(forcedConfigPath);
  }

  return instance;
}

void CoreController::destroy(){
  disk_thread.join();
  delete instance;
  instance = NULL;
}

void CoreController::startApplication(int argc, char** argv){
  GUI gui(argc, argv);
  gui.createMainWindow();
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

CoreController::CoreController(std::string forcedConfigPath){
  manageConfig(forcedConfigPath);
  manageDisk();

}

//private 

void CoreController::manageConfig(std::string forcedConfigPath){
  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = ConfigurationManager::getInstance(boost::filesystem::path(configPath));

}

void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
  disk_thread = boost::thread(&Disk::mainLoop, disk);
}
