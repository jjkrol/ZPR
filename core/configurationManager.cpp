#include "../include/configurationManager.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

using namespace boost::property_tree;

ConfigurationManager * DiskConfigurationManager::instance = NULL;

ConfigurationManager* DiskConfigurationManager::getInstance(boost::filesystem::path configFilePath){


  if(instance == NULL){
    instance = new DiskConfigurationManager(configFilePath);
  }

  return instance;
}

std::string DiskConfigurationManager::getStringValue(std::string key){
  return configTree.get<std::string>(key);
}

void DiskConfigurationManager::setStringValue(std::string key, std::string value){
  configTree.put<std::string>(key, value);
}

void DiskConfigurationManager::putConfigurationTree(ptree config){
  write_xml(configFilePath.string(), config);
  configTree = config;
}

void DiskConfigurationManager::writeConfiguration(){
  write_xml(configFilePath.string(), configTree);
}

ptree DiskConfigurationManager::getConfigurationTree(){
  return configTree;
}

DiskConfigurationManager::DiskConfigurationManager(boost::filesystem::path configFilePath) : configFilePath(configFilePath){
  if(!boost::filesystem::exists(configFilePath)){
    std::cout<<"Creating new config file"<<std::endl;
    std::ofstream myfile(configFilePath.string().c_str());
    myfile.close();
    configTree.put<std::string>("library.directory", "");
    write_xml(configFilePath.string(), configTree);
  }
  else{
  read_xml(configFilePath.string(), configTree);
  }
}
