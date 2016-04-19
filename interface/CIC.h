#pragma once

#include <vector>

#include "Stub.h"

class CIC
{
  public:
    std::vector<Stub*> data_PRBF0_(12);
    
    // This has to be filled by 8 events for a train
    fillInputData(int BX, int modId, int layer)
    {
      Stub* stub=new Stub(BX, modId, layer);
      data_PRBF0_.push_back(stub);
    }
    
};
