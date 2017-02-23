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
    else // streaming
    {
      double minCLK_BX0=-1, maxCLK_BX0=-1;
      double minCLK_BX1=-1, maxCLK_BX1=-1;
      double minCLK_BX2=-1, maxCLK_BX2=-1;
      double minCLK_BX3=-1, maxCLK_BX3=-1;
      double minCLK_BX4=-1, maxCLK_BX4=-1;
      double minCLK_BX5=-1, maxCLK_BX5=-1;
      double minCLK_BX6=-1, maxCLK_BX6=-1;
      double minCLK_BX7=-1, maxCLK_BX7=-1;
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
            int bx=data_PRBF1_ByLink_.at(i_link).at(clk)->bx_;
            if (bx==0)
            {
              maxCLK_BX0=clk;
              if (minCLK_BX0==-1) minCLK_BX0=clk;
            }
            else if (bx==1)
            {
              maxCLK_BX1=clk;
              if (minCLK_BX1==-1) minCLK_BX1=clk;
            }
            else if (bx==2)
            {
              maxCLK_BX2=clk;
              if (minCLK_BX2==-1) minCLK_BX2=clk;
            }
            else if (bx==3)
            {
              maxCLK_BX3=clk;
              if (minCLK_BX3==-1) minCLK_BX3=clk;
            }
            else if (bx==4)
            {
              maxCLK_BX4=clk;
              if (minCLK_BX4==-1) minCLK_BX4=clk;
            }
            else if (bx==5)
            {
              maxCLK_BX5=clk;
              if (minCLK_BX5==-1) minCLK_BX5=clk;
            }
            else if (bx==6)
            {
              maxCLK_BX6=clk;
              if (minCLK_BX6==-1) minCLK_BX6=clk;
            }
            else if (bx==7)
            {
              maxCLK_BX7=clk;
              if (minCLK_BX7==-1) minCLK_BX7=clk;
            }
            else
            {
              std::cout<<"ERROR: In BXSplitter "<<name_<<", received BX = "<<bx<<std::endl;
            }
          }
        }
        ++clk;
      }
      while (clk<maxLinkSize);
      
      // This has to be added to the min t1in over all the links
      double mintin=1e10;
      for (unsigned int i_link=0; i_link<8; ++i_link)
      {
        if (t1in_.at(i_link)>0)
        {
          if (t1in_.at(i_link)<mintin) mintin=t1in_.at(i_link);
        }
        else
        {
          std::cout<<"ERROR: BXSplitter "<<name_<<" has no t1in set for link "<<i_link<<std::endl;
        }
      }
      
      // Compute t1out for each BX
      t1out_.at(0)=mintin+(minCLK_BX0+1)/frequency_*1000.;
      t1out_.at(1)=mintin+(minCLK_BX1+1)/frequency_*1000.;
      t1out_.at(2)=mintin+(minCLK_BX2+1)/frequency_*1000.;
      t1out_.at(3)=mintin+(minCLK_BX3+1)/frequency_*1000.;
      t1out_.at(4)=mintin+(minCLK_BX4+1)/frequency_*1000.;
      t1out_.at(5)=mintin+(minCLK_BX5+1)/frequency_*1000.;
      t1out_.at(6)=mintin+(minCLK_BX6+1)/frequency_*1000.;
      t1out_.at(7)=mintin+(minCLK_BX7+1)/frequency_*1000.;
      
      // Compute t2out for each BX
      t2out_.at(0)=mintin+(maxCLK_BX0+1)/frequency_*1000.;
      t2out_.at(1)=mintin+(maxCLK_BX1+1)/frequency_*1000.;
      t2out_.at(2)=mintin+(maxCLK_BX2+1)/frequency_*1000.;
      t2out_.at(3)=mintin+(maxCLK_BX3+1)/frequency_*1000.;
      t2out_.at(4)=mintin+(maxCLK_BX4+1)/frequency_*1000.;
      t2out_.at(5)=mintin+(maxCLK_BX5+1)/frequency_*1000.;
      t2out_.at(6)=mintin+(maxCLK_BX6+1)/frequency_*1000.;
      t2out_.at(7)=mintin+(maxCLK_BX7+1)/frequency_*1000.;
      
      // Fill histograms
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
  }
  for (unsigned int i=0; i<data_PRBF1_ByBX_.size(); ++i)
  {
    data_PRBF1_ByBX_.at(i).clear();
  }
}
