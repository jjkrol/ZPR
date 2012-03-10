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

    void getThumbnail();
    boost::gil::rgb16_image_t getImage();
    boost::filesystem::path getPath();
    /* to be added:
     * copying constructor
     * deleteFromDatabase()
     * deleteCompletely
     * save()
     * move()
     * addTag()
     * deleteTag()
     * getTags()
     * putEffect()
     * getEffectPreview(EffectParams, Size)
     * undoEffect() ??
     */

  private:
    Photo (boost::filesystem::path argumentPath);
    boost::filesystem::path photoPath;
    boost::gil::rgb16_image_t image;
};
