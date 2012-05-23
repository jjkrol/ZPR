#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/configurationManager.hpp"
#include "../include/disk.hpp"
#include "../include/gui.hpp"
#include "../include/directory.hpp"
#include "../include/dbConnector.hpp"

using namespace std;

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
  return !(db->isEmpty()); 
}

void CoreController::setLibraryPath(boost::filesystem::path libraryPath){
  configManager->setStringValue("library.directory", libraryPath.string());
  configManager->writeConfiguration();
}

Glib::RefPtr<Gtk::TreeStore> CoreController::getDatabaseTree(){
  if(database_model) return database_model;
  database_model = Gtk::TreeStore::create(dir_columns);

  Directory* rootDir = new Directory("");
  std::vector<Directory*> dirs = rootDir->getSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(database_model->append());
    row[dir_columns.name] = (*it)->getName();             //adding label
    addSubdirectories(*it, row, -1);                      //adding subdirectories
  }
  return database_model;
}

Glib::RefPtr<Gtk::TreeStore> CoreController::getDirectoryTree(){
  if(directory_model) return directory_model;
  directory_model = Gtk::TreeStore::create(dir_columns);

  Directory* rootDir = new Directory("");
  std::vector<Directory*> dirs = rootDir->getAbsoluteSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(directory_model->append());
    row[dir_columns.name] = (*it)->getName();             //adding label
    row[dir_columns.path] = (*it)->getPath().string();
    addAbsoluteSubdirectories(*it, row, 0);               //adding subdirectories
  }
  return directory_model;
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
    path = (*child)[dir_columns.path];
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

vector<string> CoreController::getAllTags(){
  //TODO ask db for all tags
  vector<string> retVec;
  return retVec;
}

vector<string> CoreController::getTagsLike(string query){
  //TODO ask db
  vector<string> retVec;
  return retVec;
}

photos_t CoreController::getPhotosWithTags(vector<string>){
  //TODO ask db for paths
  vector<boost::filesystem::path> paths;
  vector<boost::filesystem::path>::iterator it;
  photos_t photosWithTags;
  for(it = paths.begin(); it!=paths.end(); it++){
    photosWithTags.push_back(Photo::getInstance((*it)));
  }
  return photosWithTags;
}

//private 

CoreController::CoreController(string forcedConfigPath){
  manageConfig(forcedConfigPath);
  manageDatabase();
  manageDisk();
}

void CoreController::manageConfig(string forcedConfigPath){
  string configPath = forcedConfigPath=="" ? "config.cfg" : forcedConfigPath;
  configManager = DiskConfigurationManager::getInstance(boost::filesystem::path(configPath));
}

void CoreController::manageDisk(){
  disk = Disk::getInstance(getLibraryDirectoryPath());
}

void CoreController::manageDatabase(){
  db = DBConnectorFactory::getInstance("kotek");
  db->open("DB.sqlite");
}

void CoreController::addSubdirectories(Directory *dir,
    const Gtk::TreeModel::Row &row, int depth) {
  if(!dir->hasSubdirectories()) return;
  std::vector<Directory*> dirs = dir->getSubdirectories();
  Gtk::TreeModel::Row childrow;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    childrow = *(database_model->append(row.children()));
    childrow[db_columns.name] = (*it)->getName();              //adding label
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
    childrow = *(directory_model->append(row.children()));
    childrow[dir_columns.name] = (*it)->getName();              //adding label
    childrow[dir_columns.path] = (*it)->getPath().string();     //adding path
    if(depth != 0)
      addAbsoluteSubdirectories(*it, childrow, depth-1);        //adding subdirectories
  }
}

void CoreController::addFolderToDB(const Gtk::TreeModel::iterator &folder) {
  //selecting folder
  (*folder)[dir_columns.stock_id] = Gtk::StockID(Gtk::Stock::FIND).get_string();
  (*folder)[dir_columns.included] = true;

  //storing changes in container (to handle OK/Cancel/Apply buttons)
  added_folders.push_back(folder);

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
  if(!(bool)(*folder)[dir_columns.included]) return;

  //unselecting folder
  (*folder)[dir_columns.stock_id] = "";
  (*folder)[dir_columns.included] = false;
 
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
    db_vector.push_back((std::string)(**folder)[dir_columns.path]);
  }
  //db->addPhotosFromDirectories(db_vector);
  added_folders.clear();
  db_vector.clear();
 
  //copying vector and sending it to DB
  for(folder = deleted_folders.begin(); folder != deleted_folders.end(); ++folder) {
    db_vector.push_back((std::string)(**folder)[dir_columns.path]);
  }
  //db->deleteDirectories(db_vector);
  deleted_folders.clear();
}

void CoreController::cancelDBChanges() {
  std::vector<Gtk::TreeModel::iterator>::iterator folder;

  //cancel adding folders
  for(folder = added_folders.begin(); folder != added_folders.end(); ++folder) {
    (**folder)[dir_columns.stock_id] = "";
    (**folder)[dir_columns.included] = false;
  }
  added_folders.clear();

  //cancel removing folders
  for(folder = deleted_folders.begin(); folder != deleted_folders.end(); ++folder) {
    (**folder)[dir_columns.stock_id] = Gtk::StockID(Gtk::Stock::FIND).get_string();
    (**folder)[dir_columns.included] = true;
  }
  deleted_folders.clear();
}
