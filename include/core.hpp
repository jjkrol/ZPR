#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "directory.hpp"
#include "photo.hpp"
#include "configurationManager.hpp"
#include "gui.hpp"

class Directory;

class CoreController {

public:

  static CoreController* initialize(bool enableGui=TRUE,std::string forcedConfigPath="");

  Directory* getDirectoryTree();
  boost::property_tree::ptree getConfiguration();
  boost::filesystem::path getLibraryDirectoryPath();

private:
  CoreController (bool enableGui=TRUE, std::string forcedConfigPath="");
  CoreController& operator= (const CoreController&);
  CoreController (const CoreController&);
  virtual ~CoreController (){};
  ConfigurationManager* configManager;
};
