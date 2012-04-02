#pragma once

#include <gtkmm.h>

/** @class MainWindow
 *  @brief Class representing main window of an application.
 */

class UserInterface;

class MainWindow : public Gtk::Window {
  private:
    //connection with UserInterface class
    UserInterface *gui;
    Photo *current_photo;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;

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
    void zoomImage();

    //additional functions
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);

  public:
    MainWindow();
    ~MainWindow();

    void changePhoto(Photo *);
};
