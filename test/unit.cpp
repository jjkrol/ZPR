#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "../include/directory.hpp"
#include "../include/photo.hpp"
using namespace boost::filesystem;

BOOST_AUTO_TEST_SUITE( testSuite )

  BOOST_AUTO_TEST_CASE( photoClassTest ) {
    Photo* myphoto = new Photo("test/zdj.jpg");
    path p("test/zdj.jpg");
    BOOST_CHECK(myphoto->getPath() == p );
  }

  BOOST_AUTO_TEST_CASE( directoryClassTest ) {

  }

BOOST_AUTO_TEST_SUITE_END()

