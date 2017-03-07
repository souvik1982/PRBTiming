#include <math.h>

std::string itoa(int i) 
{
  char res[10];
  sprintf(res, "%d", i);
  std::string ret(res);
  return ret;
}

double statistics(std::vector<double> *v, double *stddev)
{
  double n=v->size();
  double sum=0, sumsq=0;
  for (unsigned int i=0; i<v->size(); ++i)
  {
    double data=v->at(i);
    sum+=data;
    sumsq+=data*data;
  }
  double mean=sum/n;
  double variance=(sumsq-(sum*sum)/n)/(n-1);
  *stddev=pow(variance, 0.5);
  return mean;
}

void plot_LatencyVsPU(float percentile=0.9)
{
  std::vector<double> v_PU={140, 200, 250};
  std::vector<std::vector<double> > v_v_Parking_t2out_mean(6), v_v_Parking_t2out_stddev(6);
  std::vector<std::vector<double> > v_v_Streaming_t2out_mean(6), v_v_Streaming_t2out_stddev(6);
  std::vector<double> v_Parking_maxMean, v_Parking_maxStddev;
  std::vector<double> v_Streaming_maxMean, v_Streaming_maxStddev;
  
  // j = 0 is parking, 1 is streaming
  for (unsigned int j=0; j<2; ++j)
  {
    std::string directory=j?"StreamingBX/":"ParkingBX/"; // 0 = Parking, 1 = Streaming
    std::cout<<"j = "<<j<<std::endl;
    // Loop over PU scenarios 140, 200, 250
    for (unsigned int i_PU=0; i_PU<3; ++i_PU)
    {
      std::vector<std::vector<double> > v_v_t2out(6); // t2 out for various layers
      for (unsigned int i_PRB=0; i_PRB<8; ++i_PRB)
      {
        std::string filename=directory+"PU"+itoa(v_PU.at(i_PU))+"/layerSplitter1_300"+itoa(i_PRB)+"0.root";
        TFile *f_layerSplitter=new TFile(filename.c_str());
        for (unsigned int i_layer=0; i_layer<6; ++i_layer)
        {
          std::string histname="h_t2out_layerSplitter1_300"+itoa(i_PRB)+"0_"+itoa(i_layer);
          TH1F *h_t2out=(TH1F*)f_layerSplitter->Get(histname.c_str());
          double perc[1]={percentile};
          double value[1];
          h_t2out->GetQuantiles(1, value, perc);
          v_v_t2out.at(i_layer).push_back(value[0]);
          delete h_t2out;
        }
        delete f_layerSplitter;
      }
      
      std::cout<<"PU "<<v_PU.at(i_PU)<<std::endl;
      double maxMean=0, maxStddev=0;
      for (unsigned int i_layer=0; i_layer<6; ++i_layer)
      {
        double mean, stddev;
        mean=statistics(&(v_v_t2out.at(i_layer)), &stddev);
        std::cout<<" Layer = "<<i_layer<<", mean = "<<mean<<", stddev = "<<stddev<<std::endl;
        if (j==0) // Parking
        {
          v_v_Parking_t2out_mean.at(i_layer).push_back(mean);
          v_v_Parking_t2out_stddev.at(i_layer).push_back(stddev);
        }
        else
        {
          v_v_Streaming_t2out_mean.at(i_layer).push_back(mean);
          v_v_Streaming_t2out_stddev.at(i_layer).push_back(stddev);
        }
        if (mean>maxMean) {maxMean=mean; maxStddev=stddev;}
      }
      std::cout<<"maxMean = "<<maxMean<<std::endl;
      std::cout<<"maxStddev = "<<maxStddev<<std::endl;
      if (j==0) // Parking
      {
        v_Parking_maxMean.push_back(maxMean);
        v_Parking_maxStddev.push_back(maxStddev);
      }
      else
      {
        v_Streaming_maxMean.push_back(maxMean);
        v_Streaming_maxStddev.push_back(maxStddev);
      }
    }
  }
  
  std::vector<double> v_zero(3, 0);
  for (unsigned int i_layer=0; i_layer<6; ++i_layer)
  {
    TGraphErrors *g_t2outVsPU_Parking=new TGraphErrors(3, &(v_PU[0]), &(v_v_Parking_t2out_mean[i_layer][0]), &(v_zero[0]), &(v_v_Parking_t2out_stddev[i_layer][0]));
    TGraphErrors *g_t2outVsPU_Streaming=new TGraphErrors(3, &(v_PU[0]), &(v_v_Streaming_t2out_mean[i_layer][0]), &(v_zero[0]), &(v_v_Streaming_t2out_stddev[i_layer][0]));
    
    std::string filename="v_t2outVsPU_layer"+itoa(i_layer)+".png";
    TCanvas *c=new TCanvas("c", "c", 700, 700);
    g_t2outVsPU_Parking->SetTitle(("; <PU>; Layer "+itoa(i_layer)+" <t2out> #Lambda_{0.90} (ns)").c_str());
    g_t2outVsPU_Parking->GetYaxis()->SetTitleOffset(1.4);
    g_t2outVsPU_Parking->SetLineColor(kBlack); g_t2outVsPU_Parking->SetLineWidth(2);
    g_t2outVsPU_Streaming->SetLineColor(kRed); g_t2outVsPU_Streaming->SetLineWidth(2);
    g_t2outVsPU_Parking->SetMinimum(0); g_t2outVsPU_Parking->SetMaximum(1000);
    g_t2outVsPU_Parking->Draw("ALP");
    g_t2outVsPU_Streaming->Draw("LP same");
    TLegend *leg=new TLegend(0.6, 0.2, 0.89, 0.4);
    leg->SetLineColor(0); leg->SetFillColor(0);
    leg->SetHeader("250 MHz CLOCK");
    leg->AddEntry(g_t2outVsPU_Parking, "Parking BX Splitter", "L");
    leg->AddEntry(g_t2outVsPU_Streaming, "Streaming BX Splitter", "L");
    leg->Draw();
    c->SaveAs(filename.c_str());
    delete c;
    delete g_t2outVsPU_Parking;
    delete g_t2outVsPU_Streaming;
    delete leg;
  }
  
  std::cout<<"v_Parking_maxMean[0] = "<<v_Parking_maxMean[0]<<std::endl;
  
  TGraphErrors *g_t2outMaxVsPU_Parking=new TGraphErrors(3, &(v_PU[0]), &(v_Parking_maxMean[0]), &(v_zero[0]), &(v_Parking_maxStddev[0]));
  TGraphErrors *g_t2outMaxVsPU_Streaming=new TGraphErrors(3, &(v_PU[0]), &(v_Streaming_maxMean[0]), &(v_zero[0]), &(v_Streaming_maxStddev[0]));
  TCanvas *c=new TCanvas("c", "c", 700, 700);
  g_t2outMaxVsPU_Parking->SetTitle("; <PU>; Maximum latency #Lambda_{0.90} (ns)");
  g_t2outMaxVsPU_Parking->GetYaxis()->SetTitleOffset(1.4);
  g_t2outMaxVsPU_Parking->SetLineColor(kBlack); g_t2outMaxVsPU_Parking->SetLineWidth(2);
  g_t2outMaxVsPU_Streaming->SetLineColor(kRed); g_t2outMaxVsPU_Streaming->SetLineWidth(2);
  g_t2outMaxVsPU_Parking->SetMinimum(0); g_t2outMaxVsPU_Parking->SetMaximum(1000);
  g_t2outMaxVsPU_Parking->Draw("ALP");
  g_t2outMaxVsPU_Streaming->Draw("LP same");
  TLegend *leg=new TLegend(0.6, 0.2, 0.89, 0.4);
  leg->SetLineColor(0); leg->SetFillColor(0);
  leg->SetHeader("250 MHz CLOCK");
  leg->AddEntry(g_t2outMaxVsPU_Parking, "Parking BX Splitter", "L");
  leg->AddEntry(g_t2outMaxVsPU_Streaming, "Streaming BX Splitter", "L");
  leg->Draw();
  c->SaveAs("MaxLatencyVsPU.png");
  delete c;
  delete leg;
  delete g_t2outMaxVsPU_Parking;
  delete g_t2outMaxVsPU_Streaming;
  
}
