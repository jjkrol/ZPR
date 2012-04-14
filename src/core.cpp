#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/configurationManager.hpp"
#include "../include/disk.hpp"
#include "../include/gui.hpp"
#include "../include/directory.hpp"
#include "../include/dbConnector.hpp"

CoreController* CoreController::instance = NULL;

CoreController* CoreController::getInstance(std::string forcedConfigPath){
  if(instance == NULL){
    instance = new CoreController(forcedConfigPath);
  }

  return instance;
}

void CoreController::destroy(){
  //message to terminate gui
  //gui->destroy();
  guiThread.join();
  delete instance;
  instance = NULL;
}

void CoreController::startApplication(int argc, char** argv){
  gui = UserInterface::getInstance(argc, argv);
  guiThread = boost::thread(&UserInterface::init, gui);
  //initialize database
  //
}

Directory* CoreController::getDirectoryTree(){
  return new Directory("");
}

boost::filesystem::path CoreController::getLibraryDirectoryPath(){
  return boost::filesystem::path(configManager->getStringValue("library.directory"));
}


// configuration

boost::property_tree::ptree CoreController::getConfiguration(){
  return configManager->getConfigurationTree();
}

void CoreController::putConfiguration(boost::property_tree::ptree config){
  configManager->putConfigurationTree(config);
}




// tags
  
  std::vector<std::string> CoreController::getAllTags(){
    //TODO ask db for all tags
    std::vector<std::string> retVec;
    return retVec;
  }

  std::vector<std::string> CoreController::getTagsLike(std::string query){
    //TODO ask db
    std::vector<std::string> retVec;
    return retVec;
  }

  photos_t CoreController::getPhotosWithTags(std::vector<std::string>){
    //TODO ask db for paths
    std::vector<boost::filesystem::path> paths;
    std::vector<boost::filesystem::path>::iterator it;
    photos_t photosWithTags;
    for(it = paths.begin(); it!=paths.end(); it++){
      photosWithTags.push_back(Photo::getInstance((*it)));
    }
    return photosWithTags;
  }

//private 

CoreController::CoreController(std::string forcedConfigPath){
  manageConfig(forcedConfigPath);
  manageDatabase();
  manageDisk();
}

void CoreController::manageConfig(std::string forcedConfigPath){
  std::string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = ConfigurationManager::getInstance(boost::filesystem::path(configPath));

}

void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
}

void CoreController::manageDatabase(){
  db = DBConnectorFactory::getInstance("kotek");
  db->open("DB.sqlite");
}
void CoreController::doSomeLongLastingTask(){
}
