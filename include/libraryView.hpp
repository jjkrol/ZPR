#pragma once

#include <gtkmm.h>
//#include <vector>
#include "./global.hpp"

/** @class LibraryView
 *  @brief Uses State pattern to switch MainWindow to library view
 *         and handles tasks associated with this view.
 */
class LibraryView : public WindowContent {
  public:
    friend class MainWindow;
    friend class UserInterface;

    virtual void refreshView();

  private:
    LibraryView() {};
    ~LibraryView();
    LibraryView(MainWindow *);

    //connection with upper classes
    CoreController *core;
    MainWindow *window;

    //widgets
    Gtk::DrawingArea images;

    //database prompt
    Gtk::InfoBar *db_prompt;

    //directory tree
    Glib::RefPtr<Gtk::TreeStore> directory_tree;
    Gtk::TreeView directory_view;
    DirectoryTreeColumns dir_columns;

    //tags list
    Glib::RefPtr<Gtk::ListStore> tags_list;
    Gtk::TreeView tags_view;
    TagsListColumns tags_columns;

    //signal handlers
    void loadImagesByDirectory(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
    void loadImagesByTags();

    //other methods
    void fillTagsList();
    void fillDatabaseTree();
    void promptAboutDatabase();
    void addSubdirectories(Directory *, Gtk::TreeModel::Row &);
};
