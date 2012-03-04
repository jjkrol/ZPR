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

  private:
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
    std::vector<Directory*> subdirectories;
};
