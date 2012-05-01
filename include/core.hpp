#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>
#include "../include/global.hpp"
#include <set>
#include <gtkmm.h>
#include "asynchronous.hpp"

class Disk;
class Photo;
class PhotoData;
class Directory;
class DBConnector;
class UserInterface;
class ConfigurationManager;

typedef std::vector<Photo*> photos_t;

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
     * @TODO implement
     */
    bool hasLibraryPathSet();

    /**
     * sets the library path
     */ 
    void setLibraryPath(boost::filesystem::path libraryPath);

    /**
     * @returns an object representing the directory tree 
     */
    Glib::RefPtr<Gtk::TreeStore> getDirectoryTree();

    /**
     * @returns an object representing the database directory tree 
     */
    Glib::RefPtr<Gtk::TreeStore> getDatabaseTree();

void expandDirectory(Gtk::TreeModel::Path path);

    bool hasPhotos(const boost::filesystem::path& directoryPath);

      /**
       * @returns data of the photos in a given directory
       */
      std::vector<PhotoData> getPhotos(boost::filesystem::path directoryPath);

      /**
       * @returns data of the thumbnails in a given directory
       */
      std::vector<PhotoData> getThumbnails(boost::filesystem::path directoryPath);

      /**
       * sets current photo, should setCurrentDirectory or setCurrentTagSet first
       */
      void setCurrentPhoto(boost::filesystem::path photoPath);

      /**
       * sets current directory
       */
      void setCurrentDirectory(boost::filesystem::path directoryPath);

      /**
       * sets current tag set, from which next and prev photos are taken
       */
      void setCurrentTagSet(std::set<std::string> tagSet);

      /**
       * @returns data of the current photo
       */
      PhotoData getCurrentPhoto();

      /**
       * @returns next photo and sets it as a current photo
       */
      PhotoData getNextPhoto();

      /**
       * @returns previous photo and sets it as the current photo
       */
      PhotoData getPreviousPhoto();


      /**
       * @returns A tree of key-value pairs extracted from the configuration file.
       */
      boost::property_tree::ptree getConfiguration();

      /**
       * @brief Saves configuration stored in a property tree
       */
      void putConfiguration(boost::property_tree::ptree config);

      /**
       * @warning Should be used carefully! Access to photos and directories should be
       * provided by the Disk class.
       * @returns Absolute path to the LibraryDirectory
       */
      boost::filesystem::path getLibraryDirectoryPath();

      /**
       * @returns all tags
       */

      std::vector<std::string> getAllTags();

      /**
       *@returns tags similar to the query
       */
      std::vector<std::string> getTagsLike(std::string query);

      /**
       *
       */
      photos_t getPhotosWithTags(std::vector<std::string>);

      private:
      CoreController (std::string forcedConfigPath="");
      CoreController& operator= (const CoreController&);
      CoreController (const CoreController&);
      ~CoreController (){};

      void manageConfig(std::string);
      void manageDisk();
      void manageDatabase();
      void doSomeLongLastingTask();

    /**
     * Method responsible for adding subdirectories to directory tree.
     *@param dir Directory in which we look for subdirectories.
     *@param row Tree row to which we append new children.
     *@param depth How deep should the method go. Set -1 for going deepest
     */
    void addSubdirectories(Directory *dir, Gtk::TreeModel::Row &row, int depth);
    void addAbsoluteSubdirectories(Directory *dir, Gtk::TreeModel::Row &row, int depth);

      static CoreController* instance;

      std::vector<Photo*>::iterator currentPhoto;
      Directory* currentDirectory;
      std::set<std::string> currentTagSet;
      std::vector<Photo*> currentPhotoSet;

      DBConnector* db;
      UserInterface *gui;
      ConfigurationManager* configManager;
      Disk* disk;
      boost::thread guiThread;

      Glib::RefPtr<Gtk::TreeStore> directory_model;
      Glib::RefPtr<Gtk::TreeStore> database_model;
      DirTreeColumns dir_columns;
      DbTreeColumns db_columns;

    };
