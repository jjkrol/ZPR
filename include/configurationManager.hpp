#pragma once

#include  <boost/filesystem.hpp>
#include  <boost/property_tree/ptree.hpp>
#include  <boost/property_tree/xml_parser.hpp>

class ConfigurationManager {

public:
  static ConfigurationManager* initialize(boost::filesystem::path configFilePath);

  std::string getStringValue(std::string);
  boost::property_tree::ptree getConfigurationTree();
  void putConfiguration(boost::property_tree::ptree);

private:
  ConfigurationManager (boost::filesystem::path);
  ConfigurationManager& operator= (ConfigurationManager&);
  ConfigurationManager (ConfigurationManager&);
  ~ConfigurationManager (){};
  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;
};
