#include "test_helper.hpp"

// Include All Test Files
#include "order_test.hpp"
#include "book_test.hpp"
#include "exchange_test.hpp"

int main() {
  ADD_TEST(OrderTest);
  ADD_TEST(BookTest);
  ADD_TEST(ExchangeTest);
  RUN_TESTS();
}
