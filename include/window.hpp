#pragma once

#include <gtkmm.h>

class PreferencesDialog;
class DBManagerDialog;
class CoreController;
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
    friend class UnsavedPhotosPrompt;
    friend class UserInterface;
    friend class LibraryView;
    friend class EditView;
    friend class DBPrompt;

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
