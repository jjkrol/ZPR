#pragma once

#include <string>
#include <gtkmm.h>
#include "plugin.hpp"

class Photo;
class Effect;

/// @class DesaturatePlugin
/// @brief Class representing plugin which desaturates the photo.
class DesaturatePlugin : public Plugin {
  public:
    DesaturatePlugin();
    virtual ~DesaturatePlugin(){};
    Gtk::Widget* getWidget();
    std::string getName();
    Effect* getEffect();
  private:
    Gtk::Box *box;
    Gtk::SpinButton *spin_button;
    Gtk::ToolButton *button;
};
