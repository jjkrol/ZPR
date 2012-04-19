#pragma once

#include <gtkmm.h>
#include "./photoData.hpp"

class WindowContent;
class Directory;

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
    void editPreferences();
    void editDatabase();
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
    CoreController *core;
    MainWindow *window;

    //widgets
    Gtk::Image image;
    Gtk::Box edit_buttons;
    Gtk::ToolButton left_button, right_button;
    Gtk::Box basic_box, colors_box, effects_box;
    Gtk::Label basic_label, colors_label, effects_label;
    Gtk::ToolButton library_button;
    Gtk::Button undo_button, redo_button;

    //storing current photo
    PhotoData current_photo;

    //handling signals
    void onPageSwitch(Gtk::Widget *, guint);
    void fitImage(Gtk::Allocation &);
    void loadImage();
    void zoomImage();
    void nextImage();
    void prevImage();

    //signals storing (for disconnecting)
    sigc::connection zoom_signal, fit_signal, page_signal;

    //additional function for fitting Pixbuf to widget
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);
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
    CoreController *core;
    MainWindow *window;

    //widgets
    Gtk::DrawingArea images;
    Gtk::Label tags_label;

    //database prompt
    Gtk::InfoBar *db_prompt;

    Gtk::TreeView directory_tree;

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        ModelColumns() { add(name); }
        Gtk::TreeModelColumn<Glib::ustring> name;
    } columns;
    Glib::RefPtr<Gtk::TreeStore> directory_model;

    //signal handlers
    void loadImages(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

    //other methods
    void createDatabase(int);
    void fillDirectoryTree();
    void promptAboutDatabase();
    void addSubdirectories(Directory *, Gtk::TreeModel::Row &);
};
