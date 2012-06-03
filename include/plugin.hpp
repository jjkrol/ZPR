#pragma once

#include <string>
#include <gtkmm.h>

class Params;
class Effect;

/**
 * Responsible for providing certain services to application
 * mostly returning effects
 */
class Plugin{
  public:
    Plugin(){};
    virtual  ~Plugin(){};

    /**
     * @returns widget for setting parameters
     */
    virtual Gtk::Widget * getWidget() = 0;
    
    /**
     * @returns prepared effect ready to be put on a photo
     */
    virtual Effect* getEffect() = 0;

    /**
     * @returns name of the plugin
     */
    virtual std::string getName() = 0;

  protected:
    std::string name;
};
