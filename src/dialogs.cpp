#include "../include/core.hpp"
#include "../include/global.hpp"
#include "../include/dialogs.hpp"

DBManagerDialog::DBManagerDialog(Gtk::Window *parent) :
  Gtk::Dialog("Database Manager", *parent), box(Gtk::ORIENTATION_HORIZONTAL),
  right_box(Gtk::ORIENTATION_VERTICAL), button_box(Gtk::ORIENTATION_VERTICAL),
  frame("Folder options"), info("Select dirs, lol."),
  ignore_button("Exclude from database"), scan_button("Scan for photos automatically") {

  //obtaining CoreController instance
  core = CoreController::getInstance();

  //setting size
  set_size_request(600, 400);

  //adding buttons
  add_button(Gtk::Stock::OK, 0);
  add_button(Gtk::Stock::APPLY, 1);
  add_button(Gtk::Stock::CANCEL, 2);

  //adding contents
  Gtk::Box *content_area = get_content_area();
  content_area->pack_start(box, true, true);
  box.set_spacing(5);
  box.set_margin_left(2);
  box.set_margin_right(2);
  box.set_margin_top(2);
  box.set_margin_bottom(2);
  box.pack_start(scroll, true, true);
  box.pack_start(right_box, true, true);
  right_box.pack_start(info, true, true);
  right_box.pack_start(frame, false, false);
  scroll.add(directory_tree);

  //editing folder options frame
  frame.set_shadow_type(Gtk::SHADOW_IN);        //WTF?!
  Gtk::RadioButton::Group group = ignore_button.get_group();
  button_box.pack_start(ignore_button, false, false);
  button_box.pack_start(scan_button, false, false);
  scan_button.set_group(group);
  frame.add(button_box);
  
  //loading directory tree
  directory_model = core->getDirectoryTree();
  directory_tree.set_model(directory_model);
  directory_tree.append_column("Folder List", dir_columns.name);
  directory_tree.signal_cursor_changed().connect(sigc::mem_fun(
        *this, &DBManagerDialog::selectFolder));

  //displaying window
  show_all_children();
}

void DBManagerDialog::selectFolder() {
  Gtk::TreeModel::Path path;
  Gtk::TreeView::Column *column;
  directory_tree.get_cursor(path, column);
  Gtk::TreeModel::iterator row = directory_model->get_iter(path);

  //setting frame label
  frame.set_label((*row)[dir_columns.name]);

  //selecting proper RadioButton
  if((*row)[dir_columns.included]) scan_button.set_active();
  else ignore_button.set_active();
}
