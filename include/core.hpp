#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>
#include "global.hpp"
#include <set>
#include <gtkmm.h>
#include "asynchronous.hpp"

class Disk;
class Photo;
class Plugin;
class PhotoData;
class Directory;
class DBConnector;
class PluginManager;
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
     * @returns an object representing the directory tree for DB manager
     */
    Glib::RefPtr<Gtk::TreeStore> getFilesystemTree();

    void expandDirectory(const Gtk::TreeModel::iterator &iter,
        const Gtk::TreeModel::Path &path);

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
    void setCurrentTagSet(const std::set<std::string> &tagSet);

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
    Glib::RefPtr<Gtk::ListStore> getTagsList();

    void addTagToActivePhoto(std::string tag);
    void removeTagFromActivePhoto(std::string tag);
    Glib::RefPtr<Gtk::ListStore> getTagsOfActivePhoto();

    void addFolderToDB(const Gtk::TreeModel::iterator &);
    void removeFolderFromDB(const Gtk::TreeModel::iterator &);

    void sendChangesToDB();
    void cancelDBChanges();

    void setExternalEditor(std::string name);

    /**
     * Invoke gimp to edit photo
     */
    void editInExternalEditor();

    /**
     * Apply selected effect
     */

    void applyEffectOfSelectedPlugin();

    /**
     * @returns a widget for setting plugin parameters
     */
    Gtk::Widget * getPluginBox(std::string name);

    /**
     * @returns names of all available plugins
     */
    std::vector<std::string> getPluginNames();

    /**
     * saves all modified photos
     */
    void savePhotos();

    void saveCurrentPhoto();
    /**
     * @returns true if there are photos modified, which are not saved
     */
    bool modifiedPhotosExist();

    bool isCurrentPhotoSet();

  private:
    CoreController (std::string forcedConfigPath="");
    CoreController& operator= (const CoreController&);
    CoreController (const CoreController&);
    ~CoreController (){};

    Plugin * selectedPlugin;
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
    void addSubdirectories(Directory *, const Gtk::TreeModel::Row &, int);
    void addAbsoluteSubdirectories(Directory *, const Gtk::TreeModel::Row &, int);

    static CoreController* instance;

    std::vector<Photo*>::iterator currentPhoto;
    Directory* currentDirectory;
    std::vector<Photo*> currentPhotoSet;
    std::set<Photo*> modifiedPhotos;

    DBConnector* db;
    UserInterface *gui;
    ConfigurationManager* configManager;
    Disk* disk;
    PluginManager* pm;
    boost::thread guiThread;

    //GTK trees
    Glib::RefPtr<Gtk::TreeStore> database_tree;
    Glib::RefPtr<Gtk::TreeStore> filesystem_tree;
    Glib::RefPtr<Gtk::ListStore> tags_list;
    DirectoryTreeColumns dir_columns;
    FilesystemTreeColumns fs_columns;
    TagsListColumns tags_columns;

    //containers for comminucating with DBManager
    std::vector<Gtk::TreeModel::iterator> added_folders;
    std::vector<Gtk::TreeModel::iterator> deleted_folders;


};
