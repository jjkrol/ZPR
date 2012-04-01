#pragma once

#include <gtkmm.h>
#include <vector>

/** @class UserInterface
 *  @brief Class representing Graphical User Interface.
 *
 *  Class uses GTKmm, it serves as a link between user and program
 *  taking commands from user and sending them to the Core class.
 */

class CoreController;
class Directory;
class Photo;

class UserInterface {
  private:
    //singleton instance
    static UserInterface* instance;

    //comunication with CoreController
    CoreController *core;
    Directory *current_dir;
    std::vector<Photo*> photos;
    std::vector<Photo*>::iterator current_photo;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;

    //main window and kit
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
    Gtk::Button *fit_button;
    Gtk::Button *left_button, *right_button;
    Gtk::Scale *image_zoom;
    Gtk::Label *filename_label;
    Gtk::Label *basic_label, *colors_label, *effects_label;
    Gtk::Image *image;

    //methods for handling signals
    void fitImage(Gtk::Allocation &);
    void loadImage();
    void nextImage();
    void prevImage();
    void zoomImage();

    //additional functions
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);
    ~UserInterface() {};

  public:
    UserInterface(int argc, char *argv[]);
    static UserInterface* getInstance(int argc = 0, char *argv[] = NULL);
    void destroy();
    void showEditWindow();
};
