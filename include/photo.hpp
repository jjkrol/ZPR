#pragma once

#include <boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <map>
#include <set>
#include <gtkmm.h>


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
class DBConnector;
class PhotoCache;
class Effect;

typedef std::set<std::string> taglist_t;

class Photo {

  public:
    static Photo * getInstance(boost::filesystem::path argumentPath);

    /**
     * @TODO Implement this
     */
    Glib::RefPtr<Gdk::Pixbuf> getThumbnail();

    /**
     * @TODO optimisation, implement various image types
     */
    Glib::RefPtr<Gdk::Pixbuf> getPixbuf();

    void setPixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

    /**
     * @returns A relative path to the photo.
     */
    boost::filesystem::path getPath();
    boost::filesystem::path getFilename();

    /**
     * @returns absolute path of a photo
     * @warning use with caution!
     */
    boost::filesystem::path getAbsolutePath();
    /**
     * @brief moves photo to a new destination in the filesystem 
     * and updates the database
     */
    void      move(boost::filesystem::path destinationPath);

    /**
     * @brief deletes the photo only from library (stays on disk)
     * @warning this function also destroys the photo object
     */
    void      deleteFromLibrary();

    /**
     * @biref the photo will fall into the deepest darkness of nothingness
     * @warning this function also destroys the photo object
     */
    void      deleteFromLibraryAndDisk();

    /**
     * @brief adds a new tag to the photo
     */
    void      addTag(std::string tag);

    /**
     * removes tag from the photo
     * @TODO db function
     */
    void      removeTag(std::string tag);

    taglist_t getTags();

    /**
     * checks if the photo has tag a certain tag
     */
    bool      hasTag(std::string tag);

    /**
     * puts specified effect on the photo
     */
    void putEffect(Effect * effect);

    /**
     * saves modified fixbuf to disk
     */
    void save();

  private:
    Photo (boost::filesystem::path argumentPath);
    Photo& operator=(const Photo&);
    Photo( const Photo&);
    ~Photo ();

    Disk* disk;
    DBConnector* db;
    PhotoCache * cache;
    boost::filesystem::path photoPath;
    std::set<std::string> tags;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf;


};

