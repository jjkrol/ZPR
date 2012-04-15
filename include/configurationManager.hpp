#pragma once

#include  <boost/filesystem.hpp>
#include  <boost/thread.hpp>
#include  <boost/property_tree/ptree.hpp>
#include  <boost/property_tree/xml_parser.hpp>

/** 
 *  An abstract class, which represents an interface for a configuration manager
 *  It is advised, that each derived class should be a singleton.
 *  Configuration manager is responsible for handling configuration and providing 
 *  access to its individual values.
 *  Uses boost::property_tree to represent configuration key-value tress
 */

class ConfigurationManager {

public:

  /**
   * @returns A string value represented in configuration by the given key
   */
  virtual std::string getStringValue(std::string key) = 0;

  /**
   * Sets a string value for the string key in the config.
   */
  virtual void setStringValue(std::string key, std::string value) = 0;

  /**
   * @returns the configuration tree
   */
  virtual boost::property_tree::ptree getConfigurationTree() = 0;

  /**
   * Overrides the whole configuration with given values.
   * Saves the config in an appropriate file.
   */
  virtual void putConfigurationTree(boost::property_tree::ptree) = 0;

  /**
   * writes configuration, so it can be recreated after the application is closed
   */
  virtual void writeConfiguration() = 0;
};

/**
 * A configuration manager class, that sotres config data to an xml
 * file
 */
class DiskConfigurationManager : public ConfigurationManager{

public:
  static ConfigurationManager* getInstance(boost::filesystem::path configFilePath);

  virtual std::string getStringValue(std::string key);

  virtual void setStringValue(std::string key, std::string value);

  virtual boost::property_tree::ptree getConfigurationTree();

  virtual void putConfigurationTree(boost::property_tree::ptree);

  virtual void writeConfiguration();

private:

  DiskConfigurationManager (boost::filesystem::path);
  DiskConfigurationManager& operator= (ConfigurationManager&);
  DiskConfigurationManager (ConfigurationManager&);
  ~DiskConfigurationManager (){};

  static ConfigurationManager* instance;

  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;

};

/**
 * A configuration manager class, that depends on a database for storing 
 * config data
 * @TODO to be implemented
 */
class DatabaseConfigurationManager : public ConfigurationManager{

public:
  static ConfigurationManager* getInstance(boost::filesystem::path configFilePath);

  virtual std::string getStringValue(std::string key);

  virtual void setStringValue(std::string key, std::string value);

  virtual boost::property_tree::ptree getConfigurationTree();

  virtual void putConfigurationTree(boost::property_tree::ptree);

  virtual void writeConfiguration();

private:

  DatabaseConfigurationManager (boost::filesystem::path);
  DatabaseConfigurationManager& operator= (ConfigurationManager&);
  DatabaseConfigurationManager (ConfigurationManager&);
  ~DatabaseConfigurationManager (){};

  static ConfigurationManager* instance;

  boost::filesystem::path       configFilePath;
  boost::property_tree::ptree   configTree;

};
