// === Pattern Recognition Board Timing Model === 
// Author: Souvik Das 
// Date: Apr 26, 2016
// ============================================== 

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TH1F.h"
#include "TChain.h"

#include "interface/ComponentRelation.h"
#include "interface/Stub.h"
#include "interface/CIC.h"
#include "interface/Receiver.h"
#include "interface/BXSplitter.h"
#include "interface/LayerSplitter.h"
#include "interface/loader.h"
#include "src/CommandLineArguments.cc"
#include "src/ReadConfigurationFile.cc"

int main(int argc, char *argv[])
{
  std::string schematicDir;
  std::string schematicFile="Schematic.txt";
  std::string inputDir;
  std::string inputFile="tracks.root";
  
  // Get command line arguments
  std::map<std::string, std::string> cmdMap=commandLineArguments(argc, argv);
  if (cmdMap.find("-schematicDir")!=cmdMap.end()) schematicDir=cmdMap["-schematicDir"];
  if (cmdMap.find("-schematicFile")!=cmdMap.end()) schematicFile=cmdMap["-schematicFile"];
  if (cmdMap.find("-inputDir")!=cmdMap.end())  inputDir=cmdMap["-inputDir"];
  if (cmdMap.find("-inputFile")!=cmdMap.end()) inputFile=cmdMap["-inputFile"];
  
  // Read the schematic file recursively
  MapComponentRelations *map_componentRelations=new MapComponentRelations;
  MapModuleIDCIC *map_modId_CIC=new MapModuleIDCIC;
  readConfigurationFile(schematicDir, schematicFile, map_componentRelations, map_modId_CIC);
  
  // Read ROOT event files and iterate
  TChain *tree=new TChain("ntupler/tree");
  tree->Add((inputDir+"/"+inputFile).c_str());
  
  std::vector<float> *stubs_modId=0;
  std::vector<float> *stubs_r=0;
  std::vector<float> *stubs_coordy=0;
  std::vector<float> *stubs_simpT=0;
  
  tree->SetBranchAddress("TTStubs_modId", &(stubs_modId));
  tree->SetBranchAddress("TTStubs_r", &(stubs_r));
  tree->SetBranchAddress("TTStubs_coordy", &(stubs_coordy));
  tree->SetBranchAddress("TTStubs_simPt", &(stubs_simpT));
  
  unsigned int nEvents=tree->GetEntries();
  for (unsigned int i_event=0; i_event<nEvents; i_event+=8)
  // for (unsigned int i_event=0; i_event<200; i_event+=8)
  {
  
    // Bunch 8 events into a train
    for (unsigned int i_BX=0; i_BX<8; ++i_BX)
    {
      unsigned int j_event=i_event+i_BX;
      tree->GetEntry(j_event);
      for (unsigned int i_stub=0; i_stub<stubs_modId->size(); ++i_stub)
      {
        float stub_modId=stubs_modId->at(i_stub);
        float stub_coordy=stubs_coordy->at(i_stub);
        float stub_simpT=stubs_simpT->at(i_stub);
        if (fabs(stub_simpT)>3)
        {
          int layer=int(stub_modId/10000);
          std::string segment;
          if (5<=layer && layer<=7)
          {
            if (stub_coordy<16) segment="L";
            else segment="R";
          }
          else if (8<=layer && layer<=10)
          {
            if (stub_coordy==0) segment="L";
            else segment="R";
          }
          if (map_modId_CIC->find(itoa(float(stub_modId))+segment)!=map_modId_CIC->end())
          {
            CIC *cic=(*map_modId_CIC)[itoa(float(stub_modId))+segment];
            cic->fillInputData(i_BX, stub_modId, layer);
            // std::cout<<"Found a represented module "<<stub_modId<<", which is in layer "<<layer<<" and filled it with Stub BX = "<<i_BX<<std::endl;
          }
          else
          {
            // std::cout<<"WARNING: ModuleID = "<<stub_modId<<" appears in the stub data but does not exist in the Schematic."<<std::endl;
          }
        }
      }
    }
    
    // Propagate data and timing down the chain
    
    // std::cout<<"Start with CIC"<<std::endl;
    // First do the CICs
    // for (MapComponentRelations::reverse_iterator i_comp=map_componentRelations->rbegin(); i_comp!=map_componentRelations->rend(); ++i_comp)
    for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
    {
      ComponentRelation *componentRelation=i_comp->second;
      Component *component=componentRelation->comp_;
      
      if (component->get_type()=="CIC")
      {
        // Compute timing for CIC
        CIC *cic=(CIC*)component;
        cic->computeOutputTimes();
        
        // Distribute data, and t1in and t2in to Receiver
        for (unsigned int i_pin=0; i_pin<componentRelation->i_comp_.size(); ++i_pin)
        {
          int targetIndex=componentRelation->i_comp_.at(i_pin);
          if (map_componentRelations->find(targetIndex)!=map_componentRelations->end())
          {
            if ((*map_componentRelations)[targetIndex]->comp_->get_type()=="Receiver")
            {
              Receiver *receiver=(Receiver*)((*map_componentRelations)[targetIndex]->comp_);
              receiver->fillInputData(componentRelation->i_input_.at(i_pin), cic->data_PRBF0_);
              
              // Update the t1in and t2in
              receiver->set_t1in(componentRelation->i_input_.at(i_pin), cic->get_t1out(componentRelation->i_output_.at(i_pin)));
              receiver->set_t2in(componentRelation->i_input_.at(i_pin), cic->get_t2out(componentRelation->i_output_.at(i_pin)));
            }
            else
            {
              std::cout<<"ERROR: CIC "<<cic->get_name()<<" is not connected to a Receiver."<<std::endl;
            }
          }
          else
          {
            std::cout<<"ERROR: CIC "<<cic->get_name()<<" is connected to Component with index "<<targetIndex<<" that does not exist in Schematic."<<std::endl;
          }
        }
      }
    }
    // std::cout<<"End with CIC"<<std::endl;
    
    // Then do the Receivers
    for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
    {
      ComponentRelation *componentRelation=i_comp->second;
      Component *component=componentRelation->comp_;
      
      if (component->get_type()=="Receiver")
      {
        Receiver *receiver=(Receiver*)component;
        
        // Compute timing for receiver and propagate t1out and t2out into inputs for BXSplitter
        receiver->computeOutputTimes();
        // receiver->printOutputTimes();
        
        // Distribute data, and t1in and t2in to BXSplitter
        int targetIndex=componentRelation->i_comp_.at(0);
        if (map_componentRelations->find(targetIndex)!=map_componentRelations->end())
        {
          if ((*map_componentRelations)[targetIndex]->comp_->get_type()=="BXSplitter")
          {
            // Distribute data
            BXSplitter *bxSplitter=(BXSplitter*)((*map_componentRelations)[targetIndex]->comp_);
            bxSplitter->fillInputData(receiver->data_PRBF_RX_);
            
            // Update t1in and t2in
            for (unsigned int i_pin=0; i_pin<componentRelation->i_comp_.size(); ++i_pin)
            {
              bxSplitter->set_t1in(componentRelation->i_input_.at(i_pin), receiver->get_t1out(componentRelation->i_output_.at(i_pin)));
              bxSplitter->set_t2in(componentRelation->i_input_.at(i_pin), receiver->get_t2out(componentRelation->i_output_.at(i_pin)));
            }
          }
          else
          {
            std::cout<<"ERROR: Receiver "<<receiver->get_name()<<" is not connected to a BXSplitter."<<std::endl;
          }
        }
        else
        {
          std::cout<<"ERROR: Receiver "<<receiver->get_name()<<" is connected to Component with index "<<targetIndex<<" that does not exist in Schematic."<<std::endl;
        }
        
      }
    }
    // std::cout<<"End with Receivers"<<std::endl;
    
    // Then do the BXSplitters
    for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
    {
      ComponentRelation *componentRelation=i_comp->second;
      Component *component=componentRelation->comp_;
      
      if (component->get_type()=="BXSplitter")
      {
        BXSplitter *bxSplitter=(BXSplitter*)component;
        
        // Compute timing for bxSplitter
        bxSplitter->computeOutputTimes();
        
        // Distribute data, and t1in and t2in to LayerSplitter
        for (unsigned int i_pin=0; i_pin<componentRelation->i_comp_.size(); ++i_pin)
        {
          int targetIndex=componentRelation->i_comp_.at(i_pin);
          if (map_componentRelations->find(targetIndex)!=map_componentRelations->end())
          {
            if ((*map_componentRelations)[targetIndex]->comp_->get_type()=="LayerSplitter")
            {
              // Distribute data
              LayerSplitter *layerSplitter=(LayerSplitter*)((*map_componentRelations)[targetIndex]->comp_);
              layerSplitter->fillInputData(componentRelation->i_output_.at(i_pin), componentRelation->i_input_.at(i_pin), bxSplitter->data_PRBF1_);
              
              // Update the t1in and t2in 
              layerSplitter->set_t1in(componentRelation->i_input_.at(i_pin), bxSplitter->get_t1out(componentRelation->i_output_.at(i_pin)));
              layerSplitter->set_t2in(componentRelation->i_input_.at(i_pin), bxSplitter->get_t2out(componentRelation->i_output_.at(i_pin)));
            }
            else
            {
              std::cout<<"ERROR: BXSplitter "<<bxSplitter->get_name()<<" is not connected to a LayerSplitter."<<std::endl;
            }
          }
          else
          {
            std::cout<<"ERROR: BXSplitter "<<bxSplitter->get_name()<<" is connected to Component with index "<<targetIndex<<" that does not exist in Schematic."<<std::endl;
          }
        }
        
      }
    }
    
    // Then do the LayerSplitters
    for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
    {
      ComponentRelation *componentRelation=i_comp->second;
      Component *component=componentRelation->comp_;
      
      if (component->get_type()=="LayerSplitter")
      {
        LayerSplitter *layerSplitter=(LayerSplitter*)component;
        
        // Compute timing for layerSplitter
        layerSplitter->computeOutputTimes();
      }
    }
    
    // Clear data in all components
    for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
    {
      i_comp->second->comp_->clearData();
    }
      
    if (i_event%1000==0) std::cout<<"Events "<<i_event<<" out of "<<nEvents<<" have been processed."<<std::endl;
    
  } // Event loop
  
  
  // Write component histograms to file
  for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
  {
    i_comp->second->comp_->writeHistograms();
    i_comp->second->comp_->writeDataHistograms();
  }
    
  
}
