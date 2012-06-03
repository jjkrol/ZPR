#pragma once

#include <map>
#include <gtkmm.h>
#include <boost/filesystem.hpp>

class Disk;

class PhotoCache{
  public:
  static PhotoCache* getInstance();
  void storePixbuf(boost::filesystem::path path, Glib::RefPtr<Gdk::Pixbuf> pixbuf);
  Glib::RefPtr<Gdk::Pixbuf> getPhotoFile(boost::filesystem::path photoPath);

private:
  PhotoCache();
  PhotoCache& operator= (const PhotoCache &);
  PhotoCache (const PhotoCache&);
  virtual ~PhotoCache(){};

  static PhotoCache* instance;
  std::map<boost::filesystem::path, Glib::RefPtr< Gdk::Pixbuf> > photos;
  Disk * disk;
};
