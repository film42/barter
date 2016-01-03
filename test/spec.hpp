#ifndef _SPEC_HPP_
#define _SPEC_HPP_

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <unordered_map>
#include <vector>

class Spec {
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

  static void lock() { s_add_test_mutex.lock(); }
  static void unlock() { s_add_test_mutex.unlock(); }

  static void run() {
    // Set seed to current time
    std::srand( std::time(0) );

    std::cout << "Running test suite with seed: " << std::time(0) << std::endl;

    for( auto& test_group : s_tests ) {
      std::cout << "Describe " << test_group.first << std::endl;
      auto test_group_tests = test_group.second;

      // Shuffle "it" level test groups
      std::random_device device;
      std::mt19937 generator( device() );
      std::shuffle( test_group_tests.begin(), test_group_tests.end(), generator );

      for( auto& test_pair :  test_group_tests ) {
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
  static std::mutex s_add_test_mutex;
  static std::string s_klass_name;
  static std::unordered_map< std::string, std::vector< std::pair< std::string, std::function<std::string ()> > > > s_tests;
};

inline std::string  __bool_to_string( bool some_bool ) {
  return some_bool ? "true" : "false";
}

#define done() { return std::string(); }
#define it(description, body) Spec::it(description, [](){ body });
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
#define _class_name(klass) __spec__##klass##Test__
#define describe(klass, tests)                                          \
  class _class_name(klass) {                                            \
  public:                                                               \
    _class_name(klass)() {                                              \
      ::Spec::lock();                                                   \
      ::Spec::describe(#klass);                                         \
      tests;                                                            \
      ::Spec::unlock();                                                 \
    }                                                                   \
  private:                                                              \
    static _class_name(klass) _self_ref;                                \
  };                                                                    \
                                                                        \
  _class_name(klass) _class_name(klass)::_self_ref = _class_name(klass)(); \

#define RUN_TESTS() { ::Spec::run(); }

#endif // _SPEC_HPP_
