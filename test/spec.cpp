#include "spec.hpp"

// Init the tests vector
std::mutex Spec::s_add_test_mutex;
std::string Spec::s_klass_name = "";
std::unordered_map< std::string, std::vector< std::pair< std::string, std::function<std::string ()> > > > Spec::s_tests = {};
