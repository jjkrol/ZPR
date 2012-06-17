#pragma once

#include <gtkmm.h>
#include <vector>
#include <string>
#include <map>
#include "./global.hpp"

class PreferencesDialog;
class DBManagerDialog;
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

    void refreshActiveView();

  private:
    //connection with CoreController
    CoreController *core;

    //defines active type of view
    WindowContent *content;

    //containers
    Gtk::Grid grid;
    Gtk::Box toolbar;
    Gtk::Notebook notebook;
    Gtk::Box left_box, right_box, bottom_box;
    Gtk::ScrolledWindow display;

    //menubar
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Glib::RefPtr<Gtk::ActionGroup> action_group;
    Glib::RefPtr<Gtk::RadioAction> library_view, edit_view;

    //bottom bar widgets
    Gtk::Scale zoom_slider;
    Gtk::Image zoom_icon;
    Gtk::Label statusbar;

    //toolbar
    Gtk::ToolButton save_button, delete_button;
    Gtk::ToolButton edit_button, tags_button;

    //prompt
    Gtk::InfoBar *prompt;
    
    //constructor and desctructor
    MainWindow();
    ~MainWindow();

    //signal handlers for changing type of view
    void showLibraryView();
    void showEditView();

    //other signal handlers
    void editPreferences();
    void editDatabase();
    void editPhotoTags();
    void showAbout();
    bool on_delete_event(GdkEventAny* event);
    void managePromptResponse(int response);
};

/** @class WindowContent
 *  @brief Abstract class, parent of LibraryViewContent and EditViewContent,
 *         uses State pattern to define type of view (library/photo edit).
 */

class WindowContent {
  public:
    virtual void refreshView() = 0;
    WindowContent() {};
    virtual ~WindowContent() {};
};

/** @class EditView
 *  @brief Uses State pattern to switch MainWindow to photo edit view
 *         and handles tasks associated with this view.
 */
class EditView : public WindowContent {
  public:
    friend class MainWindow;
    friend class UserInterface;

    virtual void refreshView();

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

    //plugin widgets
    Gtk::ToolButton put_effect_button;
    std::map<std::string, Gtk::ToolButton*> plugin_map;
    Gtk::Box plugin_buttons;

    //storing current photo
    PhotoData current_photo;

    //handling signals
    void editWithExternalEditor();
    void onPageSwitch(Gtk::Widget *, guint);
    void fitImage(Gtk::Allocation &);
    void loadImage();
    void zoomImage();
    void nextImage();
    void prevImage();
    void applyEffect();
    void showPluginBox(std::string name);

    //signals storing (for disconnecting)
    sigc::connection zoom_signal, fit_signal, page_signal;

    //additional function for fitting Pixbuf to widget
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);
};

/** @class LibraryView
 *  @brief Uses State pattern to switch MainWindow to library view
 *         and handles tasks associated with this view.
 */
class LibraryView : public WindowContent {
  public:
    friend class MainWindow;
    friend class UserInterface;

    virtual void refreshView();

  private:
    LibraryView() {};
    ~LibraryView();
    LibraryView(MainWindow *);

    //connection with upper classes
    CoreController *core;
    MainWindow *window;

    //widgets
    Gtk::DrawingArea images;

    //database prompt
    Gtk::InfoBar *db_prompt;

    //directory tree
    Glib::RefPtr<Gtk::TreeStore> directory_tree;
    Gtk::TreeView directory_view;
    DirectoryTreeColumns dir_columns;

    //tags list
    Glib::RefPtr<Gtk::ListStore> tags_list;
    Gtk::TreeView tags_view;
    TagsListColumns tags_columns;

    //signal handlers
    void loadImagesByDirectory(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
    void loadImagesByTags();

    //other methods
    void fillTagsList();
    void fillDatabaseTree();
    void createDatabase(int);
    void promptAboutDatabase();
    void addSubdirectories(Directory *, Gtk::TreeModel::Row &);
};
