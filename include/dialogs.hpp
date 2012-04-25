#pragma once

#include <gtkmm.h>

class CoreController;

class DBManagerDialog : public Gtk::Dialog {
  public:
    DBManagerDialog(Gtk::Window *);
    ~DBManagerDialog() {};

  private:
    //connection with CoreController class
    CoreController *core;

    //widgets
    Gtk::Box box, right_box, button_box;
    Gtk::Frame frame;
    Gtk::Label info;
    Gtk::ScrolledWindow scroll;
    Gtk::RadioButton ignore_button, scan_button;

    //directory tree
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        ModelColumns() { add(name); }
        Gtk::TreeModelColumn<Glib::ustring> name;
    } columns;
    Glib::RefPtr<Gtk::TreeStore> directory_model;
    Gtk::TreeView directory_tree;

    //included folders list
    Glib::RefPtr<Gtk::TreeStore> included_model;
    Gtk::TreeView included_list;

    //signal handlers
    void selectFolder(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

    //hidden methods
    DBManagerDialog() {};
};
