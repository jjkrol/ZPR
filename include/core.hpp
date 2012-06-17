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

/**
 * @class CoreController
 * @brief A singleton class acting as a main application controller.
 */
class CoreController {
  public:
    static CoreController* getInstance(std::string forcedConfigPath = "");
    void startApplication(int argc = 0, char ** argv = NULL);
    void destroy();

    //methods associated with photos loading
    bool isCurrentPhotoSet();
    PhotoData getCurrentPhoto();
    PhotoData getNextPhoto();
    PhotoData getPreviousPhoto();
    void setCurrentPhoto(boost::filesystem::path photoPath);
    void setCurrentDirectory(boost::filesystem::path directoryPath);
    std::vector<PhotoData> getThumbnails(boost::filesystem::path directoryPath);

    //methods associated with database
    bool hasLibraryPathSet();
    void sendChangesToDB();
    void cancelDBChanges();
    void addFolderToDB(const Gtk::TreeModel::iterator &);
    void removeFolderFromDB(const Gtk::TreeModel::iterator &);
    void expandDirectory(const Gtk::TreeModel::iterator &iter,
        const Gtk::TreeModel::Path &path);
    bool hasPhotos(const boost::filesystem::path& directoryPath);
    Glib::RefPtr<Gtk::TreeStore> getDirectoryTree();
    Glib::RefPtr<Gtk::TreeStore> getFilesystemTree();

    //methods associated with tags
    Glib::RefPtr<Gtk::ListStore> getTagsList();
    Glib::RefPtr<Gtk::ListStore> getTagsOfActivePhoto();
    void setCurrentTagSet(const std::set<std::string> &tagSet);
    void addTagToActivePhoto(std::string tag);
    void removeTagFromActivePhoto(std::string tag);

    //methods associated with plugins and effects
    void savePhotos();
    void saveCurrentPhoto();
    bool modifiedPhotosExist();
    void editInExternalEditor();
    void applyEffectOfSelectedPlugin();
    void undoLastEffect();
    void redoLastEffect();
    std::vector<std::string> getPluginNames();
    Gtk::Widget* getPluginBox(std::string name);

    //methods associated with configuration
    void setExternalEditor(std::string name);
    boost::filesystem::path getLibraryDirectoryPath();
    void setLibraryPath(boost::filesystem::path libraryPath);
    void putConfiguration(boost::property_tree::ptree config);

  private:
    //hidden constructors and descructor
    CoreController(std::string forcedConfigPath = "");
    CoreController& operator=(const CoreController&);
    CoreController(const CoreController&);
    ~CoreController() {};

    //other private methods
    void manageConfig(std::string);
    void manageDisk();
    void manageDatabase();
    void addSubdirectories(Directory *, const Gtk::TreeModel::Row &, int);
    void addAbsoluteSubdirectories(Directory *, const Gtk::TreeModel::Row &, int);
    std::vector<PhotoData> getPhotos(boost::filesystem::path directoryPath);

    static CoreController* instance;

    //variables representing current state of application
    Plugin *selectedPlugin;
    Directory* currentDirectory;
    std::vector<Photo*>::iterator currentPhoto;
    std::vector<Photo*> currentPhotoSet;
    std::set<Photo*> modifiedPhotos;

    //connections with other classes
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
