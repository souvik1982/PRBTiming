#pragma once

#include <vector>

#include "Stub.h"

class CIC
{
  public:
    std::vector<Stub> data_PRBF0_L_(12), data_PRBF0_R_(12);
    
    // This has to be filled by 8 events for a train
    fillInputData(int BX, int modId, int layer)
    {
    
    }
    
};
