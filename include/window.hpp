#pragma once

#include <gtkmm.h>

class WindowContent;

/** @class MainWindow
 *  @brief Class representing main window of an application.
 *         Consist of elements visible in every mode (library view/photo edit
 *         view), and WindowContent Decorator, which defines type of view and
 *         is responsible of handling view-specific tasks and properties.
 */

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
    Gtk::Box toolbar;
    Gtk::Notebook notebook;
    Gtk::Box left_box, right_box, bottom_box;
    Gtk::ScrolledWindow display;

    //menubar creating
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Glib::RefPtr<Gtk::ActionGroup> action_group;
    Glib::RefPtr<Gtk::RadioAction> library_view, edit_view;

    //bottom bar widgets
    Gtk::Scale zoom_slider;
    Gtk::Image zoom_icon;
    Gtk::Label statusbar;

    //toolbar
    Gtk::ToolButton save_button, delete_button;
    Gtk::ToolButton star_button, tags_button;

    //constructor and desctructor
    MainWindow();
    ~MainWindow();

    //signal handlers for changing type of view
    void showLibraryView();
    void showEditView();

    //other signal handlers
    void editPreferences() {};
    void showAbout();
};

/** @class WindowContent
 *  @brief Abstract class, parent of LibraryViewContent and EditViewContent,
 *         uses Decorator pattern to define type of view (library/photo edit).
 */

class WindowContent {
  public:
    WindowContent() {};
    virtual ~WindowContent() {};
    virtual void updatePixbuf() = 0;
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
    Gtk::Box edit_buttons;
    Gtk::ToolButton left_button, right_button;
    Gtk::Box basic_box, colors_box, effects_box;
    Gtk::Label basic_label, colors_label, effects_label;
    Gtk::Button library_button, undo_button, redo_button;

    //storing pixbuf of current photo
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;

    //handling signals
    void onPageSwitch(Gtk::Widget *, guint);
    void fitImage(Gtk::Allocation &);
    void loadImage();
    void zoomImage();

    //signals storing (for disconnecting)
    sigc::connection zoom_signal, fit_signal, page_signal;

    //additional function for fitting Pixbuf to widget
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);

    //allows UserInterface class to change displayed Photo
    virtual void updatePixbuf();
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
    Gtk::Label tags_label;

    //directory tree
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        ModelColumns() { add(name); }
        Gtk::TreeModelColumn<Glib::ustring> name;
    } columns;

    Gtk::TreeView directory_tree;
    Glib::RefPtr<Gtk::TreeStore> directory_model;

    //signal handlers
    void loadImages(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

    //other methods
    virtual void updatePixbuf() {};
    void fillDirectoryTree();
};
