TEST?="scanner-input.txt"

build: main.cpp ../Scanner.cpp ../Scanner.h ../Parser.cpp ../Parser.h
	g++ -std=c++17 main.cpp ../Scanner.cpp ../Parser.cpp -o test

run: build
	./test < $(TEST)

clean:
	rm -f test