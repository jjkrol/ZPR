#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"

/// @fn MainWindow::MainWindow()
/// @brief MainWindow constructor - connects to UserInterface, creates menubar,
///        and builds GUI widgets common for both views.
MainWindow::MainWindow() : zoom_slider(Gtk::ORIENTATION_HORIZONTAL),
  zoom_icon(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON),
  save_button(Gtk::Stock::FLOPPY), delete_button(Gtk::Stock::DELETE),
  star_button(Gtk::Stock::ABOUT), tags_button(Gtk::Stock::EDIT) {

  //connecting to UserInterface
  gui = UserInterface::getInstance();

  //customizing window
  set_title("Imagine");
  maximize();

  //editing widgets
  zoom_slider.set_draw_value(false);
  zoom_slider.set_show_fill_level(true);
  zoom_slider.set_size_request(200, -1);

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
  toolbar.pack_start(star_button, true, true);
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
