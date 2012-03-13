/** \class ConfigurationManager
 * A class responsible for handling configuration and providing access
 * to its individual values
 */

#include "../include/configurationManager.hpp"

using namespace boost::property_tree;

ConfigurationManager::ConfigurationManager(boost::filesystem::path configFilePath) : configFilePath(configFilePath){
  read_xml(configFilePath.string(), configTree);
}

std::string ConfigurationManager::getStringValue(std::string key){
  return configTree.get<std::string>(key);
}

ptree ConfigurationManager::getConfigurationTree(){
  return configTree;
}
