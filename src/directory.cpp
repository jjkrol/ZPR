#include "../include/directory.hpp"

using namespace boost::filesystem;
using namespace std;

/** 
 * @TODO create a singleton-like class, (one path - one object) ?
 * @TODO make appending paths look better
 * @TODO add error handling to constructor
 */
class CoreController;

Directory::Directory(path inputPath):directoryPath(inputPath){
  /// @warning There is no error handling in case the provided path is a file
  disk = Disk::getInstance();
  if(exists(directoryPath) && is_directory(directoryPath)){

  }
}

Directory::~Directory(){
}

vector <Directory*> Directory::getSubdirectories(){
  vector<path> subdirFilenames = disk->getSubdirectoriesPaths(directoryPath);

  subdirectories.clear();

  //create a vector of pointers to Directory Objects
  for(vector<path>::iterator it = subdirFilenames.begin(); it!=subdirFilenames.end(); it++){
    path subdirPath = directoryPath;
    subdirectories.push_back(new Directory(subdirPath/=path("/")/=(*it)));
  }

  return subdirectories;
}

vector<Photo*> Directory::getPhotos(){
  vector<path> photoFilenames = disk->getPhotosPaths(directoryPath);

  photos.clear();

  //create vector of pointers to Photo objects
  for(vector<path>::iterator it = photoFilenames.begin(); it!=photoFilenames.end(); it++){
    path photoPath = directoryPath;
    photos.push_back(Photo::getInstance(photoPath/=path("/")/=(*it)));
  }

  return photos;
}

path Directory::getPath(){
  return path(directoryPath.string()+"/"); //TODO find a way to fix this, that's awful
}

string Directory::getName(){
  return directoryPath.filename().string();
}

bool Directory::hasPhotos(){
  return disk->hasPhotos(directoryPath);
}

bool Directory::hasSubdirectories(){
  return disk->hasSubdirectories(directoryPath);
}
