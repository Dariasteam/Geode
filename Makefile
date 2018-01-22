CC=clang++
CXXFLAGS=-g -std=c++11

doc:
	rm -rf docs/*
	doxygen backend
	mv html/* docs/

clean:
	rm -rf backend/*.o example
