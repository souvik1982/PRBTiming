#include <iostream>

#include "../interface/BXSplitter.h"

BXSplitter::BXSplitter(std::string name, double frequency)
{
  type_="BXSplitter";
  name_=name;
  frequency_=frequency;
  
  for (unsigned int i=0; i<8; ++i)
  {
    t1in_.push_back(-999); // by link
    t2in_.push_back(-999); // by link
    t1out_.push_back(-999); // by BX
    t2out_.push_back(-999); // by BX
    
    v_h_t1in_.push_back(new TH1F(("h_t1in_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" Link "+itoa(i)+" t1in").c_str(), 2000, 0, 1000.));
    v_h_t2in_.push_back(new TH1F(("h_t2in_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" Link "+itoa(i)+" t2in").c_str(), 2000, 0, 1000.));
    v_h_t1out_.push_back(new TH1F(("h_t1out_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" BX "+itoa(i)+" t1out").c_str(), 2000, 0, 1000.));
    v_h_t2out_.push_back(new TH1F(("h_t2out_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" BX "+itoa(i)+" t2out").c_str(), 2000, 0, 1000.));
    
    v_h_nStubs_.push_back(new TH1F(("h_nStubs_"+name_+"_"+itoa(i)).c_str(), ("; BXSplitter "+name_+" BX "+itoa(i)+" nStubs").c_str(), 200, 0, 200));
  }
  
  data_PRBF1_ByLink_.resize(8);
  data_PRBF1_ByBX_.resize(8);
  time_PRBF1_ByBX_.resize(8);
  
  std::cout<<"LOG: Initialized BXSplitter "<<name_<<" with operating frequency = "<<frequency_<<std::endl;
}

void BXSplitter::fillInputData(std::vector<std::vector<Stub*> > data_PRBF_RX)
{
  data_PRBF1_ByLink_=data_PRBF_RX;
  for (unsigned int i_link=0; i_link<8; ++i_link)
  {
    for (unsigned int i_Stub=0; i_Stub<data_PRBF_RX.at(i_link).size(); ++i_Stub)
    {
      Stub *stub=data_PRBF_RX.at(i_link).at(i_Stub);
      data_PRBF1_ByBX_.at(stub->bx_).push_back(stub); // If this fails, check bx
    }
  }
}

bool BXSplitter::computeOutputTimes()
{
  if (frequency_>0)
  {
    // No streaming
    if (false)
    {
      double maxtin=-999;
      for (unsigned int i_link=0; i_link<8; ++i_link)
      {
        if (t1in_.at(i_link)>0 && t2in_.at(i_link)>0)
        {
          if (t1in_.at(i_link)+60./frequency_*1000. > maxtin) maxtin=t1in_.at(i_link)+60./frequency_*1000.;
          if (t2in_.at(i_link) > maxtin) maxtin=t2in_.at(i_link);
          
          v_h_t1in_.at(i_link)->Fill(t1in_.at(i_link));
          v_h_t2in_.at(i_link)->Fill(t2in_.at(i_link));
        }
        else
        {
          std::cout<<"ERROR: BXSplitter "<<name_<<" has no t1in and t2in set for link = "<<i_link<<std::endl;
          return false;
        }
      }
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        t1out_.at(i_BX)=maxtin;
        t2out_.at(i_BX)=maxtin+data_PRBF1_ByBX_.at(i_BX).size()/frequency_*1000.;
        
        v_h_t1out_.at(i_BX)->Fill(t1out_.at(i_BX));
        v_h_t2out_.at(i_BX)->Fill(t2out_.at(i_BX));
        
        v_h_nStubs_.at(i_BX)->Fill(data_PRBF1_ByBX_.at(i_BX).size());
      }
    }
    else // Streaming
    {
      // Clear out data_PRBF1_ByBX_. It will be refilled by CLK. In parallel to time_PRBF1_ByBX_
      data_PRBF1_ByBX_.clear();
      data_PRBF1_ByBX_.resize(8);
      
      // Minimum and maximum CLKs for each BX
      std::vector<double> v_minCLK_BX(8, -1), v_maxCLK_BX(8, -1);
      
      // This has to be added to the min t1in over all the links
      double mintin=1e10;
      for (unsigned int i_link=0; i_link<8; ++i_link)
      {
        if (t1in_.at(i_link)>0)
        {
          if (t1in_.at(i_link)<mintin) mintin=t1in_.at(i_link);
        }
        else std::cout<<"ERROR: BXSplitter "<<name_<<" has no t1in set for link "<<i_link<<std::endl;
      }
      unsigned int maxLinkSize=0;
      for (unsigned int i_link=0; i_link<8; ++i_link)
        if (data_PRBF1_ByLink_.at(i_link).size()>maxLinkSize) maxLinkSize=data_PRBF1_ByLink_.at(i_link).size();
      unsigned int clk=0;
      do
      {
        for (unsigned int i_link=0; i_link<8; ++i_link)
        {
          if (clk<data_PRBF1_ByLink_.at(i_link).size())
          {
            Stub *stub=data_PRBF1_ByLink_.at(i_link).at(clk);
            int bx=stub->bx_;
            if (bx>=0 && bx<=7)
            {
              v_maxCLK_BX[bx]=clk;
              if (v_minCLK_BX[bx]==-1) v_minCLK_BX[bx]=clk;
              
              // testing
              // std::cout<<"link = "<<link<<", bx = "<<bx<<" has stub = "<<data_PRBF1_ByLink_[i_link][clk]<<std::endl;
              // std::cout<<"stub by bx. link = "<<link<<", bx = "<<bx<<" has stub = "<<data_PRBF1_ByBX_[bx][clk]<<std::endl;
              data_PRBF1_ByBX_.at(bx).push_back(stub);
              time_PRBF1_ByBX_.at(bx).push_back(mintin+(clk+1)/frequency_*1000.);
            }
            else
            {
              std::cout<<"ERROR: In BXSplitter "<<name_<<", received BX = "<<bx<<std::endl;
            }
          }
        }
        ++clk;
      }
      while (clk<maxLinkSize && clk<60);
      
      // Another implementation of parking. Re-writes the timing for each stub
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        unsigned int nStubs=time_PRBF1_ByBX_.at(i_BX).size();
        for (unsigned int i_stub=0; i_stub<nStubs; ++i_stub)
        {
          time_PRBF1_ByBX_.at(i_BX).at(i_stub)=mintin+(60.+i_stub)/frequency_*1000.;
        }
      }
      
      // Check if minCLK for a BX is really the minimum time
      // Fill t1out and t2out
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        // std::cout<<"BX = "<<i_BX<<", t1out = "<<mintin+(60.)/frequency_*1000.<<std::endl;
        // std::cout<<"t2out = "<<mintin+(60.+v_maxCLK_BX[i_BX])/frequency_*1000<<std::endl;
        double tmin=999, tmax=-1;
        for (unsigned int i_stub=0; i_stub<time_PRBF1_ByBX_.at(i_BX).size(); ++i_stub)
        {
          double time=time_PRBF1_ByBX_.at(i_BX).at(i_stub);
          if (time<tmin) tmin=time;
          if (time>tmax) tmax=time;
        }
        // std::cout<<"Min time = "<<tmin<<std::endl;
        // std::cout<<"Max time = "<<tmax<<std::endl;
        t1out_.at(i_BX)=tmin;
        t2out_.at(i_BX)=tmax;
      }
      
      // Fill t1out and t2out
      /*
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        t1out_.at(i_BX)=mintin+(v_minCLK_BX[i_BX]+1)/frequency_*1000.;
        t2out_.at(i_BX)=mintin+(v_maxCLK_BX[i_BX]+1)/frequency_*1000.;
      }
      */
      
      // Fill histograms
      for (unsigned int i_link=0; i_link<8; ++i_link)
      {
        v_h_t1in_.at(i_link)->Fill(t1in_.at(i_link));
        v_h_t2in_.at(i_link)->Fill(t2in_.at(i_link));
      }
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        v_h_t1out_.at(i_BX)->Fill(t1out_.at(i_BX));
        v_h_t2out_.at(i_BX)->Fill(t2out_.at(i_BX));
        
        v_h_nStubs_.at(i_BX)->Fill(data_PRBF1_ByBX_.at(i_BX).size());
      }
    }
  }
  else
  {
    std::cout<<"ERROR: BXSplitter "<<name_<<" has no operating frequency set."<<std::endl;
    return false;
  }
  
  return true;
}

void BXSplitter::writeDataHistograms()
{
  TFile *file=new TFile((name_+".root").c_str(), "update");
  for (unsigned int i_BX=0; i_BX<8; ++i_BX)
  {
    v_h_nStubs_.at(i_BX)->Write();
  }
  file->Close();
}

void BXSplitter::clearData()
{
  for (unsigned int i=0; i<data_PRBF1_ByLink_.size(); ++i)
  {
    data_PRBF1_ByLink_.at(i).clear();
    t1in_.at(i)=-999;
    t2in_.at(i)=-999;
  }
  for (unsigned int i=0; i<data_PRBF1_ByBX_.size(); ++i)
  {
    data_PRBF1_ByBX_.at(i).clear();
    time_PRBF1_ByBX_.at(i).clear();
    t1out_.at(i)=-999;
    t2out_.at(i)=-999;
  }
}
