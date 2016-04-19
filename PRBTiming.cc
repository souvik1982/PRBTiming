#include <iostream>

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"

#include "src/CommandLineArguments.cc"

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
  
  std::vector<ComponentRelation*> componentRelations;
  
  // Read the schematic file and store Component Characteristics
  // and the schematic arrangement between components
  std::ifstream file(schematicFilename.c_str());
  std::string s;
  getline(file, s);
  int index=-1;
  while (!file.eof())
  {
  
  
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
    
    
  
}
