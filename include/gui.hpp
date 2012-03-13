#ifndef GUI_HPP_
#define GUI_HPP_

/** \class GUI
 *  \brief Class representing Graphical User Interface.
 *
 *  Class uses GTKmm, it serves as a link between user and program (Core class)
 *  taking commands from user and sending them to the Core class.
 */

#include <gtkmm.h>

class GUI {
  private:
    Gtk::Main kit;
    Gtk::Window *main_window;
    Gtk::Box *box;
    Gtk::Image *image;
    Gtk::Button *button;
  public:
    GUI(int argc, char *argv[]);
    void createMainWindow();
};

#endif
