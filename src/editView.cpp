#include "../include/gui.hpp"
#include "../include/core.hpp"
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
  redo_button(Gtk::Stock::REDO), put_effect_button("Apply effect"),
  plugin_buttons(Gtk::ORIENTATION_VERTICAL) {

  //obtaining CoreController instance
  core = CoreController::getInstance();

  //editing widgets
  edit_buttons.set_margin_left(2);
  edit_buttons.set_margin_right(2);
  edit_buttons.set_margin_top(1);
  edit_buttons.set_margin_bottom(1);
  edit_buttons.set_spacing(2);
  plugin_buttons.set_margin_left(2);
  plugin_buttons.set_margin_right(2);
  plugin_buttons.set_margin_top(1);
  plugin_buttons.set_margin_bottom(1);
  plugin_buttons.set_spacing(2);

  //adding tooltips to buttons
  left_button.set_tooltip_text("Load previous photo");
  right_button.set_tooltip_text("Load next photo");
  undo_button.set_tooltip_text("Undo last operation");
  redo_button.set_tooltip_text("Redo previously cancelled operation");
  library_button.set_tooltip_text("Go back to library view");

  //organising widgets
  edit_buttons.pack_start(undo_button, true, true);
  edit_buttons.pack_start(redo_button, true, true);
  basic_box.pack_start(basic_label, true, true);
  colors_box.pack_start(colors_label, true, true);
  effects_box.pack_start(plugin_buttons, true, true);
  window->toolbar.pack_end(library_button, false, false);
  window->left_box.pack_start(edit_buttons, false, false);
  window->notebook.append_page(basic_box, "Basic");
  window->notebook.append_page(colors_box, "Colors");
  window->notebook.append_page(effects_box, "Effects");
  window->bottom_box.pack_start(left_button, false, false);
  window->bottom_box.pack_start(right_button, false, false);
  window->display.add(image);

  //connecting signals
  right_button.signal_clicked().connect(sigc::mem_fun(this, &EditView::nextImage));
  left_button.signal_clicked().connect(sigc::mem_fun(this, &EditView::prevImage));
  undo_button.signal_clicked().connect(sigc::mem_fun(*core, &CoreController::undoLastEffect));
  redo_button.signal_clicked().connect(sigc::mem_fun(*core, &CoreController::redoLastEffect));
  library_button.signal_clicked().connect(sigc::mem_fun(window, &MainWindow::showLibraryView));
  put_effect_button.signal_clicked().connect(sigc::mem_fun(this, &EditView::applyEffect));
  zoom_signal = window->zoom_slider.signal_value_changed().connect(
      sigc::mem_fun(this, &EditView::zoomImage));
  fit_signal = window->display.signal_size_allocate().connect_notify(
     sigc::mem_fun(this, &EditView::fitImage));
  page_signal = window->notebook.signal_switch_page().connect(
      sigc::mem_fun(this, &EditView::onPageSwitch));

  //loading plugins
  std::vector<std::string> plugin_names = core->getPluginNames();
  std::vector<std::string>::iterator it;
  for(it = plugin_names.begin(); it != plugin_names.end(); ++it) {
    Gtk::ToolButton *but = new Gtk::ToolButton((*it));
    but->signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &EditView::showPluginBox),(*it)));
    plugin_map[*it] = but;
    plugin_buttons.pack_start(*but, false, false);
  }

  //loading image
  current_photo = core->getCurrentPhoto();
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
  //@TODO remove plugin buttons
}

///
void EditView::showPluginBox(std::string name) { 
  Gtk::Box *pluginBox = dynamic_cast<Gtk::Box *>(core->getPluginBox(name));
  pluginBox->set_margin_left(2);
  pluginBox->set_margin_right(2);
  pluginBox->set_margin_top(1);
  pluginBox->set_margin_bottom(1);
  pluginBox->set_spacing(2);
  effects_box.remove(plugin_buttons);
  effects_box.remove(edit_buttons);
  effects_box.pack_start(*pluginBox, false, false);
  effects_box.pack_start(put_effect_button, false, false);
  effects_box.pack_end(edit_buttons, false, false);
  effects_box.show_all_children();
}

/// @fn void EditView::applyEffect()
/// @brief applies effect to the current photo
void EditView::applyEffect() {
  core->applyEffectOfSelectedPlugin();
  refreshView();
}

/// @fn EditView::refreshView()
/// @brief EditView method for refreshing the view.
void EditView::refreshView() {
  loadImage();
}

/// @fn void EditView::loadImage()
/// @brief Method (signal handler) for loading image into Gtk::Image widget.
///        Called when Next/Previous button is clicked or edit view is created.
void EditView::loadImage() {
  current_photo = core->getCurrentPhoto();
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_photo.pixbuf;
  Gdk::Rectangle rectangle = window->display.get_allocation();
  if(!pixbuf) return;

  //displaying filename
  window->statusbar.set_label(current_photo.path.string());

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
  if(!current_photo.pixbuf) return;
  double zoom = window->zoom_slider.get_value() / 100;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = current_photo.pixbuf;
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

/// @fn void EditView::nextImage()
/// @brief Method (signal handler) responsible for loading next image from folder.
void EditView::nextImage() {
  current_photo = core->getNextPhoto();
  loadImage();
}

/// @fn void EditView::prevImage()
/// @brief Method (signal handler) responsible for loading previous image from folder.
void EditView::prevImage() {
  current_photo = core->getPreviousPhoto();
  loadImage();
}
