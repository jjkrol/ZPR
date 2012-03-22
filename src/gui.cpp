#include "../include/gui.hpp"
#include "../include/core.hpp"
#include <iostream>

/** @class GUI
 *  @brief Class representing Graphical User Interface.
 *
 *  Class uses GTKmm, it serves as a link between user and program
 *  taking commands from user and sending them to the Core class.
 */

//GUI constructor - creates main window and its widgets
GUI::GUI(int argc, char *argv[]) : kit(argc, argv) {
  //creating main window
  main_window = new Gtk::Window(Gtk::WINDOW_TOPLEVEL);
  main_window->set_title("Image Viewer");

  //creating widgets
  image = new Gtk::Image();
  image_window = new Gtk::ScrolledWindow();
  menu = new Gtk::MenuBar();
  library_button = new Gtk::Button("Back to library");
  open_button = new Gtk::Button(Gtk::Stock::OPEN);
  fit_button = new Gtk::Button(Gtk::Stock::ZOOM_FIT);
  left_button = new Gtk::Button(Gtk::Stock::GO_BACK);
  right_button = new Gtk::Button(Gtk::Stock::GO_FORWARD);
  filename_label = new Gtk::Label("");
  basic_label = new Gtk::Label("Basic editing");
  colors_label = new Gtk::Label("Colors modification");
  effects_label = new Gtk::Label("Other effects");
  notebook = new Gtk::Notebook();
  image_zoom = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);

  //editing widgets
  image_zoom->set_range(1, 100);
  image_zoom->set_draw_value(false);
  image_zoom->set_show_fill_level(true);
  notebook->append_page(*basic_label, "Basic");
  notebook->append_page(*colors_label, "Colors");
  notebook->append_page(*effects_label, "Effects");

  //organising widgets
  bottom_box = new Gtk::Box();
  bottom_box->set_hexpand(true);
  bottom_box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  bottom_box->set_spacing(1);
  bottom_box->pack_start(*left_button, false, false);
  bottom_box->pack_start(*right_button, false, false);
  bottom_box->pack_start(*filename_label, true, true);
  bottom_box->pack_start(*fit_button, false, false);
  bottom_box->pack_start(*image_zoom, false, false);
  bottom_box->pack_start(*open_button, false, false);

  right_box = new Gtk::Box();
  right_box->set_hexpand(true);
  right_box->set_vexpand(true);
  right_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  right_box->set_spacing(4);
  right_box->pack_start(*image_window, true, true);
  right_box->pack_start(*bottom_box, false, false);

  left_box = new Gtk::Box();
  left_box->set_vexpand(true);
  left_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  left_box->set_spacing(4);
  left_box->pack_start(*library_button, false, false);
  left_box->pack_start(*notebook, true, true);

  grid = new Gtk::Grid();
  grid->attach(*menu, 0, 0, 2, 1);
  grid->attach(*left_box, 0, 2, 1, 1);
  grid->attach(*right_box, 1, 2, 1, 1);

  image_window->add(*image);
  main_window->add(*grid);

  //connecting to core and loading directory
  core = CoreController::initialize(0, NULL, false);

  //loading image
  current_dir = core->getDirectoryTree();
  photos = current_dir->getPhotos();
  current_photo = photos.begin();
}

//GUI class descructor
GUI::~GUI() {
  delete main_window;
  delete grid;
  delete left_box;
  delete right_box;
  delete bottom_box;
  delete menu;
  delete library_button;
  delete notebook;
  delete open_button;
  delete fit_button;
  delete left_button;
  delete right_button;
  delete filename_label;
  delete basic_label;
  delete colors_label;
  delete effects_label;
  delete image;
  delete image_zoom;
}

//function connects signals and shows main window
void GUI::createMainWindow() {
  //connecting buttons signals to functions
  open_button->signal_clicked().connect(sigc::mem_fun(this, &GUI::openImage));
  fit_button->signal_clicked().connect(sigc::mem_fun(this, &GUI::fitImage));

  //loading image
  image->set((*current_photo)->getPixbuf());
  fitImage();

  //showing widgets
  main_window->show_all_children();
  main_window->maximize();
  if(main_window) kit.run(*main_window);
}

//function creates file open dialog and sets image source
void GUI::openImage() {
  //file choose dialog
  Gtk::FileChooserDialog dialog("Open image", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //file filter
  Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
  filter->add_pixbuf_formats();
  dialog.add_filter(filter);

  //waiting for OK
  switch(dialog.run()) {
    case Gtk::RESPONSE_ACCEPT:
      image->set(dialog.get_filename());
      filename_label->set_text(dialog.get_filename());
      fitImage();
      break;
    default:
      break;
  }

  dialog.hide();
}

//method for fitting image into Gtk::Image widget
void GUI::fitImage() {
  //checking if fitting image is needed
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = image->get_pixbuf();
  Gdk::Rectangle rectangle = image_window->get_allocation();
  if(!pixbuf) return;
  if(rectangle.get_width() > pixbuf->get_width() &&
     rectangle.get_height() > pixbuf->get_height()) return;

  //calculating desired width and height
  int width, height;
  float widget_raito = (float)rectangle.get_width() / (float)rectangle.get_height();
  float image_raito = (float)pixbuf->get_width() / (float)pixbuf->get_height();

  if(widget_raito >= image_raito) {
    height = rectangle.get_height() - 4;
    width = height * image_raito;
  } else {
    width = rectangle.get_width() - 4;
    height = width / image_raito;
  }

  //image resizing
  pixbuf = pixbuf->scale_simple(width, height, Gdk::INTERP_BILINEAR);
  image->set(pixbuf);
}
