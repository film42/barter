#include "test_helper.hpp"

// Init the tests vector
std::mutex TestHelper::s_add_test_mutex;
std::string TestHelper::s_klass_name = "";
std::unordered_map< std::string, std::vector< std::pair< std::string, std::function<std::string ()> > > > TestHelper::s_tests = {};
