#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/window.hpp"

/// @fn LibraryView::LibraryView(MainWindow *w)
/// @brief LibraryView constructor - connects to UserInterface and builds library view gui.
/// @param w pointer to parent class MainWindow (needed for communication).
LibraryView::LibraryView(MainWindow *w) : window(w), db_prompt(NULL) {
  //obtaining CoreController instance
  core = CoreController::getInstance();

  //organising widgets
  window->notebook.append_page(directory_view, "Folders");
  window->notebook.append_page(tags_view, "Tags");
  window->display.add(images);

  if(!core->hasLibraryPathSet()) {
    promptAboutDatabase();
  }
  else {
    fillDatabaseTree();
    fillTagsList();
  }
}

/// @fn LibraryView::~LibraryView()
/// @brief LibraryView descructor - disconnects signals and resets widgets.
LibraryView::~LibraryView() {
  window->statusbar.set_label("");
  if(db_prompt) {
    window->right_box.remove(*db_prompt);
    delete db_prompt;
    db_prompt = NULL;
  }
}

/// @fn LibraryView::refreshView()
/// @brief LibraryView method for refreshing the view.
void LibraryView::refreshView() {
  if(db_prompt) {
    window->right_box.remove(*db_prompt);
    delete db_prompt;
    db_prompt = NULL;
  }

  directory_tree = core->getDirectoryTree();
  directory_view.set_model(directory_tree);
  tags_list = core->getTagsList();
  tags_view.set_model(tags_list);

  if(!core->hasLibraryPathSet())
    promptAboutDatabase();

  window->show_all_children();
}

/// @fn void LibraryView::fillDirectoryTree()
/// @brief Method responsible for loading directory tree from library.
void LibraryView::fillDatabaseTree() {
  directory_tree = core->getDirectoryTree();
  directory_view.set_model(directory_tree);
  directory_view.append_column("Directory Tree", dir_columns.name);
  directory_view.signal_row_activated().connect(sigc::mem_fun(*this,
        &LibraryView::loadImagesByDirectory));
}

/// @fn void LibraryView::fillTagsList()
/// @brief Method responsible for loading tags list from library.
void LibraryView::fillTagsList() {
  tags_list = core->getTagsList();
  tags_view.set_model(tags_list);
  tags_view.append_column("Tags List", tags_columns.name);
  Gtk::TreeViewColumn *column = tags_view.get_column(0);
  if(column) column->set_expand(true);
  tags_view.append_column_editable("", tags_columns.selected);
  tags_view.signal_cursor_changed().connect(sigc::mem_fun(*this,
        &LibraryView::loadImagesByTags));
}

/// @fn void LibraryView::loadImagesByDirectory()
/// @brief Method (signal handler) responsible for updating thumbnails in right panel.
///        Called when directory from directory view is clicked.
/// @param path Path to selected row, provided by signal system.
/// @param column Clicked column, provided by signal system, not used.
void LibraryView::loadImagesByDirectory(const Gtk::TreeModel::Path &path,
    Gtk::TreeViewColumn *column) {
  Gtk::TreeModel::iterator row = directory_tree->get_iter(path);
  std::stack<Glib::ustring> buffer;
  boost::filesystem::path dir_path;
  if(!row) return;

  //storing directory path
  while(row) {
    buffer.push((*row)[dir_columns.name]);
    row = row->parent();
  }

  //writing directory path in reverse order
  while(!buffer.empty()) {
    dir_path /= Glib::filename_from_utf8(buffer.top());
    buffer.pop();
  }

  //loading photos
  if(core->hasPhotos(dir_path)) {
    core->setCurrentDirectory(dir_path);
    window->showEditView();
  }
}

/// @fn void LibraryView::loadImagesByTags()
/// @brief Method (signal handler) responsible for updating thumbnails in right panel.
///        Called when tag from tags list is selected.
/// @param path Path to selected row, provided by signal system.
/// @param column Clicked column, provided by signal system, not used.
void LibraryView::loadImagesByTags() {
  Gtk::TreeModel::Children children = tags_list->children();
  Gtk::TreeModel::Children::iterator iter = children.begin();
  Gtk::TreeModel::Row row;
  std::set<std::string> selected_tags;

  //looking for selected tags
  for(; iter != children.end(); ++iter) {
    row = *iter;
    if((*row)[tags_columns.selected])
      selected_tags.insert((std::string)(*row)[tags_columns.name]);
  }

  //loading photos
  if(!selected_tags.empty()) {
    core->setCurrentTagSet(selected_tags);
    //window->showEditView();
  }
}
 

/// @fn void LibraryView::promptAboutDatabase()
/// @brief Method responsible for prompting the user if db is not created.
void LibraryView::promptAboutDatabase() {
  //creating dialog
  db_prompt = new Gtk::InfoBar;
  Gtk::Box *box = new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
  Gtk::Container *container = dynamic_cast<Gtk::Container*>(db_prompt->get_content_area());
  if(container) container->add(*box);
  box->set_spacing(20);

  //adding image
  Gtk::Image *icon = new Gtk::Image;
  if(Gtk::Stock::lookup(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG, *icon))
    box->pack_start(*icon, false, false);

  //adding label
  Gtk::Label *label = new Gtk::Label("It seems like the photo database is not created. You must create photo database by setting folders in which Imagine should look for your photos.");
  label->set_line_wrap(true);
  box->pack_start(*label, false, false);

  //adding button
  db_prompt->add_button("Create database", 0);
  db_prompt->signal_response().connect(sigc::mem_fun(*this,
                       &LibraryView::createDatabase));

  //displaying
  window->right_box.remove(window->display);
  window->right_box.pack_start(*db_prompt, false, false);
  window->right_box.pack_start(window->display, true, true);
  window->show_all_children();
}

/// @fn void LibraryView::createDatabase(int response)
/// @brief Method responsible for displaying the DB Manager Dialog.
void LibraryView::createDatabase(int response) {
  window->editDatabase();
}
