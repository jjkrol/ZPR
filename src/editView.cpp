#include "../include/gui.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"

/// @fn EditView::EditView(MainWindow *w)
/// @brief EditView constructor - connects to UserInterface and builds edit view gui.
/// @param w pointer to parent class MainWindow (needed for communication).
EditView::EditView(MainWindow *w) : window(w),
  left_button(Gtk::Stock::GO_BACK), right_button(Gtk::Stock::GO_FORWARD),
  basic_box(Gtk::ORIENTATION_VERTICAL), colors_box(Gtk::ORIENTATION_VERTICAL),
  effects_box(Gtk::ORIENTATION_VERTICAL), basic_label("Basic editing"),
  colors_label("Colors modification"), effects_label("Other effects"),
  library_button(Gtk::Stock::QUIT), undo_button(Gtk::Stock::UNDO),
  redo_button(Gtk::Stock::REDO) {

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
  basic_box.pack_start(basic_label, true, true);
  colors_box.pack_start(colors_label, true, true);
  effects_box.pack_start(effects_label, true, true);
  window->toolbar.pack_end(library_button, false, false);
  window->left_box.pack_start(edit_buttons, false, false);
  window->notebook.append_page(basic_box, "Basic");
  window->notebook.append_page(colors_box, "Colors");
  window->notebook.append_page(effects_box, "Effects");
  window->bottom_box.pack_start(left_button, false, false);
  window->bottom_box.pack_start(right_button, false, false);
  window->display.add(image);

  //connecting signals
  right_button.signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::nextImage));
  left_button.signal_clicked().connect(sigc::mem_fun(gui, &UserInterface::prevImage));
  library_button.signal_clicked().connect(sigc::mem_fun(window, &MainWindow::showLibraryView));
  zoom_signal = window->zoom_slider.signal_value_changed().connect(sigc::mem_fun(this, &EditView::zoomImage));
  fit_signal = window->signal_size_allocate().connect_notify(sigc::mem_fun(this, &EditView::fitImage));
  page_signal = window->notebook.signal_switch_page().connect(sigc::mem_fun(this, &EditView::onPageSwitch));

  //loading image
  updatePixbuf();
}

/// @fn EditView::~EditView()
/// @brief EditView descructor - disconnects signals and resets widgets.
EditView::~EditView() {
  window->statusbar.set_label("");
  window->notebook.remove_page(basic_label);
  window->notebook.remove_page(colors_label);
  window->notebook.remove_page(effects_label);
  window->toolbar.remove(library_button);
  window->bottom_box.remove(left_button);
  window->bottom_box.remove(right_button);
  window->display.remove();
  zoom_signal.disconnect();
  fit_signal.disconnect();
  page_signal.disconnect();
}

/// @fn void EditView::updatePixbuf()
/// @brief Method for changing displayed Photo.
void EditView::updatePixbuf() {
  current_pixbuf = ((*gui->current_photo))->getPixbuf();
}

/// @fn void EditView::loadImage()
/// @brief Method (signal handler) for loading image into Gtk::Image widget.
///        Called when Next/Previous button is clicked or edit view is created.
void EditView::loadImage() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  Gdk::Rectangle rectangle = window->display.get_allocation();
  if(!pixbuf) return;

  //displaying filename
  window->statusbar.set_label(((*(gui->current_photo))->getFilename()).string());

  //fitting image if needed
  if(rectangle.get_width() < pixbuf->get_width() ||
     rectangle.get_height() < pixbuf->get_height())
    pixbuf = resizeImage(pixbuf, rectangle);
  else {
    window->zoom_slider.set_range(100.0, 400.0);
    window->zoom_slider.set_value(100.0);
  }

  //resizing and setting image
  image.set(pixbuf);
}

/// @fn Glib::RefPtr<Gdk::Pixbuf> EditView::resizeImage()
/// @brief Additional function for fitting pixbuf into window.
/// @param pixbuf Pixbuf to resize.
/// @param rectangle Object containing window dimensions.
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

/// @fn void EditView::zoomImage()
/// @brief Method (signal handler) for zooming image.
///        Called by Gtk::Scale, when its value is changed.
void EditView::zoomImage() {
  if(!current_pixbuf) return;
  double zoom = window->zoom_slider.get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_pixbuf;
  pixbuf = pixbuf->scale_simple(pixbuf->get_width() * zoom,
                                pixbuf->get_height() * zoom, Gdk::INTERP_BILINEAR);
  image.set(pixbuf);
}

/// @fn void EditView::fitImage(Gtk::Allocation &allocation)
/// @brief Method (signal handler) for automatic image resizing.
///        Called by Gtk::Window resize event.
/// @param allocation Provided by signal system, not used.
void EditView::fitImage(Gtk::Allocation &allocation) {
  if(window->zoom_slider.get_state_flags() & Gtk::STATE_FLAG_FOCUSED)
    return;
  loadImage();
  return;
}

/// @fn void EditView::onPageSwitch(Gtk::Widget *page, guint number)
/// @brief Method (signal handler) for moving Undo/Redo buttons to another page.
///        Called by page switch signal from Gtk::Notebook.
/// @param page Pointer to widget in actual page, provided by signal system.
/// @param number Actual page number, provided by signal system, not used.
void EditView::onPageSwitch(Gtk::Widget *page, guint number) {
  edit_buttons.get_parent()->remove(edit_buttons);
  dynamic_cast<Gtk::Box *>(page)->pack_end(edit_buttons, false, false);
}