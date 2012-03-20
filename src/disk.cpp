
#include "../include/disk.hpp"

using namespace boost::filesystem;
using namespace boost::gil;
using namespace std;

Disk::Disk(){
  CoreController * core = CoreController::initialize();
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

rgb8_image_t Disk::getPhotoFile(path photoPath){
  rgb8_image_t image;
  path absolutePath = makeAbsolutePath(photoPath);
  jpeg_read_image(absolutePath.string(), image);
  return image;
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
