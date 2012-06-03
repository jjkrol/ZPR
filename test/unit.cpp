#define BOOST_TEST_MAIN

#include <fstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/directory.hpp"
#include "../include/dbConnector.hpp"
#include "../include/disk.hpp"

using namespace boost::filesystem;
using namespace std;

BOOST_AUTO_TEST_SUITE( testSuite )

  /*BOOST_AUTO_TEST_CASE( coreClassTest ){
    CoreController* core = CoreController::getInstance("test.cfg");

    BOOST_CHECK(core->getLibraryDirectoryPath()=="./test/test_tree/");

  }*/

  /*BOOST_AUTO_TEST_CASE( photoClassTest ) {
    path p("test/zdj.jpg");
    Photo* myphoto = Photo::getInstance(p);
    BOOST_CHECK(myphoto->getPath() == p );
    //double initialization
    BOOST_CHECK(Photo::getInstance(p) == Photo::getInstance(p));
    
    myphoto->addTag("raz");
    myphoto->addTag("dwa");
    myphoto->addTag("trzy");
    myphoto->removeTag("dwa");

    BOOST_CHECK(myphoto->hasTag("raz"));
    BOOST_CHECK(myphoto->hasTag("trzy"));

    taglist_t tags = myphoto->getTags();

    BOOST_CHECK(tags.size() == 2);
  }

  BOOST_AUTO_TEST_CASE( directoryClassTest ) {
    CoreController* core = CoreController::getInstance("test.cfg");

    Directory* myTestDir = new Directory(""); 
    BOOST_CHECK(myTestDir->getPath()=="/");
    std::vector<Directory*> subdirs = myTestDir->getSubdirectories();

    Directory* alpha = subdirs[0];
    BOOST_REQUIRE(alpha->getName() == "alpha");
    BOOST_CHECK(alpha->getPath() == "/alpha/");

    Directory* beta = subdirs[1];
    BOOST_REQUIRE(beta->getName() == "beta");
    BOOST_CHECK(beta->hasPhotos() == true);
    BOOST_CHECK(beta->hasSubdirectories() == false);
    
    Directory* delta = subdirs[3];
    BOOST_REQUIRE(delta->getName() == "delta");
    BOOST_CHECK(delta->hasPhotos() == false);
    BOOST_CHECK(delta->hasSubdirectories() == false);
    BOOST_CHECK(myTestDir->hasSubdirectories() == true);
    BOOST_CHECK(myTestDir->getSubdirectories().size() == 4);
    BOOST_CHECK(myTestDir->getPhotos().size() == 1);
    // movePhoto check
    Photo* myPhoto = myTestDir->getPhotos()[0]; 
    myPhoto->move(path("/delta/"));
    BOOST_CHECK(delta->hasPhotos() == true);
    myPhoto->deleteFromLibraryAndDisk();
    BOOST_CHECK(delta->hasPhotos() == false);
  } */

  BOOST_AUTO_TEST_CASE( SQLiteConnectorTest ) {
    CoreController *core = CoreController::getInstance("test.cfg");
    path libraryPath = core->getLibraryDirectoryPath();
    Disk *disk = Disk::getInstance();
    //making an instance of a connector which should be tested
    DBConnector *sqlconnector = DBConnectorFactory::getInstance("sqlite");
    string output;

  /******************************************/
  /*    OPENING AND CLOSING DATABASE        */
  /******************************************/

    //closing database which is not opened
    BOOST_CHECK(sqlconnector->close() == DBConnector::CLOSED);
    //closing opened database connection
    BOOST_CHECK(sqlconnector->close() == DBConnector::FAILURE);

    //reopening an existing database
    BOOST_REQUIRE(sqlconnector->open("DB.sqlite") == DBConnector::OPENED);

    //opening database which has been already opened (multi-connections
    //are not allowed, so the function should exit with a FAILURE flag)
    BOOST_CHECK(sqlconnector->open("DB.sqlite") == DBConnector::FAILURE);

  /***********************************************/
  /* ADDING AND DELETING PHOTOS AND DIRECTORIES  */
  /***********************************************/
    //adding photos from directories
    vector<path> paths = disk->getAbsoluteSubdirectoriesPaths(libraryPath);

    for(vector<path>::iterator i=paths.begin(); i!= paths.end(); ++i) {
      (*i) = libraryPath / (*i);
    }

    BOOST_CHECK(sqlconnector->isEmpty() == true);
    BOOST_REQUIRE(sqlconnector->addPhotosFromDirectories(paths) == true);
    BOOST_CHECK(sqlconnector->isEmpty() == false);
    BOOST_CHECK(sqlconnector->close() == DBConnector::CLOSED);
    BOOST_CHECK(sqlconnector->open("DB.sqlite") == DBConnector::OPENED);

    //getting directories from database
    vector<path> paths2;
    BOOST_REQUIRE(sqlconnector->getDirectoriesFromDB(paths2) == true );
    {
      vector<path>::const_iterator i, j;
      i = paths.begin();
      j = paths2.begin();
 
      while( i != paths.end())
        BOOST_CHECK (*(i++) == *(j++));
    }

    //deleting photos
    sqlconnector->getPhotosFromDirectory(paths2[0], paths);
    for(vector<path>::const_iterator i=paths.begin() ; i!= paths.end(); ++i) {
      output += i->string() + "\n";
    }

    BOOST_CHECK(output ==
      libraryPath.string()+"alpha/2011-04-30 00.09.53-2.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.09.55-2.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.10.08.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.10.21.jpg\n"+
      libraryPath.string()+"alpha/3.jpg\n"+
      libraryPath.string()+"alpha/4.jpg\n");

    sqlconnector->deletePhoto(libraryPath/path("alpha/3.jpg"));
    sqlconnector->getPhotosFromDirectory(libraryPath/path("alpha"), paths);

    output.clear();
    for(vector<path>::const_iterator i=paths.begin() ; i!= paths.end(); ++i)
      output += i->string() + "\n";

    BOOST_CHECK(output ==
      libraryPath.string()+"alpha/2011-04-30 00.09.53-2.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.09.55-2.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.10.08.jpg\n"+
      libraryPath.string()+"alpha/2011-04-30 00.10.21.jpg\n"+
      libraryPath.string()+"alpha/4.jpg\n");

    //deleting directory
    sqlconnector->getDirectoriesFromDB(paths);
    paths.erase(find(paths.begin(), paths.end(), libraryPath/path("alpha")));
    sqlconnector->deleteDirectory(libraryPath/path("alpha"));
    sqlconnector->getDirectoriesFromDB(paths2);
    BOOST_REQUIRE(paths.size() == paths2.size());
    {
      vector<path>::const_iterator i, j;
      i = paths.begin();
      j = paths2.begin();
 
      while( i != paths.end())
        BOOST_CHECK (*(i++) == *(j++));
    }

  /*****************************/
  /*    TAGS MANIPULATION      */
  /*****************************/
    //adding tags
    set<string> tags;

    sqlconnector->addTagToPhoto(libraryPath/path("beta/1.jpg"),"architecture");
    sqlconnector->addTagToPhoto(libraryPath/path("charlie/1.jpg"),"bed");
    sqlconnector->addTagToPhoto(libraryPath/path("charlie/1.jpg"),
                                "architecture");

    sqlconnector->getPhotosTags(libraryPath/path("beta/1.jpg"),tags);
    BOOST_CHECK(tags.find("architecture") != tags.end());
    sqlconnector->getPhotosTags(libraryPath/path("charlie/1.jpg"),tags);
    BOOST_CHECK(tags.find("architecture") != tags.end() &&
                tags.find("bed") != tags.end());

    sqlconnector->getAllTags(tags);
    BOOST_CHECK(tags.find("architecture") != tags.end() &&
                tags.find("bed") != tags.end());

    //deleting tags
    sqlconnector->deleteTagFromPhoto(libraryPath/path("charlie/1.jpg"), "bed");
    sqlconnector->getAllTags(tags);
    BOOST_CHECK(tags.find("architecture") != tags.end() &&
                tags.find("bed") == tags.end());

    //getting photos with all specified tags
    vector<path> photos;
    string test_string;
    sqlconnector->getPhotosWithTags(tags,photos);

    output.clear();
    for(vector<path>::const_iterator i = photos.begin() ;
        i != photos.end() ; ++i) {
      output += i->string() + "\n";
    }

    BOOST_CHECK(output == libraryPath.string() + "beta/1.jpg\n"+
                          libraryPath.string() + "charlie/1.jpg\n");

  /*********************************************************************/
  /* CHECKING COMPATIBILITY AND CHECKING IF DISK STRUCTURE HAS CHANGED */
  /*********************************************************************/
    //checking compatibility
    BOOST_CHECK(sqlconnector->checkCompatibility() == true);
    rename(libraryPath/path("beta/1.jpg"), libraryPath/path("beta/1prim.jpg"));
    BOOST_CHECK(sqlconnector->checkCompatibility() == false);
    rename(libraryPath/path("beta/1prim.jpg"), libraryPath/path("beta/1.jpg"));
    
    //checking if a disk structure of directories from the database has changed
    
    BOOST_CHECK(sqlconnector->close() == DBConnector::CLOSED);

  }

BOOST_AUTO_TEST_SUITE_END()

