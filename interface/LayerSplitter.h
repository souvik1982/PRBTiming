#include <vector>

#include "Stub.h"
#include "Component.h"

class LayerSplitter: public Component
{
  public:
    LayerSplitter(std::string name, double frequency);
    void fillInputData(std::vector<std::vector<Stub*> > data_PRBF1);
    bool computeOutputTimes();
    void writeHistograms();
    
    std::vector<std::vector<Stub*> > data_PRBF2_; // [layer]
    
};
