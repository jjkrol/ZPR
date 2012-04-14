#include "../include/photo.hpp"
#include "../include/disk.hpp"
#include "../include/dbConnector.hpp"

using namespace boost::gil;
using namespace std;

Photo* Photo::getInstance(boost::filesystem::path argumentPath){
  /// photos are saved in a map indexed by paths, so you can't initialize
  /// two instances of the same photo.

  static map<boost::filesystem::path, Photo*> initializedPhotos;

  map<boost::filesystem::path, Photo*>::iterator foundPhoto = initializedPhotos.find(argumentPath);

  if (foundPhoto != initializedPhotos.end()){
    return (*foundPhoto).second;
  }
  else{
    Photo* createdPhoto = new Photo(argumentPath);
    initializedPhotos[argumentPath] = createdPhoto;
    return createdPhoto; 
  }
}

Photo::Photo(boost::filesystem::path argumentPath):photoPath(argumentPath){
  db = DBConnectorFactory::getInstance("kotek");

  //@TODO load photos from db
  
  disk = Disk::getInstance();
}

Photo::~Photo(){
  static map<boost::filesystem::path, Photo*> initializedPhotos;
  initializedPhotos.erase(photoPath);
}

rgb8_image_t Photo::getThumbnail(){
  return getImage();
}

rgb8_image_t Photo::getImage(){
  //FIXME
}

Glib::RefPtr<Gdk::Pixbuf> Photo::getPixbuf() {
  return disk->getPhotoFile(photoPath);
}

boost::filesystem::path Photo::getPath(){
  return photoPath;
}

boost::filesystem::path Photo::getFilename(){
  return photoPath.filename();
}

//@TODO should be bool?
void Photo::move(boost::filesystem::path destinationPath){
  boost::filesystem::path newPathWithFilename = destinationPath;
  newPathWithFilename /= getFilename();

  db->movePhoto(photoPath, newPathWithFilename);

  boost::thread moveThread(&Disk::movePhoto, disk, photoPath, newPathWithFilename);

  photoPath = newPathWithFilename;
  moveThread.join();
}

//@TODO should be bool?
void Photo::deleteFromLibrary(){
  db->deletePhoto(photoPath);
  delete this;
}

void Photo::deleteFromLibraryAndDisk(){
  boost::thread deleteThread(&Disk::deletePhoto, disk, photoPath);
  deleteFromLibrary();
  deleteThread.join();
}

void      Photo::addTag(std::string tag ){
  ///@TODO use db function
  tags.insert(tag);
}

void      Photo::removeTag(std::string tag){
  ///@TODO use db function
  tags.erase(tag);
}

taglist_t Photo::getTags(){
  return tags;
}

bool      Photo::hasTag(std::string tag){
  return !(tags.find(tag) == tags.end());
}
