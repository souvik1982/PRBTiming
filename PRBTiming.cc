#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"

#include "interface/ComponentRelation.h"
#include "interface/Stub.h"
#include "interface/CIC.h"
#include "interface/loader.h"

#include "src/CommandLineArguments.cc"
#include "src/ReadConfigurationLine.cc"

int main(int argc, char *argv[])
{
  std::string schematicFilename="Schematic.txt";
  std::string inputDir;
  std::string inputFile="stubs.root";
  
  // Get command line arguments
  std::map<std::string, std::string> cmdMap=commandLineArguments(argc, argv);
  if (cmdMap.find("-schematic")!=cmdMap.end()) schematicFilename=cmdMap["-schematic"];
  if (cmdMap.find("-inputDir")!=cmdMap.end())  inputDir=cmdMap["-inputDir"];
  if (cmdMap.find("-inputFile")!=cmdMap.end()) inputFile=cmdMap["-inputFile"];
  
  std::map<int, ComponentRelation*> map_componentRelations;
  
  // Read the schematic file and store Component Characteristics
  // and the schematic arrangement between components
  std::ifstream file(schematicFilename.c_str());
  std::string s;
  getline(file, s);
  int index=-1;
  while (!file.eof())
  {
    getline(file, s);
    removeSpaces(s);
    
    if (s=="") continue; // This is an empty line
    if (s.at(0)=='#') continue; // This is a comment
    
    if (s.find("Index")!=std::string::npos)
    {
      int index=atoi(argInLine(s, "Index").c_str());
      if (map_componentRelations.find(index)==map_componentRelations.end())
      {
        getline(file, s);
        std::string componentType=argInLine(s, "ComponentType");
        
        if (componentType=="CIC")
        {
          std::map<std::string, std::string> map_CIC=readConfigurationLine(&file, s);
          std::string name_s;  if (map_CIC.find("ComponentName")!=map_CIC.end()) name_s=map_CIC["ComponentName"]; else std::cout<<"ERROR: CIC ComponentName does not exist in configuration file"<<std::endl;
          int moduleID;        if (map_CIC.find("ModuleID")!=map_CIC.end()) moduleID=atoi(map_CIC["ModuleID"].c_str()); else std::cout<<"ERROR: CIC ModuleID does not exist in configuration file"<<std::endl;
          std::string segment; if (map_CIC.find("Segment")!=map_CIC.end()) segment=map_CIC["Segment"]; else std::cout<<"ERROR: CIC Segment does not exist in configuration file"<<std::endl;
          double frequency;    if (map_CIC.find("Frequency")!=map_CIC.end()) frequency=atof(map_CIC["Frequency"].c_str()); else std::cout<<"ERROR: CIC Frequency does not exist in configuration file"<<std::endl;
          CIC *cic=new CIC(name_s, moduleID, segment, frequency);
          ComponentRelation *compRelation=new ComponentRelation();
          while (s!="")
          {
            std::string outputConnections=argInLine(s, "OutputConnections");
            compRelation->extractComponentRelation(outputConnections);
            getline(file, s);
          }
          compRelation->comp_=cic;
          map_componentRelations[index]=compRelation;
        }
        
        
        
      }
      else
      {
        std::cout<<"WARNING: Index of this component "<<index<<" already exists."<<std::endl;
      }
    } // Found new Index
  } // Done reading Schematic
  
  /*
  // Read ROOT event files and iterate
  TFile *eventFile=new TFile((inputDir+"/"+inputFile).c_str());
  TTree *tree=(TTree*)eventFile->Get("ntupler/tree");
  
  std::vector<float> *stubs_modId=0;
  std::vector<float> *stubs_r=0;
  
  tree->SetBranchAddress("TTStubs_modId", &(stubs_modId));
  tree->SetBranchAddress("TTStubs_r", &(stubs_r));
  
  unsigned int nEvents=tree->GetEntries();
  for (unsigned int i_event=0; i_event<nEvents; i_event+=8)
  {
    for (unsigned int i_BX=0; i_BX<8; ++i_BX)
    {
      unsigned int j_event=i_event+i_BX;
      tree->GetEntry(j_event);
    }
  }
  */
    
  
}
