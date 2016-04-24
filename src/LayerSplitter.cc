#include <iostream>

#include "../interface/LayerSplitter.h"

LayerSplitter::LayerSplitter(std::string name, double frequency)
{
  type_="LayerSplitter";
  name_=name;
  frequency_=frequency;
  
  data_PRBF2_.resize(6);
  
  std::cout<<"LOG: Initialized LayerSplitter "<<name_<<" with operating frequency = "<<frequency_<<std::endl;
}

void LayerSplitter::fillInputData(std::vector<std::vector<Stub*> > data_PRBF1)
{
  for (unsigned int i=0; i<data_PRBF1.size(); ++i)
  {
    for (unsigned int j=0; j<data_PRBF1.at(i).size(); ++j)
    {
      Stub *stub=data_PRBF1.at(i).at(j);
      data_PRBF2_.at(stub->layer_-5).push_back(stub);
    }
  }
}

bool LayerSplitter::computeOutputTimes()
{
  return true;
}

void LayerSplitter::writeHistograms()
{
}
      
