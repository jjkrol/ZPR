#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "directory.hpp"
#include "photo.hpp"
#include "configurationManager.hpp"
#include "gui.hpp"

class Directory;

/**
 * @class CoreController
 * @brief A singleton class acting as a main application controller
 */

class CoreController {

public:


  static CoreController* initialize(bool enableGui=TRUE,std::string forcedConfigPath="");
  ~CoreController (){};

  /**
   * @returns An object representing the top directory of the library. Lower levels of
   * the library can be accessed through the getSubdirectories() function.
   */
  Directory* getDirectoryTree();

  /**
   * @returns A tree of key-value pairs extracted from the configuration file.
   */
  boost::property_tree::ptree getConfiguration();

  /**
   * @warning Should be used carefully! Access to photos and directories should be
   * provided by the Disk class.
   * @returns Absolute path to the LibraryDirectory
   */
  boost::filesystem::path getLibraryDirectoryPath();

private:
  CoreController (bool enableGui=TRUE, std::string forcedConfigPath="");
  CoreController& operator= (const CoreController&);
  CoreController (const CoreController&);

  static CoreController* instance;
  ConfigurationManager* configManager;
};
