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

CoreController* CoreController::getInstance(string forcedConfigPath){
  if(instance == NULL){
    instance = new CoreController(forcedConfigPath);
  }
  return instance;
}

void CoreController::destroy(){
  //message to terminate gui
  //gui->destroy();
  guiThread.join();
  delete instance;
  instance = NULL;
}

void CoreController::startApplication(int argc, char** argv){
  gui = UserInterface::getInstance(argc, argv);
  guiThread = boost::thread(&UserInterface::init, gui);
}

bool CoreController::hasLibraryPathSet(){
  return !(db->isEmpty()) && getLibraryDirectoryPath() != ""; 
}

void CoreController::setLibraryPath(boost::filesystem::path libraryPath){
  configManager->setStringValue("library.directory", libraryPath.string());
  configManager->writeConfiguration();
}

Glib::RefPtr<Gtk::TreeStore> CoreController::getDirectoryTree(){
  database_tree = Gtk::TreeStore::create(dir_columns);

  //getting folders from database
  vector<path> paths;
  db->getDirectoriesFromDB(paths);

  //getting library path
  path library_path = configManager->getStringValue("library.directory");

  //filling tree
  Gtk::TreeModel::Row row;
      Gtk::TreeModel::Row temp_row;
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

//@TODO change for paths?
Glib::RefPtr<Gtk::TreeStore> CoreController::getFilesystemTree(){
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

  //@TODO check watched folders

  return filesystem_tree;
}

void CoreController::expandDirectory(const Gtk::TreeModel::iterator &parent,
    const Gtk::TreeModel::Path &tree_path){
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

bool CoreController::hasPhotos(const boost::filesystem::path &directoryPath){
  Directory * dir = new Directory(directoryPath);
  return dir->hasPhotos();
}

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

vector<PhotoData> CoreController::getThumbnails(boost::filesystem::path directoryPath){
  vector<PhotoData> retPhotos;
  PhotoData singlePhoto;

  Directory* dir = new Directory(directoryPath);
  vector<Photo*> photos = dir->getPhotos();
  for(vector<Photo*>::iterator it = photos.begin(); it!=photos.end(); it++){
    singlePhoto.path = (*it)->getPath();
    singlePhoto.pixbuf = (*it)->getThumbnail();
    retPhotos.push_back(singlePhoto);
  }
  return retPhotos;
}

struct find_photo : std::unary_function<Photo*, bool> {
  boost::filesystem::path path;
  find_photo(boost::filesystem::path):path(path) { }
  bool operator()(Photo* const& p) const {
    return p->getPath() == path;
  }
};

void CoreController::setCurrentPhoto(boost::filesystem::path photoPath){
  currentPhoto = find_if(currentPhotoSet.begin(), currentPhotoSet.end(),
      find_photo(photoPath));
}

void CoreController::setCurrentDirectory(boost::filesystem::path directoryPath){
  currentDirectory = new Directory(directoryPath);
  currentPhotoSet = currentDirectory->getPhotos();
  currentPhoto = currentPhotoSet.begin();
}

void CoreController::setCurrentTagSet(set<string> tagSet){
  currentTagSet = tagSet; 
  //set current photo set
}

PhotoData CoreController::getCurrentPhoto(){
  PhotoData retPhoto;

  retPhoto.path = (*currentPhoto)->getPath();
  retPhoto.pixbuf = (*currentPhoto)->getPixbuf();
  return retPhoto;
}

PhotoData CoreController::getNextPhoto(){
  currentPhoto++;
  if(currentPhoto == currentPhotoSet.end()){
    currentPhoto--;
  }
  return getCurrentPhoto(); 
}

PhotoData CoreController::getPreviousPhoto(){
  if(currentPhoto == currentPhotoSet.begin())
    return getCurrentPhoto();

  currentPhoto--;
  return getCurrentPhoto(); 
}

boost::filesystem::path CoreController::getLibraryDirectoryPath(){
  return boost::filesystem::path(configManager->getStringValue("library.directory"));
}

// configuration
boost::property_tree::ptree CoreController::getConfiguration(){
  return configManager->getConfigurationTree();
}

void CoreController::putConfiguration(boost::property_tree::ptree config){
  configManager->putConfigurationTree(config);
}

// tags

Glib::RefPtr<Gtk::ListStore> CoreController::getTagsList(){
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

photos_t CoreController::getPhotosWithTags(vector<string>){
  //TODO ask DB
  vector<boost::filesystem::path> paths;
  vector<boost::filesystem::path>::iterator it;
  photos_t photosWithTags;
  for(it = paths.begin(); it!=paths.end(); it++){
    photosWithTags.push_back(Photo::getInstance((*it)));
  }
  return photosWithTags;
}

void CoreController::addTagToActivePhoto(std::string tag) {
  path photo = (*currentPhoto)->getAbsolutePath();
  db->addTagToPhoto(photo, tag);
}

void CoreController::removeTagFromActivePhoto(std::string tag) {
  path photo = (*currentPhoto)->getAbsolutePath();
  db->deleteTagFromPhoto(photo, tag);
}

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

//private 

CoreController::CoreController(string forcedConfigPath){
  manageConfig(forcedConfigPath);
  manageDisk();
  manageDatabase();
  pm = new PluginManager();
}

void CoreController::manageConfig(string forcedConfigPath){
  string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = DiskConfigurationManager::getInstance(boost::filesystem::path(configPath));
}

void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
}

void CoreController::manageDatabase(){
  db = DBConnectorFactory::getInstance("sqlite");
  db->open("DB.sqlite");
}

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

void CoreController::setExternalEditor(std::string name) {
  configManager->setStringValue("externalEditor", name);
  configManager->writeConfiguration();
}

void CoreController::editInExternalEditor() {
  //check if any photo is loaded/selected
  if(currentPhoto == currentPhotoSet.end()) return;

  Glib::RefPtr<Gio::AppInfo> editor = Gio::AppInfo::create_from_commandline(
      "gimp", "GIMP", Gio::APP_INFO_CREATE_SUPPORTS_URIS);
  Glib::RefPtr<Gio::File> photo = Gio::File::create_for_path(
      (*currentPhoto)->getAbsolutePath().string());

  editor->launch(photo);
}

vector<string> CoreController::getPluginNames(){
  vector<Plugin*> plugins = pm->getPluginList();
  vector<Plugin*>::iterator it;
  vector<string> retVector;
  for(it = plugins.begin(); it != plugins.end(); ++it){
    retVector.push_back((*it)->getName());
  }
  return retVector;
}

void CoreController::applyEffectOfSelectedPlugin(){
  Effect * effect =  selectedPlugin->getEffect();
  (*currentPhoto)->putEffect(effect);
  std::cout<<"Apply effect"<<std::endl;
  modifiedPhotos.insert(*currentPhoto);
}

Gtk::Widget * CoreController::getPluginBox(string name){
  Plugin * plugin = pm->getPluginByName(name);
  selectedPlugin = plugin;
  return selectedPlugin->getWidget();
}

void CoreController::savePhotos(){
  cout<<"Save photos"<<endl;
  set<Photo*>::iterator it;
  for(it = modifiedPhotos.begin(); it != modifiedPhotos.end(); ++it){
    (*it)->save();
  }
  modifiedPhotos.clear();
}

bool CoreController::modifiedPhotosExist(){
  return modifiedPhotos.size() > 0;
}
