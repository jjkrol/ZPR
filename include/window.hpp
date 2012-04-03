#pragma once

#include <gtkmm.h>

/** @class MainWindow
 *  @brief Class representing main window of an application.
 *         Consist of elements visible in every mode (library view/photo edit
 *         view), and WindowContent Decorator, which defines type of view and
 *         is responsible of handling view-specific tasks and properties.
 */

class WindowContent;

class MainWindow : public Gtk::Window {
  public:
    friend class UserInterface;
    friend class LibraryView;
    friend class EditView;

  private:
    //connection with UserInterface class
    UserInterface *gui;

    //defines active type of view
    WindowContent *content;

    //containers
    Gtk::Grid grid;
    Gtk::Notebook notebook;
    Gtk::Box left_box, right_box, bottom_box;
    Gtk::ScrolledWindow display;

    //menubar
    Gtk::MenuBar menu;

    //bottom bar widgets
    Gtk::Scale zoom_slider;
    Gtk::Image zoom_icon;
    Gtk::Label statusbar;

    //constructor and desctructor
    MainWindow();
    ~MainWindow() {};

    //signal handlers for changing tyoe if view
    void showLibraryView();
    void showEditView();
};

/** @class WindowContent
 *  @brief Abstract class, parent of LibraryViewContent and EditViewContent,
 *         uses Decorator pattern to define type of view (library/photo edit).
 */

class WindowContent {
  public:
    WindowContent() {};
    virtual ~WindowContent() {};
    virtual void changePhoto(Photo *) = 0;
};

/** @class EditView
 *  @brief Uses Decorator pattern to switch MainWindow to photo edit view
 *         and handles tasks associated with this view.
 */

class EditView : public WindowContent {
  public:
    friend class MainWindow;
    friend class UserInterface;
  private:
    EditView() {};
    ~EditView();
    EditView(MainWindow *);

    //connection with upper classes
    UserInterface *gui;
    MainWindow *window;

    //widgets
    Gtk::Image image;
    Gtk::Label basic_label, colors_label, effects_label;
    Gtk::Button library_button, left_button, right_button;

    //storing current photo
    Photo *current_photo;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;

    //handling signals
    void fitImage(Gtk::Allocation &);
    void loadImage();
    void zoomImage();

    //signals storing (for disconnecting)
    sigc::connection zoom_signal;
    sigc::connection fit_signal;

    //additional function for fitting Pixbuf to widget
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);

    //allows UserInterface class to change displayed Photo
    virtual void changePhoto(Photo *);
};

/** @class LibraryView
 *  @brief Uses Decorator pattern to switch MainWindow to library view
 *         and handles tasks associated with this view.
 */

class LibraryView : public WindowContent {
  public:
    friend class MainWindow;
    friend class UserInterface;
  private:
    LibraryView() {};
    ~LibraryView();
    LibraryView(MainWindow *);

    //connection with upper classes
    UserInterface *gui;
    MainWindow *window;

    //widgets
    Gtk::DrawingArea images;
    Gtk::TreeView directory_tree;
    Gtk::Label tags_label;
    Gtk::Button edit_button;

    virtual void changePhoto(Photo *) {};
};
