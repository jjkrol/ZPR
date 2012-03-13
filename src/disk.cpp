/** \class Disk
 * A disk class providing an adapter to the disk space.
 */

#include "../include/disk.hpp"
using namespace boost::filesystem;
using namespace std;

Disk::Disk(){
  libraryDirectoryPath = "./test/test_tree"; //FIXME shouldn't be hardcoded
}

vector <path> Disk::getSubdirectories(path directoryPath){
  vector<path> subdirectories;

  path absolutePath = makeAbsolutePath(directoryPath);
  vector<path> directoryContents = getDirectoryContents(absolutePath);

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it))
      subdirectories.push_back((*it).filename());
  } 

  return subdirectories;
}

vector<path> Disk::getPhotos(path directoryPath){
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
  if(getPhotos(directoryPath).size() == 0)
    return false;
  else
    return true;
/*
  path absolutePath = makeAbsolutePath(directoryPath);
  vector<path> directoryContents = getDirectoryContents(absolutePath);

  if(directoryContents.size() == 0)
    return false;

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(!is_directory(*it))
      return true;
  }

  return false;*/
}

bool Disk::hasSubdirectories(path directoryPath){
  if(getSubdirectories(directoryPath).size() == 0)
    return false;
  else
    return true;
/*  path absolutePath = makeAbsolutePath(directoryPath);
  vector<path> directoryContents = getDirectoryContents(absolutePath);

  if(directoryContents.size() == 0)
    return false;

  for(vector<path>::const_iterator it (directoryContents.begin()); it != directoryContents.end(); ++it){
    if(is_directory(*it))
      return true;
  }

  return false;*/
}

//private methods

vector<path> Disk::getDirectoryContents(path directoryPath){
  vector<path> directoryContents;
  copy(directory_iterator(directoryPath), directory_iterator(), back_inserter(directoryContents));
  sort(directoryContents.begin(), directoryContents.end());
  return directoryContents;
}

path Disk::makeAbsolutePath(path relativePath){
  return path(libraryDirectoryPath.string() + relativePath.string());
  //  return libraryDirectoryPath.append(relativePath);
}
