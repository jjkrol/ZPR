#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/directory.hpp"

UserInterface* UserInterface::instance = NULL;

UserInterface* UserInterface::getInstance(int argc, char *argv[]) {
  if(instance == NULL) instance = new UserInterface(argc, argv);
  return instance;
}

//UserInterface constructor - creates main window and its widgets
UserInterface::UserInterface(int argc, char *argv[]) : kit(argc, argv) {
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
  image_zoom = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);

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
  current_pixbuf = (*current_photo)->getPixbuf();
}

//UserInterface class descructor
void UserInterface::destroy() {
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
  delete instance;
  instance = NULL;
}

//function connects signals and shows main window
void UserInterface::showEditWindow() {
  //changing images
  right_button->signal_clicked().connect(sigc::mem_fun(this, &UserInterface::nextImage));
  left_button->signal_clicked().connect(sigc::mem_fun(this, &UserInterface::prevImage));

  //zooming
  fit_button->signal_clicked().connect(sigc::mem_fun(this, &UserInterface::loadImage));
  image_zoom->signal_value_changed().connect(sigc::mem_fun(this, &UserInterface::zoomImage));

  //auto resize
  main_window->signal_size_allocate().connect_notify(sigc::mem_fun(this, &UserInterface::fitImage));

  //showing widgets
  main_window->maximize();
  main_window->show_all_children();
  if(main_window) kit.run(*main_window);
}

//method for loading image into Gtk::Image widget
void UserInterface::loadImage() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  Gdk::Rectangle rectangle = image_window->get_allocation();
  if(!pixbuf) return;

  //displaying filename and resetting zoom widget
  filename_label->set_label(((*current_photo)->getFilename()).string());
  image_zoom->set_range(100.0, 400.0);
  image_zoom->set_value(100.0);

  //fitting image if needed
  if(rectangle.get_width() < pixbuf->get_width() ||
     rectangle.get_height() < pixbuf->get_height())
    pixbuf = resizeImage(pixbuf, rectangle);

  //resizing and setting image
  image->set(pixbuf);
}

//additional function for fitting pixbuf into window
Glib::RefPtr<Gdk::Pixbuf> UserInterface::resizeImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                           Gdk::Rectangle rectangle) {
  int width, height;
  double zoom_raito;
  double widget_raito = (double)rectangle.get_width() / (double)rectangle.get_height();
  double image_raito = (double)pixbuf->get_width() / (double)pixbuf->get_height();

  //calculating desired width and height
  if(widget_raito >= image_raito) {
    height = rectangle.get_height() - 4;
    width = height * image_raito;
    zoom_raito = (double)rectangle.get_height() / (double)pixbuf->get_height();
  } else {
    width = rectangle.get_width() - 4;
    height = width / image_raito;
    zoom_raito = (double)rectangle.get_width() / (double)pixbuf->get_width();
  }

  //zoom widget adjusting
  image_zoom->set_range(zoom_raito * 100.0, 400.0);
  image_zoom->set_value(zoom_raito * 100.0);

  //image resizing
  return pixbuf->scale_simple(width, height, Gdk::INTERP_BILINEAR);
}

//method for loading next image from folder
void UserInterface::nextImage() {
  if(current_photo == --photos.end()) return;
  current_photo++;
  current_pixbuf = (*current_photo)->getPixbuf();
  loadImage();
}

//method for loading previous image from folder
void UserInterface::prevImage() {
  if(current_photo == photos.begin()) return;
  current_photo--;
  current_pixbuf = (*current_photo)->getPixbuf();
  loadImage();
}

//method for zooming image
void UserInterface::zoomImage() {
  double zoom = image_zoom->get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  pixbuf = pixbuf->scale_simple(pixbuf->get_width() * zoom,
                                pixbuf->get_height() * zoom, Gdk::INTERP_BILINEAR);
  image->set(pixbuf);
}

//method for automatic image resizing
void UserInterface::fitImage(Gtk::Allocation &allocation) {
  if(image_zoom->get_state_flags() & Gtk::STATE_FLAG_FOCUSED)
    return;
  loadImage();
  return;
}
