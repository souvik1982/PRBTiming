#include <vector>

#include "Stub.h"
#include "Component.h"

class LayerSplitter: public Component
{
  public:
    LayerSplitter(std::string name, double frequency);
    // void fillInputData(int bXoutputPin, int prbInputPin, std::vector<std::vector<Stub*> > data_PRBF1);
    void fillInputData(int bXoutputPin, int prbInputPin, std::vector<std::vector<Stub*> > data_PRBF1, std::vector<std::vector<double> > time_PRBF1);
    
    bool computeOutputTimes();
    void writeDataHistograms();
    void clearData();
    
    std::vector<std::vector<Stub*> > data_PRBF2_ByPRB_; // [PRB][i] -- is required for computing t1out
    std::vector<std::vector<Stub*> > data_PRBF2_ByLayer_; // [Layer][i] -- is used for plotting
    std::vector<std::vector<double> > time_PRBF2_ByPRB_; // [PRB][i] -- for tin
    std::vector<std::vector<double> > time_PRBF2_ByLayer_; // [Layer][i] -- for tout
    
    std::vector<TH1F*> v_h_nStubs_ByPRB_;
    std::vector<TH1F*> v_h_nStubs_ByLayer_;
};
