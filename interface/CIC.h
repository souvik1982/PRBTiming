#include <vector>

#include "Stub.h"

#include "Component.h"

class CIC: public Component
{
  public:
    CIC(std::string name, int moduleID, std::string segment, double f);
    void fillInputData(int BX, int modId, int layer);
    bool computeOutputTimes();
    void writeHistograms();
    
    int moduleID_;
    std::string segment_;
    double f_;
    std::vector<Stub*> data_PRBF0_;
};
