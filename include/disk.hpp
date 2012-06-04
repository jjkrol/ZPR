#pragma once

#include <boost/filesystem.hpp>
#include <gtkmm.h>
#include <unistd.h>
#include "asynchronous.hpp"


/**
 * this is a shorthand for frequently used vector
 * containing paths
 */
typedef std::vector<boost::filesystem::path> paths_t;


/** @class Disk
 *  @brief A class providing an adapter to the disk space. Works in a separate
 *         thread.
 */
class Disk : public Asynchronous {
  public:
    //singletons' methods
    static Disk* getInstance(boost::filesystem::path libraryDirectoryPath="");

    //getting paths of photos and subdirectories
    paths_t getPhotosPaths(boost::filesystem::path directoryPath);
    paths_t getSubdirectoriesPaths(boost::filesystem::path directoryPath);
    paths_t getAbsolutePhotosPaths(boost::filesystem::path directoryPath);
    paths_t getAbsoluteSubdirectoriesPaths(boost::filesystem::path directoryPath);

    //checking if directory has photos or subdirectories
    bool hasPhotos(boost::filesystem::path directoryPath);
    bool hasSubdirectories(boost::filesystem::path directoryPath);
    bool hasAbsolutePhotos(boost::filesystem::path directoryPath);
    bool hasAbsoluteSubdirectories(boost::filesystem::path directoryPath);

    //checking if file exist
    bool exists(boost::filesystem::path file);
    bool absoluteExists(boost::filesystem::path file);

    //other methods
    Glib::RefPtr<Gdk::Pixbuf> getPhotoFile(boost::filesystem::path photoPath);
    void deletePhoto(boost::filesystem::path photoPath);
    boost::filesystem::path makeAbsolutePath(boost::filesystem::path);
    boost::filesystem::path movePhoto(
      boost::filesystem::path source_path,
      boost::filesystem::path destination_path);

  private:
    Disk (boost::filesystem::path);
    Disk& operator= (const Disk&);
    Disk (const Disk&);
    ~Disk (){};

    //internal methods for getting paths of photos and subdirectories
    void * internalGetPhotosPaths(boost::filesystem::path directoryPath);
    void * internalGetSubdirectoriesPaths(boost::filesystem::path directoryPath);
    void * internalGetAbsolutePhotosPaths(boost::filesystem::path directoryPath);
    void * internalGetAbsoluteSubdirectoriesPaths(boost::filesystem::path directoryPath);

    //internal methods for checking if directory has photos or subdirectories
    void * internalHasPhotos(boost::filesystem::path);
    void * internalHasSubdirectories(boost::filesystem::path directoryPath);
    void * internalAbsoluteHasPhotos(boost::filesystem::path);
    void * internalAbsoluteHasSubdirectories(boost::filesystem::path directoryPath);

    //other internal methods
    void * internalGetPhotoFile(boost::filesystem::path photoPath);
    void * internalDeletePhoto(boost::filesystem::path photoPath);
    void * internalMovePhoto(
      boost::filesystem::path source, boost::filesystem::path destination);
    bool isAcceptableExtension(std::string);
    boost::filesystem::path makeSystemAbsolutePath(boost::filesystem::path);
    paths_t getDirectoryContents(boost::filesystem::path);

    //properties of a class
    static Disk* instance;
    boost::filesystem::path libraryDirectoryPath;
};

