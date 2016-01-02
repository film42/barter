#ifndef _EXCHANGE_HPP_
#define _EXCHANGE_HPP_

#include <iostream>
#include <sstream>

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

  void clear() {
    m_buy_book.clear();
    m_sell_book.clear();
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
    }

    if( new_order.get_status() == FILLED ) {
      notify_filled( new_order );
    }

    // Check orders that we crossed against and notify.
    for( auto& processed_order : processed_orders ) {
      if( processed_order.get_status() == FILLED ) {
        notify_filled( processed_order );
      } else {
        // TODO: Other notification events.
      }
    }
  }

  void print() {
    std::cout << to_string() << std::endl;
  }

  std::string to_string() {
    std::ostringstream output;
    output << "Exchange for security: " << get_security() << std::endl;
    output << m_buy_book.to_string();
    output << m_sell_book.to_string();
    return output.str();
  }

  class Notifications {
  public:
    virtual void on_filled( Order<Buy> order ) = 0;
    virtual void on_filled( Order<Sell> order ) = 0;
  };

  template< typename T >
  void subscribe() {
    m_subscribers.push_back( std::unique_ptr<Notifications>(new T()));
  }

private:
  std::string m_security;
  std::vector< std::unique_ptr<Notifications> > m_subscribers;
  Book<Buy> m_buy_book;
  Book<Sell> m_sell_book;

  template< class S >
  void notify_filled(Order<S> order) {
    for( auto& subscriber : m_subscribers ) {
      subscriber->on_filled( order );
    }
  }

  template< class S >
  Book<S>& side_book( S side );

  Book<Buy>& side_book(Buy) { return m_buy_book; }
  Book<Sell>& side_book(Sell) { return m_sell_book; }

  Book<Buy>& opposite_side_book(Sell) { return m_buy_book; }
  Book<Sell>& opposite_side_book(Buy) { return m_sell_book; }
};

#endif // _EXCHANGE_HPP_
