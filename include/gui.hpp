#pragma once

#include <gtkmm.h>

/** \class GUI
 *  \brief Class representing Graphical User Interface.
 *
 *  Class uses GTKmm, it serves as a link between user and program
 *  taking commands from user and sending them to the Core class.
 */

class GUI {
  private:
    Gtk::Main kit;
    Gtk::Window *main_window;

    //containers and menubar
    Gtk::Grid *grid;
    Gtk::Box *left_box, *right_box, *bottom_box;
    Gtk::ScrolledWindow *image_window;
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

    //methods for handling signals
    void openImage();
    void fitImage();

  public:
    GUI(int argc, char *argv[]);
    ~GUI();
    void createMainWindow();
};
