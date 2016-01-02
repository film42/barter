#include "test_helper.hpp"

// Init the tests vector
std::string TestHelper::s_klass_name = "";
std::map< std::string, std::vector< std::pair< std::string, std::function<std::string ()> > > > TestHelper::s_tests = {};
