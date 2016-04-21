ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs)

all: PRBTiming

clean:
	rm *.o

PRBTiming: RootDictionary.o ComponentRelation.o Component.o Stub.o CIC.o Receiver.o PRBTiming.o
	c++ RootDictionary.o ComponentRelation.o Component.o Stub.o CIC.o Receiver.o PRBTiming.o -o PRBTiming $(ROOTFLAGS) $(ROOTLIBS)

RootDictionary.o: RootDictionary.cc
	c++ -c RootDictionary.cc -c $(ROOTFLAGS)
        
ComponentRelation.o: src/ComponentRelation.cc
	c++ -c src/ComponentRelation.cc -c $(ROOTFLAGS)

Component.o: src/Component.cc
	c++ -c src/Component.cc -c $(ROOTFLAGS)

Stub.o: src/Stub.cc
	c++ -c src/Stub.cc -c $(ROOTFLAGS)
 
CIC.o: src/CIC.cc src/Component.cc
	c++ -c src/CIC.cc -c $(ROOTFLAGS)
        
Receiver.o: src/Receiver.cc src/Component.cc
	c++ -c src/Receiver.cc -c $(ROOTFLAGS)

PRBTiming.o: PRBTiming.cc src/ReadConfigurationFile.cc
	c++ -c PRBTiming.cc -c $(ROOTFLAGS) 
