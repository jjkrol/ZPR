#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "../include/core.hpp"
#include "../include/photo.hpp"
#include "../include/directory.hpp"
#include "../include/dbConnector.hpp"
#include "../include/disk.hpp"

using namespace boost::filesystem;

BOOST_AUTO_TEST_SUITE( testSuite )

/*  BOOST_AUTO_TEST_CASE( coreClassTest ){
    CoreController* core = CoreController::getInstance("test.cfg");

    BOOST_CHECK(core->getLibraryDirectoryPath()=="./test/test_tree/");

  }

  BOOST_AUTO_TEST_CASE( photoClassTest ) {
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
  }*/

  BOOST_AUTO_TEST_CASE( SQLiteConnectorTest ) {
    using std::vector;
    using boost::filesystem::path;

    CoreController* core = CoreController::getInstance("test.cfg");
    Disk *disk = Disk::getInstance();
    //making an instance of a connector which should be tested
    DBConnector *sqlconnector = DBConnectorFactory::getInstance("sqlite");

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


  /******************************************/
  /*    ADDING AND DELETING PHOTOS          */
  /******************************************/
    vector<path> dirs = disk->getSubdirectoriesPaths(path("/"));
    BOOST_CHECK(sqlconnector->addPhotosFromDirectories(dirs) == true);
    sqlconnector->close();


  //adding photos from directories
  //addPhotosFromDirectories(di)
  
  }

BOOST_AUTO_TEST_SUITE_END()

