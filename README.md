Barter
======

A simple matching engine written in C++. Largely written passively while binge watching various youtube channels
over the holiday break. Currently all `Order`s are limit orders.

#### Example

```c++
auto exchange = Exchange("TSLA");

exchange.insert( Order<Buy>(100.00, 30) );
exchange.insert( Order<Sell>(30.00, 10) );

exchange.print()
// Exchange for security: TSLA
// Printing BUY Book:
//        Price: 100
//                (Side: BUY, Id: c8uvyy98, Price: 100, Size: 20, Status: 1)
// Printing SELL Book:
```

The status of 1 means the order was partially filled. We can also wire up notifications to get events.

#### Notifications

```c++
class CustomNotifications : public Exchange::Notifications {
public:
  virtual void on_filled( Order<Buy> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }

  virtual void on_filled( Order<Sell> order) {
    std::cout << "ORDER FILLED: " << order.to_string() << std::endl;
  }
};

exchange.subscribe< CustomNotifications >();
```

#### Testings and Running

To run the sandbox example file, run `make` and it will build and run the `barter` executable.

If you want to run the test suite (which was made from scratch) you can run `make test`.

#### License

MIT
