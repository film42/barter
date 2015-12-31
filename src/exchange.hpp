#ifndef _EXCHANGE_HPP_
#define _EXCHANGE_HPP_

#include <iostream>
#include "book.hpp"
#include "order.hpp"

//
// Exchange
//
class Exchange {
public:
  Exchange( std::string security ) : m_security(security) {}

  std::string get_security() const { return m_security; }
  std::string set_security( std::string security ) { return m_security = security; }

  Book<Buy>& get_buy_book() {
    return m_buy_book;
  }

  Book<Sell>& get_sell_book() {
    return m_sell_book;
  }

  template< class S >
  void insert(Order<S> order) {
    S side;
    auto orders = opposite_side_book(side).cross( order );
    auto processed_orders = orders.first;
    auto new_order = orders.second;

    // Insert the new order if it's not FILLED
    if( new_order.get_status() != FILLED ) {
      side_book(side).insert( new_order );
    } else if( new_order.get_status() == FILLED ) {
      notify_filled( new_order );
    }

    // Check orders that we crossed against and notify.
    for( auto& processed_order : processed_orders ) {
      if( processed_order.get_status() == FILLED ) {
        notify_filled( processed_order );
      } else {
        // What do we do here?
      }
    }
  }

  void print() {
    std::cout << "Exchange for security: "  << get_security() << std::endl;
    m_buy_book.print();
    m_sell_book.print();
  }

private:
  std::string m_security;
  Book<Buy> m_buy_book;
  Book<Sell> m_sell_book;

  template< class S >
  void notify_filled(Order<S> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }

  template< class S >
  Book<S>& side_book( S side );

  Book<Buy>& side_book(Buy) { return m_buy_book; }
  Book<Sell>& side_book(Sell) { return m_sell_book; }

  Book<Buy>& opposite_side_book(Sell) { return m_buy_book; }
  Book<Sell>& opposite_side_book(Buy) { return m_sell_book; }
};

#endif // _EXCHANGE_HPP_
