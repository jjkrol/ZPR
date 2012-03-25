#pragma once

#include <boost/filesystem.hpp>
#include <gtkmm.h>
#include <unistd.h>

#include "core.hpp"
#include "photo.hpp"

/** @class Disk
 *  @brief A class providing an adapter to the disk space.
 */
class Disk {

  public:
    static Disk* getInstance(boost::filesystem::path libraryDirectoryPath="");

    void mainLoop();

    std::vector<boost::filesystem::path> getPhotosPaths(boost::filesystem::path directoryPath);
    std::vector<boost::filesystem::path> getSubdirectoriesPaths(boost::filesystem::path directoryPath);

    bool hasPhotos(boost::filesystem::path directoryPath);
    bool hasSubdirectories(boost::filesystem::path directoryPath);

    Glib::RefPtr<Gdk::Pixbuf> getPhotoFile(boost::filesystem::path photoPath);

  private:
    Disk (boost::filesystem::path);
    Disk& operator= (const Disk&);
    Disk (const Disk&);
    ~Disk (){};

    static Disk* instance;
    std::vector<boost::filesystem::path> getDirectoryContents(boost::filesystem::path);
    boost::filesystem::path makeAbsolutePath(boost::filesystem::path);
    boost::filesystem::path libraryDirectoryPath;
};
