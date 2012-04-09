#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"
#include "../include/directory.hpp"
#include <iostream>
#include <string>

using std::vector;

/// @fn LibraryView::LibraryView(MainWindow *w)
/// @brief LibraryView constructor - connects to UserInterface and builds library view gui.
/// @param w pointer to parent class MainWindow (needed for communication).
LibraryView::LibraryView(MainWindow *w) : window(w),
  tags_label("tags browsing") {

  //obtaining UserInterface instance
  gui = UserInterface::getInstance();

  //organising widgets
  window->notebook.append_page(directory_tree, "Folders");
  window->notebook.append_page(tags_label, "Tags");
  window->display.add(images);

  //loading directory tree
  fillDirectoryTree();
}

/// @fn LibraryView::~LibraryView()
/// @brief LibraryView descructor - disconnects signals and resets widgets.
LibraryView::~LibraryView() {
  window->statusbar.set_label("");
}

/// @fn void LibraryView::fillDirectoryTree()
/// @brief Method responsible for loading directory tree from library.
void LibraryView::fillDirectoryTree() {
  directory_model = Gtk::TreeStore::create(columns);
  directory_tree.set_model(directory_model);

  vector<Directory*> dirs = gui->root_dir->getSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(directory_model->append());
    row[columns.name] = (*it)->getName();                 //adding label
    addSubdirectories(*it, row);                          //adding subdirectories
  }

  directory_tree.append_column("", columns.name);
  directory_tree.signal_row_activated().connect(sigc::mem_fun(*this, &LibraryView::loadImages));
}

/// @fn void LibraryView::fillDirectoryTree()
/// @brief Method responsible for loading directory tree from library.
void LibraryView::addSubdirectories(Directory *dir, Gtk::TreeModel::Row &row) {
  if(!dir->hasSubdirectories()) return;
  vector<Directory*> dirs = dir->getSubdirectories();

  Gtk::TreeModel::Row childrow;

  //filling tree
  for(vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    childrow = *(directory_model->append(row.children()));
    childrow[columns.name] = (*it)->getName();            //adding label
    addSubdirectories(*it, childrow);                     //adding subdirectories
  }
}

/// @fn void LibraryView::loadImages()
/// @brief Method (signal handler) responsible for updating thumbnails in right panel.
///        Called when directory from directory view is clicked.
/// @param path Path to selected row, provided by signal system.
/// @param column Clicked column, provided by signal system, not used.
void LibraryView::loadImages(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column) {
  Gtk::TreeModel::iterator row = directory_model->get_iter(path);
  std::stack<Glib::ustring> buffer;
  boost::filesystem::path dir_path;
  if(!row) return;

  //storing directory path
  while(row) {
    buffer.push((*row)[columns.name]);
    row = row->parent();
  }

  //writing directory path in reverse order
  while(!buffer.empty()) {
    dir_path /= Glib::filename_from_utf8(buffer.top());
    buffer.pop();
  }

  //creating directory object from path
  gui->current_dir = new Directory(dir_path);

  //loading photos
  if(gui->current_dir->hasPhotos()) {
    gui->photos = gui->current_dir->getPhotos();
    gui->current_photo = gui->photos.begin();
    window->showEditView();
  }
}
