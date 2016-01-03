#include "test_helper.hpp"

describe(Order, {
  it("can create a buy order", []() {
    auto buy_order = Order<Buy>(50.00, 25);
    expect_eq(50.00, buy_order.get_price());
    expect_eq(25, buy_order.get_size());
    expect_eq(Buy::name(), buy_order.side());
    done();
  })

  it("can create a sell order", []() {
    auto sell_order = Order<Sell>(50.00, 25);
    expect_eq(50.00, sell_order.get_price());
    expect_eq(25, sell_order.get_size());
    expect_eq(Sell::name(), sell_order.side());
    done();
  })

  it("can cross crossable orders", []() {
    auto buy_order = Order<Buy>(50.00, 25);
    auto sell_order = Order<Sell>(40.00, 5);

    expect_true( buy_order.can_cross(sell_order) );
    expect_true( sell_order.can_cross(buy_order) );
    done();
  })

  it("cannot cross orders that don't match", []() {
    auto buy_order = Order<Buy>(50.00, 25);
    auto sell_order = Order<Sell>(70.00, 5);

    expect_false( buy_order.can_cross(sell_order) );
    expect_false( sell_order.can_cross(buy_order) );
    done();
  })

  it("can create a debug string", []() {
    auto order = Order<Buy>("123", 50.00, 25);
    auto debug = "(Side: BUY, Id: 123, Price: 50, Size: 25, Status: 0)";
    expect_eq( debug, order.to_string() );
    done();
  })
})
