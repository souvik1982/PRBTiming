void plot_LatencyVsPU()
{
  std::vector<double> v_PU={140, 200, 250};
  std::vector<double> v_PRB0_t2out={710, 810, 860};
  std::vector<double> v_PRB0_t2out_new={480, 580, 630};
  
  TGraph *g_LatencyVsPU=new TGraph(3, &(v_PU[0]), &(v_PRB0_t2out[0]));
  TGraph *g_LatencyVsPU_new=new TGraph(3, &(v_PU[0]), &(v_PRB0_t2out_new[0]));
  g_LatencyVsPU->SetTitle("; <PU>; PRB 0 t2out #Lambda_{0.90} (ns)");
  g_LatencyVsPU->GetYaxis()->SetTitleOffset(1.4);
  g_LatencyVsPU->SetLineColor(kBlack); g_LatencyVsPU->SetLineWidth(2);
  g_LatencyVsPU_new->SetLineColor(kRed); g_LatencyVsPU_new->SetLineWidth(2);
  g_LatencyVsPU->SetMinimum(0); g_LatencyVsPU->SetMaximum(1000);
  TLegend *leg=new TLegend(0.6, 0.2, 0.89, 0.4);
  leg->SetLineColor(0); leg->SetFillColor(0);
  leg->SetHeader("250 MHz CLOCK", "C");
  leg->AddEntry(g_LatencyVsPU, "Waiting BX Splitter", "L");
  leg->AddEntry(g_LatencyVsPU_new, "Streaming BX Splitter", "L");
  TCanvas *c_LatencyVsPU=new TCanvas("c_LatencyVsPU", "c_LatencyVsPU", 700, 700);
  g_LatencyVsPU->Draw("AL*");
  g_LatencyVsPU_new->Draw("L* same");
  leg->Draw();
  c_LatencyVsPU->SaveAs("c_LatencyVsPU_Waiting_vs_Streaming.png");
}
