#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/configurationManager.hpp"
#include "../include/disk.hpp"
#include "../include/gui.hpp"
#include "../include/directory.hpp"
#include "../include/dbConnector.hpp"
#include "../include/plugins/pluginManager.hpp"
#include "../include/plugins/plugin.hpp"
#include "../include/plugins/effect.hpp"

using namespace std;
using namespace boost::filesystem;

CoreController* CoreController::instance = NULL;

/// @fn CoreController* CoreController::getInstance(string forcedConfigPath)
/// @brief Method allowing access to unique CoreController class instance.
/// @returns Pointer to CoreController instance.
CoreController* CoreController::getInstance(string forcedConfigPath) {
  if(instance == NULL) {
    instance = new CoreController(forcedConfigPath);
  }
  return instance;
}

/// @fn void CoreController::startApplication(int argc, char** argv)
/// @brief Creates and runs the gui.
void CoreController::startApplication(int argc, char** argv) {
  gui = UserInterface::getInstance(argc, argv);
  guiThread = boost::thread(&UserInterface::init, gui);
}

/// @fn void CoreController::destroy()
/// @brief Destroys the CoreController object and joins all the threads.
void CoreController::destroy() {
  guiThread.join();
  delete instance;
  instance = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//                               PHOTOS LOADING                              //
///////////////////////////////////////////////////////////////////////////////

/// @fn bool CoreController::isCurrentPhotoSet()
/// @brief Method checking if currentPhoto is initialized.
bool CoreController::isCurrentPhotoSet() {
  return currentPhoto != currentPhotoSet.end();
}

/// @fn PhotoData CoreController::getCurrentPhoto()
/// @return PhotoData object representing current photo.
PhotoData CoreController::getCurrentPhoto() {
  PhotoData retPhoto;
  retPhoto.path = (*currentPhoto)->getPath();
  retPhoto.pixbuf = (*currentPhoto)->getPixbuf();
  return retPhoto;
}

/// @fn PhotoData CoreController::getNextPhoto()
/// @return PhotoData object representing next photo.
PhotoData CoreController::getNextPhoto() {
  currentPhoto++;
  if(currentPhoto == currentPhotoSet.end())
    currentPhoto--;
  return getCurrentPhoto(); 
}

/// @fn PhotoData CoreController::getPreviousPhoto()
/// @return PhotoData object representing previous photo.
PhotoData CoreController::getPreviousPhoto() {
  if(currentPhoto == currentPhotoSet.begin())
    return getCurrentPhoto();
  currentPhoto--;
  return getCurrentPhoto(); 
}

struct find_photo : std::unary_function<Photo*, bool> {
  boost::filesystem::path path;
  find_photo(boost::filesystem::path) : path(path) {}
  bool operator()(Photo* const& p) const {
    return p->getPath() == path;
  }
};

/// @fn void CoreController::setCurrentPhoto(boost::filesystem::path photoPath)
/// @brief Method looking for photo with path specyfied in the parameter
///        and setting that photo to current photo if found.
/// @param photoPath Path of the photo to load.
void CoreController::setCurrentPhoto(boost::filesystem::path photoPath) {
  currentPhoto = find_if(currentPhotoSet.begin(), currentPhotoSet.end(),
      find_photo(photoPath));
}

/// @fn void CoreController::setCurrentDirectory(boost::filesystem::path directoryPath) 
/// @brief Method modificating the current directory and loading photos from it.
/// @param directoryPath Path of the directory to load.
void CoreController::setCurrentDirectory(boost::filesystem::path directoryPath) {
  currentDirectory = new Directory(directoryPath);
  currentPhotoSet = currentDirectory->getPhotos();
  currentPhoto = currentPhotoSet.begin();
}

/// @fn vector<PhotoData> CoreController::getThumbnails(boost::filesystem::path directoryPath)
/// @return Vector with thumbnails of photos in currently loaded photo set.
vector<PhotoData> CoreController::getThumbnails(boost::filesystem::path directoryPath){
  vector<PhotoData> retPhotos;
  PhotoData singlePhoto;
  Directory *dir = new Directory(directoryPath);
  vector<Photo*> photos = dir->getPhotos();

  //loading thumbnails
  for(vector<Photo*>::iterator it = photos.begin(); it!=photos.end(); it++){
    singlePhoto.path = (*it)->getPath();
    singlePhoto.pixbuf = (*it)->getThumbnail();
    retPhotos.push_back(singlePhoto);
  }

  return retPhotos;
}

///////////////////////////////////////////////////////////////////////////////
//                                  DATABASE                                 //
///////////////////////////////////////////////////////////////////////////////

/// @fn bool CoreController::hasLibraryPathSet()
/// @brief Method checking if database files are created and present.
bool CoreController::hasLibraryPathSet(){
  return !(db->isEmpty()) && getLibraryDirectoryPath() != ""; 
}

/// @fn void CoreController::sendChangesToDB() 
/// @brief Method responsible for applying changes made in DB Manager.
void CoreController::sendChangesToDB() {
  std::vector<Gtk::TreeModel::iterator>::iterator folder;
  std::vector<boost::filesystem::path> db_vector;

  //copying vector and sending it to DB
  for(folder = added_folders.begin(); folder != added_folders.end(); ++folder) {
    std::string path = (std::string)(**folder)[fs_columns.path];
    path = path.substr(0, path.size()-1);
    db_vector.push_back(path);
  }
  db->addPhotosFromDirectories(db_vector);
  added_folders.clear();
  db_vector.clear();

  //copying vector and sending it to DB
  for(folder = deleted_folders.begin(); folder != deleted_folders.end(); ++folder) {
    std::string path = (std::string)(**folder)[fs_columns.path];
    path = path.substr(0, path.size()-1);
    db_vector.push_back(path);
  }
  db->deleteDirectories(db_vector);
  deleted_folders.clear();
}

/// @fn void CoreController::cancelDBChanges() 
/// @brief Method responsible for cancelling changes made in DB Manager.
void CoreController::cancelDBChanges() {
  std::vector<Gtk::TreeModel::iterator>::iterator folder;

  //cancel adding folders
  for(folder = added_folders.begin(); folder != added_folders.end(); ++folder) {
    (**folder)[fs_columns.stock_id] = "";
    (**folder)[fs_columns.included] = false;
  }
  added_folders.clear();

  //cancel removing folders
  for(folder = deleted_folders.begin(); folder != deleted_folders.end(); ++folder) {
    (**folder)[fs_columns.stock_id] = Gtk::StockID(Gtk::Stock::FIND).get_string();
    (**folder)[fs_columns.included] = true;
  }
  deleted_folders.clear();
}

/// @fn void CoreController::addFolderToDB(const Gtk::TreeModel::iterator &folder) 
/// @brief Method responsible for selecting folder in DB Manager.
/// @params folder Gtk::TreeModel::iterator pointing to selected folder.
void CoreController::addFolderToDB(const Gtk::TreeModel::iterator &folder) {
  //selecting folder
  (*folder)[fs_columns.stock_id] = Gtk::StockID(Gtk::Stock::FIND).get_string();
  (*folder)[fs_columns.included] = true;

  //storing changes in container (to handle OK/Cancel/Apply buttons)
  added_folders.push_back(folder);

  //checking if library path has not changed
  path library_path = getLibraryDirectoryPath();
  if(library_path.empty())
    setLibraryPath((std::string)(**folder)[fs_columns.path]);

  //expanding and checking for subdirectories
  Gtk::TreeModel::Path path;
  expandDirectory(folder, path);
  Gtk::TreeModel::Children children = folder->children();
  if(children.empty()) return;

  //adding subfolders to db recursively
  Gtk::TreeModel::Children::iterator child = children.begin();
  for(; child != children.end(); ++child)
    addFolderToDB(child);
}

/// @fn void CoreController::removeFolderFromDB(const Gtk::TreeModel::iterator &folder) 
/// @brief Method responsible for deselecting folder in DB Manager.
/// @params folder Gtk::TreeModel::iterator pointing to selected folder.
void CoreController::removeFolderFromDB(const Gtk::TreeModel::iterator &folder) {
  if(!(bool)(*folder)[fs_columns.included]) return;

  //unselecting folder
  (*folder)[fs_columns.stock_id] = "";
  (*folder)[fs_columns.included] = false;

  //storing changes in container (to handle OK/Cancel/Apply buttons)
  std::vector<Gtk::TreeModel::iterator>::iterator it;
  it = std::find(added_folders.begin(), added_folders.end(), folder);

  //checking whether user already added that folder to DB
  if(it != added_folders.end())
    added_folders.erase(it);
  else
    deleted_folders.push_back(folder);

  //checking for subdirectories
  Gtk::TreeModel::Children children = folder->children();
  if(children.empty()) return;

  //removing subfolders from db recursively
  Gtk::TreeModel::Children::iterator child = children.begin();
  for(; child != children.end(); ++child)
    removeFolderFromDB(child);
}

/// @fn void CoreController::expandDirectory()
/// @brief Method responsible for expanding the filesystem tree node.
/// @param path Iterator pointing to expanded node, provided by signal system.
/// @param tree_path Tree path to expanded node, provided by signal system, not used.
void CoreController::expandDirectory(const Gtk::TreeModel::iterator &parent,
                                     const Gtk::TreeModel::Path &tree_path) {
  Gtk::TreeModel::Children children = parent->children();
  Gtk::TreeModel::Children::iterator child = children.begin();
  string path;

  //checking if expanding was already done
  for(; child != children.end(); ++child)
    if(!child->children().empty()) return;

  //adding subdirectories to row children
  for(child = children.begin(); child != children.end(); ++child) {
    path = (*child)[fs_columns.path];
    Directory *dir = new Directory(path);
    addAbsoluteSubdirectories(dir, *child, 0);
    delete dir;
  }
}

/// @fn bool CoreController::hasPhotos(const boost::filesystem::path &directoryPath)
/// @brief Method used for checking if directory contains photos.
/// @param directoryPath path of directory to check.
bool CoreController::hasPhotos(const boost::filesystem::path &directoryPath){
  Directory * dir = new Directory(directoryPath);
  return dir->hasPhotos();
}

/// @fn Glib::RefPtr<Gtk::TreeStore> CoreController::getDirectoryTree()
/// @brief Method constructing the library directory tree.
/// @return An Gtk::TreeStore representing the directory tree.
Glib::RefPtr<Gtk::TreeStore> CoreController::getDirectoryTree() {
  database_tree = Gtk::TreeStore::create(dir_columns);

  //getting folders from database and library path
  vector<path> paths;
  db->getDirectoriesFromDB(paths);
  path library_path = configManager->getStringValue("library.directory");

  //filling tree
  Gtk::TreeModel::Row row, temp_row;
  for(vector<path>::iterator it = paths.begin(); it != paths.end(); ++it) {
    path temp_path = (*it);
    if(temp_path == library_path.parent_path()) {
      row = *(database_tree->append());
      row[dir_columns.name] = "/";    //adding label
    }
    else{
      vector<path> temp_paths;
      //go to library path
      while(temp_path != library_path.parent_path()){
        temp_paths.push_back(temp_path.filename());
        temp_path = temp_path.parent_path();
      }
      //descend
      bool alreadyInserted = false;

      //check first
      path popped_path = temp_paths.back();
      temp_paths.pop_back();

      Gtk::TreeModel::Children::iterator ch_it;
      Gtk::TreeModel::Children children = row.children();
      for(ch_it = children.begin(); ch_it != children.end(); ++ch_it){
        if((*ch_it)[dir_columns.name] == popped_path.string()){
          alreadyInserted = true;
          break;
        }
      }
      if(alreadyInserted){
        temp_row = *ch_it;
      }
      else{
        temp_row = *(database_tree->append(row.children()));
        temp_row[dir_columns.name] = popped_path.string();    //adding label
      }

      //check the rest
      while(temp_paths.size() > 0){
        alreadyInserted = false;
        popped_path = temp_paths.back();
        temp_paths.pop_back();
        children = temp_row.children();
        for(ch_it = children.begin(); ch_it != children.end(); ++ch_it){
          if((*ch_it)[dir_columns.name] == popped_path.string()){
            alreadyInserted = true;
            break;
          }
        }
        if(alreadyInserted){
          temp_row = *ch_it;
        }
        else{
          temp_row = *(database_tree->append(temp_row.children()));
          temp_row[dir_columns.name] = popped_path.string();    //adding label
        }

      }
    }
  }
 
  return database_tree;
}

/// @fn Glib::RefPtr<Gtk::TreeStore> CoreController::getFilesystemTree()
/// @brief Method constructing the filesystem directory tree.
/// @return An Gtk::TreeStore representing the directory tree for DB manager
Glib::RefPtr<Gtk::TreeStore> CoreController::getFilesystemTree() {
  if(filesystem_tree) return filesystem_tree;
  filesystem_tree = Gtk::TreeStore::create(fs_columns);

  Directory* rootDir = new Directory("/");
  std::vector<Directory*> dirs = rootDir->getAbsoluteSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(filesystem_tree->append());
    row[fs_columns.name] = (*it)->getName();              //adding label
    row[fs_columns.path] = (*it)->getPath().string();
    row[fs_columns.included] = false;
    addAbsoluteSubdirectories(*it, row, 0);               //adding subdirectories
  }

  ///@TODO select watched folders

  return filesystem_tree;
}

///////////////////////////////////////////////////////////////////////////////
//                                     TAGS                                  //
///////////////////////////////////////////////////////////////////////////////

/// @fn Glib::RefPtr<Gtk::ListStore> CoreController::getTagsList()
/// @brief 
Glib::RefPtr<Gtk::ListStore> CoreController::getTagsList() {
  tags_list = Gtk::ListStore::create(tags_columns);
  std::set<std::string> tags;
  db->getAllTags(tags);

  Gtk::TreeModel::Row row;
  for(std::set<std::string>::iterator it = tags.begin(); it!=tags.end(); it++){
    row = *(tags_list->append());
    row[tags_columns.name] = *it;
  }

  return tags_list;
}

/// @fn Glib::RefPtr<Gtk::ListStore> CoreController::getTagsOfActivePhoto() 
/// @brief 
Glib::RefPtr<Gtk::ListStore> CoreController::getTagsOfActivePhoto() {
  Glib::RefPtr<Gtk::ListStore> photo_tags_list = Gtk::ListStore::create(tags_columns);
  std::set<std::string> tags;
  path photo = (*currentPhoto)->getAbsolutePath();
  db->getPhotosTags(photo, tags);

  Gtk::TreeModel::Row row;
  for(std::set<std::string>::iterator it = tags.begin(); it!=tags.end(); it++){
    row = *(photo_tags_list->append());
    row[tags_columns.name] = *it;
    row[tags_columns.stock_id] = Gtk::StockID(Gtk::Stock::DELETE).get_string();
  }

  return photo_tags_list;
}

/// @fn void CoreController::setCurrentTagSet(const set<string> &tagSet) 
/// @brief 
void CoreController::setCurrentTagSet(const set<string> &tagSet) {
  vector<path> photosPaths;
  db->getPhotosWithTags(tagSet, photosPaths);

  currentPhotoSet.clear();
  for(vector<path>::const_iterator i = photosPaths.begin();
      i != photosPaths.end() ; ++i ) {
    int libraryPathLength = getLibraryDirectoryPath().string().length();
    path photoPath = path((*i).string().substr(libraryPathLength));
    currentPhotoSet.push_back(Photo::getInstance(photoPath));
  }

  currentPhoto = currentPhotoSet.begin(); 
}

/// @fn void CoreController::addTagToActivePhoto(std::string tag) 
/// @brief 
void CoreController::addTagToActivePhoto(std::string tag) {
  path photo = (*currentPhoto)->getAbsolutePath();
  db->addTagToPhoto(photo, tag);
}

/// @fn void CoreController::removeTagFromActivePhoto(std::string tag) 
/// @brief 
void CoreController::removeTagFromActivePhoto(std::string tag) {
  path photo = (*currentPhoto)->getAbsolutePath();
  db->deleteTagFromPhoto(photo, tag);
}

///////////////////////////////////////////////////////////////////////////////
//                           PLUGINS AND EFFECTS                             //
///////////////////////////////////////////////////////////////////////////////

/// @fn void CoreController::savePhotos()
/// @brief 
void CoreController::savePhotos() {
  set<Photo*>::iterator it;
  for(it = modifiedPhotos.begin(); it != modifiedPhotos.end(); ++it)
    (*it)->save();
  modifiedPhotos.clear();
}

/// @fn void CoreController::saveCurrentPhoto() 
/// @brief 
void CoreController::saveCurrentPhoto() {
  if(!isCurrentPhotoSet()) return;
  (*currentPhoto)->save();
}

/// @fn bool CoreController::modifiedPhotosExist()
/// @brief 
bool CoreController::modifiedPhotosExist() {
  return modifiedPhotos.size() > 0;
}

/// @fn void CoreController::editInExternalEditor() 
/// @brief 
void CoreController::editInExternalEditor() {

  //check if any photo is loaded/selected
  if(currentPhoto == currentPhotoSet.end()) return;

  ///@todo make editor changeable
  Glib::RefPtr<Gio::AppInfo> editor = Gio::AppInfo::create_from_commandline(
      "gimp", "GIMP", Gio::APP_INFO_CREATE_SUPPORTS_URIS);
  Glib::RefPtr<Gio::File> photo = Gio::File::create_for_path(
      (*currentPhoto)->getAbsolutePath().string());

  editor->launch(photo);
}

/// @fn void CoreController::applyEffectOfSelectedPlugin()
/// @brief 
void CoreController::applyEffectOfSelectedPlugin() {
  Effect * effect = selectedPlugin->getEffect();
  (*currentPhoto)->putEffect(effect);
  modifiedPhotos.insert(*currentPhoto);
}

/// @fn void CoreController::undoLastEffect() {
/// @brief Method responsible for reverting last change from active photo.
void CoreController::undoLastEffect() {
}

/// @fn void CoreController::redoLastEffect() {
/// @brief Method responsible for applying undone effect one more time.
void CoreController::redoLastEffect() {
}

/// @fn vector<string> CoreController::getPluginNames()
/// @brief 
vector<string> CoreController::getPluginNames() {
  vector<Plugin*> plugins = pm->getPluginList();
  vector<Plugin*>::iterator it;
  vector<string> retVector;
  for(it = plugins.begin(); it != plugins.end(); ++it){
    retVector.push_back((*it)->getName());
  }
  return retVector;
}

/// @fn Gtk::Widget * CoreController::getPluginBox(string name)
/// @brief 
Gtk::Widget* CoreController::getPluginBox(string name) {
  Plugin *plugin = pm->getPluginByName(name);
  if(plugin) {
    selectedPlugin = plugin;
    return selectedPlugin->getWidget();
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//                              CONFIGURATION                                //
///////////////////////////////////////////////////////////////////////////////

/// @fn void CoreController::setExternalEditor(std::string name) 
/// @brief 
void CoreController::setExternalEditor(std::string name) {
  configManager->setStringValue("externalEditor", name);
  configManager->writeConfiguration();
}

/// @fn boost::filesystem::path CoreController::getLibraryDirectoryPath()
/// @brief 
/// @warning Should be used carefully! Access to photos and directories should be
///          provided by the Disk class.
/// @returns Absolute path to the LibraryDirectory
boost::filesystem::path CoreController::getLibraryDirectoryPath() {
  return boost::filesystem::path(configManager->getStringValue("library.directory"));
}

/// @fn void CoreController::setLibraryPath(boost::filesystem::path libraryPath)
/// @brief 
void CoreController::setLibraryPath(boost::filesystem::path libraryPath) {
  configManager->setStringValue("library.directory", libraryPath.string());
  configManager->writeConfiguration();
}

/// @fn void CoreController::putConfiguration(boost::property_tree::ptree config)
/// @brief Saves configuration stored in a property tree.
void CoreController::putConfiguration(boost::property_tree::ptree config) {
  configManager->putConfigurationTree(config);
}

///////////////////////////////////////////////////////////////////////////////
//                              PRIVATE METHODS                              //
///////////////////////////////////////////////////////////////////////////////

/// @fn CoreController::CoreController(string forcedConfigPath)
/// @brief 
CoreController::CoreController(string forcedConfigPath){
  manageConfig(forcedConfigPath);
  manageDisk();
  manageDatabase();
  pm = new PluginManager();
}

/// @fn void CoreController::manageConfig(string forcedConfigPath)
/// @brief 
void CoreController::manageConfig(string forcedConfigPath){
  string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = DiskConfigurationManager::getInstance(boost::filesystem::path(configPath));
}

/// @fn void CoreController::manageDisk()
/// @brief 
void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
}

/// @fn void CoreController::manageDatabase()
/// @brief 
void CoreController::manageDatabase(){
  db = DBConnectorFactory::getInstance("sqlite");
  db->open("DB.sqlite");
}

/// @fn void CoreController::addSubdirectories(Directory *dir
/// @brief Method responsible for adding subdirectories to directory tree.
/// @param dir Directory in which we look for subdirectories.
/// @param row Tree row to which we append new children.
/// @param depth How deep should the method go. Set -1 for going deepest.
void CoreController::addSubdirectories(Directory *dir,
    const Gtk::TreeModel::Row &row, int depth) {
  if(!dir->hasSubdirectories()) return;
  std::vector<Directory*> dirs = dir->getSubdirectories();
  Gtk::TreeModel::Row childrow;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    childrow = *(database_tree->append(row.children()));
    childrow[fs_columns.name] = (*it)->getName();              //adding label
    if(depth != 0)
      addSubdirectories(*it, childrow, depth-1);               //adding subdirectories
  }
}

/// @fn void CoreController::addAbsoluteSubdirectories(Directory *dir
/// @brief 
void CoreController::addAbsoluteSubdirectories(Directory *dir,
    const Gtk::TreeModel::Row &row, int depth) {
  if(!dir->hasAbsoluteSubdirectories()) return;
  std::vector<Directory*> dirs = dir->getAbsoluteSubdirectories();
  Gtk::TreeModel::Row childrow;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    childrow = *(filesystem_tree->append(row.children()));
    childrow[fs_columns.name] = (*it)->getName();              //adding label
    childrow[fs_columns.path] = (*it)->getPath().string();     //adding path
    if(depth != 0)
      addAbsoluteSubdirectories(*it, childrow, depth-1);        //adding subdirectories
  }
}

/// @fn vector<PhotoData> CoreController::getPhotos(boost::filesystem::path directoryPath)
/// @brief 
vector<PhotoData> CoreController::getPhotos(boost::filesystem::path directoryPath){
  vector<PhotoData> retPhotos;
  PhotoData singlePhoto;

  Directory* dir = new Directory(directoryPath);
  vector<Photo*> photos = dir->getPhotos();
  for(vector<Photo*>::iterator it = photos.begin(); it!=photos.end(); it++){
    singlePhoto.path = (*it)->getPath();
    singlePhoto.pixbuf = (*it)->getPixbuf();
    retPhotos.push_back(singlePhoto);
  }
  return retPhotos;
}
