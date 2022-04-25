all: a.out foo

a.out: main.cc
	g++ -O3 -std=c++2a main.cc

foo: hello.cc foo.cc
	g++ -O3 -std=c++2a -fmodules-ts hello.cc foo.cc -ofoo

clean:
	rm -f *.o a.out foo core
