#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>


class Disk;
class Directory;
class UserInterface;
class ConfigurationManager;

/**
 * @class CoreController
 * @brief A singleton class acting as a main application controller
 */

class CoreController {

public:

  static CoreController* getInstance(std::string forcedConfigPath="");
  /**
   * @brief destroys the CoreController object, but joins all the threads before
   *
   */
  void destroy();
  /**
   * @brief runs the gui, all the preparations are done in constructor by now
   */
  void startApplication(int argc=0, char ** argv=NULL);

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
  CoreController (std::string forcedConfigPath="");
  CoreController& operator= (const CoreController&);
  CoreController (const CoreController&);
  ~CoreController (){};

  void manageConfig(std::string);
  void manageDisk();
  void doSomeLongLastingTask();

  static CoreController* instance;
  UserInterface *gui;
  ConfigurationManager* configManager;
  Disk* disk;
  boost::thread disk_thread;
};
