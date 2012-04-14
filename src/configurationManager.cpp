#include "../include/configurationManager.hpp"

using namespace boost::property_tree;

ConfigurationManager * ConfigurationManager::instance = NULL;

ConfigurationManager* ConfigurationManager::getInstance(boost::filesystem::path configFilePath){


  if(instance == NULL){
    instance = new ConfigurationManager(configFilePath);
  }

  return instance;
}

std::string ConfigurationManager::getStringValue(std::string key){
  return configTree.get<std::string>(key);
}

void ConfigurationManager::setStringValue(std::string key, std::string value){
  configTree.put<std::string>(key, value);
}

void ConfigurationManager::putConfigurationTree(ptree config){
  write_xml(configFilePath.string(), config);
  configTree = config;
}

void ConfigurationManager::writeConfiguration(){
  write_xml(configFilePath.string(), configTree);
}

ptree ConfigurationManager::getConfigurationTree(){
  return configTree;
}

ConfigurationManager::ConfigurationManager(boost::filesystem::path configFilePath) : configFilePath(configFilePath){
  read_xml(configFilePath.string(), configTree);

}
