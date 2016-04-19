#include "../interface/CIC.h"

CIC::CIC(double f)
{
  f_=f;
}

void CIC::fillInputData(int bx, int modID, int layer)
{
  Stub* stub=new Stub(bx, modID, layer);
  data_PRBF0_.push_back(stub);
}
