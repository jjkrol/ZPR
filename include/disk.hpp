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
 *  @brief A class providing an adapter to the disk space.
 */
class Disk : public Asynchronous {

  public:
    /**
     * singleton initialization method
     */
    static Disk* getInstance(boost::filesystem::path libraryDirectoryPath="");

    paths_t getPhotosPaths(boost::filesystem::path directoryPath);
    paths_t getSubdirectoriesPaths(boost::filesystem::path directoryPath);

    paths_t getAbsolutePhotosPaths(boost::filesystem::path directoryPath);
    paths_t getAbsoluteSubdirectoriesPaths(boost::filesystem::path directoryPath);

    bool hasPhotos(boost::filesystem::path directoryPath);
    bool hasSubdirectories(boost::filesystem::path directoryPath);

    bool hasAbsolutePhotos(boost::filesystem::path directoryPath);
    bool hasAbsoluteSubdirectories(boost::filesystem::path directoryPath);

    Glib::RefPtr<Gdk::Pixbuf> getPhotoFile(boost::filesystem::path photoPath);

    void deletePhoto(boost::filesystem::path photoPath);

    boost::filesystem::path movePhoto(boost::filesystem::path sourcePath, boost::filesystem::path destinationPath);

    bool exists(boost::filesystem::path file);

    bool absoluteExists(boost::filesystem::path file);

    boost::filesystem::path makeAbsolutePath(boost::filesystem::path);

  private:
    Disk (boost::filesystem::path);
    Disk& operator= (const Disk&);
    Disk (const Disk&);
    ~Disk (){};

    //internal functions
    void * internalGetPhotosPaths(boost::filesystem::path directoryPath);
    void * internalGetSubdirectoriesPaths(boost::filesystem::path directoryPath);

    void * internalGetAbsolutePhotosPaths(boost::filesystem::path directoryPath);
    void * internalGetAbsoluteSubdirectoriesPaths(boost::filesystem::path directoryPath);

    void * internalHasPhotos(boost::filesystem::path);
    void * internalHasSubdirectories(boost::filesystem::path directoryPath);

    void * internalAbsoluteHasPhotos(boost::filesystem::path);
    void * internalAbsoluteHasSubdirectories(boost::filesystem::path directoryPath);

    void * internalGetPhotoFile(boost::filesystem::path photoPath);

    void * internalDeletePhoto(boost::filesystem::path photoPath);

    void * internalMovePhoto(boost::filesystem::path sourcePath, boost::filesystem::path destinationPath);

    bool isAcceptableExtension(std::string);

    static Disk* instance;
    paths_t getDirectoryContents(boost::filesystem::path);
    boost::filesystem::path makeSystemAbsolutePath(boost::filesystem::path);
    boost::filesystem::path libraryDirectoryPath;
};

