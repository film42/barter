#include <atomic>

#include "test_helper.hpp"
#include "book.hpp"
#include "exchange.hpp"
#include "order.hpp"

describe(Exchange, {
  it("can give the exchange a security name", []() {
    auto exchange = Exchange("TSLA");
    expect_eq( "TSLA", exchange.get_security() );
    done();
  })

  it("can cross orders of the same size", []() {
    auto exchange = Exchange("TSLA");
    exchange.insert( Order<Buy>(10.00, 40) );
    exchange.insert( Order<Sell>(10.00, 40) );
    expect_true( exchange.get_buy_book().is_empty() );
    expect_true( exchange.get_sell_book().is_empty() );
    done();
  })

  it("can cross an order of greater size", []() {
    auto exchange = Exchange("TSLA");
    exchange.insert( Order<Buy>(10.00, 40) );
    exchange.insert( Order<Sell>(10.00, 80) );
    expect_true( exchange.get_buy_book().is_empty() );
    expect_false( exchange.get_sell_book().is_empty() );
    expect_eq( PARTIAL, exchange.get_sell_book().top_of_book().get_status() );
    expect_eq( 40, exchange.get_sell_book().top_of_book().get_size() );
    done();
  })

  it("can cross an order of greater size", []() {
    auto exchange = Exchange("TSLA");
    exchange.insert( Order<Buy>(10.00, 80) );
    exchange.insert( Order<Sell>(10.00, 40) );
    expect_false( exchange.get_buy_book().is_empty() );
    expect_true( exchange.get_sell_book().is_empty() );
    expect_eq( PARTIAL, exchange.get_buy_book().top_of_book().get_status() );
    expect_eq( 40, exchange.get_buy_book().top_of_book().get_size() );
    done();
  })
})
