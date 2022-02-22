/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2007-2014 Igor Kulakov
 *
 * TPCCATracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TPCCATracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

void Eff() {

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
plain->SetOptStat(0);
plain->SetOptFit(0011);
plain->SetStatW(0.225);
plain->SetStatH(0.06);
//plain->SetStatX(0.7);
plain->cd();


  TString name = "HLTTPCCATrackerPerformance.root";

  TFile *f = new TFile(name.Data(),"read");
  TDirectory *dir = f->GetDirectory("HLTTPCCATrackerPerformance.root:/HLTTPCCATrackerPerformance");

  const int NPlots = 2;
  
  TProfile *hPlots[NPlots];

  string sPlots[NPlots];
  
  sPlots[0] = "EffVsP";
  sPlots[1] = "EffVsNHits";

  string title[NPlots];
  title[0] = "Efficiency vs Momentum";
  title[1] = "Efficiency vs Number of Hits";

  string titleX[NPlots];
  titleX[0] = "Momentum [GeV/c]";
  titleX[1] = "Number of Hits";

  string titleY[NPlots];
  titleY[0] = "Efficiency";
  titleY[1] = "Efficiency";


  for(int i=0; i<NPlots; i++)
  {
    hPlots[i] = (TProfile*) dir->Get(sPlots[i].data());
  }

  TCanvas *c1;
  c1 = new TCanvas("c1","Efficiency",0,0,1200,800);
  c1 -> Divide(NPlots);

  c1->UseCurrentStyle();

  int NPad=0;
  string AxisName;
  for(int i=0; i<NPlots; i++)
  {
    NPad++;
    c1->cd(NPad);
    hPlots[i]->SetTitle(title[i].data());
    hPlots[i]->SetLineColor(2);
    hPlots[i]->SetMarkerColor(2);

    hPlots[i]->GetXaxis()->SetTitle(titleX[i].data());
    hPlots[i]->GetYaxis()->SetTitle(titleY[i].data());
    hPlots[i]->Draw();
  }

  c1->SaveAs("TrackEfficiency.pdf");
 
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
