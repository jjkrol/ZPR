#pragma once

#include <boost/filesystem.hpp>
#include <vector>

#include "photo.hpp"

class Directory {

  public:
    Directory (boost::filesystem::path);
    virtual ~Directory () { };

    std::vector<Directory*> getSubdirectories();
    std::vector<Photo*> getPhotos();
    boost::filesystem::path getPath();
    std::string getName();
    bool hasPhotos();
    bool hasSubdirectories();

  private:
    std::vector<boost::filesystem::path> getDirectoryContents();
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
    std::vector<Directory*> subdirectories;
};
