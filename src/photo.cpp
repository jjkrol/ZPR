#include "../include/photo.hpp"

using namespace boost::gil;
using namespace std;



Photo* Photo::initialize(boost::filesystem::path argumentPath){
  /// photos are saved in a map indexed by paths, so you can't initialize
  /// two instances of the same photo.

  static map<boost::filesystem::path, Photo*> initializedPhotos;

  map<boost::filesystem::path, Photo*>::iterator foundPhoto = initializedPhotos.find(argumentPath);

  if (foundPhoto != initializedPhotos.end()){
    return (*foundPhoto).second;
  }
  else{
    Photo* createdPhoto = new Photo(argumentPath);
    initializedPhotos[argumentPath] = createdPhoto;
    return createdPhoto; 
  }
}

Photo::Photo(boost::filesystem::path argumentPath):photoPath(argumentPath){
  disk = new Disk();
}

Photo::~Photo(){
  static map<boost::filesystem::path, Photo*> initializedPhotos;
  initializedPhotos.erase(photoPath);
}

rgb8_image_t Photo::getThumbnail(){
  return getImage();
}

rgb8_image_t Photo::getImage(){
  return disk->getPhotoFile(photoPath);
}

Glib::RefPtr<Gdk::Pixbuf> Photo::getPixbuf(){
  rgb8_image_t image;

  image = getImage();

  const rgb8_image_t::view_t image_view = view(image);
  Glib::RefPtr<Gdk::Pixbuf> ptr = Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(&image),
      Gdk::COLORSPACE_RGB,
      false,
      8,
      image.width(),
      image.height(),
      image.width() * 3);
  return ptr;
}

boost::filesystem::path Photo::getPath(){
  return photoPath;
}

boost::filesystem::path Photo::getFilename(){
  return photoPath.filename();
}
