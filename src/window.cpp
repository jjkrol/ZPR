#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/dialogs.hpp"
#include "../include/window.hpp"

/// @fn MainWindow::MainWindow()
/// @brief MainWindow constructor - connects to UserInterface, creates menubar,
///        and builds GUI widgets common for both views.
MainWindow::MainWindow() : zoom_slider(Gtk::ORIENTATION_HORIZONTAL),
  zoom_icon(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON),
  save_button(Gtk::Stock::FLOPPY), delete_button(Gtk::Stock::DELETE),
  edit_button(Gtk::Stock::EDIT), tags_button(Gtk::Stock::INDEX) {

  //obtaining CoreController instance
  core = CoreController::getInstance();

  //customizing window
  set_title("Imagine");
  maximize();

  //editing zoom slider
  zoom_slider.set_draw_value(false);
  zoom_slider.set_show_fill_level(true);
  zoom_slider.set_size_request(200, -1);

  //adding tooltips to buttons
  save_button.set_tooltip_text("Save image to disk");
  delete_button.set_tooltip_text("Delete image from disk");
  edit_button.set_tooltip_text("Edit image in external editor");
  tags_button.set_tooltip_text("Add/delete image tags");

  //connecting signals
  edit_button.signal_clicked().connect(sigc::mem_fun(core,
        &CoreController::editInExternalEditor));
  tags_button.signal_clicked().connect(sigc::mem_fun(*this,
        &MainWindow::editPhotoTags));
  save_button.signal_clicked().connect(sigc::mem_fun(core,
        &CoreController::saveCurrentPhoto));

  //menubar creating
  Gtk::RadioAction::Group view_type;
  action_group = Gtk::ActionGroup::create();
  action_group->add(Gtk::Action::create("FileMenu", "File")); 
  action_group->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT, "_Quit",
                    "Quit"), sigc::mem_fun(*this, &MainWindow::hide));
  action_group->add(Gtk::Action::create("EditMenu", "Edit")); 
  action_group->add(Gtk::Action::create("EditPreferences",
                    Gtk::Stock::PREFERENCES, "_Preferences", "Preferences"),
                    sigc::mem_fun(*this, &MainWindow::editPreferences));
  action_group->add(Gtk::Action::create("EditDatabase",
                    Gtk::Stock::DIRECTORY, "_Database", "Database Manager"),
                    sigc::mem_fun(*this, &MainWindow::editDatabase));
  action_group->add(Gtk::Action::create("ViewMenu", "View")); 
  library_view = Gtk::RadioAction::create(view_type, "LibraryView", "Library View");
  action_group->add(library_view, sigc::mem_fun(*this, &MainWindow::showLibraryView));
  edit_view = Gtk::RadioAction::create(view_type, "EditView", "Edit View");
  action_group->add(edit_view, sigc::mem_fun(*this, &MainWindow::showEditView));
  action_group->add(Gtk::Action::create("HelpMenu", "Help")); 
  action_group->add(Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT, "_About",
                    "About"), sigc::mem_fun(*this, &MainWindow::showAbout));
  ui_manager = Gtk::UIManager::create();
  ui_manager->insert_action_group(action_group);
  add_accel_group(ui_manager->get_accel_group());

  try {
    ui_manager->add_ui_from_file("menubar.xml");
  } catch(const Glib::Error& ex) {
    std::cerr << "building menus failed: " <<  ex.what();
  }

  Gtk::Widget *menubar = ui_manager->get_widget("/MenuBar");
  if(menubar) grid.attach(*menubar, 0, 0, 2, 1);

  //setting up containers
  grid.set_margin_left(2);
  grid.set_margin_right(2);
  grid.set_margin_bottom(2);
  left_box.set_vexpand(true);
  left_box.set_margin_right(1);
  left_box.set_size_request(200, -1);
  left_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  right_box.set_hexpand(true);
  right_box.set_vexpand(true);
  right_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  bottom_box.set_spacing(4);
  bottom_box.set_hexpand(true);
  bottom_box.set_margin_right(6);
  bottom_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  toolbar.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  //organising widgets into containers
  toolbar.pack_start(save_button, true, true);
  toolbar.pack_start(delete_button, true, true);
  toolbar.pack_start(edit_button, true, true);
  toolbar.pack_start(tags_button, true, true);
  bottom_box.pack_end(zoom_icon, false, false);
  bottom_box.pack_end(zoom_slider, false, false);
  bottom_box.pack_end(statusbar, true, true);
  right_box.pack_start(display, true, true);
  right_box.pack_end(bottom_box, false, false);
  left_box.pack_end(notebook, true, true);
  left_box.pack_end(toolbar, false, false);
  grid.attach(left_box, 0, 2, 1, 1);
  grid.attach(right_box, 1, 2, 1, 1);
  add(grid);
}

/// @fn MainWindow::~MainWindow()
/// @brief MainWindow descrutor - destroys active WindowContent.
MainWindow::~MainWindow() {
  if(content) delete content;
}

/// @fn MainWindow::refreshActiveView() {
/// @brief MainWindow method for refreshing the active view.
void MainWindow::refreshActiveView() {
  if(content) content->refreshView();
}

/// @fn MainWindow::showLibraryView()
/// @brief Method responsible for switching to library view.
void MainWindow::showLibraryView() {
  /// @todo set menubar item active
  if(content) delete content;
  content = new LibraryView(this);
  show_all_children();
}

/// @fn MainWindow::showEditView()
/// @brief Method responsible for switching to edit view.
void MainWindow::showEditView() {
  /// @todo set menubar item active
  if(!core->isCurrentPhotoSet()) return;
  if(content) delete content;
  content = new EditView(this);
  show_all_children();
}

/// @fn MainWindow::showAbout()
/// @brief Method responsible for displaying About dialog.
void MainWindow::showAbout() {
  Gtk::AboutDialog dialog;
  std::vector<Glib::ustring> authors;
  authors.push_back(" jjkrol - Jakub Król");
  authors.push_back(" mc - Maciej Suchecki");
  authors.push_back(" nme - Jacek Witkowski");
  dialog.set_program_name("Imagine");
  dialog.set_copyright("IMAGINE - Image Manipulation And orGanisation Is Now Easy!");
  dialog.set_comments("GTK+ smart photo organiser and editor");
  //dialog.set_logo(Gdk::Pixbuf::create_from_file("logo_beta.png"));
  dialog.set_authors(authors);
  dialog.run();
}

/// @fn void MainWindow::editPreferences()
/// @brief Method responsible for displaying Preferences dialog.
void MainWindow::editPreferences() {
  PreferencesDialog *preferences_editor = new PreferencesDialog(this);
  preferences_editor ->run();
  delete preferences_editor;
}

/// @fn void MainWindow::editDatabase()
/// @brief Method responsible for displaying Database Manager dialog.
void MainWindow::editDatabase() {
  DBManagerDialog *db_manager = new DBManagerDialog(this);
  db_manager->run();
  delete db_manager;
}

/// @fn void MainWindow::editPhotoTags()
/// @brief Method responsible for displaying Edit Photo Tags dialog.
void MainWindow::editPhotoTags() {
  if(!core->isCurrentPhotoSet()) return;
  TagsDialog *tags_dialog = new TagsDialog(this);
  tags_dialog->run();
  delete tags_dialog;
}

/// @fn void MainWindow::checkForUnsavedPhotos()
/// @brief Method responsible for checking for unsaved photos and
///        displaying the prompt or closing the application.
bool MainWindow::on_delete_event(GdkEventAny* event) {
  if(!core->modifiedPhotosExist())
    return false;

  //prompt is already displayed
  if(prompt) return true;

  //displaying prompt
  prompt = new Gtk::InfoBar;
  Gtk::Box *box = new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
  Gtk::Container *container = dynamic_cast<Gtk::Container*>(prompt->get_content_area());
  if(container) container->add(*box);
  box->set_spacing(20);

  //adding image
  Gtk::Image *icon = new Gtk::Image;
  if(Gtk::Stock::lookup(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG, *icon))
    box->pack_start(*icon, false, false);

  //adding label
  Gtk::Label *label = new Gtk::Label("It seems like there are some unsaved photos. If you don't save them, changes will be lost.");
  label->set_line_wrap(true);
  box->pack_start(*label, false, false);

  //adding button
  prompt->add_button("Close without saving", 0);
  prompt->add_button("Save photos", 1);
  prompt->add_button("Cancel", 2);
  prompt->signal_response().connect(sigc::mem_fun(*this,
                       &MainWindow::managePromptResponse));

  //displaying
  right_box.remove(display);
  right_box.pack_start(*prompt, false, false);
  right_box.pack_start(display, true, true);
  show_all_children();

  return true;
}

/// @fn void MainWindow::managePromptResponse(int response)
/// @brief Method responsible for handling the save photos prompt.
void MainWindow::managePromptResponse(int response) {
  switch(response) {
    case 0:
      UserInterface::getInstance()->destroy();
      break;
    case 1:
      core->savePhotos();
      right_box.remove(*prompt);
      delete prompt;
      prompt = NULL;
      break;
    case 2:
      right_box.remove(*prompt);
      delete prompt;
      prompt = NULL;
      break;
    default:
      break;
  }
}
