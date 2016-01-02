#ifndef _TEST_HELPER_HPP_
#define _TEST_HELPER_HPP_

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

class TestHelper {
public:
  static void describe( std::string klass_name ) {
    s_klass_name = klass_name;
  }

  static void it( std::string description, std::function<std::string()> body ) {
    if( s_tests.count( s_klass_name ) <= 0 ) {
      s_tests[ s_klass_name ] = {};
    }

    auto test_group = std::pair< std::string, std::function<std::string()> >( description, body );
    s_tests[ s_klass_name ].push_back( test_group );
  }

  static void run() {
    std::cout << "Running test suite." << std::endl;

    for( auto& test_group : s_tests ) {
      std::cout << "Describe " << test_group.first << std::endl;

      for( auto& test_pair : test_group.second ) {
        auto description = test_pair.first;
        auto body = test_pair.second;

        std::cout << "  It " << description << ": ";
        std::string output = body();

        if( output.empty() ) {
          std::cout << "\033[1;32mpassed\033[0m." << std::endl;
        } else {
          std::cout << "\033[1;31mfailed\033[0m." << std::endl;
          std::cout << "\033[1;31m    " << output << "\033[0m" << std::endl;
        }
      }
    }
  }

private:
  static std::string s_klass_name;
  static std::map< std::string, std::vector< std::pair< std::string, std::function<std::string ()> > > > s_tests;
};

static std::string  __bool_to_string( bool some_bool ) {
  return some_bool ? "true" : "false";
}

#define done() { return std::string(); }
#define it(description, body) TestHelper::it(description, body);
#define expect_eq(expected, actual) do {                                \
    if( expected != actual ) {                                          \
      std::ostringstream output;                                        \
      output << "Expected \"" << expected << "\" but got \"" << actual << "\" "; \
      output << "in " __FILE__ << ":" << __LINE__ << ".";               \
      return output.str();                                              \
    }                                                                   \
  } while(false)

#define expect_true(a) expect_eq( "true", __bool_to_string(a) )
#define expect_false(a) expect_eq( "false", __bool_to_string(a) )
#define _class_name(klass) __##klass##Test__
#define describe(klass, tests)                                          \
  namespace __internal__ {                                              \
    class _class_name(klass) {                                               \
    public:                                                             \
    _class_name(klass)() { ::TestHelper::describe(#klass); tests }           \
    private:                                                            \
    static __internal__::_class_name(klass) _self_ref;                               \
    };                                                                  \
  }                                                                     \
                                                                        \
  __internal__::_class_name(klass) __internal__::_class_name(klass)::_self_ref = __internal__::_class_name(klass)(); \

#define RUN_TESTS() { ::TestHelper::run(); }

#endif // _TEST_HELPER_HPP_
