#pragma once

#include<boost/filesystem.hpp>

#include "photo.hpp"

class Disk {

  public:
    Disk ();
    virtual ~Disk (){};

    std::vector<boost::filesystem::path> getPhotos(boost::filesystem::path);
    std::vector<boost::filesystem::path> getSubdirectories(boost::filesystem::path);

    bool hasPhotos(boost::filesystem::path);
    bool hasSubdirectories(boost::filesystem::path);

  private:
    std::vector<boost::filesystem::path> getDirectoryContents(boost::filesystem::path);
    boost::filesystem::path makeAbsolutePath(boost::filesystem::path);
    boost::filesystem::path libraryDirectoryPath;
};
