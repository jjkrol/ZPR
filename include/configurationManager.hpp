#pragma once

#include  <boost/filesystem.hpp>
#include  <boost/property_tree/ptree.hpp>
#include  <boost/property_tree/xml_parser.hpp>

class ConfigurationManager {

public:
  ConfigurationManager (boost::filesystem::path);
  ~ConfigurationManager (){};

  std::string getStringValue(std::string);
  boost::property_tree::ptree getConfigurationTree();
  void putConfiguration(boost::property_tree::ptree);

private:
  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;
};
