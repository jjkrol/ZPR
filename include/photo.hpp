#pragma once

#include <boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <map>

class Photo {

  public:
    static Photo * initialize(boost::filesystem::path argumentPath);
    ~Photo ();

    boost::gil::rgb8_image_t getThumbnail();
    boost::gil::rgb8_image_t getImage();

    boost::filesystem::path getPath();
    boost::filesystem::path getFilename();

  private:
    Photo (boost::filesystem::path argumentPath);
    boost::filesystem::path photoPath;
};
