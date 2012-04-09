#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"
#include <iostream>

/// @fn LibraryView::LibraryView(MainWindow *w)
/// @brief LibraryView constructor - connects to UserInterface and builds library view gui.
/// @param w pointer to parent class MainWindow (needed for communication).
LibraryView::LibraryView(MainWindow *w) : window(w),
  tags_label("tags browsing") {

  //obtaining UserInterface instance
  gui = UserInterface::getInstance();

  //organising widgets
  window->left_box.remove(window->notebook);
  window->left_box.remove(window->toolbar);
  window->left_box.pack_start(window->toolbar, false, false);
  window->left_box.pack_start(window->notebook, true, true);
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
  window->notebook.remove_page(directory_tree);
  window->notebook.remove_page(tags_label);
}

/// @fn void LibraryView::fillDirectoryTree()
/// @brief Method responsible for displaying directories from library in Gtk::TreeView.
void LibraryView::fillDirectoryTree() {
  directory_model = Gtk::TreeStore::create(columns);
  directory_tree.set_model(directory_model);

  Gtk::TreeModel::Row row = *(directory_model->append());
  row[columns.name] = "2012";

  Gtk::TreeModel::Row childrow = *(directory_model->append(row.children()));
  childrow[columns.name] = "zdjecia1";

  childrow = *(directory_model->append(row.children()));
  childrow[columns.name] = "zdjecia2";

  row = *(directory_model->append());
  row[columns.name] = "2011";

  childrow = *(directory_model->append(row.children()));
  childrow[columns.name] = "zdjecia3";

  directory_tree.append_column("", columns.name);
  
  directory_tree.signal_row_activated().connect(sigc::mem_fun(*this, &LibraryView::loadImages));
}

/// @fn void LibraryView::loadImages()
/// @brief Method (signal handler) responsible for updating thumbnails in right panel.
///        Called when directory from directory view is clicked.
/// @param path Path to selected row, provided by signal system.
/// @param column Clicked column, provided by signal system, not used.
void LibraryView::loadImages(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column) {
  Gtk::TreeModel::iterator row = directory_model->get_iter(path);
  if(row) std::cout << (*row)[columns.name] << " selected" << std::endl;
}
