#include <iostream>

#include "../interface/Receiver.h"

Receiver::Receiver(std::string name, double frequency, double delayCLK)
{
  type_="Receiver";
  name_=name;
  frequency_=frequency;
  delayCLK_=delayCLK;
  
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
      
