#include <vector>

#include "Stub.h"
#include "Component.h"

class BXSplitter: public Component
{
  public:
    BXSplitter(std::string name, double frequency);
    void fillInputData(std::vector<std::vector<Stub*> > data_PRBF_RX_);
    bool computeOutputTimes();
    void writeDataHistograms();
    void clearData();
    
    std::vector<std::vector<Stub*> > data_PRBF1_ByLink_; // [Link]
    std::vector<std::vector<Stub*> > data_PRBF1_ByBX_;   // [BX]
    
    std::vector<TH1F*> v_h_nStubs_;
    
};  
    
    
