#pragma once

#include <boost/filesystem.hpp>
#include <vector>

#include "photo.hpp"
#include "disk.hpp"

class Directory {

  public:
    Directory (boost::filesystem::path);
    virtual ~Directory () { };

    std::vector<Directory*> getSubdirectories();
    std::vector<Photo*> getPhotos();

    bool hasPhotos();
    bool hasSubdirectories();

    boost::filesystem::path getPath();
    std::string getName();

  private:
    Disk* disk;
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
    std::vector<Directory*> subdirectories;
};
