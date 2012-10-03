#pragma once

#include <gtkmm.h>
#include "window.hpp"

class DBPrompt : public Gtk::InfoBar {
  public:
    friend class MainWindow;

    DBPrompt(MainWindow *);
    ~DBPrompt() {};

  private:
    MainWindow *window;
    Gtk::Box box;
    Gtk::Image icon;
    Gtk::Label label;
    void createDatabase(int);
};

class UnsavedPhotosPrompt : public Gtk::InfoBar {
  public:
    friend class MainWindow;

    UnsavedPhotosPrompt(MainWindow *);
    ~UnsavedPhotosPrompt() {};

  private:
    MainWindow *window;
    Gtk::Box box;
    Gtk::Image icon;
    Gtk::Label label;
};
