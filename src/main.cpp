#include <iostream>
#include "exchange.hpp"

// template<> void cross<Sell>(Order<Sell>& order) {
//   auto top_order = top_of_book();
//   if( top_order.can_cross(order) ) {

//   }
// }

// template< typename S >
// class MarketOrder {
// public:
// };

class CustomNotifications : public Exchange::Notifications {
public:
  virtual void on_filled( Order<Buy> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }

  virtual void on_filled( Order<Sell> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }
};

int main() {
  std::cout << std::endl;

  auto buy_order = Order<Buy>(30.00, 10);
  auto sell_order = Order<Sell>(20.00, 5);
  auto sell_order_2 = Order<Sell>(50.00, 5);

  auto book = Book<Sell>();

  book.insert(sell_order);
  book.insert(sell_order_2);
  book.print();

  std::cout << std::endl;

  if( sell_order.can_cross( buy_order ) ) {
    std::cout << "We can cross!" << std::endl;
  } else {
    std::cout << "No crossing possible!" << std::endl;
  }

  if( buy_order.can_cross( sell_order ) ) {
    std::cout << "We can cross!" << std::endl;
  } else {
    std::cout << "No crossing possible!" << std::endl;
  }

  if( buy_order.can_cross( sell_order_2 ) ) {
    std::cout << "We can cross!" << std::endl;
  } else {
    std::cout << "No crossing possible!" << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Exchange Testing..." << std::endl;

  Exchange exchange("AAPL");
  exchange.subscribe<CustomNotifications>();

  exchange.print();

  std::cout << std::endl;
  std::cout << "Adding un-crossables..." << std::endl;

  exchange.insert( buy_order );
  exchange.insert( sell_order_2 );

  exchange.print();

  std::cout << std::endl;
  std::cout << "Adding crossables..." << std::endl;

  exchange.insert( sell_order );
  exchange.insert( sell_order );
  exchange.insert( Order<Sell>(80, 10) );
  exchange.insert( Order<Sell>(50, 10) );
  exchange.insert( Order<Sell>(50, 10) );
  exchange.insert( Order<Sell>(90, 40) );
  exchange.insert( Order<Buy>(100.00, 15) );

  exchange.print();
}
