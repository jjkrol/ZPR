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
    /* test directory scheme:
     * -alpha
     *  -1.jpg
     *  -2.jpg
     *  -3.jpg
     *  -4.jpg
     * -beta
     *  -1.jpg
     * -charlie
     *  -1.jpg
     * -1.jpg
     */
    path p("test/test_tree");
    Directory* myTestDir = new Directory(p);
    std::vector<Directory*> subdirs = myTestDir->getSubdirectories();
    Directory* alpha = subdirs[0];
    BOOST_REQUIRE(alpha->getName() == "alpha");
    Photo* currentPhoto = alpha->getCurrentPhoto();
    BOOST_CHECK(currentPhoto->getFilename() == "1.jpg");
    BOOST_CHECK(alpha->getNextPhoto()->getFilename() == "2.jpg");
    BOOST_CHECK(alpha->getNextPhoto()->getFilename() == "3.jpg");
    BOOST_CHECK(alpha->getNextPhoto()->getFilename() == "4.jpg");
    BOOST_CHECK(alpha->getNextPhoto()->getFilename() == "4.jpg");
    BOOST_CHECK(alpha->getPreviousPhoto()->getFilename() == "3.jpg");

    Directory* beta = subdirs[1];
    BOOST_REQUIRE(beta->getName() == "beta");
    currentPhoto = beta->getCurrentPhoto();
    BOOST_CHECK(currentPhoto->getFilename() == "1.jpg");
    BOOST_CHECK(beta->getNextPhoto()->getFilename() == "1.jpg");
    BOOST_CHECK(beta->getPreviousPhoto()->getFilename() == "1.jpg");

    BOOST_CHECK(myTestDir->getCachedSubdirectories().size() == 3);
    BOOST_CHECK(myTestDir->getPhotos().size() == 1);

  }

BOOST_AUTO_TEST_SUITE_END()

