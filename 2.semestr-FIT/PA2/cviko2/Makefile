CXX=g++
CXXFLAGS=-pedantic -Wall -c

LD=g++
LDFLAGS=
queue: build/main.o build/queue.o
	mkdir -p build
	$(LD) $(LDFLAGS) $^ -o $@

build/main.o: src/main.cpp src/queue.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@

build/queue.o: src/queue.cpp src/queue.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm queue
	rm -r build
.PHONY: clean

