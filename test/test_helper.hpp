#ifndef _TEST_HELPER_HPP_
#define _TEST_HELPER_HPP_

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

class TestHelper {
public:
  static void it( std::string description, std::function<std::string()> body ) {
    s_tests.push_back( std::pair< std::string, std::function<std::string()> >( description, body ) );
  }

  static void run() {
    std::cout << "Running test suite." << std::endl;

    for( auto& test_pair : s_tests ) {
      auto description = test_pair.first;
      auto body = test_pair.second;

      std::cout << "it " << description << ": ";
      std::string output = body();

      if( output.empty() ) {
        std::cout << "\033[1;32mpassed\033[0m." << std::endl;
      } else {
        std::cout << "\033[1;31mfailed\033[0m." << std::endl;
        std::cout << "\033[1;31m  Because " << output << "\033[0m" << std::endl;
      }
    }
  }

private:
  static std::vector< std::pair< std::string, std::function<std::string ()> > > s_tests;
};

namespace __internal__ {
  class Test {
  public:
    virtual void add_tests() = 0;
  };
}

// Init the tests vector
std::vector< std::pair< std::string, std::function<std::string ()> > > TestHelper::s_tests = {};

#define done() { return std::string(); }
#define it(description, body) TestHelper::it(description, body);
#define expect_eq(expected, actual) do {                                \
    if( expected != actual ) {                                          \
      std::ostringstream output;                                        \
      output << "\"" << expected << "\" is not equal to \"" << actual << "\"."; \
      return output.str();                                              \
    }                                                                   \
  } while(false)

#define expect_true(a) expect_eq(true, a)
#define expect_false(a) expect_eq(false, a)

#define describe(klass, tests)                      \
  namespace __internal__ {                          \
    class klass {                                   \
    public:                                         \
    virtual void add_tests() { tests }              \
    };                                              \
  }

#define ADD_TEST(klass) { __internal__::klass().add_tests(); }
#define RUN_TESTS() { ::TestHelper::run(); }

#endif // _TEST_HELPER_HPP_
