#include <vector>

#include "Stub.h"
#include "../interface/Component.h"

class Receiver: public Component
{
  public:
    Receiver(std::string name, double frequency, double delayCLK);
    void fillInputData(int link, std::vector<Stub*> data_PRBF0);
    
    std::vector<std::vector<Stub*> > data_PRBF_RX_;
    double delayCLK_;
};    
      
