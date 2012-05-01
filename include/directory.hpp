#pragma once

#include <boost/filesystem.hpp>
#include <vector>


class Disk;
class Photo;

/** @class Directory
 *  @brief A class representing a single directory, providing its photos and subdirectories.
 */
class Directory {

  public:
    Directory (boost::filesystem::path);
    ~Directory ();

    std::vector<Directory*> getSubdirectories();
    std::vector<Photo*> getPhotos();
    std::vector<Directory*> getAbsoluteSubdirectories();

    /** 
     * @brief This method is better than checking the size of getPhotos, because it does not
     * initialize new Photo objects
     */
    bool hasPhotos();

    /** 
     * @brief This method is better than checking the size of getSubdirectories, because it does not
     * initialize new Directory objects
     */
    bool hasSubdirectories();
    bool hasAbsoluteSubdirectories();

    /**
     * @returns A path, relative to the main library directory.
     */
    boost::filesystem::path getPath();
    std::string getName();

  protected:
    Disk* disk;
    boost::filesystem::path directoryPath;
    std::vector<Photo*> photos;
  private:
    std::vector<Directory*> subdirectories;
};

class AbsoluteDirectory : public Directory{

  public:
    AbsoluteDirectory (boost::filesystem::path);
    ~AbsoluteDirectory ();

    std::vector<AbsoluteDirectory*> getSubdirectories();
    std::vector<Photo*> getPhotos();

    /** 
     * @brief This method is better than checking the size of getPhotos, because it does not
     * initialize new Photo objects
     */
    bool hasPhotos();

    /** 
     * @brief This method is better than checking the size of getSubdirectories, because it does not
     * initialize new AbsoluteDirectory objects
     */
    bool hasSubdirectories();

    /**
     * @returns A path, relative to the main library AbsoluteDirectory.
     */
    boost::filesystem::path getPath();
    std::string getName();
  private:
    std::vector<AbsoluteDirectory*> subdirectories;

};

