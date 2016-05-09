#include <vector>

#include "Stub.h"
#include "Component.h"

class CIC: public Component
{
  public:
    CIC(std::string name, int moduleID, std::string segment, double frequency);
    void fillInputData(int BX, int modId, int layer);
    bool computeOutputTimes();
    void writeDataHistograms();
    void clearData();
    
    int moduleID_;
    std::string segment_;
    std::vector<Stub*> data_PRBF0_;
    TH1F *h_nStubs_;
};
