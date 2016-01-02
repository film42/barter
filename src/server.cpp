#include <iostream>
#include "exchange.hpp"
#include "server.hpp"


class PrintNotifications : public Exchange::Notifications {
public:
  virtual void on_filled( Order<Buy> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }

  virtual void on_filled( Order<Sell> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }
};

int main() {
  Exchange exchange("YOLO");
  exchange.subscribe< PrintNotifications >();

  // Run telnet server
  std::cout << "Starting telnet server on port 3000" << std::endl;
  Server server( exchange );
  server.run( 3000 );
}
