#include "../include/core.hpp"
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
  box.set_margin_left(5);
  box.set_margin_right(5);
  box.set_margin_top(5);
  box.set_margin_bottom(5);
  box.pack_start(scroll, true, true);
  box.pack_start(right_box, true, true);
  right_box.pack_start(info, true, true);
  right_box.pack_start(frame, false, false);
  scroll.add(directory_tree);

  //editing folder options frame
  frame.set_shadow_type(Gtk::SHADOW_IN);        //WTF?!
  Gtk::RadioButton::Group group = ignore_button.get_group();
  scan_button.set_group(group);
  button_box.pack_start(ignore_button, false, false);
  button_box.pack_start(scan_button, false, false);
  frame.add(button_box);
  
  //loading directory tree
  //@TODO - create core method for this
  directory_model = core->getDirectoryTree();
  directory_tree.set_model(directory_model);
  directory_tree.append_column("Folder List", columns.name);
  directory_tree.signal_row_activated().connect(sigc::mem_fun(
        *this, &DBManagerDialog::selectFolder));

  //displaying window
  show_all_children();
}

void DBManagerDialog::selectFolder(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column) {
  Gtk::TreeModel::iterator row = directory_model->get_iter(path);
  frame.set_label((*row)[columns.name]);
}
