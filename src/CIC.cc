#include <iostream>

#include "../interface/CIC.h"

CIC::CIC(std::string name, int moduleID, std::string segment, double frequency)
{
  type_="CIC";
  name_=name;
  moduleID_=moduleID;
  segment_=segment;
  frequency_=frequency;
  
  t1in_.push_back(0);
  t2in_.push_back(0);
  t1out_.push_back(1./frequency*1000.);
  t2out_.push_back(64./frequency*1000.);
  
  v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_).c_str(), ("; CIC "+name_+" t1in (ns)").c_str(), 800, 0, 400.));
  v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_).c_str(), ("; CIC "+name_+" t2in (ns)").c_str(), 800, 0, 400.));
  
  v_h_t1in_.at(0)->Fill(t1in_.at(0));
  v_h_t2in_.at(0)->Fill(t2in_.at(0));
  
  v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_).c_str(), ("; CIC "+name_+" t1out (ns)").c_str(), 800, 0, 400.));
  v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_).c_str(), ("; CIC "+name_+" t2out (ns)").c_str(), 800, 0, 400.));
  
  v_h_t1out_.at(0)->Fill(t1out_.at(0));
  v_h_t2out_.at(0)->Fill(t2out_.at(0));
  
  std::cout<<"LOG: Initialized CIC "<<name_<<" with moduleID = "<<moduleID_<<", segment = "<<segment_<<", operating frequency = "<<frequency_<<std::endl;
}

void CIC::fillInputData(int bx, int modID, int layer)
{
  Stub* stub=new Stub(bx, modID, layer);
  data_PRBF0_.push_back(stub);
}

bool CIC::computeOutputTimes()
{
  return true;
}

void CIC::clearData()
{
  data_PRBF0_.clear();
}
