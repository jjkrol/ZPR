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

  BOOST_AUTO_TEST_CASE( coreClassTest ){
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
  }

  BOOST_AUTO_TEST_CASE( SQLiteConnectorTest ) {
    using std::vector;
    using boost::filesystem::path;
    //making an instance of a connector which should be tested
    DBConnector *sqlconnector = DBConnectorFactory::getInstance("sqlite");

  /******************************************/
  /*    OPENING AND CLOSING DATABASE        */
  /******************************************/

    //closing database which is not opened
    BOOST_CHECK(sqlconnector->close() == DBConnector::FAILURE);

    //opening non-existing database (should be created)
    BOOST_REQUIRE(sqlconnector->open("DB.sqlite")
                  == (DBConnector::OPENED | DBConnector::CREATED));

    //closing opened database connection
    BOOST_REQUIRE(sqlconnector->close() == DBConnector::CLOSED);

    //opening existing database
    BOOST_REQUIRE(sqlconnector->open("DB.sqlite") == DBConnector::OPENED);

    //opening database which has been already opened (multi-connections
    //are not allowed, so the function should exit with a FAILURE flag)
    BOOST_CHECK(sqlconnector->open("DB.sqlite") == DBConnector::FAILURE);


  /******************************************/
  /*    ADDING AND DELETING PHOTOS          */
  /******************************************/
    Disk *disk = Disk::getInstance();
    vector<boost::filesystem::path> dirs = disk->getSubdirectoriesPaths(
                                           boost::filesystem::path("/"));

    //adding photos from directory non-recursively
    //dirs.push_back(DirectoriesPath ("/alpha/", false));


    //sqlconnector->addPhotosFromDirectories(dirs);
    //sqlconnector->showPhotos(std::cout);

    //deleting directories;
    //deleting photos;
    //checking if a structure of files on a disk has changed
    //checking if a database is empty (contains no directories)
    //adding tags to photo
    //adding removing tags from photo;
    //removing tag

    //closing connection to database finalizes the test suite
    BOOST_CHECK(sqlconnector->close() == DBConnector::CLOSED);
  }

BOOST_AUTO_TEST_SUITE_END()

