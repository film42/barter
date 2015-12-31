all:
	g++ -g -std=c++11 -stdlib=libc++ -I src src/main.cpp -o barter
	./barter

test:
	g++ -g -std=c++11 -stdlib=libc++ -I src -I test test/main.cpp -o barter_tests
	./barter_tests

clean:
	rm barter
	rm -r barter.dSYM
	rm barter_tests
	rm -r barter_tests.dSYM

.PHONY: all test clean
