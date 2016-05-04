#include <iostream>

#include "../interface/BXSplitter.h"

BXSplitter::BXSplitter(std::string name, double frequency)
{
  type_="BXSplitter";
  name_=name;
  frequency_=frequency;
  
  for (unsigned int i=0; i<8; ++i)
  {
    t1in_.push_back(-999); // by link
    t2in_.push_back(-999); // by link
    t1out_.push_back(-999); // by BX
    t2out_.push_back(-999); // by BX
    
    v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" Link "+itoa(i)+" t1in").c_str(), 1000, 0, 1000.));
    v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" Link "+itoa(i)+" t2in").c_str(), 1000, 0, 1000.));
    v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" BX "+itoa(i)+" t1out").c_str(), 1000, 0, 1000.));
    v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" BX "+itoa(i)+" t2out").c_str(), 1000, 0, 1000.));
  }
  
  data_PRBF1_.resize(8);
  
  std::cout<<"LOG: Initialized BXSplitter "<<name_<<" with operating frequency = "<<frequency_<<std::endl;
}

void BXSplitter::fillInputData(std::vector<std::vector<Stub*> > data_PRBF_RX)
{
  for (unsigned int i_link=0; i_link<8; ++i_link)
  {
    for (unsigned int i_Stub=0; i_Stub<data_PRBF_RX.at(i_link).size(); ++i_Stub)
    {
      Stub *stub=data_PRBF_RX.at(i_link).at(i_Stub);
      data_PRBF1_.at(stub->bx_).push_back(stub); // If this fails, check bx
    }
  }
}

bool BXSplitter::computeOutputTimes()
{
  if (frequency_>0)
  {
    double maxtin=-999;
    for (unsigned int i_link=0; i_link<8; ++i_link)
    {
      if (t1in_.at(i_link)>0 && t2in_.at(i_link)>0)
      {
        if (t1in_.at(i_link)+60./frequency_*1000. > maxtin) maxtin=t1in_.at(i_link)+60./frequency_*1000.;
        if (t2in_.at(i_link) > maxtin) maxtin=t2in_.at(i_link);
        
        v_h_t1in_.at(i_link)->Fill(t1in_.at(i_link));
        v_h_t2in_.at(i_link)->Fill(t2in_.at(i_link));
      }
      else
      {
        std::cout<<"ERROR: BXSplitter "<<name_<<" has no t1in and t2in set for link = "<<i_link<<std::endl;
        return false;
      }
    }
    for (unsigned int i_BX=0; i_BX<8; ++i_BX)
    {
      t1out_.at(i_BX)=maxtin;
      t2out_.at(i_BX)=maxtin+data_PRBF1_.at(i_BX).size()*frequency_/1000.;
      
      v_h_t1out_.at(i_BX)->Fill(t1out_.at(i_BX));
      v_h_t2out_.at(i_BX)->Fill(t2out_.at(i_BX));
    }
  }
  else
  {
    std::cout<<"ERROR: BXSplitter "<<name_<<" has no operating frequency set."<<std::endl;
    return false;
  }
  
  return true;
}

void BXSplitter::clearData()
{
  for (unsigned int i=0; i<data_PRBF1_.size(); ++i)
  {
    data_PRBF1_.at(i).clear();
  }
}
