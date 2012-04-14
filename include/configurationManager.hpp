#pragma once

#include  <boost/filesystem.hpp>
#include  <boost/thread.hpp>
#include  <boost/property_tree/ptree.hpp>
#include  <boost/property_tree/xml_parser.hpp>

/** @class ConfigurationManager
 *  @brief A class responsible for handling configuration and providing access to its individual values
 *  uses boost::property_tree to represent configuration values
 */

class ConfigurationManager {

public:
  static ConfigurationManager* getInstance(boost::filesystem::path configFilePath);

  /**
   * @returns A string value represented in configuration by the given key
   */
  std::string getStringValue(std::string key);

  void setStringValue(std::string key, std::string value);

  boost::property_tree::ptree getConfigurationTree();

  /**
   * @brief Overrides the whole configuration with given values.
   * Saves the config in an appropriate file.
   */
  void putConfigurationTree(boost::property_tree::ptree);

  /**
   * writes configuration, so it can be recreated after the application is closed
   */
  void writeConfiguration();

private:

  ConfigurationManager (boost::filesystem::path);
  ConfigurationManager& operator= (ConfigurationManager&);
  ConfigurationManager (ConfigurationManager&);
  ~ConfigurationManager (){};

  static ConfigurationManager* instance;

  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;
};
