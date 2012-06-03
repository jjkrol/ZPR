#pragma once

#include <gtkmm.h>
#include "./global.hpp"
#include "./window.hpp"

class CoreController;

class DBManagerDialog : public Gtk::Dialog {
  public:
    DBManagerDialog(Gtk::Window *);
    ~DBManagerDialog() {};

  private:
    //connection with CoreController class
    CoreController *core;
    MainWindow *main_window;

    //widgets
    Gtk::Box box, right_box, button_box;
    Gtk::Frame frame;
    Gtk::Label info;
    Gtk::ScrolledWindow scroll;
    Gtk::RadioButton ignore_button, scan_button;

    //directory tree
    Glib::RefPtr<Gtk::TreeStore> directory_model;
    Gtk::TreeView directory_tree;
    FilesystemTreeColumns dir_columns;

    //signal handlers
    void selectFolder();
    void addFolderToDB();
    void removeFolderFromDB();
    void handleButtonPush(int);

    //hidden methods
    DBManagerDialog() {};
};

class PreferencesDialog : public Gtk::Dialog {
  public:
    PreferencesDialog(Gtk::Window *);
    ~PreferencesDialog() {};

  private:
    //connection with CoreController class
    CoreController *core;

    //widgets

    //signal handlers
    void handleButtonPush(int);

    //hidden methods
    PreferencesDialog() {};
};

class TagsDialog : public Gtk::Dialog {
  public:
    TagsDialog(Gtk::Window *);
    ~TagsDialog() {};

  private:
    //connection with CoreController class
    CoreController *core;

    //widgets
    Gtk::ToolButton add_tag_button;
    Gtk::Box box, top_box;
    Gtk::Entry tag_entry;
    Gtk::ScrolledWindow scroll;
    
    //tags list
    Glib::RefPtr<Gtk::ListStore> tags_list;
    Gtk::TreeView tags_view;
    TagsListColumns tags_columns;

    //additional functions
    void refreshTagsList();

    //signal handlers
    void addTag();
    void deleteTag(const Gtk::TreeModel::Path &path,
        Gtk::TreeViewColumn *column);

    //hidden methods
    TagsDialog() {};
};
