#include "../include/photo.hpp"
#include "../include/disk.hpp"

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

void Photo::move(boost::filesystem::path destinationPath){
  ///@TODO use db function
  boost::thread moveThread(&Disk::movePhoto, disk, photoPath, destinationPath);
  
}

void Photo::deleteFromLibrary(){
   ///@TODO use db function
  delete this;
}

void Photo::deleteFromLibraryAndDisk(){
  deleteFromLibrary();
  boost::thread deleteThread(&Disk::deletePhoto, disk, photoPath);
}

boost::filesystem::path Photo::getPath(){
  return photoPath;
}

boost::filesystem::path Photo::getFilename(){
  return photoPath.filename();
}
