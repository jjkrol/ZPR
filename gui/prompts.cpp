#include "../include/prompts.hpp"

DBPrompt::DBPrompt(MainWindow *w) : window(w), box(Gtk::ORIENTATION_HORIZONTAL),
  label("It seems like the photo database is not created. You must create photo database by setting folders in which Imagine should look for your photos.") {
  Gtk::Container *container = dynamic_cast<Gtk::Container*>(get_content_area());
  if(container) container->add(box);
  box.set_spacing(20);

  //adding image
  if(Gtk::Stock::lookup(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG, icon))
    box.pack_start(icon, false, false);

  //adding label
  label.set_line_wrap(true);
  box.pack_start(label, false, false);

  //adding button
  add_button("Create database", 0);
  signal_response().connect(sigc::mem_fun(*this, &DBPrompt::createDatabase));
}

/// Method responsible for displaying the DB Manager Dialog.
void DBPrompt::createDatabase(int response) {
  window->editDatabase();
}

UnsavedPhotosPrompt::UnsavedPhotosPrompt(MainWindow *w) : window(w),
  box(Gtk::ORIENTATION_HORIZONTAL),
  label("It seems like there are some unsaved photos. If you don't save them, changes will be lost.") {
  Gtk::Container *container = dynamic_cast<Gtk::Container*>(get_content_area());
  if(container) container->add(box);
  box.set_spacing(20);

  //adding image
  if(Gtk::Stock::lookup(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG, icon))
    box.pack_start(icon, false, false);

  //adding label
  label.set_line_wrap(true);
  box.pack_start(label, false, false);

  //adding button
  add_button("Close without saving", 0);
  add_button("Save photos", 1);
  add_button("Cancel", 2);
  signal_response().connect(sigc::mem_fun(window, &MainWindow::managePromptResponse));
}
