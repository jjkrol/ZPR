/** \class Directory
 * A class representing a single directory, providing its photos and subdirectories.
 */
#include "../include/directory.hpp"

using namespace boost::filesystem;
using namespace std;

Directory::Directory(path inputPath):directoryPath(inputPath){
  /// There is no error handling in case the provided path is a file
  if(exists(directoryPath) && is_directory(directoryPath)){

    vector<path> directoryContents;

    copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));

    for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){

      if(is_directory(*it)){
        subdirectories.push_back(new Directory(*it));
      }
      else{
        if( (*it).extension()==".jpg" ) // right now we only use jpg
          photos.push_back(new Photo(*it));
      }

    } //for

  }


}

std::vector <Directory*> Directory::getSubdirectories(){
  return subdirectories;
}

std::vector<Photo*> Directory::getPhotos(){
  return photos;
}

path Directory::getPath(){
  return directoryPath;
}


