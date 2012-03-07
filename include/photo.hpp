#pragma once

#include <boost/filesystem.hpp>
#include <map>

class Photo {

  public:
    static Photo * initialize(boost::filesystem::path argumentPath);
    ~Photo ();

    void getThumbnail();
    void getImage();
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
};
