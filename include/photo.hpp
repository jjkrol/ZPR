#pragma once

#include <boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <map>
#include <gtkmm.h>

#include "disk.hpp"

/** @class Photo
 * @brief A class representing a single photo. 
 *
 * Main features (most to be
 * yet implemented):
 *  - a real photo (file or db entry) has *only one* object representing it.
 *  this means that you can't have two Photo objects representing an image
 *  with the same id/path. This implies some manipulations in constructor
 *  (similar to singleton maybe?)
 *  - providing thumbnails and full image
 *  - changing tags
 *  - moving and deleting the photo
 *  - placing effects on the photo and getting effect previews of certain size
 */

class Disk;

class Photo {

  public:
    static Photo * getInstance(boost::filesystem::path argumentPath);

    /**
     * @TODO Implement this
     */
    boost::gil::rgb8_image_t getThumbnail();

    /**
     * @TODO optimisation, implement various image types
     */
    boost::gil::rgb8_image_t getImage();

    Glib::RefPtr<Gdk::Pixbuf> getPixbuf();

    /**
     * @returns A relative path to the photo.
     */
    boost::filesystem::path getPath();
    boost::filesystem::path getFilename();

  private:
    Photo (boost::filesystem::path argumentPath);
    Photo& operator=(const Photo&);
    Photo( const Photo&);
    ~Photo ();

    Disk* disk;
    boost::filesystem::path photoPath;
};

