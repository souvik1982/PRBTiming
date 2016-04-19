#pragma once

#include <vector>

#include "Stub.h"

class CIC
{
  public:
    CIC(double f);
    void fillInputData(int BX, int modId, int layer);
    
    double f_;
    std::vector<Stub*> data_PRBF0_;
};
