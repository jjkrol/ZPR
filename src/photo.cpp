/** \class Photo
 * A class representing a single photo. Main features (most to be
 * yet implemented):
 *  - a real photo (file or db entry) has *only one* object representing it.
 *  this means that you can't have two Photo objects representing an image
 *  with the same id/path. This implies some manipulations in constructor
 *  (similar to singleton maybe?)
 *  - providing thumbnails and full image
 *  - changing tags
 *  - moving and deleting the photo
 *  - placing effects on the photo and getting effect previews of certain size
 */
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
