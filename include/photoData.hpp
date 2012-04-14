#pragma once

#include <boost/filesystem.hpp>
#include <gtkmm.h>

struct PhotoData{
  boost::filesystem::path path;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};
