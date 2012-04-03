#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"

//MainWindow constructor
MainWindow::MainWindow() :
  zoom_slider(Gtk::ORIENTATION_HORIZONTAL),
  zoom_icon(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON) {

  //connecting to UserInterface
  gui = UserInterface::getInstance();

  //customizing window
  set_title("Image Viewer");
  maximize();

  //editing widgets
  zoom_slider.set_draw_value(false);
  zoom_slider.set_show_fill_level(true);
  zoom_slider.set_size_request(200, -1);

  //setting up containers
  grid.set_margin_left(1);
  grid.set_margin_right(1);
  grid.set_margin_bottom(1);
  left_box.set_spacing(1);
  left_box.set_vexpand(true);
  left_box.set_margin_right(1);
  left_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  right_box.set_spacing(1);
  right_box.set_hexpand(true);
  right_box.set_vexpand(true);
  right_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  bottom_box.set_spacing(2);
  bottom_box.set_hexpand(true);
  bottom_box.set_margin_right(5);
  bottom_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  //organising widgets into containers
  bottom_box.pack_end(zoom_icon, false, false);
  bottom_box.pack_end(zoom_slider, false, false);
  bottom_box.pack_end(statusbar, true, true);
  right_box.pack_end(bottom_box, false, false);
  left_box.pack_end(notebook, true, true);
  grid.attach(menu, 0, 0, 2, 1);
  grid.attach(left_box, 0, 2, 1, 1);
  grid.attach(right_box, 1, 2, 1, 1);
  add(grid);
 
  //showing photo edit window
  content = new EditView(this);
}

//MainWindow descructor
MainWindow::~MainWindow() {
}

//method for changing displayed Photo
void EditView::changePhoto(Photo *photo) {
  if(!photo) return;
  current_photo = photo;
  current_pixbuf = photo->getPixbuf();
}

//method for loading image into Gtk::Image widget
void EditView::loadImage() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  Gdk::Rectangle rectangle = image_window->get_allocation();
  if(!pixbuf) return;

  //displaying filename and resetting zoom widget
  window->statusbar.set_label((current_photo->getFilename()).string());
  window->zoom_slider.set_range(100.0, 400.0);
  window->zoom_slider.set_value(100.0);

  //fitting image if needed
  if(rectangle.get_width() < pixbuf->get_width() ||
     rectangle.get_height() < pixbuf->get_height())
    pixbuf = resizeImage(pixbuf, rectangle);

  //resizing and setting image
  image->set(pixbuf);
}

//additional function for fitting pixbuf into window
Glib::RefPtr<Gdk::Pixbuf> EditView::resizeImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
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
  window->zoom_slider.set_range(zoom_raito * 100.0, 400.0);
  window->zoom_slider.set_value(zoom_raito * 100.0);

  //image resizing
  return pixbuf->scale_simple(width, height, Gdk::INTERP_BILINEAR);
}

//method for zooming image
void EditView::zoomImage() {
  double zoom = window->zoom_slider.get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  pixbuf = pixbuf->scale_simple(pixbuf->get_width() * zoom,
                                pixbuf->get_height() * zoom, Gdk::INTERP_BILINEAR);
  image->set(pixbuf);
}

//method for automatic image resizing
void EditView::fitImage(Gtk::Allocation &allocation) {
  if(window->zoom_slider.get_state_flags() & Gtk::STATE_FLAG_FOCUSED)
    return;
  loadImage();
  return;
}

EditView::EditView(MainWindow *w) : window(w),
  library_button("Back to library"),
  basic_label("Basic editing"),
  colors_label("Colors modification"),
  effects_label("Other effects") {

  gui = UserInterface::getInstance();

  window->left_box.pack_start(library_button, false, false);
  window->notebook.append_page(basic_label, "Basic");
  window->notebook.append_page(colors_label, "Colors");
  window->notebook.append_page(effects_label, "Effects");

  image = new Gtk::Image();
  image_window = new Gtk::ScrolledWindow();
  left_button = new Gtk::Button(Gtk::Stock::GO_BACK);
  right_button = new Gtk::Button(Gtk::Stock::GO_FORWARD);

  window->bottom_box.pack_start(*left_button, false, false);
  window->bottom_box.pack_start(*right_button, false, false);
  window->right_box.pack_start(*image_window, true, true);
  image_window->add(*image);

  //changing images
  right_button->signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::nextImage));
  left_button->signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::prevImage));

  //zooming
  window->zoom_slider.signal_value_changed().connect(sigc::mem_fun(this, &EditView::zoomImage));

  //auto resize
  window->signal_size_allocate().connect_notify(sigc::mem_fun(this, &EditView::fitImage));
}

EditView::~EditView() {
  delete left_button;
  delete right_button;
  delete image;
  delete image_window;
}
