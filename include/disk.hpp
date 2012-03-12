#include<boost/filesystem.hpp>

#include"../include/photo.hpp"
#include"../include/directory.hpp"

class Disk {
  public:
    Disk ();
    virtual ~Disk (){};
    std::vector<Photo*> getPhotosFromDirectory(boost::filesystem::path p);
    std::vector<Directory*> getSubdirectories(boost::filesystem::path p);

  private:
};
