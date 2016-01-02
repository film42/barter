#include "order.hpp"

//
// Cross for each type
//
template<> template<> bool Order<Buy>::can_cross<Sell>(Order<Sell> order) {
  // print_cross_details(order);
  return order.get_price() <= get_price();
}

template<> template<> bool Order<Sell>::can_cross<Buy>(Order<Buy> order) {
  // print_cross_details(order);
  return order.get_price() >= get_price();
}
