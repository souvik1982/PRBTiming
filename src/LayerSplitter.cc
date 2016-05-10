#include <iostream>
#include "ToolBox.cc"

#include "../interface/LayerSplitter.h"

LayerSplitter::LayerSplitter(std::string name, double frequency)
{
  type_="LayerSplitter";
  name_=name;
  frequency_=frequency;
  
  for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
  {
    t1in_.push_back(-999);
    t2in_.push_back(-999);
    
    v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" t1in").c_str(), 2000, 0, 1000.));
    v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" t2in").c_str(), 2000, 0, 1000.));
  
    v_h_nStubs_ByPRB_.push_back(new TH1F(("h_nStubs_ByPRB_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" nStubs").c_str(), 1000, 0, 200.));
  }
  for (unsigned int i_layer=0; i_layer<6; ++i_layer)
  {
    t1out_.push_back(-999);
    t2out_.push_back(-999);
    
    v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" layer "+itoa(i_layer)+" t1out").c_str(), 2000, 0, 1000.));
    v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" layer "+itoa(i_layer)+" t2out").c_str(), 2000, 0, 1000.));
  
    v_h_nStubs_ByLayer_.push_back(new TH1F(("h_nStubs_ByLayer_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" Layer "+itoa(i_layer)+" nStubs").c_str(), 1000, 0, 200.));
  }
  
  data_PRBF2_ByPRB_.resize(8);
  data_PRBF2_ByLayer_.resize(6);
  
  std::cout<<"LOG: Initialized LayerSplitter "<<name_<<" with operating frequency = "<<frequency_<<std::endl;
}

void LayerSplitter::fillInputData(int bXoutputPin, int prbInputPin, std::vector<std::vector<Stub*> > data_PRBF1)
{
  for (unsigned int j=0; j<data_PRBF1.at(bXoutputPin).size(); ++j)
  {
    Stub *stub=data_PRBF1.at(bXoutputPin).at(j);
    data_PRBF2_ByPRB_.at(prbInputPin).push_back(stub);
    data_PRBF2_ByLayer_.at(stub->layer_-5).push_back(stub);
  }
}

bool LayerSplitter::computeOutputTimes()
{
  if (frequency_>0)
  {
    // Find the min CLK at which the first stub in a layer appears, iterating over all PRBs that feed this data
    double minCLK_layer0=-1, maxCLK_layer0=-1;
    double minCLK_layer1=-1, maxCLK_layer1=-1;
    double minCLK_layer2=-1, maxCLK_layer2=-1;
    double minCLK_layer3=-1, maxCLK_layer3=-1;
    double minCLK_layer4=-1, maxCLK_layer4=-1;
    double minCLK_layer5=-1, maxCLK_layer5=-1;
    unsigned int maxPRBSize=0;
    for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      if (data_PRBF2_ByPRB_.at(i_PRB).size()>maxPRBSize) maxPRBSize=data_PRBF2_ByPRB_.at(i_PRB).size();
    unsigned int clk=0;
    do
    {
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        if (clk<data_PRBF2_ByPRB_.at(i_PRB).size())
        {
          int layer=data_PRBF2_ByPRB_.at(i_PRB).at(clk)->layer_;
          if (layer==5)
          {
            maxCLK_layer0=clk;
            if (minCLK_layer0==-1) minCLK_layer0=clk;
          }
          else if (layer==6)
          {
            maxCLK_layer1=clk;
            if (minCLK_layer1==-1) minCLK_layer1=clk;
          }
          else if (layer==7)
          {
            maxCLK_layer2=clk;
            if (minCLK_layer2==-1) minCLK_layer2=clk;
          }
          else if (layer==8)
          {
            maxCLK_layer3=clk;
            if (minCLK_layer3==-1) minCLK_layer3=clk;
          }
          else if (layer==9)
          {
            maxCLK_layer4=clk;
            if (minCLK_layer4==-1) minCLK_layer4=clk;
          }
          else if (layer==10)
          {
            maxCLK_layer5=clk;
            if (minCLK_layer5==-1) minCLK_layer5=clk;
          }
        }
      }
      ++clk;
    }
    while (clk<maxPRBSize);
    
    // std::cout<<"LayerSplitter "<<name_<<" minCLK_layer0 = "<<minCLK_layer0<<", maxCLK_layer0 = "<<maxCLK_layer0<<std::endl;
          
    // This has to be added to the min t1in over all PRBs
    double mintin=1e10;
    for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
    {
      if (t1in_.at(i_PRB)>0)
      {
        if (t1in_.at(i_PRB)<mintin) mintin=t1in_.at(i_PRB);
      }
      else
      {
        std::cout<<"ERROR: LayerSplitter "<<name_<<" has no t1in set for PRB = "<<i_PRB<<std::endl;
      }
    }
    
    // Compute t1out for each layer
    t1out_.at(0)=mintin+(minCLK_layer0+1)*frequency_/1000.;
    t1out_.at(1)=mintin+(minCLK_layer1+1)*frequency_/1000.;
    t1out_.at(2)=mintin+(minCLK_layer2+1)*frequency_/1000.;
    t1out_.at(3)=mintin+(minCLK_layer3+1)*frequency_/1000.;
    t1out_.at(4)=mintin+(minCLK_layer4+1)*frequency_/1000.;
    t1out_.at(5)=mintin+(minCLK_layer5+1)*frequency_/1000.;
    
    // Compute t2out for each layer
    t2out_.at(0)=mintin+(maxCLK_layer0+1)*frequency_/1000.;
    t2out_.at(1)=mintin+(maxCLK_layer1+1)*frequency_/1000.;
    t2out_.at(2)=mintin+(maxCLK_layer2+1)*frequency_/1000.;
    t2out_.at(3)=mintin+(maxCLK_layer3+1)*frequency_/1000.;
    t2out_.at(4)=mintin+(maxCLK_layer4+1)*frequency_/1000.;
    t2out_.at(5)=mintin+(maxCLK_layer5+1)*frequency_/1000.;
    
    // Fill histograms
    for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
    {
      v_h_t1in_.at(i_PRB)->Fill(t1in_.at(i_PRB));
      v_h_t2in_.at(i_PRB)->Fill(t2in_.at(i_PRB));
      
      v_h_nStubs_ByPRB_.at(i_PRB)->Fill(data_PRBF2_ByPRB_.at(i_PRB).size());
    }
    for (unsigned int i_layer=0; i_layer<6; ++i_layer)
    {
      v_h_t1out_.at(i_layer)->Fill(t1out_.at(i_layer));
      v_h_t2out_.at(i_layer)->Fill(t2out_.at(i_layer));
      
      v_h_nStubs_ByLayer_.at(i_layer)->Fill(data_PRBF2_ByLayer_.at(i_layer).size());
    }
  }
  else
  {
    std::cout<<"ERROR: LayerSplitter "<<name_<<" has no operational frequency set."<<std::endl;
  }
  
  return true;
}

void LayerSplitter::writeDataHistograms()
{
  TFile *file=new TFile((name_+".root").c_str(), "update");
  for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
  {
    v_h_nStubs_ByPRB_.at(i_PRB)->Write();
  }
  for (unsigned int i_layer=0; i_layer<6; ++i_layer)
  {
    v_h_nStubs_ByLayer_.at(i_layer)->Write();
  }
  file->Close();
}

void LayerSplitter::clearData()
{
  for (unsigned int i=0; i<data_PRBF2_ByPRB_.size(); ++i)
  {
    data_PRBF2_ByPRB_.at(i).clear();
  }
}     
