#include "../include/core.hpp"
#include "../include/global.hpp"
#include "../include/dialogs.hpp"

DBManagerDialog::DBManagerDialog(Gtk::Window *parent) :
  Gtk::Dialog("Database Manager", *parent), box(Gtk::ORIENTATION_HORIZONTAL),
  right_box(Gtk::ORIENTATION_VERTICAL), button_box(Gtk::ORIENTATION_VERTICAL),
  frame("Folder options"), info("Please select folders in which Imagine should look for your photos. Selected folders will be scanned for new photos automatically every time you launch Imagine."),
  ignore_button("Exclude from database"), scan_button("Scan for photos automatically") {

  //obtaining CoreController instance
  core = CoreController::getInstance();
  main_window = static_cast<MainWindow*>(parent);

  //setting size
  set_size_request(600, 400);

  //adding buttons
  add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //adding and setting widgets
  info.set_line_wrap(true);
  Gtk::Box *content_area = get_content_area();
  content_area->pack_start(box, true, true);
  box.set_spacing(5);
  box.set_margin_left(2);
  box.set_margin_right(2);
  box.set_margin_top(2);
  box.set_margin_bottom(2);
  box.pack_start(scroll, true, true);
  box.pack_start(right_box, false, false);
  right_box.pack_start(info, true, true);
  right_box.pack_start(frame, false, false);
  scroll.set_min_content_width(350);
  scroll.add(directory_tree);

  //loading directory tree
  directory_model = core->getFilesystemTree();
  directory_tree.set_model(directory_model);
  directory_tree.signal_cursor_changed().connect(sigc::mem_fun(
        *this, &DBManagerDialog::selectFolder));
  directory_tree.signal_row_expanded().connect(sigc::mem_fun(
        *core, &CoreController::expandDirectory));

  //appending columns
  directory_tree.append_column("Folder List", dir_columns.name);
  Gtk::TreeViewColumn *column = directory_tree.get_column(0);
  if(column) column->set_expand(true);
  Gtk::CellRendererPixbuf *cell = Gtk::manage(new Gtk::CellRendererPixbuf);
  directory_tree.append_column("", *cell);
  column = directory_tree.get_column(1);
  if(column) {
    column->add_attribute(cell->property_stock_id(), dir_columns.stock_id);
    column->add_attribute(cell->property_visible(), dir_columns.included);
  }

  //editing folder options frame
  ignore_button.signal_clicked().connect(sigc::mem_fun(
        *this, &DBManagerDialog::removeFolderFromDB));
  scan_button.signal_clicked().connect(sigc::mem_fun(
        *this, &DBManagerDialog::addFolderToDB));
  Gtk::RadioButton::Group group = ignore_button.get_group();
  button_box.pack_start(ignore_button, false, false);
  button_box.pack_start(scan_button, false, false);
  scan_button.set_group(group);
  frame.add(button_box);

  //handling OK/Cancel/Apply buttons
  this->signal_response().connect(sigc::mem_fun(
        *this, &DBManagerDialog::handleButtonPush));
  
  //displaying dialog
  show_all_children();
}

void DBManagerDialog::selectFolder() {
  //acquiring row
  Gtk::TreeModel::Path path;
  Gtk::TreeView::Column *column;
  directory_tree.get_cursor(path, column);
  Gtk::TreeModel::iterator row = directory_model->get_iter(path);

  //setting frame label
  std::string label = (*row)[dir_columns.path];
  frame.set_label(label);

  //selecting proper RadioButton
  if((*row)[dir_columns.included]) scan_button.set_active();
  else ignore_button.set_active();
}

void DBManagerDialog::addFolderToDB() {
  //checking if anything in tree is selected
  if(frame.get_label() == "Folder options") return;

  //sending message to core
  Gtk::TreeModel::Path path;
  Gtk::TreeView::Column *column;
  directory_tree.get_cursor(path, column);
  Gtk::TreeModel::iterator folder = directory_model->get_iter(path);
  core->addFolderToDB(folder);
}

void DBManagerDialog::removeFolderFromDB() {
  //checking if anything in tree is selected
  if(frame.get_label() == "Folder options") return;

  //sending message to core
  Gtk::TreeModel::Path path;
  Gtk::TreeView::Column *column;
  directory_tree.get_cursor(path, column);
  Gtk::TreeModel::iterator folder = directory_model->get_iter(path);
  core->removeFolderFromDB(folder);
}

void DBManagerDialog::handleButtonPush(int button_id) {
  switch(button_id) {
    case Gtk::RESPONSE_OK:
      core->sendChangesToDB();
      main_window->refreshActiveView();
      break;
    case Gtk::RESPONSE_APPLY:
      /// @todo make dialog stay on screen
      core->sendChangesToDB();
      main_window->refreshActiveView();
      break;
    case Gtk::RESPONSE_CANCEL:
      core->cancelDBChanges();
      break;
  }
}

PreferencesDialog::PreferencesDialog(Gtk::Window *parent) : 
  Gtk::Dialog("Preferences", *parent) {

  //obtaining CoreController instance
  core = CoreController::getInstance();

  //setting size
  set_size_request(600, 400);

  //adding buttons
  add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
}

void PreferencesDialog::handleButtonPush(int button_id) {
  switch(button_id) {
    case Gtk::RESPONSE_OK:
      //send changes to core
      //quit
      break;
    case Gtk::RESPONSE_APPLY:
      //send changes to core
      break;
    case Gtk::RESPONSE_CANCEL:
      //quit
      break;
  }
}

TagsDialog::TagsDialog(Gtk::Window *parent) : 
  Gtk::Dialog("Edit Photo Tags", *parent), add_tag_button(Gtk::Stock::ADD),
  box(Gtk::ORIENTATION_VERTICAL), top_box(Gtk::ORIENTATION_HORIZONTAL) {

  //obtaining CoreController instance
  core = CoreController::getInstance();

  //setting size
  set_size_request(300, 300);

  //adding buttons
  add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

  //adding and setting widgets
  Gtk::Box *content_area = get_content_area();
  content_area->pack_start(box, true, true);
  box.set_spacing(5);
  box.set_margin_left(2);
  box.set_margin_right(2);
  box.set_margin_top(2);
  box.set_margin_bottom(2);
  box.pack_start(top_box, false, false);
  box.pack_start(scroll, true, true);
  top_box.pack_start(tag_entry, true, true);
  top_box.pack_start(add_tag_button, false, false);
  scroll.add(tags_view);

  //creating tags list
  tags_list = core->getTagsOfActivePhoto();
  tags_view.set_model(tags_list);
  tags_view.append_column("", tags_columns.name);
  Gtk::TreeViewColumn *column = tags_view.get_column(0);
  if(column) column->set_expand(true);
  Gtk::CellRendererPixbuf *cell = Gtk::manage(new Gtk::CellRendererPixbuf);
  tags_view.append_column("", *cell);
  column = tags_view.get_column(1);
  if(column)
    column->add_attribute(cell->property_stock_id(), tags_columns.stock_id);

  //connecting signals
  add_tag_button.signal_clicked().connect(sigc::mem_fun(
        *this, &TagsDialog::addTag));
  tags_view.signal_row_activated().connect(sigc::mem_fun(*this,
        &TagsDialog::deleteTag));
  
  //displaying dialog
  show_all_children();
}

void TagsDialog::refreshTagsList() {
  tags_list = core->getTagsOfActivePhoto();
  tags_view.set_model(tags_list);
}

void TagsDialog::addTag() {
  Glib::RefPtr<Gtk::EntryBuffer> buffer = tag_entry.get_buffer();
  core->addTagToActivePhoto(buffer->get_text());
  refreshTagsList();
}

void TagsDialog::deleteTag(const Gtk::TreeModel::Path &path,
    Gtk::TreeViewColumn *column) {
  if(column != tags_view.get_column(1)) return;
  Gtk::TreeModel::iterator row = tags_list->get_iter(path);
  core->removeTagFromActivePhoto((std::string)(*row)[tags_columns.name]);
  refreshTagsList();
}
