#include <iostream>
#include "include/gui.hpp"

void resize_image(Gtk::Image *image) {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = image->get_pixbuf();
  if(pixbuf) {
    Gdk::Rectangle rectangle = image->get_allocation();
    pixbuf->scale_simple(rectangle.get_width(), rectangle.get_height(), Gdk::INTERP_BILINEAR);
    image->set(pixbuf);
  }
}

int main(int argc, char *argv[]) {
  GUI gui(argc, argv);
  gui.createMainWindow();

  return 0;
}
