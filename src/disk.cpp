#include "../include/disk.hpp"
#include "../include/core.hpp"
#include "../include/photo.hpp"

using namespace boost::filesystem;
using namespace std;

Disk* Disk::instance = NULL;

Disk* Disk::getInstance(path libraryDirectoryPath){
  if(instance == NULL)
    instance = new Disk(libraryDirectoryPath);
  return instance;
}

Disk::Disk(path libraryDirectoryPath):libraryDirectoryPath(libraryDirectoryPath){

}

paths_t Disk::getSubdirectoriesPaths(path directoryPath){
  return returnByValueConcurrent<paths_t>(
      boost::bind(
        &Disk::internalGetSubdirectoriesPaths, this, directoryPath
        )
      );
}

paths_t Disk::getPhotosPaths(path directoryPath){
  return returnByValueConcurrent<paths_t>(
      boost::bind(
        &Disk::internalGetPhotosPaths, this, directoryPath
        )
      );
}

bool Disk::hasPhotos(path directoryPath){
  return returnByValueConcurrent<bool>(
      boost::bind(
        &Disk::internalHasPhotos, this, directoryPath
        )
      );
}

bool Disk::hasSubdirectories(path directoryPath){
  return returnByValueConcurrent<bool>(
      boost::bind(
        &Disk::internalHasSubdirectories, this, directoryPath
        )
      );
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getPhotoFile(path photoPath){
  return returnByValueConcurrent< Glib::RefPtr<Gdk::Pixbuf> >(
      boost::bind(
        &Disk::internalGetPhotoFile, this, photoPath
        )
      );
}

path Disk::movePhoto(path sourcePath, path destinationPath){
  return returnByValueConcurrent<path>(
      boost::bind(
        &Disk::internalMovePhoto, this, sourcePath, destinationPath
        )
      );
}

void Disk::deletePhoto(path photoPath){
  returnByValueConcurrent<bool>(
      boost::bind(
        &Disk::internalDeletePhoto, this, photoPath
        )
      );
}

//private methods

void * Disk::internalGetPhotosPaths(boost::filesystem::path directoryPath){
  paths_t * photos = new paths_t();

  path absolutePath = makeAbsolutePath(directoryPath);
  paths_t directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      if( (*it).extension()==".jpg" ) // right now we only use jpg
        photos->push_back((*it).filename());

  } 

  return photos;
}

void * Disk::internalGetSubdirectoriesPaths(boost::filesystem::path directoryPath){
  paths_t * subdirectories = new paths_t();

  path absolutePath = makeAbsolutePath(directoryPath);
  paths_t  directoryContents = getDirectoryContents(absolutePath);

  for(paths_t::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it))
      subdirectories->push_back((*it).filename());
  } 

  return subdirectories;
}

void * Disk::internalHasPhotos(boost::filesystem::path directoryPath){
  if(getPhotosPaths(directoryPath).size() == 0)
    return new bool(false);
  else
    return new bool(true);
}

void * Disk::internalHasSubdirectories(boost::filesystem::path directoryPath){
  if(getSubdirectoriesPaths(directoryPath).size() == 0)
    return new bool(false);
  else
    return new bool(true);
}

void * Disk::internalGetPhotoFile(path photoPath){
  path absolutePath = makeAbsolutePath(photoPath);
  Glib::RefPtr<Gdk::Pixbuf> * retPtr = new Glib::RefPtr<Gdk::Pixbuf>(Gdk::Pixbuf::create_from_file(absolutePath.string()));
  return retPtr;
}

void * Disk::internalMovePhoto(path sourcePath, path destinationPath){
  path destinationWithFilename = destinationPath;
  destinationWithFilename /= sourcePath.filename();
  rename( makeAbsolutePath(sourcePath), makeAbsolutePath(destinationWithFilename) );
  return new path(destinationWithFilename);
}

void * Disk::internalDeletePhoto(path photoPath){
  remove(makeAbsolutePath(photoPath) );
  return new bool(true);
}


// returns content of given directory (all files)
paths_t Disk::getDirectoryContents(path directoryPath){
  vector<path> directoryContents;
  copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
  sort(directoryContents.begin(), directoryContents.end());
  return directoryContents;
}

path Disk::makeAbsolutePath(path relativePath){\
  path absolutePath = libraryDirectoryPath;
  absolutePath /= relativePath.relative_path();
  return absolutePath;
}
