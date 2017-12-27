CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = src/abstract_serializable.o src/serializable.o src/object.o src/manager.o src/parser.o src/main.o
TESTS = src/abstract_serializable.o src/serializable.o src/object.o src/manager.o src/parser.o tests/test.o

test:
	(cd backend && qmake)
	make -C backend
	mv backend/example .

doc:
	doxygen

clean:
	rm -rf backend/*.o example
