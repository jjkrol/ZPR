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
  fit_button = new Gtk::Button(Gtk::Stock::ZOOM_FIT);
  left_button = new Gtk::Button(Gtk::Stock::GO_BACK);
  right_button = new Gtk::Button(Gtk::Stock::GO_FORWARD);
  filename_label = new Gtk::Label("");
  basic_label = new Gtk::Label("Basic editing");
  colors_label = new Gtk::Label("Colors modification");
  effects_label = new Gtk::Label("Other effects");
  notebook = new Gtk::Notebook();
  image_zoom = new Gtk::Scale(Gtk::Adjustment::create(100.0, 100.0, 400.0),
                              Gtk::ORIENTATION_HORIZONTAL);

  //editing widgets
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
  bottom_box->pack_start(*image_zoom, true, true);
  bottom_box->pack_start(*fit_button, false, false);

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
  core = CoreController::getInstance();

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
  fit_button->signal_clicked().connect(sigc::mem_fun(this, &GUI::loadImage));
  main_window->signal_show().connect(sigc::mem_fun(this, &GUI::loadImage));
  main_window->signal_configure_event().connect_notify(sigc::mem_fun(this, &GUI::onWindowResize));
  main_window->signal_window_state_event().connect_notify(sigc::mem_fun(this, &GUI::onWindowStateEvent));
  right_button->signal_clicked().connect(sigc::mem_fun(this, &GUI::nextImage));
  left_button->signal_clicked().connect(sigc::mem_fun(this, &GUI::prevImage));
  image_zoom->signal_value_changed().connect(sigc::mem_fun(this, &GUI::zoomImage));

  //showing widgets
  main_window->maximize();
  main_window->show_all_children();
  if(main_window) kit.run(*main_window);
}

//method for loading image into Gtk::Image widget
void GUI::loadImage() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = (*current_photo)->getPixbuf();
  Gdk::Rectangle rectangle = image_window->get_allocation();
  if(!pixbuf) return;

  //displaying filename
  filename_label->set_label(((*current_photo)->getFilename()).string());

  //checking if fitting image is needed
  if(rectangle.get_width() > pixbuf->get_width() &&
     rectangle.get_height() > pixbuf->get_height()) {
    image->set(pixbuf);
    image_zoom->set_adjustment(Gtk::Adjustment::create(100.0, 100.0, 400.0));
    return;
  }

  //resizing and setting image
  pixbuf = resizeImage(pixbuf, rectangle);
  image->set(pixbuf);
}

//additional function for fitting pixbuf into window
Glib::RefPtr<Gdk::Pixbuf> GUI::resizeImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                      Gdk::Rectangle rectangle) {
  //calculating desired width and height
  int width, height;
  float widget_raito = (float)rectangle.get_width() / (float)rectangle.get_height();
  float image_raito = (float)pixbuf->get_width() / (float)pixbuf->get_height();

  //std::cout << rectangle.get_width() << " " << rectangle.get_height() << std::endl;

  if(widget_raito >= image_raito) {
    height = rectangle.get_height() - 4;
    width = height * image_raito;
  } else {
    width = rectangle.get_width() - 4;
    height = width / image_raito;
  }

  //image resizing
  return pixbuf->scale_simple(width, height, Gdk::INTERP_BILINEAR);
}

//method for loading next image from folder
void GUI::nextImage() {
  if(current_photo == --photos.end()) return;
  current_photo++;
  loadImage();
}

//method for loading previous image from folder
void GUI::prevImage() {
  if(current_photo == photos.begin()) return;
  current_photo--;
  loadImage();
}

void GUI::zoomImage() {
  std::cout << image_zoom->get_value() << std::endl;
}

void GUI::onWindowResize(GdkEventConfigure *event) {
  loadImage();
  return;
}

void GUI::onWindowStateEvent(GdkEventWindowState *state) {
  if(state->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
    main_window->maximize();
  loadImage();
  return;
}
