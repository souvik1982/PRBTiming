#include <iostream>

#include "../interface/Receiver.h"

Receiver::Receiver(std::string name, double frequency, double delayCLK)
{
  type_="Receiver";
  name_=name;
  frequency_=frequency;
  delayCLK_=delayCLK;
  
  for (unsigned int i=0; i<40; ++i)
  {
    t1in_.push_back(1./frequency_*1000.);
    t2in_.push_back(64./frequency_*1000.);
    
    v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_+"_"+itoa(i)).c_str(), ("; Receiver "+name_+" Link "+itoa(i)+" t1in").c_str(), 2000, 0, 1000.));
    v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_+"_"+itoa(i)).c_str(), ("; Receiver "+name_+" Link "+itoa(i)+" t2in").c_str(), 2000, 0, 1000.));
  }
  
  for (unsigned int i=0; i<8; ++i)
  {
    t1out_.push_back(-999);
    t2out_.push_back(-999);
    
    v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_+"_"+itoa(i)).c_str(), ("; Receiver "+name_+" Link "+itoa(i)+" t1out").c_str(), 2000, 0, 1000.));
    v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_+"_"+itoa(i)).c_str(), ("; Receiver "+name_+" Link "+itoa(i)+" t2out").c_str(), 2000, 0, 1000.));
  
    v_h_nStubs_.push_back(new TH1F(("h_nStubs_"+name_+"_"+itoa(i)).c_str(), ("; Receiver "+name_+" Link "+itoa(i)+" nStubs").c_str(), 1000, 0, 200.));
  }
  
  data_PRBF_RX_.resize(8);
  
  std::cout<<"LOG: Initialized Receiver "<<name_<<" with operating frequency = "<<frequency_<<", delay = "<<delayCLK_<<" CLK"<<std::endl;
}

void Receiver::fillInputData(int link, std::vector<Stub*> data_PRBF0)
{
  for (unsigned int i=0; i<data_PRBF0.size(); ++i)
  {
    data_PRBF_RX_.at(link/5).push_back(data_PRBF0.at(i));
  }
}

bool Receiver::computeOutputTimes()
{
  for (unsigned int i=0; i<40; ++i)
  {
    v_h_t1in_.at(i)->Fill(t1in_.at(i));
    v_h_t2in_.at(i)->Fill(t2in_.at(i));
  }
  for (unsigned int i=0; i<8; ++i)
  {
    t1out_.at(i)=t2in_.at(i*5)+1./frequency_*1000.;
    t2out_.at(i)=t1out_.at(i)+60./frequency_*1000.;
    
    v_h_t1out_.at(i)->Fill(t1out_.at(i));
    v_h_t2out_.at(i)->Fill(t2out_.at(i));
    
    v_h_nStubs_.at(i)->Fill(data_PRBF_RX_.at(i).size());
  }
  return true;
}

void Receiver::clearData()
{
  for (unsigned int i=0; i<data_PRBF_RX_.size(); ++i)
  {
    data_PRBF_RX_.at(i).clear();
  }
}
