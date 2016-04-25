#include <iostream>

#include "../interface/Receiver.h"

Receiver::Receiver(std::string name, double frequency, double delayCLK)
{
  type_="Receiver";
  name_=name;
  frequency_=frequency;
  delayCLK_=delayCLK;
  
  for (unsigned int i=0; i<40; ++i)
  {
    t1in_.push_back(1./frequency_*1000.);
    t2in_.push_back(64./frequency_*1000.);
  }
  
  for (unsigned int i=0; i<8; ++i)
  {
    t1out_.push_back(-999);
    t2out_.push_back(-999);
  }
  
  data_PRBF_RX_.resize(8);
  
  std::cout<<"LOG: Initialized Receiver "<<name_<<" with operating frequency = "<<frequency_<<", delay = "<<delayCLK_<<" CLK"<<std::endl;
}

void Receiver::fillInputData(int link, std::vector<Stub*> data_PRBF0)
{
  for (unsigned int i=0; i<data_PRBF0.size(); ++i)
  {
    data_PRBF_RX_.at(link/5).push_back(data_PRBF0.at(i));
  }
}

bool Receiver::computeOutputTimes()
{
  for (unsigned int i=0; i<8; ++i)
  {
    t1out_.at(i)=(1.+delayCLK_+1.)/frequency_*1000.;
    t2out_.at(i)=(64.+delayCLK_+60.)/frequency_*1000.;
  }
  return true;
}

void Receiver::writeHistograms()
{
}
