#include "../../include/plugins/sampleEffect.hpp"
#include "../../include/photo.hpp"
#include <gtkmm.h>

SampleEffect::SampleEffect(Operation operation):
  operation(operation) {
}

void SampleEffect::execute(Photo * photo) {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = photo->getPixbuf();
  switch(operation){
    case VERTICAL:
      photo->setPixbuf(pixbuf->flip(false));
      break;
    case HORIZONTAL:
      photo->setPixbuf(pixbuf->flip(true));
      break;
    default:
      break;
  }
}
