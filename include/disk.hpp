#pragma once

#include<boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>

#include "core.hpp"

#include "photo.hpp"

/** @class Disk
 *  @brief A class providing an adapter to the disk space.
 */
class Disk {

  public:
    Disk ();
    virtual ~Disk (){};

    std::vector<boost::filesystem::path> getPhotosPaths(boost::filesystem::path directoryPath);
    std::vector<boost::filesystem::path> getSubdirectoriesPaths(boost::filesystem::path directoryPath);

    bool hasPhotos(boost::filesystem::path directoryPath);
    bool hasSubdirectories(boost::filesystem::path directoryPath);

    boost::gil::rgb8_image_t getPhotoFile(boost::filesystem::path photoPath);

  private:
    std::vector<boost::filesystem::path> getDirectoryContents(boost::filesystem::path);
    boost::filesystem::path makeAbsolutePath(boost::filesystem::path);
    boost::filesystem::path libraryDirectoryPath;
};
