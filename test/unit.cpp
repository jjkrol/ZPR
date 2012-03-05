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

  }

BOOST_AUTO_TEST_SUITE_END()

