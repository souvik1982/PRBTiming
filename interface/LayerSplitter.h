#include <vector>

#include "Stub.h"
#include "Component.h"

class LayerSplitter: public Component
{
  public:
    LayerSplitter(std::string name, double frequency);
    void fillInputData(int bXoutputPin, int prbInputPin, std::vector<std::vector<Stub*> > data_PRBF1);
    bool computeOutputTimes();
    void clearData();
    
    std::vector<std::vector<Stub*> > data_PRBF2_ByPRB_; // [PRB][i] -- is required for computing t1out
    
};
