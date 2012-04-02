#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"

//MainWindow constructor
MainWindow::MainWindow() {
  //connecting to UserInterface
  gui = UserInterface::getInstance();

  //customizing window
  set_title("Image Viewer");
  maximize();

  //creating widgets
  image = new Gtk::Image();
  image_window = new Gtk::ScrolledWindow();
  menu = new Gtk::MenuBar();
  library_button = new Gtk::Button("Back to library");
  left_button = new Gtk::Button(Gtk::Stock::GO_BACK);
  right_button = new Gtk::Button(Gtk::Stock::GO_FORWARD);
  filename_label = new Gtk::Label("");
  basic_label = new Gtk::Label("Basic editing");
  colors_label = new Gtk::Label("Colors modification");
  effects_label = new Gtk::Label("Other effects");
  notebook = new Gtk::Notebook();
  image_zoom = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);
  zoom_icon = new Gtk::Image(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON);

  //editing widgets
  image_zoom->set_draw_value(false);
  image_zoom->set_show_fill_level(true);
  notebook->append_page(*basic_label, "Basic");
  notebook->append_page(*colors_label, "Colors");
  notebook->append_page(*effects_label, "Effects");

  //organising widgets
  bottom_box = new Gtk::Box();
  bottom_box->set_margin_right(5);
  bottom_box->set_hexpand(true);
  bottom_box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  bottom_box->set_spacing(2);
  bottom_box->pack_start(*left_button, false, false);
  bottom_box->pack_start(*right_button, false, false);
  bottom_box->pack_start(*filename_label, true, true);
  bottom_box->pack_start(*image_zoom, false, false);
  bottom_box->pack_start(*zoom_icon, false, false);

  right_box = new Gtk::Box();
  right_box->set_hexpand(true);
  right_box->set_vexpand(true);
  right_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  right_box->set_spacing(1);
  right_box->pack_start(*image_window, true, true);
  right_box->pack_start(*bottom_box, false, false);

  left_box = new Gtk::Box();
  left_box->set_margin_right(1);
  left_box->set_vexpand(true);
  left_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  left_box->set_spacing(1);
  left_box->pack_start(*library_button, false, false);
  left_box->pack_start(*notebook, true, true);

  grid = new Gtk::Grid();
  grid->set_margin_bottom(1);
  grid->set_margin_left(1);
  grid->set_margin_right(1);
  grid->attach(*menu, 0, 0, 2, 1);
  grid->attach(*left_box, 0, 2, 1, 1);
  grid->attach(*right_box, 1, 2, 1, 1);

  image_window->add(*image);
  add(*grid);

  image_zoom->set_size_request(200, -1);

  //changing images
  right_button->signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::nextImage));
  left_button->signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::prevImage));

  //zooming
  image_zoom->signal_value_changed().connect(sigc::mem_fun(this, &MainWindow::zoomImage));

  //auto resize
  signal_size_allocate().connect_notify(sigc::mem_fun(this, &MainWindow::fitImage));
}

//MainWindow descructor
MainWindow::~MainWindow() {
  delete grid;
  delete left_box;
  delete right_box;
  delete bottom_box;
  delete menu;
  delete library_button;
  delete notebook;
  delete zoom_icon;
  delete left_button;
  delete right_button;
  delete filename_label;
  delete basic_label;
  delete colors_label;
  delete effects_label;
  delete image;
  delete image_zoom;
}

//method for changing displayed Photo
void MainWindow::changePhoto(Photo *photo) {
  if(!photo) return;
  current_photo = photo;
  current_pixbuf = photo->getPixbuf();
}

//method for loading image into Gtk::Image widget
void MainWindow::loadImage() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  Gdk::Rectangle rectangle = image_window->get_allocation();
  if(!pixbuf) return;

  //displaying filename and resetting zoom widget
  filename_label->set_label((current_photo->getFilename()).string());
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
Glib::RefPtr<Gdk::Pixbuf> MainWindow::resizeImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
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

//method for zooming image
void MainWindow::zoomImage() {
  double zoom = image_zoom->get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  pixbuf = pixbuf->scale_simple(pixbuf->get_width() * zoom,
                                pixbuf->get_height() * zoom, Gdk::INTERP_BILINEAR);
  image->set(pixbuf);
}

//method for automatic image resizing
void MainWindow::fitImage(Gtk::Allocation &allocation) {
  if(image_zoom->get_state_flags() & Gtk::STATE_FLAG_FOCUSED)
    return;
  loadImage();
  return;
}
