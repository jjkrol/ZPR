/** \class Directory
 * A class representing a single directory, providing its photos and subdirectories.
 */
#include "../include/directory.hpp"

using namespace boost::filesystem;
using namespace std;

/** 
 * \TODO create a singleton-like class, (one path - one object) ?
 * \TODO make appending paths look better
 * \TODO add error handling to constructor
 */
class CoreController;

Directory::Directory(path inputPath):directoryPath(inputPath){
  /// There is no error handling in case the provided path is a file
  //TODO implement error handling
  disk = new Disk();
  if(exists(directoryPath) && is_directory(directoryPath)){

  }
}

Directory::~Directory(){
  delete disk;
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
    photos.push_back(Photo::initialize(photoPath/=path("/")/=(*it)));
  }

  return photos;
}

path Directory::getPath(){
  return path(directoryPath.string()+"/"); //TODO find a way to fix this, that's awful
}

string Directory::getName(){
  return directoryPath.filename().string();
}

/** this method is better than checking the size of getPhotos, because it does not
 * initialize new Photo objects
 */
bool Directory::hasPhotos(){
  return disk->hasPhotos(directoryPath);
}

/** this method is better than checking the size of getSubdirectories, because it does not
 * initialize new Directory objects
 */
bool Directory::hasSubdirectories(){
  return disk->hasSubdirectories(directoryPath);
}
