#include <set>

#include "../include/disk.hpp"
#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../boost/algorithm/string/case_conv.hpp"

using namespace boost::filesystem;
using namespace std;

Disk* Disk::instance = NULL;

Disk* Disk::getInstance(path libraryDirectoryPath){
  if(instance == NULL)
    instance = new Disk(libraryDirectoryPath);
  return instance;
}

Disk::Disk(path libraryDirectoryPath):libraryDirectoryPath(libraryDirectoryPath),Asynchronous(){
}

paths_t Disk::getSubdirectoriesPaths(path directoryPath){
  return returnByValueQueued<paths_t>(
      boost::bind(
        &Disk::internalGetSubdirectoriesPaths, this, directoryPath
        )
      );
}

paths_t Disk::getPhotosPaths(path directoryPath){
  return returnByValueQueued<paths_t>(
      boost::bind(
        &Disk::internalGetPhotosPaths, this, directoryPath
        )
      );
}

paths_t Disk::getAbsoluteSubdirectoriesPaths(path directoryPath){
  return returnByValueQueued<paths_t>(
      boost::bind(
        &Disk::internalGetAbsoluteSubdirectoriesPaths, this, directoryPath
        )
      );
}

paths_t Disk::getAbsolutePhotosPaths(path directoryPath){
  return returnByValueQueued<paths_t>(
      boost::bind(
        &Disk::internalGetAbsolutePhotosPaths, this, directoryPath
        )
      );
}

bool Disk::hasPhotos(path directoryPath){
  return returnByValueQueued<bool>(
      boost::bind(
        &Disk::internalHasPhotos, this, directoryPath
        )
      );
}

bool Disk::hasSubdirectories(path directoryPath){
  return returnByValueQueued<bool>(
      boost::bind(
        &Disk::internalHasSubdirectories, this, directoryPath
        )
      );
}

bool Disk::hasAbsolutePhotos(path directoryPath){
  return returnByValueQueued<bool>(
      boost::bind(
        &Disk::internalAbsoluteHasPhotos, this, directoryPath
        )
      );
}

bool Disk::hasAbsoluteSubdirectories(path directoryPath){
  return returnByValueQueued<bool>(
      boost::bind(
        &Disk::internalAbsoluteHasSubdirectories, this, directoryPath
        )
      );
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getPhotoFile(path photoPath){
  return returnByValueQueued< Glib::RefPtr<Gdk::Pixbuf> >(
      boost::bind(
        &Disk::internalGetPhotoFile, this, photoPath
        )
      );
}

path Disk::movePhoto(path sourcePath, path destinationPath){
  return returnByValueQueued<path>(
      boost::bind(
        &Disk::internalMovePhoto, this, sourcePath, destinationPath
        )
      );
}

void Disk::deletePhoto(path photoPath){
  returnByValueQueued<bool>(
      boost::bind(
        &Disk::internalDeletePhoto, this, photoPath
        )
      );
}

    bool Disk::exists(boost::filesystem::path file) {
      return boost::filesystem::exists(makeSystemAbsolutePath(file));
    } 

    bool Disk::absoluteExists(boost::filesystem::path file) {
      return boost::filesystem::exists(file);
    } 
//private methods

void * Disk::internalGetPhotosPaths(boost::filesystem::path directoryPath){
  paths_t * photos = new paths_t();

  path absolutePath = makeAbsolutePath(directoryPath);
  paths_t directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      if( isAcceptableExtension((*it).extension().string())) // right now we only use jpg
        photos->push_back((*it).filename());

  } 

  return photos;
}

void * Disk::internalGetSubdirectoriesPaths(boost::filesystem::path directoryPath){
  paths_t * subdirectories = new paths_t();
  path absolutePath = makeAbsolutePath(directoryPath);
  paths_t  directoryContents;
  directoryContents = getDirectoryContents(absolutePath);
  for(paths_t::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it) && (*it).filename().string().substr(0,1)!=".")
      subdirectories->push_back((*it).filename());
  } 
  return subdirectories;
}

void * Disk::internalGetAbsolutePhotosPaths(boost::filesystem::path directoryPath){
  paths_t * photos = new paths_t();

  path absolutePath = makeSystemAbsolutePath(directoryPath);
  paths_t directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      if( isAcceptableExtension((*it).extension().string())) // right now we only use jpg
        photos->push_back((*it).filename());

  } 

  return photos;
}

void * Disk::internalGetAbsoluteSubdirectoriesPaths(boost::filesystem::path directoryPath){
  paths_t * subdirectories = new paths_t();

  path absolutePath = makeSystemAbsolutePath(directoryPath);
  paths_t  directoryContents = getDirectoryContents(absolutePath);

  for(paths_t::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it) && (*it).filename().string().substr(0,1)!=".")
      subdirectories->push_back((*it).filename());
  } 

  return subdirectories;
}

void * Disk::internalHasPhotos(boost::filesystem::path directoryPath){
paths_t * paths = reinterpret_cast<paths_t*>(internalGetPhotosPaths(directoryPath));
bool * retVal;
  if(paths->size() == 0)
    retVal = new bool(false);
  else
    retVal =  new bool(true);
delete paths;
return retVal;
}

void * Disk::internalHasSubdirectories(boost::filesystem::path directoryPath){
  paths_t *  paths = reinterpret_cast<paths_t*>(internalGetSubdirectoriesPaths(directoryPath));
  bool * retVal;
  if(paths->size() == 0)
    retVal =  new bool(false);
  else
    retVal =  new bool(true);
  delete paths;
  return retVal;
}

void * Disk::internalAbsoluteHasPhotos(boost::filesystem::path directoryPath){
paths_t * paths = reinterpret_cast<paths_t*>(internalGetAbsolutePhotosPaths(directoryPath));
bool * retVal;
  if(paths->size() == 0)
    retVal = new bool(false);
  else
    retVal =  new bool(true);
delete paths;
return retVal;
}

void * Disk::internalAbsoluteHasSubdirectories(boost::filesystem::path directoryPath){
paths_t *  paths; 
  try{
  paths = reinterpret_cast<paths_t*>(internalGetAbsoluteSubdirectoriesPaths(directoryPath));
  }
  catch (boost::filesystem::filesystem_error &e) 
  { 
    std::cerr << e.what() << std::endl; 
    return new bool(false);
  } 
  bool * retVal;
  if(paths->size() == 0)
    retVal =  new bool(false);
  else
    retVal =  new bool(true);
  delete paths;
  return retVal;
}

void * Disk::internalGetPhotoFile(path photoPath){
  path absolutePath = makeAbsolutePath(photoPath);
  Glib::RefPtr<Gdk::Pixbuf> * retPtr = new Glib::RefPtr<Gdk::Pixbuf>(Gdk::Pixbuf::create_from_file(absolutePath.string()));
  return retPtr;
}

void * Disk::internalMovePhoto(path sourcePath, path destinationPath){
  rename( makeAbsolutePath(sourcePath), makeAbsolutePath(destinationPath) );
  return new path(destinationPath);
}

void * Disk::internalDeletePhoto(path photoPath){
  remove(makeAbsolutePath(photoPath) );
  return new bool(true);
}


// returns content of given directory (all files)
paths_t Disk::getDirectoryContents(path directoryPath){
  vector<path> directoryContents;
  try{
  copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
  sort(directoryContents.begin(), directoryContents.end());
  }
  catch (boost::filesystem::filesystem_error &e) 
  { 
    std::cerr << e.what() << std::endl; 
  } 
  return directoryContents;
}

path Disk::makeAbsolutePath(path relativePath){\
  path absolutePath = libraryDirectoryPath;
  absolutePath /= relativePath.relative_path();
  return absolutePath;
}

path Disk::makeSystemAbsolutePath(path relativePath){\
  path absolutePath("/");
  absolutePath /= relativePath.relative_path();
  return absolutePath;
}

bool Disk::isAcceptableExtension(string givenExtension){
  set<string> extensions; 
  boost::algorithm::to_lower(givenExtension);
  extensions.insert(".jpeg");
  extensions.insert(".jpg");
  extensions.insert(".png");
  extensions.insert(".bmp");
  extensions.insert(".ico");
  if(extensions.count(givenExtension) > 0)
    return true;
  else
    return false;
}
