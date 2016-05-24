void plot_LatencyVsPU()
{
  std::vector<double> v_PU={140, 200, 250};
  std::vector<double> v_PRB0_t2out={710, 810, 860};
  
  TGraph *g_LatencyVsPU=new TGraph(3, &(v_PU[0]), &(v_PRB0_t2out[0]));
  g_LatencyVsPU->SetTitle("; <PU>; PRB 0 t2out #Lambda_{0.90} (ns)");
  g_LatencyVsPU->GetYaxis()->SetTitleOffset(1.4);
  TCanvas *c_LatencyVsPU=new TCanvas("c_LatencyVsPU", "c_LatencyVsPU", 700, 700);
  g_LatencyVsPU->Draw("AL*");
  c_LatencyVsPU->SaveAs("c_LatencyVsPU.png");
}
