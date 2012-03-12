#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "../include/directory.hpp"
#include "../include/photo.hpp"
using namespace boost::filesystem;

BOOST_AUTO_TEST_SUITE( testSuite )

  BOOST_AUTO_TEST_CASE( photoClassTest ) {
    path p("test/zdj.jpg");
    Photo* myphoto = Photo::initialize(p);
    BOOST_CHECK(myphoto->getPath() == p );
    //double initialization
    BOOST_CHECK(Photo::initialize(p) == Photo::initialize(p));
  }

  BOOST_AUTO_TEST_CASE( directoryClassTest ) {
    path p("test/test_tree");
    Directory* myTestDir = new Directory(p);
    std::vector<Directory*> subdirs = myTestDir->getSubdirectories();
    Directory* alpha = subdirs[0];
    BOOST_REQUIRE(alpha->getName() == "alpha");

    Directory* beta = subdirs[1];
    BOOST_REQUIRE(beta->getName() == "beta");
    BOOST_CHECK(beta->hasPhotos() == true);
    BOOST_CHECK(beta->hasSubdirectories() == false);

    Directory* delta = subdirs[3];
    BOOST_REQUIRE(delta->getName() == "delta");
    BOOST_CHECK(delta->hasPhotos() == false);
    BOOST_CHECK(beta->hasSubdirectories() == false);

    BOOST_CHECK(myTestDir->hasSubdirectories() == true);
    BOOST_CHECK(myTestDir->getSubdirectories().size() == 4);
    BOOST_CHECK(myTestDir->getPhotos().size() == 1);

  }

BOOST_AUTO_TEST_SUITE_END()

