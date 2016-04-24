#include <iostream>

#include "../interface/BXSplitter.h"

BXSplitter::BXSplitter(std::string name, double frequency)
{
  type_="BXSplitter";
  name_=name;
  frequency_=frequency;
  
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
      int bx=stub->bx_;
      data_PRBF1_.at(0).push_back(stub); // If this fails, check bx
    }
  }
}

bool BXSplitter::computeOutputTimes()
{
  return true;
}

void BXSplitter::writeHistograms()
{
}
