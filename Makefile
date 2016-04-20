ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs)

all: RootDictionary.o ComponentRelation.o Component.o Stub.o CIC.o PRBTiming.o

clean:
	rm *.o

RootDictionary.o: RootDictionary.cc
	c++ -c RootDictionary.cc -c $(ROOTFLAGS)
        
ComponentRelation.o: src/ComponentRelation.cc
	c++ -c src/ComponentRelation.cc -c $(ROOTFLAGS)

Component.o: src/Component.cc
	c++ -c src/Component.cc -c $(ROOTFLAGS)

Stub.o: src/Stub.cc
	c++ -c src/Stub.cc -c $(ROOTFLAGS)
 
CIC.o: src/CIC.cc
	c++ -c src/CIC.cc -c $(ROOTFLAGS)

PRBTiming.o: PRBTiming.cc
	c++ -c PRBTiming.cc -c $(ROOTFLAGS) 
