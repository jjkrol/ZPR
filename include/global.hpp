#pragma once

#include <boost/filesystem.hpp>
#include <gtkmm.h>

struct PhotoData {
  boost::filesystem::path path;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};

class TagsListColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    TagsListColumns() {
      add(name);
      add(stock_id);
      add(selected);
    }
    Gtk::TreeModelColumn<std::string> name;
    Gtk::TreeModelColumn<std::string> stock_id;
    Gtk::TreeModelColumn<bool> selected;
};

class DirectoryTreeColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    DirectoryTreeColumns() {
      add(name);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
};

class FilesystemTreeColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    FilesystemTreeColumns() {
      add(name);
      add(path);
      add(stock_id);
      add(included);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<std::string> path;
    Gtk::TreeModelColumn<std::string> stock_id;
    Gtk::TreeModelColumn<bool> included;
};
