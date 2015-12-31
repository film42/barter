#ifndef _ORDER_HPP_
#define _ORDER_HPP_

#include <iostream>
#include <sstream>

//
// Enums
//
enum OrderStatus {
  PENDING,
  PARTIAL,
  FILLED,
  CANCELED
};

//
// Side types
//
class Side {};
class Buy : public Side {
public:
  static std::string name() { return "BUY"; }
};
class Sell : public Side {
public:
  static std::string name() { return "SELL"; }
};

//
// Order Base
//
template< class S >
class Order {
public:
  Order(float price, float size, OrderStatus status = PENDING) :
    m_id( generate_id() ), m_price(price), m_size(size), m_status(status), m_original_size(size) {}
  Order(float price, float size, OrderStatus status, float original_size) :
    m_id( generate_id() ), m_price(price), m_size(size), m_status(status), m_original_size(original_size) {}
  Order(std::string id, float price, float size, OrderStatus status = PENDING) :
    m_id(id), m_price(price), m_size(size), m_status(status), m_original_size(size) {}
  Order(std::string id, float price, float size, OrderStatus status, float original_size) :
    m_id(id), m_price(price), m_size(size), m_status(status), m_original_size(original_size) {}
  Order( const Order<S>& order_to_copy ):
    m_id(order_to_copy.get_id()), m_price(order_to_copy.get_price()), m_size(order_to_copy.get_size()),
    m_status(order_to_copy.get_status()), m_original_size(order_to_copy.get_original_size()) {}

  Order<S> copy() {
    return Order<S>( *this );
  }

  std::string get_id() const { return m_id; }
  float get_price() const { return m_price; }
  float get_size() const { return m_size; }
  float get_original_size() const { return m_original_size; }
  OrderStatus get_status() const { return m_status; }

  Order<S> cross();

  void set_id(std::string id) { m_id = id; }
  void set_status(OrderStatus status) { m_status = status; }
  void set_size(float size) { m_size = size; }

  template < class O >
  bool can_cross(Order<O> order);

  std::string side() { return S::name(); }

  std::string to_string() {
    std::ostringstream output;
    output << "(Side: " << side();
    output << ", Id: " << get_id();
    output << ", Price: " << get_price();
    output << ", Size: " << get_size();
    output << ", Status: " << get_status() << ")";
    return output.str();
  }

private:

  // HACK: Create a lame guid-like thing. Don't do this.
  std::string generate_id() {
    // Mostly copied from: http://stackoverflow.com/a/12468109/1457934
    auto random_alpha_numeric_char = []() -> char {
        const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };

    // Create a 8-char id. Kind of like a guid.
    std::string new_id(8, 0);
    std::generate_n( new_id.begin(), 8, random_alpha_numeric_char );
    return new_id;
  }

  template< class O >
  void print_cross_details(Order<O> order) {
    std::cout << to_string() << " ...crossing... " << order.to_string() << std::endl;
  }

  std::string m_id;
  OrderStatus m_status;
  float m_price;
  float m_size;
  float m_original_size;
};

//
// Cross for each type
//
template<> template<> bool Order<Buy>::can_cross<Sell>(Order<Sell> order) {
  print_cross_details(order);
  return order.get_price() <= get_price();
}

template<> template<> bool Order<Sell>::can_cross<Buy>(Order<Buy> order) {
  print_cross_details(order);
  return order.get_price() >= get_price();
}

#endif // _ORDER_HPP_
