#pragma once

#include <boost/filesystem.hpp>

class Photo {

  public:
    Photo (boost::filesystem::path argumentPath);
    virtual ~Photo () { };

    void getThumbnail();
    void getImage();
    boost::filesystem::path getPath();

  private:
    boost::filesystem::path photoPath;
};
