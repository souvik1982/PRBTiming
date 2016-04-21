#include <iostream>

#include "../interface/CIC.h"

CIC::CIC(std::string name, int moduleID, std::string segment, double frequency)
{
  type_="CIC";
  name_=name;
  moduleID_=moduleID;
  segment_=segment;
  frequency_=frequency;
  
  std::cout<<"LOG: Initialized CIC "<<name_<<" with moduleID = "<<moduleID_<<", segment = "<<segment_<<", operating frequency = "<<frequency_<<std::endl;
}

void CIC::fillInputData(int bx, int modID, int layer)
{
  Stub* stub=new Stub(bx, modID, layer);
  data_PRBF0_.push_back(stub);
}

bool CIC::computeOutputTimes()
{
  return true;
}

void CIC::writeHistograms()
{
}
