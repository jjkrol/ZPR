#include "../include/gui.hpp"
#include "../include/core.hpp"
#include "../include/window.hpp"

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
/// @param argc Number of parameters (for handling GTK+ command line arguments). 
/// @param argv Table of parameters (for handling GTK+ command line arguments). 
UserInterface::UserInterface(int argc, char *argv[]) : kit(argc, argv) {
  core = CoreController::getInstance();
}

/// @fn void UserInterface::init()
/// @brief Method responsible for creating MainWindow and loading Photo object.
void UserInterface::init() {
  main_window = new MainWindow();
  main_window->showLibraryView();
  if(main_window) kit.run(*main_window);
}

/// @fn void UserInterface::destroy()
/// @brief Method responsible for destroying UserInterface unique object.
void UserInterface::destroy() {
  kit.quit();
  delete main_window;
  delete instance;
  instance = NULL;
}
