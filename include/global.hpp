#pragma once

#include <boost/filesystem.hpp>
#include <gtkmm.h>

struct PhotoData {
  boost::filesystem::path path;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};

class DbTreeColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    DbTreeColumns() {
      add(name);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
};

class DirTreeColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    DirTreeColumns() {
      add(name);
      add(included);
      add(path);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<std::string> path;
    Gtk::TreeModelColumn<bool> included;
};
