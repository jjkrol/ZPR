
#include "../include/disk.hpp"
#include <iostream>

using namespace boost::filesystem;
using namespace boost::gil;
using namespace std;

Disk::Disk(){
  int argc = 0;
  char ** argv = NULL;
  CoreController * core = CoreController::initialize(argc, argv);
  libraryDirectoryPath = core->getLibraryDirectoryPath();
}

vector <path> Disk::getSubdirectoriesPaths(path directoryPath){
  vector<path> subdirectories;

  path absolutePath = makeAbsolutePath(directoryPath);
  vector<path> directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it))
      subdirectories.push_back((*it).filename());
  } 

  return subdirectories;
}

vector<path> Disk::getPhotosPaths(path directoryPath){
  vector<path> photos;

  path absolutePath = makeAbsolutePath(directoryPath);
  vector<path> directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      if( (*it).extension()==".jpg" ) // right now we only use jpg
        photos.push_back((*it).filename());

  } 

  return photos;
}

bool Disk::hasPhotos(path directoryPath){
  if(getPhotosPaths(directoryPath).size() == 0)
    return false;
  else
    return true;
}

bool Disk::hasSubdirectories(path directoryPath){
  if(getSubdirectoriesPaths(directoryPath).size() == 0)
    return false;
  else
    return true;
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getPhotoFile(path photoPath){
  path absolutePath = makeAbsolutePath(photoPath);
  return Gdk::Pixbuf::create_from_file(absolutePath.string());
}
//private methods

// returns content of given directory (all files)
vector<path> Disk::getDirectoryContents(path directoryPath){
  vector<path> directoryContents;
  copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
  sort(directoryContents.begin(), directoryContents.end());
  return directoryContents;
}

path Disk::makeAbsolutePath(path relativePath){
  return path(libraryDirectoryPath.string() + relativePath.string());
}
