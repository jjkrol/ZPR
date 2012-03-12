#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "directory.hpp"
#include "photo.hpp"

class CoreController {
public:
  CoreController ();
  virtual ~CoreController (){};
  void startApplication(std::string);
  Directory* getDirectoryTree();
  boost::property_tree::ptree getConfiguration();
  boost::filesystem::path getLibraryDirectoryPath();
private:
  boost::filesystem::path libraryDirectoryPath;
  boost::property_tree::ptree configurationTree;
};
