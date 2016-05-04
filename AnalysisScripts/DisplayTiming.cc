#include <TH1F.h>
#include <THStack.h>
#include <TLegend.h>
#include <TArrow.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "../interface/ComponentRelation.h"
#include "../interface/CIC.h"
#include "../interface/Receiver.h"
#include "../interface/BXSplitter.h"
#include "../interface/LayerSplitter.h"
#include "../src/CommandLineArguments.cc"
#include "../src/ReadConfigurationFile.cc"

bool logScale=true;

std::string ftoa2(double i) 
{
  char res[10];
  sprintf(res, "%2.2f", i);
  std::string ret(res);
  return ret;
}

std::string ftoa3(double i) 
{
  char res[10];
  sprintf(res, "%2.3f", i);
  std::string ret(res);
  return ret;
}

int fixRange(TH1F *h)
{
  double nBinsHave=h->FindLastBinAbove(0, 1)+1;
  double nEntries=h->GetEntries();
  int rebin=0;  
  /*if (nEntries>0 && nBinsHave>0)
  {
    rebin=int(200.*nBinsHave/nEntries);
    if (rebin>0) h->Rebin(rebin);
    else rebin=-1;
  }*/
  h->GetXaxis()->SetRange(h->FindFirstBinAbove(0, 1)-1, h->FindLastBinAbove(0, 1)+1);
  return rebin;
}

void makeCanvas(TH1F *h1, double percentile, std::string name_s, std::string componentType, std::string units="", int color=kBlue)
{
  h1->SetLineColor(color);
  h1->GetXaxis()->SetRange(h1->FindFirstBinAbove(0, 1)-1, h1->FindLastBinAbove(0, 1)+1);
  TCanvas *c=new TCanvas(("c_"+name_s).c_str(), ("c_"+name_s).c_str(), 700, 700);
  if (logScale) c->SetLogy();
  h1->SetTitle(("; "+componentType+" "+name_s+" ("+units+")").c_str());
  h1->Draw("");
  double perc[1]={percentile};
  double values[1];
  h1->GetQuantiles(1, values, perc);
  TLegend *leg=new TLegend(0.6, 0.7, 0.89, 0.89);
  leg->SetBorderSize(0);
  leg->AddEntry(h1, (ftoa3(percentile)+" perc: "+ftoa2(values[0])+" "+units).c_str());
  leg->Draw();
  TArrow *arrow=new TArrow(values[0], h1->GetMaximum()*0.5, values[0], 0, 0.05, "|>");
  arrow->Draw();
  c->SaveAs(("c_"+name_s+".png").c_str());
}

void makeCanvas(TH1F *h1, TH1F *h2, double percentile, std::string name_s, std::string componentType, std::string units="")
{
  h2->SetLineColor(kGreen+2);
  int minBin=std::min(h1->FindFirstBinAbove(0, 1), h2->FindFirstBinAbove(0, 1));
  int maxBin=std::max(h1->FindLastBinAbove(0, 1), h2->FindLastBinAbove(0, 1));
  h2->GetXaxis()->SetRange(minBin-1, maxBin+1);
  TCanvas *c=new TCanvas(("c_"+name_s).c_str(), ("c_"+name_s).c_str(), 700, 700);
  if (logScale) c->SetLogy();
  h2->SetTitle(("; "+componentType+" "+name_s+" ("+units+")").c_str());
  h2->SetMaximum(h1->GetMaximum()*1.2);
  h2->Draw("");
  h1->Draw("same");
  h2->Draw("same");
  double perc1[1]={percentile};
  double values1[1];
  h1->GetQuantiles(1, values1, perc1);
  double perc2[1]={percentile};
  double values2[1];
  h2->GetQuantiles(1, values2, perc2);
  TLegend *leg=new TLegend(0.6, 0.7, 0.89, 0.89);
  leg->SetBorderSize(0);
  leg->AddEntry(h1, ("t1out "+ftoa3(percentile)+" perc: "+ftoa2(values1[0])+" "+units).c_str());
  leg->AddEntry(h1, ("t2out "+ftoa3(percentile)+" perc: "+ftoa2(values2[0])+" "+units).c_str());
  leg->Draw();
  TArrow *arrow1=new TArrow(values1[0], h1->GetMaximum()*0.5, values1[0], 0, 0.05, "|>");
  arrow1->SetLineColor(kBlue); arrow1->Draw();
  TArrow *arrow2=new TArrow(values2[0], h2->GetMaximum()*0.5, values2[0], 0, 0.05, "|>");
  arrow2->SetLineColor(kGreen+2); arrow2->Draw();
  c->SaveAs(("c_"+name_s+".png").c_str());
}

int main(int argc, char *argv[])
{
  std::string schematicDir;
  std::string schematicFile;
  float percentile;
  
  // Get command line arguments
  std::map<std::string, std::string> cmdMap=commandLineArguments(argc, argv);
  if (cmdMap.find("-schematicDir")!=cmdMap.end()) schematicDir=cmdMap["-schematicDir"];
  if (cmdMap.find("-schematicFile")!=cmdMap.end()) schematicFile=cmdMap["-schematicFile"];
  if (cmdMap.find("-percentile")!=cmdMap.end())  percentile=atof(cmdMap["-percentile"].c_str());
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  
  /*
  ofstream outfile;
  outfile.open("index.html");
  outfile<<"<html>"<<std::endl;
  outfile<<"<head>"<<std::endl;
  outfile<<" <style>"<<std::endl;
  outfile<<"  <link rel="StyleSheet" type="text/css" href="PRBDashboardStyle.css" /> 
  outfile<<"</head>"<<std::endl;
  outfile<<"<body>"<<std::endl;
  outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
  outfile<<"<table border='1'>"<<std::endl;
  */
  
  // Read the schematic file recursively
  MapComponentRelations *map_componentRelations=new MapComponentRelations;
  MapModuleIDCIC *map_modId_CIC=new MapModuleIDCIC;
  readConfigurationFile(schematicDir, schematicFile, map_componentRelations, map_modId_CIC);
  
  std::vector<std::vector<CIC*> > v_CIC(10);
  std::vector<std::vector<Receiver*> > v_Receiver(10);
  std::vector<std::vector<BXSplitter*> > v_BXSplitter(10);
  std::vector<std::vector<LayerSplitter*>> v_LayerSplitter(10);
  
  for (MapComponentRelations::iterator i_comp=map_componentRelations->begin(); i_comp!=map_componentRelations->end(); ++i_comp)
  {
    ComponentRelation *componentRelation=i_comp->second;
    Component *component=componentRelation->comp_;
      
    if (component->get_type()=="CIC")
    {
      int index=i_comp->second->i_comp_.at(0);
      int i_PRB=(index-10000)/10;
      CIC* cic=(CIC*)component;
      v_CIC.at(i_PRB).push_back(cic);
      std::string name_s=cic->get_name();
      int moduleID=cic->moduleID_;
      std::string segment=cic->segment_;
      double frequency=cic->get_frequency();
      TFile *f_cic=new TFile((cic->get_name()+".root").c_str());
      TH1F *h_t1in_cic=(TH1F*)f_cic->Get(("h_t1in_"+name_s).c_str());
      TH1F *h_t2in_cic=(TH1F*)f_cic->Get(("h_t2in_"+name_s).c_str());
      TH1F *h_t1out_cic=(TH1F*)f_cic->Get(("h_t2out_"+name_s).c_str());
      TH1F *h_t2out_cic=(TH1F*)f_cic->Get(("h_t2out_"+name_s).c_str());
      makeCanvas(h_t1in_cic, h_t2in_cic, percentile, name_s+"_in", "CIC", "ns");
      makeCanvas(h_t1out_cic, h_t2out_cic, percentile, name_s+"_out", "CIC", "ns");
      f_cic->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" CIC name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" ModuleID: "<<moduleID<<"<br/>"<<std::endl;
      outfile<<" Segment: "<<segment<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<img width=\"500em\" src='"<<("c_"+name_s+"_in.png")<<"'/>"<<std::endl;
      outfile<<"<img width=\"500em\" src='"<<("c_"+name_s+"_out.png")<<"'/>"<<std::endl;
      outfile<<"</body>"<<std::endl;
      outfile<<"</html>"<<std::endl;
      outfile.close();
    }
    
    if (component->get_type()=="Receiver")
    {
      int index=i_comp->first;
      int i_PRB=(index-10000)/10;
      Receiver *receiver=(Receiver*)component;
      v_Receiver.at(i_PRB).push_back(receiver);
      std::string name_s=receiver->get_name();
      double frequency=receiver->get_frequency();
      double delayCLK=receiver->delayCLK_;
      TFile *f_receiver=new TFile((receiver->get_name()+".root").c_str());
      for (unsigned int i_pin=0; i_pin<40; ++i_pin)
      {
        TH1F *h_t1in=(TH1F*)f_receiver->Get(("h_t1in_"+name_s+"_"+itoa(i_pin)).c_str());
        TH1F *h_t2in=(TH1F*)f_receiver->Get(("h_t2in_"+name_s+"_"+itoa(i_pin)).c_str());
        makeCanvas(h_t1in, h_t2in, percentile, name_s+"_in_Pin"+itoa(i_pin), "Receiver", "ns");
      }
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        TH1F *h_t1out=(TH1F*)f_receiver->Get(("h_t1out_"+name_s+"_"+itoa(i_pin)).c_str());
        TH1F *h_t2out=(TH1F*)f_receiver->Get(("h_t2out_"+name_s+"_"+itoa(i_pin)).c_str());
        makeCanvas(h_t1out, h_t2out, percentile, name_s+"_out_Pin"+itoa(i_pin), "Receiver", "ns");
      }
      f_receiver->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" Receiver name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<" Delay: "<<delayCLK<<" CLK <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<40; ++i_pin)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_in_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_out_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"</body>"<<std::endl;
      outfile<<"</html>"<<std::endl;
      outfile.close();
    }
    
    if (component->get_type()=="BXSplitter")
    {
      int index=i_comp->first;
      int i_PRB=(index-20000)/10;
      BXSplitter *bxSplitter=(BXSplitter*)component;
      v_BXSplitter.at(i_PRB).push_back(bxSplitter);
      std::string name_s=bxSplitter->get_name();
      double frequency=bxSplitter->get_frequency();
      TFile *f_bxSplitter=new TFile((bxSplitter->get_name()+".root").c_str());
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        TH1F *h_t1in=(TH1F*)f_bxSplitter->Get(("h_t1in_"+name_s+"_"+itoa(i_BX)).c_str());
        TH1F *h_t2in=(TH1F*)f_bxSplitter->Get(("h_t2in_"+name_s+"_"+itoa(i_BX)).c_str());
        TH1F *h_t1out=(TH1F*)f_bxSplitter->Get(("h_t1out_"+name_s+"_"+itoa(i_BX)).c_str());
        TH1F *h_t2out=(TH1F*)f_bxSplitter->Get(("h_t2out_"+name_s+"_"+itoa(i_BX)).c_str());
        makeCanvas(h_t1in, h_t2in, percentile, name_s+"_in_BX"+itoa(i_BX), "BXSplitter", "ns");
        makeCanvas(h_t1out, h_t2out, percentile, name_s+"_out_BX"+itoa(i_BX), "BXSplitter", "ns");
      }
      f_bxSplitter->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" BXSplitter name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_in_BX"+itoa(i_BX)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_out_BX"+itoa(i_BX)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"</body>"<<std::endl;
      outfile<<"</html>"<<std::endl;
      outfile.close();
    }
    
    if (component->get_type()=="LayerSplitter")
    {
      int index=i_comp->first;
      int i_PRB=(index-30000)/10;
      LayerSplitter *layerSplitter=(LayerSplitter*)component;
      v_LayerSplitter.at(i_PRB).push_back(layerSplitter);
      std::string name_s=layerSplitter->get_name();
      double frequency=layerSplitter->get_frequency();
      TFile *f_layerSplitter=new TFile((layerSplitter->get_name()+".root").c_str());
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        TH1F *h_t1in=(TH1F*)f_layerSplitter->Get(("h_t1in_"+name_s+"_"+itoa(i_PRB)).c_str());
        TH1F *h_t2in=(TH1F*)f_layerSplitter->Get(("h_t2in_"+name_s+"_"+itoa(i_PRB)).c_str());
        makeCanvas(h_t1in, h_t2in, percentile, name_s+"_PRB"+itoa(i_PRB), "LayerSplitter", "ns");
      }
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        TH1F *h_t1out=(TH1F*)f_layerSplitter->Get(("h_t1out_"+name_s+"_"+itoa(i_layer)).c_str());
        TH1F *h_t2out=(TH1F*)f_layerSplitter->Get(("h_t2out_"+name_s+"_"+itoa(i_layer)).c_str());
        makeCanvas(h_t1out, h_t2out, percentile, name_s+"_layer"+itoa(i_layer), "LayerSplitter", "ns");
      }
      f_layerSplitter->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" LayerSplitter name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_PRB"+itoa(i_PRB)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"<table border='1'>"<<std::endl;
      outfile<<" <tr>"<<std::endl;
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        outfile<<"  <td>"<<std::endl;
        outfile<<"   <img width=\"500em\" src='"<<("c_"+name_s+"_layer"+itoa(i_layer)+".png")<<"'/>"<<std::endl;
        outfile<<"  </td>"<<std::endl;
      }
      outfile<<" </tr>"<<std::endl;
      outfile<<"</table>"<<std::endl;
      outfile<<"</body>"<<std::endl;
      outfile<<"</html>"<<std::endl;
      outfile.close();
    }
      
  }
  
  // Draw PRB level 
  std::ofstream outfile("index.html");
  outfile<<"<html>"<<std::endl;
  outfile<<"<head>"<<std::endl;
  outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
  outfile<<"</head>"<<std::endl;
  outfile<<"<body>"<<std::endl;
  outfile<<"<h1 align='center'> L1 Track Trigger Timing Dashboard </h1>"<<std::endl;
  for (unsigned int i_PRB=0; i_PRB<v_CIC.size(); ++i_PRB)
  {
    outfile<<" <table border='1'>"<<std::endl;
    outfile<<"  <tr>"<<std::endl;
    outfile<<"   <td width='5%'>"<<std::endl;
    outfile<<"    <b>PRB "<<i_PRB<<"</b>"<<std::endl;
    outfile<<"   </td>"<<std::endl;
    outfile<<"   <td width='50%'>"<<std::endl;
    for (unsigned int i_CIC=0; i_CIC<v_CIC.at(i_PRB).size(); ++i_CIC)
    {
      CIC *cic=v_CIC.at(i_PRB).at(i_CIC);
      std::string name_s=cic->get_name();
      int moduleID=cic->moduleID_;
      std::string segment=cic->segment_;
      outfile<<"    <a href='"<<(name_s+".html")<<"'><div class='CIC'> CIC "<<segment<<" <br/> "<<moduleID<<"</div></a>"<<std::endl;
    }
    outfile<<"   </td>"<<std::endl;
    outfile<<"   <td>"<<std::endl;
    for (unsigned int i_Receiver=0; i_Receiver<v_Receiver.at(i_PRB).size(); ++i_Receiver)
    {
      Receiver *receiver=v_Receiver.at(i_PRB).at(i_Receiver);
      std::string name_s=receiver->get_name();
      outfile<<"    <a href='"<<(name_s+".html")<<"'><div class='Receiver'> Receiver <br/> "<<name_s<<"</div></a><br/>"<<std::endl;
    } 
    outfile<<"   </td>"<<std::endl;
    outfile<<"   <td>"<<std::endl;
    for (unsigned int i_BXSplitter=0; i_BXSplitter<v_BXSplitter.at(i_PRB).size(); ++i_BXSplitter)
    {
      BXSplitter *bxSplitter=v_BXSplitter.at(i_PRB).at(i_BXSplitter);
      std::string name_s=bxSplitter->get_name();
      outfile<<"    <a href='"<<(name_s+".html")<<"'><div class='BXSplitter'> BXSplitter <br/> "<<name_s<<"</div></a><br/>"<<std::endl;
    }
    outfile<<"   </td>"<<std::endl;
    outfile<<"   <td>"<<std::endl;
    for (unsigned int i_LayerSplitter=0; i_LayerSplitter<v_LayerSplitter.at(i_PRB).size(); ++i_LayerSplitter)
    {
      LayerSplitter *layerSplitter=v_LayerSplitter.at(i_PRB).at(i_LayerSplitter);
      std::string name_s=layerSplitter->get_name();
      outfile<<"    <a href='"<<(name_s+".html")<<"'><div class='LayerSplitter'> LayerSplitter <br/> "<<name_s<<"</div></a><br/>"<<std::endl;
    }
    outfile<<"   </td>"<<std::endl;
    outfile<<"  </tr>"<<std::endl;
    outfile<<" </table>"<<std::endl;
    outfile<<" <br/>"<<std::endl;
  }
  outfile<<"</body>"<<std::endl;
  outfile.close();
}
      
