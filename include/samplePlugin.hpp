#pragma once
#include <string>
#include <gtkmm.h>
#include "plugin.hpp"

class Photo;
class Params;
class Effect;

/**
 * Responsible for showing the plugin functionality
 */
class SamplePlugin : public Plugin{
  public:
    SamplePlugin();
    virtual  ~SamplePlugin(){};
    Gtk::Widget * getWidget();
    std::string getName();
    Effect* getEffect(Params * params);
  private:
    Gtk::Box * box;
    Gtk::CheckButton * checkbox;
    Gtk::ToolButton * button;
};
