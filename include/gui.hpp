#pragma once

#include <gtkmm.h>
#include <vector>

/** @class UserInterface
 *  @brief Class representing Graphical User Interface.
 *
 *  Class uses GTKmm, it serves as a link between user and program
 *  taking commands from user and sending them to the Core class.
 */

class CoreController;
class MainWindow;
class Directory;
class Photo;

class UserInterface {
  private:
    //singleton instance
    static UserInterface* instance;

    //comunication with CoreController
    CoreController *core;
    Directory *current_dir;
    std::vector<Photo*> photos;
    std::vector<Photo*>::iterator current_photo;

    //main window and kit
    Gtk::Main kit;
    MainWindow *main_window;

    UserInterface() {};
    ~UserInterface() {};
    UserInterface(int argc, char *argv[]);
    UserInterface & operator=(const UserInterface &);
    UserInterface(const UserInterface &) {};

  public:
    static UserInterface* getInstance(int argc = 0, char *argv[] = NULL);

    void init();
    void destroy();
    void showEditWindow();

    //signals
    void nextImage();
    void prevImage();
};
