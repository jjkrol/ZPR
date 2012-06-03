#include "../../include/plugins/desaturateEffect.hpp"
#include "../../include/photo.hpp"
#include <gtkmm.h>
#include <iostream>

DesaturateEffect::DesaturateEffect(double value) {
  amount = (float)value;
}

void DesaturateEffect::execute(Photo* photo) {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = photo->getPixbuf();
  Glib::RefPtr<Gdk::Pixbuf> pixbuf_edited = pixbuf->copy();
  pixbuf->saturate_and_pixelate(pixbuf_edited, 0, false);
  photo->setPixbuf(pixbuf_edited);
}
