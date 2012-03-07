/** \class Directory
 * A class representing a single directory, providing its photos and subdirectories.
 */
#include "../include/directory.hpp"

using namespace boost::filesystem;
using namespace std;
/* TODO
 * add cached directories
 * add private get directory content
 */

Directory::Directory(path inputPath):directoryPath(inputPath){
  /// There is no error handling in case the provided path is a file
  if(exists(directoryPath) && is_directory(directoryPath)){
    // everythings ok
  }
}

vector <Directory*> Directory::getSubdirectories(){
    vector<path> directoryContents = getDirectoryContents();
    for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
      if(is_directory(*it)){
        subdirectories.push_back(new Directory(*it));
      }
    } 
  return subdirectories;
}

vector <Directory*> Directory::getCachedSubdirectories(){
  return subdirectories;
}

vector<Photo*> Directory::getPhotos(){
    vector<path> directoryContents = getDirectoryContents();
    for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
      if(!is_directory(*it)){
        if( (*it).extension()==".jpg" ) // right now we only use jpg
          photos.push_back(Photo::initialize(*it));
      }
    } 
  return photos;
}
vector<Photo*> Directory::getCachedPhotos(){
  return photos;
}

path Directory::getPath(){
  return directoryPath;
}

vector<path> Directory::getDirectoryContents(){
    vector<path> directoryContents;
    copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
    return directoryContents;
}
