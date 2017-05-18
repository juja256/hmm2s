GXX:= g++
FLAGS:= -g

all: test

test: test.o hmm2s.o hmm.o blob.o scfg_gnf.o
	$(GXX) $(FLAGS) test.o hmm2s.o hmm.o blob.o scfg_gnf.o -o test

test.o: test.cpp
	$(GXX) $(FLAGS) -c test.cpp

hmm2s.o: hmm2s.cpp
	$(GXX) $(FLAGS) -c hmm2s.cpp

blob.o: blob.cpp
	$(GXX) $(FLAGS) -c blob.cpp

hmm.o: hmm.cpp
	$(GXX) $(FLAGS) -c hmm.cpp

scfg_gnf.o: scfg_gnf.cpp
	$(GXX) $(FLAGS) -c scfg_gnf.cpp

clear:
	rm -f *.o test
