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

using namespace boost::gil;
using namespace std;



Photo* Photo::initialize(boost::filesystem::path argumentPath){
  /// photos are saved in a map indexed by paths, so you can't initialize
  /// two instances of the same photo.

  static std::map<boost::filesystem::path, Photo*> initializedPhotos;

  map<boost::filesystem::path, Photo*>::iterator foundPhoto = initializedPhotos.find(argumentPath);

  if ( foundPhoto != initializedPhotos.end() ){
    return (*foundPhoto).second;
  }
  else{
    Photo* createdPhoto = new Photo(argumentPath);
    initializedPhotos[argumentPath] = createdPhoto;
    return createdPhoto; 
  }
}

Photo::Photo(boost::filesystem::path argumentPath):photoPath(argumentPath){
}

Photo::~Photo(){
  static std::map<boost::filesystem::path, Photo*> initializedPhotos;
  initializedPhotos.erase(photoPath);
}

rgb8_image_t Photo::getThumbnail(){
  rgb8_image_t image;
  jpeg_read_image(photoPath.string(), image);
  return image;
}

rgb8_image_t Photo::getImage(){
  //TODO optimisation
  //TODO image type
  rgb8_image_t image;
  jpeg_read_image(photoPath.string(), image);
  return image;
}

boost::filesystem::path Photo::getPath(){
  // temporary function, to be deleted later 
  // accessing photos through direct path is not advised
  return photoPath;
}

string Photo::getFilename(){
  return photoPath.filename().string();
}
