BARTER_SRC=src/order.cpp src/book.cpp
# HACK: test/test_helper.cpp must come before the test files.
TEST_SRC= test/main.cpp test/test_helper.cpp test/order_test.cpp test/book_test.cpp test/exchange_test.cpp

all:
	g++ -g -std=c++11 -stdlib=libc++ -I src src/server.cpp $(BARTER_SRC) -o barter
	./barter

playground:
	g++ -g -std=c++11 -stdlib=libc++ -I src src/main.cpp $(BARTER_SRC) -o barter
	./barter

test:
	g++ -g -std=c++11 -stdlib=libc++ -I src -I test $(BARTER_SRC) $(TEST_SRC) -o barter_tests
	./barter_tests

clean:
	rm barter
	rm -r barter.dSYM
	rm barter_tests
	rm -r barter_tests.dSYM

.PHONY: all test clean
