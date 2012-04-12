#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"
#include "../include/directory.hpp"

/// @fn LibraryView::LibraryView(MainWindow *w)
/// @brief LibraryView constructor - connects to UserInterface and builds library view gui.
/// @param w pointer to parent class MainWindow (needed for communication).
LibraryView::LibraryView(MainWindow *w) : window(w),
  tags_label("tags browsing"), db_prompt(NULL) {

  //obtaining UserInterface instance
  gui = UserInterface::getInstance();

  //organising widgets
  window->notebook.append_page(directory_tree, "Folders");
  window->notebook.append_page(tags_label, "Tags");
  window->display.add(images);

  //check for database - prompt if unavailable
  if(true)
    promptAboutDatabase();
  else
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

  std::vector<Directory*> dirs = gui->root_dir->getSubdirectories();
  Gtk::TreeModel::Row row;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
    row = *(directory_model->append());
    row[columns.name] = (*it)->getName();                 //adding label
    addSubdirectories(*it, row);                          //adding subdirectories
  }

  directory_tree.append_column("", columns.name);
  directory_tree.signal_row_activated().connect(sigc::mem_fun(*this, &LibraryView::loadImages));
}

/// @fn void LibraryView::addSubdirectories()
/// @brief Method responsible for adding subdirectories to directory tree.
/// @param dir Directory in which we look for subdirectories.
/// @param row Tree row to which we append new children.
void LibraryView::addSubdirectories(Directory *dir, Gtk::TreeModel::Row &row) {
  if(!dir->hasSubdirectories()) return;
  std::vector<Directory*> dirs = dir->getSubdirectories();

  Gtk::TreeModel::Row childrow;

  //filling tree
  for(std::vector<Directory*>::iterator it = dirs.begin(); it!=dirs.end(); ++it) {
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

/// @fn void LibraryView::promptAboutDatabase()
/// @brief Method responsible for prompting the user if db is not created.
void LibraryView::promptAboutDatabase() {
  //creating dialog
  db_prompt = new Gtk::InfoBar;
  Gtk::Box *box = new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
  Gtk::Container* container = dynamic_cast<Gtk::Container*>(db_prompt->get_content_area());
  if(container) container->add(*box);
  box->set_spacing(20);

  //adding image
  Gtk::Image *icon = new Gtk::Image;
  if(Gtk::Stock::lookup(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG, *icon))
    box->pack_start(*icon, false, false);

  //adding label
  Gtk::Label *label = new Gtk::Label("It seems like the photo database is not created. You must create photo database by setting folders in which Imagine should look for your photos.");
  box->pack_start(*label, false, false);
  db_prompt->add_button("Create database", 0);

  //displaying
  window->right_box.remove(window->display);
  window->right_box.pack_start(*db_prompt, false, false);
  window->right_box.pack_start(window->display, true, true);
  window->show_all_children();
}
