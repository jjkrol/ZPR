#pragma once

#include <gtkmm.h>

/** @class MainWindow
 *  @brief Class representing main window of an application.
 */

class MainWindow : public Gtk::Window {
  public:
    friend class UserInterface;

  private:
    //connection with UserInterface class
    UserInterface *gui;

    //containers
    Gtk::Grid grid;
    Gtk::Notebook notebook;
    Gtk::Box left_box, right_box, bottom_box;

    //menubar
    Gtk::MenuBar menu;

    //bottom bar widgets
    Gtk::Scale zoom_slider;
    Gtk::Image zoom_icon;
    Gtk::Label statusbar;

    ////////////////TO TRZEBA WRZUCIC W DEKORATOR/////////////////
    Photo *current_photo;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;

    Gtk::Button *library_button;

    Gtk::Image *image;
    Gtk::ScrolledWindow *image_window;

    Gtk::Button *left_button, *right_button;
    Gtk::Label *basic_label, *colors_label, *effects_label;

    void fitImage(Gtk::Allocation &);
    void loadImage();
    void zoomImage();

    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);
    //////////////////////////////////////////////////////////////

    //constructor and desctructor
    MainWindow();
    ~MainWindow();

    //allows UserInterface class to change displayed Photo
    void changePhoto(Photo *);
};
