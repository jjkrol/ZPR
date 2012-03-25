#pragma once

#include  <boost/filesystem.hpp>
#include  <boost/thread.hpp>
#include  <boost/property_tree/ptree.hpp>
#include  <boost/property_tree/xml_parser.hpp>

/** @class ConfigurationManager
 *  @brief A class responsible for handling configuration and providing access to its individual values
 */

class ConfigurationManager {

public:
  static ConfigurationManager* getInstance(boost::filesystem::path configFilePath);

  /**
   * @returns A string value represented in configuration by the given key
   */
  std::string getStringValue(std::string);

  boost::property_tree::ptree getConfigurationTree();


  /**
   * @todo Implement this
   * @brief Overrides the whole configuration with the given values.
   * Saves the config in an appropriate file.
   */
  void putConfiguration(boost::property_tree::ptree);

private:

  ConfigurationManager (boost::filesystem::path);
  ConfigurationManager& operator= (ConfigurationManager&);
  ConfigurationManager (ConfigurationManager&);
  ~ConfigurationManager (){};

  static ConfigurationManager* instance;

  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;
};
