#include "../include/photo.hpp"
#include "../include/disk.hpp"
#include "../include/dbConnector.hpp"
#include "../include/plugins/effect.hpp"
#include "../include/photoCache.hpp"

using namespace boost::gil;
using namespace std;

/**
 * @TODO optimize tags (caching)
 */

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
  db = DBConnectorFactory::getInstance("sqlite");
  cache = PhotoCache::getInstance();
  db->getPhotosTags(photoPath.string(), tags);  
  disk = Disk::getInstance();
}

Photo::~Photo(){
  static map<boost::filesystem::path, Photo*> initializedPhotos;
  initializedPhotos.erase(photoPath);
}

Glib::RefPtr<Gdk::Pixbuf> Photo::getThumbnail(){
  return getPixbuf();
}

Glib::RefPtr<Gdk::Pixbuf> Photo::getPixbuf() {
    return pixbuf = cache->getPhotoFile(photoPath);
}

void Photo::setPixbuf(Glib::RefPtr<Gdk::Pixbuf> buf){
  cache->storePixbuf(photoPath, buf);
}

boost::filesystem::path Photo::getPath(){
  return photoPath;
}

boost::filesystem::path Photo::getAbsolutePath(){
  return disk->makeAbsolutePath(photoPath);
}

boost::filesystem::path Photo::getFilename(){
  return photoPath.filename();
}

void Photo::move(boost::filesystem::path destinationPath){
  boost::filesystem::path newPathWithFilename = destinationPath;
  newPathWithFilename /= getFilename();

  //db->movePhoto(photoPath, newPathWithFilename);
  //TODO implement this in db

  boost::thread moveThread(&Disk::movePhoto, disk, photoPath, newPathWithFilename);

  photoPath = newPathWithFilename;
  moveThread.join();
}

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
  std::set<std::string> argVector;
  argVector.insert(tag);
  db->addTagsToPhoto(photoPath, argVector);
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

void Photo::putEffect(Effect * effect){
  effect->execute(this);
}

void Photo::save(){
  string extension = getAbsolutePath().extension().string().substr(1);
  string type;
  cout << "Zapis zdjecia w Photo::save()" << endl;
  if(extension == "jpg" || extension == "jpeg"){
    type = "jpeg";
  }
  else if(extension == "png"){
    type = "png";
  }
  else{
    cout<<"Illegal file type - "<<extension<<endl;
    return;
  }

  if(!pixbuf){
    std::cout<<"Photo was not modified"<<std::endl;
  }
  pixbuf->save(getAbsolutePath().string(), type);

}
