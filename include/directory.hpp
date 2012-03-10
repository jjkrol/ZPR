#pragma once

#include <boost/filesystem.hpp>
#include <vector>

#include "photo.hpp"

class Directory {

  public:
    Directory (boost::filesystem::path);
    virtual ~Directory () { };

    std::vector<Directory*> getSubdirectories();
    std::vector<Directory*> getCachedSubdirectories();
    std::vector<Photo*> getPhotos();
    std::vector<Photo*> getCachedPhotos();
    boost::filesystem::path getPath();
/* to be added:
 * 
 *
 */

  private:
    std::vector<boost::filesystem::path> getDirectoryContents();
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
    std::vector<Directory*> subdirectories;
};
