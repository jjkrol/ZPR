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
  //@TODO db function for getting library paths from db 
  return true;
}

void CoreController::setLibraryPath(boost::filesystem::path libraryPath){
  configManager->setStringValue("library.directory", libraryPath.string());
  configManager->writeConfiguration();
}

Glib::RefPtr<Gtk::TreeStore> CoreController::getDatabaseTree(){
  database_model = Gtk::TreeStore::create(dir_columns);

  Directory* rootDir = new Directory("");

  std::vector<Directory*> dirs = rootDir->getSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(database_model->append());
    row[dir_columns.name] = (*it)->getName();             //adding label
    addSubdirectories(*it, row);                          //adding subdirectories
  }
  return database_model;
}

Glib::RefPtr<Gtk::TreeStore> CoreController::getDirectoryTree(){
  directory_model = Gtk::TreeStore::create(dir_columns);
  return directory_model;
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
void CoreController::doSomeLongLastingTask(){
}

void CoreController::addSubdirectories(Directory *dir, Gtk::TreeModel::Row &row) {
  if(!dir->hasSubdirectories()) return;
  std::vector<Directory*> dirs = dir->getSubdirectories();
  Gtk::TreeModel::Row childrow;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    childrow = *(database_model->append(row.children()));
    childrow[dir_columns.name] = (*it)->getName();            //adding label
    addSubdirectories(*it, childrow);                     //adding subdirectories
  }
}
