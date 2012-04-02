#include "../include/core.hpp"
#include "../include/configurationManager.hpp"
#include "../include/disk.hpp"
#include "../include/gui.hpp"
#include "../include/directory.hpp"

CoreController* CoreController::instance = NULL;

CoreController* CoreController::getInstance(std::string forcedConfigPath){
  if(instance == NULL){
    instance = new CoreController(forcedConfigPath);
  }

  return instance;
}

void CoreController::destroy(){
  disk_thread.join();
  gui->destroy();
  delete instance;
  instance = NULL;
}

void CoreController::startApplication(int argc, char** argv){
  gui = UserInterface::getInstance(argc, argv);
  gui->init();
  gui->showEditWindow();
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

void CoreController::putConfiguration(boost::property_tree::ptree config){
  configManager->putConfigurationTree(config);
}

//private 

CoreController::CoreController(std::string forcedConfigPath){
  //an example of simple threads
  boost::thread configThread(&CoreController::manageConfig, this, forcedConfigPath);
  boost::thread somethingThread(&CoreController::doSomeLongLastingTask, this);

  configThread.join();
  somethingThread.join();
  manageDisk();

}

void CoreController::manageConfig(std::string forcedConfigPath){
  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = ConfigurationManager::getInstance(boost::filesystem::path(configPath));

}

void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
}

void CoreController::doSomeLongLastingTask(){
}
