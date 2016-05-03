ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs)

all: PRBTiming DisplayTiming

clean:
	rm *.o

PRBTiming: RootDictionary.o ComponentRelation.o Component.o Stub.o CIC.o Receiver.o BXSplitter.o LayerSplitter.o PRBTiming.o
	c++ RootDictionary.o ComponentRelation.o Component.o Stub.o CIC.o Receiver.o BXSplitter.o LayerSplitter.o PRBTiming.o -o PRBTiming $(ROOTFLAGS) $(ROOTLIBS)

DisplayTiming: ComponentRelation.o Component.o Stub.o CIC.o Receiver.o BXSplitter.o LayerSplitter.o DisplayTiming.o
	c++ ComponentRelation.o Component.o Stub.o CIC.o Receiver.o BXSplitter.o LayerSplitter.o DisplayTiming.o -o DisplayTiming $(ROOTFLAGS) $(ROOTLIBS)

RootDictionary.o: RootDictionary.cc
	c++ -c RootDictionary.cc -c $(ROOTFLAGS)

Component.o: interface/Component.h src/Component.cc
	c++ -c src/Component.cc -c $(ROOTFLAGS)
        
ComponentRelation.o: src/ComponentRelation.cc Component.o
	c++ -c src/ComponentRelation.cc -c $(ROOTFLAGS)

Stub.o: src/Stub.cc
	c++ -c src/Stub.cc -c $(ROOTFLAGS)
 
CIC.o: src/CIC.cc Component.o
	c++ -c src/CIC.cc -c $(ROOTFLAGS)
        
Receiver.o: src/Receiver.cc Component.o
	c++ -c src/Receiver.cc -c $(ROOTFLAGS)

BXSplitter.o: src/BXSplitter.cc Component.o
	c++ -c src/BXSplitter.cc -c $(ROOTFLAGS)
        
LayerSplitter.o: src/LayerSplitter.cc Component.o
	c++ -c src/LayerSplitter.cc -c $(ROOTFLAGS)

PRBTiming.o: PRBTiming.cc src/ReadConfigurationFile.cc
	c++ -c PRBTiming.cc -c $(ROOTFLAGS) 
        
DisplayTiming.o: AnalysisScripts/DisplayTiming.cc src/ReadConfigurationFile.cc
	c++ -c AnalysisScripts/DisplayTiming.cc -c $(ROOTFLAGS) 
