#include <vector>

#include "Stub.h"
#include "Component.h"

class Receiver: public Component
{
  public:
    Receiver(std::string name, double frequency, double delayCLK);
    void fillInputData(int link, std::vector<Stub*> data_PRBF0);
    bool computeOutputTimes();
    void writeHistograms();
    
    std::vector<std::vector<Stub*> > data_PRBF_RX_; // This should be 8 lines with 60 entries each. Should have a checking function.
    double delayCLK_;
};    
      
