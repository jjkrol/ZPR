#pragma once

#include <gtkmm.h>

class CoreController;
class MainWindow;

/** @class UserInterface
 *  @brief Singleton class representing Graphical User Interface.
 *         Class uses GTKmm, it serves as a link between user and program
 *         taking commands from user and sending them to the Core class.
 */

class UserInterface {
  public:
    friend class MainWindow;
    friend class LibraryView;
    friend class EditView;

    //allows access to unique class instance
    static UserInterface* getInstance(int argc = 0, char *argv[] = NULL);

    //external interface
    void init();
    void destroy();

 private:
    //singleton instance
    static UserInterface* instance;

    //communication with CoreController
    CoreController *core;

    //main window and kit
    Gtk::Main kit;
    MainWindow *main_window;

    //hidden constructors and descructors
    UserInterface() {};
    ~UserInterface() {};
    UserInterface(int argc, char *argv[]);
    UserInterface & operator=(const UserInterface &);
    UserInterface(const UserInterface &) {};
};
