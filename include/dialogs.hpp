#pragma once

#include <gtkmm.h>
#include "./global.hpp"

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
    Glib::RefPtr<Gtk::TreeStore> directory_model;
    Gtk::TreeView directory_tree;
    DirTreeColumns dir_columns;

    //included folders list
    Glib::RefPtr<Gtk::TreeStore> included_model;
    Gtk::TreeView included_list;

    //signal handlers
    void selectFolder();
    void addFolderToDB();
    void removeFolderFromDB();

    //hidden methods
    DBManagerDialog() {};
};
