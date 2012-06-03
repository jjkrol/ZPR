#include "../include/photoCache.hpp"
#include "../include/disk.hpp"

PhotoCache * PhotoCache::instance = NULL;

using namespace std;

PhotoCache * PhotoCache::getInstance(){
  if(instance == NULL){
    instance = new PhotoCache();
  }
  return instance;
}

PhotoCache::PhotoCache(){
disk = Disk::getInstance();
}

void PhotoCache::storePixbuf(boost::filesystem::path path, Glib::RefPtr<Gdk::Pixbuf> pixbuf){
  photos[path] = pixbuf;
}

Glib::RefPtr<Gdk::Pixbuf> PhotoCache::getPhotoFile(boost::filesystem::path photoPath){
  if(photos.count(photoPath) > 0){
    return photos[photoPath];
  }
  Glib::RefPtr<Gdk::Pixbuf> loadedPhoto = disk->getPhotoFile(photoPath);
  photos[photoPath] = loadedPhoto;
  return loadedPhoto;
}
