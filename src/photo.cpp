#include "../include/photo.hpp"

Photo::Photo(boost::filesystem::path argumentPath):photoPath(argumentPath){
}

void Photo::getThumbnail(){

}

void Photo::getImage(){

}

boost::filesystem::path Photo::getPath(){
  // temporary function, to be deleted later 
  // accessing photos through direct path is not advised
 return photoPath;
}
