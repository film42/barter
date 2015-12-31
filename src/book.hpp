#ifndef _BOOK_HPP_
#define _BOOK_HPP_

#include <iostream>
#include <map>
#include <deque>
#include <vector>

#include "order.hpp"

//
// Book for storing Orders (Order Book)
//
template < class S >
class Book {
public:
  Book() {}

  Order<S> top_of_book();

  bool is_empty() {
    return m_orders.empty();
  }

  // Cross the Side "S" (current side) with the opposite side "O".
  template< class O >
  std::pair< std::vector<Order<S>>, Order<O> > cross(Order<O> order) {
    if( is_empty() ) {
      return std::pair< std::vector<Order<S>>, Order<O> >({}, order);
    }

    auto top_order = top_of_book();

    if( top_order.can_cross( order ) ) {
      auto new_top_order = top_order.copy();
      auto opposite_order = order.copy();
      std::vector<Order<S>> processed_orders;

      // When they match perfectly
      if( top_order.get_size() == order.get_size() ) {
        new_top_order.set_status(FILLED);
        new_top_order.set_size(0);
        processed_orders.push_back(new_top_order);

        m_orders[ top_order.get_price() ].pop_front();

        // Now update the opposite order
        opposite_order.set_status(FILLED);
        opposite_order.set_size(0);
      }

      // When Book has a larger size than Order
      if( top_order.get_size() > order.get_size() ) {
        new_top_order.set_status(PARTIAL);
        new_top_order.set_size( top_order.get_size() - order.get_size() );

        m_orders[ top_order.get_price() ].pop_front();
        m_orders[ top_order.get_price() ].push_front( new_top_order );

        processed_orders.push_back( new_top_order );

        // Now update the opposite order
        opposite_order.set_status(FILLED);
        opposite_order.set_size(0);
      }

      // When Order has a larger size than Book
      if( top_order.get_size() < order.get_size() ) {
        auto new_top_order = top_order.copy();
        new_top_order.set_status(FILLED);
        new_top_order.set_size(0);
        processed_orders.push_back( new_top_order );

        m_orders[ top_order.get_price() ].pop_front();

        // Now update the opposite order
        opposite_order.set_status(PARTIAL);
        opposite_order.set_size(order.get_size() - top_order.get_size());
      }

      if( m_orders[ top_order.get_price() ].empty() ) {
        m_orders.erase( top_order.get_price() );
      }

      return std::pair< std::vector<Order<S>>, Order<O> >(processed_orders, opposite_order);
    }


    // Else: Return no new crossed orders and leave the order as it was.
    return std::pair< std::vector<Order<S>>, Order<O> >({}, order);
  }

  void insert(Order<S> order) {
    if( m_orders.count( order.get_price() ) <= 0 ) {
      m_orders[ order.get_price() ] = std::deque< Order<S> >();
    }

    // Push back the order at its price point
    m_orders[ order.get_price() ].push_back( order );
  }

  // Print the book at each price.
  void print() {
    std::cout << "Printing " << S::name() << " Book:" << std::endl;

    for( auto& entry : m_orders ) {
      std::cout << "\tPrice: " << entry.first << std::endl;;

      for( auto& order : entry.second ) {
        std::cout << "\t\t" << order.to_string() << std::endl;
      }
    }
  }

private:
  std::map<float, std::deque< Order<S> >> m_orders;
};

// This is important. The top of the BUY book should be the highest price. Meaining, the highest bidder
// should match the lowest seller. The spread is what's inbetween.
template<> Order<Buy> Book<Buy>::top_of_book() { return m_orders.rbegin()->second.front(); };
template<> Order<Sell> Book<Sell>::top_of_book() { return m_orders.begin()->second.front(); };

#endif // _BOOK_HPP_
