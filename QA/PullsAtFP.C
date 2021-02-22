void PullsAtFP() {

  TStopwatch timer;
  timer.Start();

//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//  basiclibs();

  TStyle *plain = new TStyle("Plain","Plain Style(no colors/fill areas)");
  //plain->SetCanvasBorderMode(0);
  //plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
    // The parameter mode can be = ksiourmen  (default = 000001111)
      // k = 1;  kurtosis printed
      // k = 2;  kurtosis and kurtosis error printed
      // s = 1;  skewness printed
      // s = 2;  skewness and skewness error printed
      // i = 1;  integral of bins printed
      // o = 1;  number of overflows printed
      // u = 1;  number of underflows printed
      // r = 1;  rms printed
      // r = 2;  rms and rms error printed
      // m = 1;  mean value printed
      // m = 2;  mean and mean error values printed
      // e = 1;  number of entries printed
      // n = 1;  name of histogram is printed
  plain->SetOptStat(111110);
  plain->SetOptFit(0011);
  plain->SetStatW(0.225);
  plain->SetStatH(0.06);
  //plain->SetStatX(0.7);
  plain->SetOptTitle(0);
  plain->cd();


  TString name = "HLTTPCCATrackerPerformance.root";

  TFile *f = new TFile(name.Data(),"read");
  
  const int NParameters = 6;
  const int NPerf = 2;
  
  string PerfName[NPerf];
  PerfName[0]="Topo Performance";
  PerfName[1]="Global Performance";

  string OutFileName[NPerf];
  OutFileName[0]="GlobalTrackParamQuality.pdf";
  OutFileName[1]="GlobalTrackParamQuality.pdf";
  
  TH1D *hRes[NParameters][NPerf];
  TH1D *hPull[NParameters][NPerf];
  TF1  *fRes[NParameters][NPerf];
  TF1  *fPull[NParameters][NPerf];
  TH1D *hProb[NPerf];
  TH1D *hChi2[NPerf];
   
  string sRes[NParameters][NPerf];
  string sPull[NParameters][NPerf];
  string sResFit[NParameters][NPerf];
  string sPullFit[NParameters][NPerf];
  
  string pullParNames[NParameters] = {"X","Y","Z","Px","Py","Pz"};
  string resParNames[NParameters] = {"X","Y","Z","Px","Py","Pz"};

  string AxisNameRes[NParameters];
  string AxisNamePull[NParameters];

  AxisNameRes[0] = "Residual (x^{reco} - x^{mc}) [cm]";
  AxisNameRes[1] = "Residual (y^{reco} - y^{mc}) [cm]";
  AxisNameRes[2] = "Residual (z^{reco} - z^{mc}) [cm]";
  AxisNameRes[3] = "Residual (p_{x}^{reco} - p_{x}^{mc}) [GeV/c] ";
  AxisNameRes[4] = "Residual (p_{y}^{reco} - p_{y}^{mc}) [GeV/c] ";
  AxisNameRes[5] = "Residual (p_{z}^{reco} - p_{z}^{mc}) [GeV/c] ";

  AxisNamePull[0] = "Pull x";
  AxisNamePull[1] = "Pull y";
  AxisNamePull[2] = "Pull z";
  AxisNamePull[3] = "Pull p_{x}";
  AxisNamePull[4] = "Pull p_{y}";
  AxisNamePull[5] = "Pull p_{z}";
  
  string res = "res";
  string pull = "pull";

  for(int iPerf=0; iPerf<NPerf; iPerf++)
  {
    if(iPerf>0) continue;
    TString DirName = "HLTTPCCATrackerPerformance.root:/";
    DirName += PerfName[iPerf].data();
    DirName += "/TrackInput";
    TDirectory *dir = f->GetDirectory(DirName.Data());

    for(int i=0; i<NParameters; i++)
    {
      sRes[i][iPerf]=res+resParNames[i];
      sPull[i][iPerf]=pull+pullParNames[i];
      sResFit[i][iPerf]=res+resParNames[i]+PerfName[iPerf]+"GaussFit";
      sPullFit[i][iPerf]=pull+pullParNames[i]+PerfName[iPerf]+"GaussFit";
    }

    for(int i=0; i<NParameters; i++)
    {
      hRes[i][iPerf] = (TH1D*) dir->Get(sRes[i][iPerf].data());
      fRes[i][iPerf] = new TF1(sResFit[i][iPerf].data(),"gaus");
      fRes[i][iPerf]->SetLineColor(2);
      hRes[i][iPerf]->Fit(sResFit[i][iPerf].data(),"","",hRes[i][iPerf]->GetXaxis()->GetXmin(),hRes[i][iPerf]->GetXaxis()->GetXmax());
    
      hPull[i][iPerf] = (TH1D*) dir->Get(sPull[i][iPerf].data());
      fPull[i][iPerf] = new TF1(sPullFit[i][iPerf].data(),"gaus");
      fPull[i][iPerf]->SetLineColor(2);
      hPull[i][iPerf]->Fit(sPullFit[i][iPerf].data(),"","",hPull[i][iPerf]->GetXaxis()->GetXmin(),hPull[i][iPerf]->GetXaxis()->GetXmax());
    }
  }

  TCanvas *c;

  for(int iPerf=0; iPerf<NPerf; iPerf++)
  {
    if(iPerf>0) continue;
    c = new TCanvas(PerfName[iPerf].data(),PerfName[iPerf].data(),0,0,400*(NParameters+1),1200);
    c -> Divide(NParameters,2);
    c -> UseCurrentStyle();
    int NPad=0;

    for(int i=0; i<NParameters; i++)
    {
      NPad++;
      c->cd(NPad);
      hRes[i][iPerf]->GetXaxis()->SetTitle(AxisNameRes[i].data());
      hRes[i][iPerf]->GetXaxis()->SetTitleOffset(1);
      hRes[i][iPerf]->Draw();
      fRes[i][iPerf]->Draw("same");
    }
    for(int i=0; i<NParameters; i++)
    {
      NPad++;
      c->cd(NPad);
      hPull[i][iPerf]->GetXaxis()->SetTitle(AxisNamePull[i].data());
      hPull[i][iPerf]->GetXaxis()->SetTitleOffset(1);
      hPull[i][iPerf]->Draw();
      fPull[i][iPerf]->Draw("same");
    }

    c->SaveAs(OutFileName[iPerf].data());
  }
    
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
