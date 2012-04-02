#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/window.hpp"
#include "../include/directory.hpp"

/// @var UserInterface::instance
/// @brief Pointer to unique UserInterface class instance.
UserInterface* UserInterface::instance = NULL;

/// @fn UserInterface* UserInterface::getInstance(int argc, char *argv[]) 
/// @brief Method allowing access to unique UserInterface class instance.
/// @returns Pointer to UserInterface instance.
UserInterface* UserInterface::getInstance(int argc, char *argv[]) {
  if(instance == NULL) instance = new UserInterface(argc, argv);
  return instance;
}

/// @fn UserInterface::UserInterface(int argc, char *argv[])
/// @brief UserInterface constructor - connects to CoreController and gets Photos.
UserInterface::UserInterface(int argc, char *argv[]) : kit(argc, argv) {
  core = CoreController::getInstance();
  current_dir = core->getDirectoryTree();
  photos = current_dir->getPhotos();
  current_photo = photos.begin();
}

/// @fn void UserInterface::init()
/// @brief Method responsible for creating MainWindow and loading Photo object.
void UserInterface::init() {
  main_window = new MainWindow();
  main_window->changePhoto(*current_photo);
}

/// @fn void UserInterface::destroy()
/// @brief Method responsible for destroying UserInterface unique object.
void UserInterface::destroy() {
  delete main_window;
  delete instance;
  instance = NULL;
}

/// @fn void UserInterface::showEditWindow()
/// @brief Method responsible for showing application main window.
void UserInterface::showEditWindow() {
  main_window->maximize();
  main_window->show_all_children();
  if(main_window) kit.run(*main_window);
}

/// @fn void UserInterface::nextImage()
/// @brief Method (signal handler) responsible for loading next image from folder.
void UserInterface::nextImage() {
  if(current_photo == --photos.end()) return;
  current_photo++;
  main_window->changePhoto(*current_photo);
}

/// @fn void UserInterface::prevImage()
/// @brief Method (signal handler) responsible for loading previous image from folder.
void UserInterface::prevImage() {
  if(current_photo == photos.begin()) return;
  current_photo--;
  main_window->changePhoto(*current_photo);
}
