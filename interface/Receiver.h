#include <vector>

#include "Stub.h"
#include "Component.h"

class Receiver: public Component
{
  public:
    Receiver(std::string name, double frequency, double delayCLK);
    void fillInputData(int link, std::vector<Stub*> data_PRBF0);
    bool computeOutputTimes();
    void writeDataHistograms();
    void clearData();
    
    std::vector<std::vector<Stub*> > data_PRBF_RX_; // This should be 8 lines with 60 entries each. Should have a checking function.
    std::vector<TH1F*> v_h_nStubs_;
    TH1F *h_nStubs_Total_;
    
    double delayCLK_;
};    
      
