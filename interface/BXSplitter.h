#include <vector>

#include "Stub.h"
#include "Component.h"

class BXSplitter: public Component
{
  public:
    BXSplitter(std::string name, double frequency);
    void fillInputData(std::vector<std::vector<Stub*> > data_PRBF_RX_);
    bool computeOutputTimes();
    void writeHistograms();
    
    std::vector<std::vector<Stub*> > data_PRBF1_; // [BX]
    
};  
    
    
