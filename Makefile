ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs)

all: Stub.o

clean:
	rm *.o

RootDictionary.o: RootDictionary.cc
	c++ -c RootDictionary.cc -c $(ROOTFLAGS)

Stub.o: src/Stub.cc
	c++ -c src/Stub.cc -c $(ROOTFLAGS)

#AMTiming.o: AMTiming.cc
#	c++ -c AMTiming.cc -c $(ROOTFLAGS) 