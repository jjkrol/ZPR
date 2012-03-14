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

    //containers and menubar
    Gtk::Grid *grid;
    Gtk::Box *left_box, *right_box, *bottom_box;
    Gtk::MenuBar *menu;

    //left side widgets
    Gtk::Button *library_button;
    Gtk::Notebook *notebook;

    //right side widgets
    Gtk::Button *open_button, *fit_button;
    Gtk::Button *left_button, *right_button;
    Gtk::Label *filename_label;
    Gtk::Label *basic_label, *colors_label, *effects_label;
    Gtk::Image *image;

  public:
    GUI(int argc, char *argv[]);
    void createMainWindow();
};

#endif
