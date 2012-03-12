/** \class Directory
 * A class representing a single directory, providing its photos and subdirectories.
 */
#include "../include/directory.hpp"

using namespace boost::filesystem;
using namespace std;
/* TODO
 * create a singleton-like class, (one path - one object) ?
 */

Directory::Directory(path inputPath):directoryPath(inputPath){
  /// There is no error handling in case the provided path is a file
  //TODO implement error handling
  if(exists(directoryPath) && is_directory(directoryPath)){
    //ok
  }
}

vector <Directory*> Directory::getSubdirectories(){
  vector<path> directoryContents = getDirectoryContents();
  subdirectories.clear();
  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it)){
      subdirectories.push_back(new Directory(*it));
    }
  } 
  return subdirectories;
}

vector<Photo*> Directory::getPhotos(){
  vector<path> directoryContents = getDirectoryContents();
  photos.clear();
  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      if( (*it).extension()==".jpg" ) // right now we only use jpg
        photos.push_back(Photo::initialize(*it));

  } 
  return photos;
}

path Directory::getPath(){
  return directoryPath;
}

string Directory::getName(){
  return directoryPath.filename().string();
}

//private methods

vector<path> Directory::getDirectoryContents(){
  vector<path> directoryContents;
  copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
  sort(directoryContents.begin(), directoryContents.end());
  return directoryContents;
}

