#pragma once

#include <map>
#include <string>
#include <gtkmm.h>

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
    Gtk::Box apply_cancel_buttons;
    Gtk::Button apply_button, cancel_button;
    std::map<std::string, Gtk::ToolButton*> plugin_map;
    Gtk::Frame pluginFrame;
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
    void showPluginsList();

    //signals storing (for disconnecting)
    sigc::connection zoom_signal, fit_signal, page_signal;

    //additional function for fitting Pixbuf to widget
    Glib::RefPtr<Gdk::Pixbuf> resizeImage(Glib::RefPtr<Gdk::Pixbuf>, Gdk::Rectangle);
};


