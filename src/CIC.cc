#include "../interface/CIC.h"

CIC::CIC(std::string name, int moduleID, std::string segment, double f)
{
  type_="CIC";
  name_=name;
  moduleID_=moduleID;
  segment_=segment;
  f_=f;
}

void CIC::fillInputData(int bx, int modID, int layer)
{
  Stub* stub=new Stub(bx, modID, layer);
  data_PRBF0_.push_back(stub);
}
