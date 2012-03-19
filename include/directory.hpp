#pragma once

#include <boost/filesystem.hpp>
#include <vector>

#include "photo.hpp"
#include "disk.hpp"
class Disk;

/** @class Directory
 *  @brief A class representing a single directory, providing its photos and subdirectories.
 */
class Directory {

  public:
    Directory (boost::filesystem::path);
    ~Directory ();

    std::vector<Directory*> getSubdirectories();
    std::vector<Photo*> getPhotos();

    /** 
     * @brief This method is better than checking the size of getPhotos, because it does not
     * initialize new Photo objects
     */
    bool hasPhotos();

    /** 
     * @brief This method is better than checking the size of getSubdirectories, because it does not
     * initialize new Directory objects
     */
    bool hasSubdirectories();

    /**
     * @returns A path, relative to the main library directory.
     */
    boost::filesystem::path getPath();
    std::string getName();

  private:
    Disk* disk;
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
    std::vector<Directory*> subdirectories;
};
