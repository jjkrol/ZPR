#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"
#include "../include/directory.hpp"

UserInterface* UserInterface::instance = NULL;

UserInterface* UserInterface::getInstance(int argc, char *argv[]) {
  if(instance == NULL) instance = new UserInterface(argc, argv);
  return instance;
}

//UserInterface constructor - creates main window and its widgets
UserInterface::UserInterface(int argc, char *argv[]) : kit(argc, argv) {
  //connecting to core and loading directory
  core = CoreController::getInstance();
  current_dir = core->getDirectoryTree();
  photos = current_dir->getPhotos();
  current_photo = photos.begin();
}

void UserInterface::init() {
  //creating main window
  main_window = new MainWindow();

  //loading image
  main_window->changePhoto(*current_photo);
}

//UserInterface class descructor
void UserInterface::destroy() {
  delete main_window;
  delete instance;
  instance = NULL;
}

//function connects signals and shows main window
void UserInterface::showEditWindow() {
  main_window->maximize();
  main_window->show_all_children();
  if(main_window) kit.run(*main_window);
}

//method for loading next image from folder
void UserInterface::nextImage() {
  if(current_photo == --photos.end()) return;
  current_photo++;
  main_window->changePhoto(*current_photo);
}

//method for loading previous image from folder
void UserInterface::prevImage() {
  if(current_photo == photos.begin()) return;
  current_photo--;
  main_window->changePhoto(*current_photo);
}
