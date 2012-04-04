#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"

//MainWindow constructor
MainWindow::MainWindow() : zoom_slider(Gtk::ORIENTATION_HORIZONTAL),
  zoom_icon(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON),
  save_button(Gtk::Stock::FLOPPY), delete_button(Gtk::Stock::DELETE),
  star_button(Gtk::Stock::ABOUT), tags_button(Gtk::Stock::EDIT) {

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
  left_box.pack_end(toolbar, true, true);
  grid.attach(menu, 0, 0, 2, 1);
  grid.attach(left_box, 0, 2, 1, 1);
  grid.attach(right_box, 1, 2, 1, 1);
  add(grid);
}

void MainWindow::showLibraryView() {
  if(content) delete content;
  content = new LibraryView(this);
  show_all_children();
}

void MainWindow::showEditView() {
  if(content) delete content;
  content = new EditView(this);
  show_all_children();
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
  Gdk::Rectangle rectangle = window->display.get_allocation();
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
  image.set(pixbuf);
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
  if(!current_pixbuf) return;
  double zoom = window->zoom_slider.get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  pixbuf = pixbuf->scale_simple(pixbuf->get_width() * zoom,
                                pixbuf->get_height() * zoom, Gdk::INTERP_BILINEAR);
  image.set(pixbuf);
}

//method for automatic image resizing
void EditView::fitImage(Gtk::Allocation &allocation) {
  if(window->zoom_slider.get_state_flags() & Gtk::STATE_FLAG_FOCUSED)
    return;
  loadImage();
  return;
}

EditView::EditView(MainWindow *w) : window(w),
  left_button(Gtk::Stock::GO_BACK), right_button(Gtk::Stock::GO_FORWARD),
  basic_label("Basic editing"), colors_label("Colors modification"),
  effects_label("Other effects"), library_button("Back to library"),
  undo_button(Gtk::Stock::UNDO), redo_button(Gtk::Stock::REDO) {

  //obtaining UserInterface instance
  gui = UserInterface::getInstance();

  //editing widgets
  edit_buttons.set_margin_left(2);
  edit_buttons.set_margin_right(2);
  edit_buttons.set_margin_top(1);
  edit_buttons.set_margin_bottom(1);
  edit_buttons.set_spacing(2);

  //organising widgets
  edit_buttons.pack_start(undo_button, true, true);
  edit_buttons.pack_start(redo_button, true, true);
  window->left_box.remove(window->notebook);
  window->left_box.remove(window->toolbar);
  window->left_box.pack_start(library_button, false, false);
  window->left_box.pack_start(window->notebook, true, true);
  window->left_box.pack_start(edit_buttons, false, false);
  window->left_box.pack_start(window->toolbar, false, false);
  window->notebook.append_page(basic_label, "Basic");
  window->notebook.append_page(colors_label, "Colors");
  window->notebook.append_page(effects_label, "Effects");
  window->bottom_box.pack_start(left_button, false, false);
  window->bottom_box.pack_start(right_button, false, false);
  window->display.add(image);

  //connecting signals
  right_button.signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::nextImage));
  left_button.signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::prevImage));
  library_button.signal_clicked().connect(sigc::mem_fun(window, &MainWindow::showLibraryView));
  zoom_signal = window->zoom_slider.signal_value_changed().connect(sigc::mem_fun(this, &EditView::zoomImage));
  fit_signal = window->signal_size_allocate().connect_notify(sigc::mem_fun(this, &EditView::fitImage));

  //loading image
  loadImage();
}

EditView::~EditView() {
  window->statusbar.set_label("");
  window->notebook.remove_page(basic_label);
  window->notebook.remove_page(colors_label);
  window->notebook.remove_page(effects_label);
  window->left_box.remove(library_button);
  window->left_box.remove(edit_buttons);
  window->bottom_box.remove(left_button);
  window->bottom_box.remove(right_button);
  window->display.remove();
  zoom_signal.disconnect();
  fit_signal.disconnect();
}

LibraryView::LibraryView(MainWindow *w) : window(w),
  tags_label("tags browsing"), edit_button("Edit picture") {

  //obtaining UserInterface instance
  gui = UserInterface::getInstance();

  //organising widgets
  window->left_box.remove(window->notebook);
  window->left_box.remove(window->toolbar);
  window->left_box.pack_start(edit_button, false, false);
  window->left_box.pack_start(window->notebook, true, true);
  window->left_box.pack_start(window->toolbar, false, false);
  window->notebook.append_page(directory_tree, "Folders");
  window->notebook.append_page(tags_label, "Tags");
  window->display.add(images);

  //connecting signals
  edit_button.signal_clicked().connect(sigc::mem_fun(window, &MainWindow::showEditView));
}

LibraryView::~LibraryView() {
  window->statusbar.set_label("");
  window->notebook.remove_page(directory_tree);
  window->notebook.remove_page(tags_label);
  window->left_box.remove(edit_button);
}
