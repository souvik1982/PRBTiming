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
    
    v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" t1in").c_str(), 2000, 0, 2000.));
    v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" t2in").c_str(), 2000, 0, 2000.));
  
    v_h_nStubs_ByPRB_.push_back(new TH1F(("h_nStubs_ByPRB_"+name_+"_"+itoa(i_PRB)).c_str(), ("; LayerSplitter "+name_+" PRB "+itoa(i_PRB)+" nStubs").c_str(), 200, 0, 200.));
  }
  for (unsigned int i_layer=0; i_layer<6; ++i_layer)
  {
    t1out_.push_back(-999);
    t2out_.push_back(-999);
    
    v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" layer "+itoa(i_layer)+" t1out").c_str(), 2000, 0, 2000.));
    v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" layer "+itoa(i_layer)+" t2out").c_str(), 2000, 0, 2000.));
  
    v_h_nStubs_ByLayer_.push_back(new TH1F(("h_nStubs_ByLayer_"+name_+"_"+itoa(i_layer)).c_str(), ("; LayerSplitter "+name_+" Layer "+itoa(i_layer)+" nStubs").c_str(), 200, 0, 200.));
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
    // Minimum and maximum CLKs for each layer
    std::vector<double> v_minCLK_layer(6, -1), v_maxCLK_layer(6, -1);
    
    // This has to be added to the min t1in over all PRBs
    double mintin=1e10;
    for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
    {
      if (t1in_.at(i_PRB)>0)
      {
        if (t1in_.at(i_PRB)<mintin) mintin=t1in_.at(i_PRB);
      }
      else
        std::cout<<"ERROR: LayerSplitter "<<name_<<" has no t1in set for PRB = "<<i_PRB<<std::endl;
    }
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
          if (layer>=5 && layer<=10)
          {
            v_maxCLK_layer[layer-5]=clk;
            if (v_minCLK_layer[layer-5]==-1) v_minCLK_layer[layer-5]=clk;
          }
          else
          {
            std::cout<<"ERROR: In LayerSplitter "<<name_<<", received stubs from layer = "<<layer<<std::endl;
          }
        }
      }
      ++clk;
    }
    while (clk<maxPRBSize);
    
    for (unsigned int i_layer=0; i_layer<6; ++i_layer)
    {
      t1out_.at(i_layer)=mintin+(v_minCLK_layer[i_layer]+1)/frequency_*1000.;
      t2out_.at(i_layer)=mintin+(v_maxCLK_layer[i_layer]+1)/frequency_*1000.;
    }
    
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
  for (unsigned int i=0; i<data_PRBF2_ByLayer_.size(); ++i)
  {
    data_PRBF2_ByLayer_.at(i).clear();
  }
}     
