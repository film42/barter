#include "test_helper.hpp"

describe(Book, {
  it("can get the top of the book for Buy orders", {
    auto buy_book = Book<Buy>();
    buy_book.insert( Order<Buy>(10.00, 10) );
    buy_book.insert( Order<Buy>(20.00, 10) );
    expect_eq( 20.00, buy_book.top_of_book().get_price() );
    done();
  })

  it("can get the top of the book for Sell orders", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );
    sell_book.insert( Order<Sell>(20.00, 10) );
    expect_eq( 10.00, sell_book.top_of_book().get_price() );
    done();
  })

  it("can cross an order of equal size", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );

    auto response = sell_book.cross( Order<Buy>(20.00, 10) );
    auto sell_orders = response.first;
    auto sell_order = sell_orders.front();
    auto this_order = response.second;

    expect_eq( 0, this_order.get_size() );
    expect_eq( 0, sell_order.get_size() );
    expect_eq( 10, this_order.get_original_size() );
    expect_eq( 10, sell_order.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );
    expect_eq( FILLED, sell_order.get_status() );

    expect_true( sell_book.is_empty() );
    done();
  })

  it("can cross an order of greater size", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );

    auto response = sell_book.cross( Order<Buy>(20.00, 20) );
    auto sell_orders = response.first;
    auto sell_order = sell_orders.front();
    auto this_order = response.second;

    expect_eq( 10, this_order.get_size() );
    expect_eq( 0, sell_order.get_size() );
    expect_eq( 20, this_order.get_original_size() );
    expect_eq( 10, sell_order.get_original_size() );
    expect_eq( PARTIAL, this_order.get_status() );
    expect_eq( FILLED, sell_order.get_status() );

    expect_true( sell_book.is_empty() );
    done();
  })

  it("can cross an order of lesser size", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );

    auto response = sell_book.cross( Order<Buy>(20.00, 5) );
    auto sell_orders = response.first;
    auto sell_order = sell_orders.front();
    auto this_order = response.second;

    expect_eq( 0, this_order.get_size() );
    expect_eq( 5, this_order.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );

    expect_eq( 10, sell_order.get_original_size() );
    expect_eq( 5, sell_order.get_size() );
    expect_eq( PARTIAL, sell_order.get_status() );

    expect_false( sell_book.is_empty() );
    done();
  })

  it("can cross using multiple top of book orders", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );
    sell_book.insert( Order<Sell>(10.00, 10) );

    auto response = sell_book.cross( Order<Buy>(20.00, 20) );
    auto sell_orders = response.first;
    auto sell_order1 = sell_orders[0];
    auto sell_order2 = sell_orders[1];
    auto this_order = response.second;

    expect_true( sell_book.is_empty() );

    expect_eq( 0, this_order.get_size() );
    expect_eq( 20, this_order.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );

    // Fills two sell orders
    expect_eq( 0, sell_order1.get_size() );
    expect_eq( 10, sell_order1.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );

    expect_eq( 0, sell_order2.get_size() );
    expect_eq( 10, sell_order2.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );
    done();
  })

  it("can cross using multiple top of book orders and end with a partially filled order", {
    auto sell_book = Book<Sell>();
    sell_book.insert( Order<Sell>(10.00, 10) );
    sell_book.insert( Order<Sell>(10.00, 10) );

    auto response = sell_book.cross( Order<Buy>(20.00, 15) );
    auto sell_orders = response.first;
    auto sell_order1 = sell_orders[0];
    auto sell_order2 = sell_orders[1];
    auto this_order = response.second;

    expect_false( sell_book.is_empty() );

    expect_eq( 0, this_order.get_size() );
    expect_eq( 15, this_order.get_original_size() );
    expect_eq( FILLED, this_order.get_status() );

    // Fills two sell orders
    expect_eq( 0, sell_order1.get_size() );
    expect_eq( 10, sell_order1.get_original_size() );
    expect_eq( FILLED, sell_order1.get_status() );

    expect_eq( 5, sell_order2.get_size() );
    expect_eq( 10, sell_order2.get_original_size() );
    expect_eq( PARTIAL, sell_order2.get_status() );
    done();
  })
})
