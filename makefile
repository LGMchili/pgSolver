IDIR =./source
CC=g++
CPPFLAGS=-std=c++11 -Wall
src=$(shell find ./source -name "*.cpp")
pgSolver: $(src)
	$(CC) $(CPPFLAGS) $(src) -o pgSolver

clean:
	rm pgSolver
