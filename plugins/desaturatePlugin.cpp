#include "../include/plugins/desaturatePlugin.hpp"
#include "../include/plugins/desaturateEffect.hpp"
#include <iostream>

using namespace std;

DesaturatePlugin::DesaturatePlugin() {
  name = "Desaturate";
}

Gtk::Widget* DesaturatePlugin::getWidget() {
  box = new Gtk::Box();
  spin_button = new Gtk::SpinButton(
      Gtk::Adjustment::create(0.5, 0.0, 1.0, 0.05, 0.1, 0.0), 0.05);
  box->pack_start(*spin_button, false, false);
  return box;
}

Effect* DesaturatePlugin::getEffect() {
  if(spin_button == NULL)
    return new DesaturateEffect(0);
  else {
    return new DesaturateEffect(spin_button->get_value());
  }
}

string DesaturatePlugin::getName() {
  return name;
}
