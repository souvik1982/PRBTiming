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

std::string ftoa1(double i) 
{
  char res[10];
  sprintf(res, "%2.1f", i);
  std::string ret(res);
  return ret;
}

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

void makeCanvas(TH1F *h1, double percentile, std::string name_s, std::string componentType, std::string units="", int rebin=1, bool logScale=false)
{
  h1->Rebin(rebin);
  h1->SetLineWidth(2);
  
  h1->GetXaxis()->SetRange(h1->FindFirstBinAbove(0, 1)-20, h1->FindLastBinAbove(0, 1)+20);
  h1->SetLineColor(kRed);
  h1->SetTitle(("; "+componentType+" "+name_s+" ("+units+")").c_str());
  double perc[2]={0.5, percentile};
  double values[2];
  h1->GetQuantiles(2, values, perc);
  
  TCanvas *c=new TCanvas(("c_"+name_s).c_str(), ("c_"+name_s).c_str(), 700, 700);
  TPad *p_1=new TPad(("p_1_"+name_s).c_str(), ("p_1_"+name_s).c_str(), 0, 0, 1.0, 0.90);
  TPad *p_2=new TPad(("p_2_"+name_s).c_str(), ("p_2_"+name_s).c_str(), 0, 0.90, 1.0, 1.0);
  p_1->Draw();
  p_2->Draw();
  
  p_1->cd();
  if (logScale) c->SetLogy();
  h1->Draw();
  TArrow *arrow=new TArrow(values[1], h1->GetMaximum()*0.5, values[1], 0, 0.05, "|>");
  arrow->SetLineColor(kBlack); arrow->Draw();
  p_1->Update();
  
  p_2->cd();
  TLegend *leg=new TLegend(0.1, -0.2, 0.9, 0.8);
  leg->SetBorderSize(0);
  leg->AddEntry(h1, ("#mu = "+ftoa2(values[0])+", #Lambda_{"+ftoa2(percentile)+"} = "+ftoa2(values[1])+" "+units).c_str());
  leg->Draw();
  p_2->Update();
  
  c->SaveAs(("c_"+name_s+".png").c_str());
}

void makeCanvas(TH1F *h1, TH1F *h2, std::string direction, double percentile, std::string name_s, std::string componentType, std::string units="", int rebin=1, bool logScale=false)
{ 
  h1->Rebin(rebin);
  h2->Rebin(rebin);
  
  int minBin=std::min(h1->FindFirstBinAbove(0, 1), h2->FindFirstBinAbove(0, 1));
  int maxBin=std::max(h1->FindLastBinAbove(0, 1), h2->FindLastBinAbove(0, 1));
  h1->GetXaxis()->SetRange(minBin-20, maxBin+20);
  h1->SetLineColor(kBlue);
  h2->SetLineColor(kGreen+2);
  h1->SetTitle(("; "+componentType+" "+name_s+" ("+units+")").c_str());
  h1->Scale(1./h1->GetSumOfWeights());
  h2->Scale(1./h2->GetSumOfWeights());
  h1->SetMaximum(std::max(h1->GetMaximum(), h2->GetMaximum())*1.1);
  double perc1[2]={0.5, percentile};
  double values1[2];
  h1->GetQuantiles(2, values1, perc1);
  double perc2[2]={0.5, percentile};
  double values2[2];
  h2->GetQuantiles(2, values2, perc2);
  
  values1[0]=h1->GetBinLowEdge(h1->FindBin(values1[0]));
  values1[1]=h1->GetBinLowEdge(h1->FindBin(values1[1]));
  values2[0]=h1->GetBinLowEdge(h1->FindBin(values2[0]));
  values2[1]=h1->GetBinLowEdge(h1->FindBin(values2[1]));
  
  TCanvas *c=new TCanvas(("c_"+name_s).c_str(), ("c_"+name_s).c_str(), 700, 700);
  TPad *p_1=new TPad(("p_1_"+name_s).c_str(), ("p_1_"+name_s).c_str(), 0, 0, 1.0, 0.90);
  TPad *p_2=new TPad(("p_2_"+name_s).c_str(), ("p_2_"+name_s).c_str(), 0, 0.90, 1.0, 1.0);
  p_1->Draw();
  p_2->Draw();
  
  p_1->cd();
  if (logScale) p_1->SetLogy();
  h1->Draw("");
  h2->Draw("same");
  TArrow *arrow1=new TArrow(values1[1], h1->GetMaximum()*0.5, values1[1], 0, 0.05, "|>");
  arrow1->SetLineColor(kBlue); arrow1->Draw();
  TArrow *arrow2=new TArrow(values2[1], h2->GetMaximum()*0.5, values2[1], 0, 0.05, "|>");
  arrow2->SetLineColor(kGreen+2); arrow2->Draw();
  p_1->Update();
  
  p_2->cd();
  TLegend *leg=new TLegend(0.1, -0.2, 0.9, 0.8);
  leg->SetBorderSize(0);
  if (direction=="in")
  {
    leg->AddEntry(h1, ("t1in #mu = "+ftoa1(values1[0])+", #Lambda_{"+ftoa2(percentile)+"} = "+ftoa1(values1[1])+" "+units).c_str());
    leg->AddEntry(h2, ("t2in #mu = "+ftoa1(values2[0])+", #Lambda_{"+ftoa2(percentile)+"} = "+ftoa1(values2[1])+" "+units).c_str());
  } 
  else
  {
    leg->AddEntry(h1, ("t1out #mu = "+ftoa1(values1[0])+", #Lambda_{"+ftoa2(percentile)+"} = "+ftoa1(values1[1])+" "+units).c_str());
    leg->AddEntry(h2, ("t2out #mu = "+ftoa1(values2[0])+", #Lambda_{"+ftoa2(percentile)+"} = "+ftoa1(values2[1])+" "+units).c_str());
  } 
  leg->Draw();
  p_2->Update();
  
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
      TH1F *h_t1out_cic=(TH1F*)f_cic->Get(("h_t1out_"+name_s).c_str());
      TH1F *h_t2out_cic=(TH1F*)f_cic->Get(("h_t2out_"+name_s).c_str());
      TH1F *h_nStubs_cic=(TH1F*)f_cic->Get(("h_nStubs_"+name_s).c_str());
      makeCanvas(h_t1in_cic, h_t2in_cic, "in", percentile, name_s+"_in", "CIC", "ns");
      makeCanvas(h_t1out_cic, h_t2out_cic, "out", percentile, name_s+"_out", "CIC", "ns");
      makeCanvas(h_nStubs_cic, percentile, name_s+"_nStubs", "CIC", "nStubs", 2);
      f_cic->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing -- CIC Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" CIC name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" ModuleID: "<<moduleID<<"<br/>"<<std::endl;
      outfile<<" Segment: "<<segment<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Data Content </h2>"<<std::endl;
      outfile<<" <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs.png")<<"'/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Timing </h2>"<<std::endl;
      outfile<<" <img width=\"500em\" src='"<<("c_"+name_s+"_in.png")<<"'/>"<<std::endl;
      outfile<<" <img width=\"500em\" src='"<<("c_"+name_s+"_out.png")<<"'/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
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
        makeCanvas(h_t1in, h_t2in, "in", percentile, name_s+"_in_Pin"+itoa(i_pin), "Receiver", "ns");
      }
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        TH1F *h_t1out=(TH1F*)f_receiver->Get(("h_t1out_"+name_s+"_"+itoa(i_pin)).c_str());
        TH1F *h_t2out=(TH1F*)f_receiver->Get(("h_t2out_"+name_s+"_"+itoa(i_pin)).c_str());
        TH1F *h_nStubs=(TH1F*)f_receiver->Get(("h_nStubs_"+name_s+"_"+itoa(i_pin)).c_str());
        makeCanvas(h_t1out, h_t2out, "out", percentile, name_s+"_out_Pin"+itoa(i_pin), "Receiver", "ns");
        makeCanvas(h_nStubs, percentile, name_s+"_nStubs_Pin"+itoa(i_pin), "Receiver", "nStubs", 2);
      }
      TH1F *h_nStubs_Total=(TH1F*)f_receiver->Get(("h_nStubs_Total_"+name_s).c_str());
      makeCanvas(h_nStubs_Total, percentile, name_s+"_nStubs_Total", "Receiver", "nStubs", 2);
      f_receiver->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing -- Receiver Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" Receiver name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<" Delay: "<<delayCLK<<" CLK <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Data Content </h2>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> Total nStubs </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs_Total.png")<<"'/>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> nStubs on the 8 output lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Timing </h2>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> Input time for 40 input lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<40; ++i_pin)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_in_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> Output time for 8 output lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_out_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
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
        TH1F *h_nStubs=(TH1F*)f_bxSplitter->Get(("h_nStubs_"+name_s+"_"+itoa(i_BX)).c_str());
        makeCanvas(h_t1in, h_t2in, "in", percentile, name_s+"_in_Pin"+itoa(i_BX), "BXSplitter", "ns");
        makeCanvas(h_t1out, h_t2out, "out", percentile, name_s+"_out_BX"+itoa(i_BX), "BXSplitter", "ns", 10);
        makeCanvas(h_nStubs, percentile, name_s+"_nStubs_BX"+itoa(i_BX), "BXSplitter", "nStubs", 2);
      }
      f_bxSplitter->Close();
      
      std::ofstream outfile((name_s+".html").c_str());
      outfile<<"<html>"<<std::endl;
      outfile<<"<head>"<<std::endl;
      outfile<<" <link rel=\"StyleSheet\" type=\"text/css\" href=\"PRBDashboardStyle.css\" />"<<std::endl;
      outfile<<"</head>"<<std::endl;
      outfile<<"<body>"<<std::endl;
      outfile<<"<h1 align='center'> L1 Track Trigger Timing -- BXSplitter Dashboard </h1>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" BXSplitter name: "<<name_s<<"<br/>"<<std::endl;
      outfile<<" Operating frequency: "<<frequency<<"MHz <br/>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Data Content </p>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> nStubs on the 8 BX output lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs_BX"+itoa(i_BX)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Timing </h2>"<<std::endl;
      outfile<<" <table border='1'"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> Input time for 8 input lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_pin=0; i_pin<8; ++i_pin)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_in_Pin"+itoa(i_pin)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> Output time for 8 BX output lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_BX=0; i_BX<8; ++i_BX)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_out_BX"+itoa(i_BX)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
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
        TH1F *h_nStubs_ByPRB=(TH1F*)f_layerSplitter->Get(("h_nStubs_ByPRB_"+name_s+"_"+itoa(i_PRB)).c_str());
        makeCanvas(h_t1in, h_t2in, "in", percentile, name_s+"_PRB"+itoa(i_PRB), "LayerSplitter", "ns", 10);
        makeCanvas(h_nStubs_ByPRB, percentile, name_s+"_nStubs_PRB"+itoa(i_PRB), "LayerSplitter", "nStubs", 2);
      }
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        TH1F *h_t1out=(TH1F*)f_layerSplitter->Get(("h_t1out_"+name_s+"_"+itoa(i_layer)).c_str());
        TH1F *h_t2out=(TH1F*)f_layerSplitter->Get(("h_t2out_"+name_s+"_"+itoa(i_layer)).c_str());
        TH1F *h_nStubs_ByLayer=(TH1F*)f_layerSplitter->Get(("h_nStubs_ByLayer_"+name_s+"_"+itoa(i_layer)).c_str());
        makeCanvas(h_t1out, h_t2out, "out", percentile, name_s+"_layer"+itoa(i_layer), "LayerSplitter", "ns", 10);
        makeCanvas(h_nStubs_ByLayer, percentile, name_s+"_nStubs_Layer"+itoa(i_layer), "LayerSplitter", "nStubs", 2);
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
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Data Content </p>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> nStubs on the 8 PRB input lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs_PRB"+itoa(i_PRB)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      outfile<<"   <td>"<<std::endl;
      outfile<<"    <p> nStubs on the 6 layer output lines </p>"<<std::endl;
      outfile<<"   </td>"<<std::endl;
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_nStubs_Layer"+itoa(i_layer)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
      outfile<<"<p>"<<std::endl;
      outfile<<" <h2> Timing </h2>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_PRB"+itoa(i_PRB)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<" <table border='1'>"<<std::endl;
      outfile<<"  <tr>"<<std::endl;
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        outfile<<"   <td>"<<std::endl;
        outfile<<"    <img width=\"500em\" src='"<<("c_"+name_s+"_layer"+itoa(i_layer)+".png")<<"'/>"<<std::endl;
        outfile<<"   </td>"<<std::endl;
      }
      outfile<<"  </tr>"<<std::endl;
      outfile<<" </table>"<<std::endl;
      outfile<<"</p>"<<std::endl;
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
      
