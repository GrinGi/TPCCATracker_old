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

#include <unistd.h> // for dir navigation

#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TProfile.h"
 #include "TStyle.h"
 #include "TFile.h"
 #include "TCanvas.h"
// #include "TF1.h"
// #include "TH2.h"
// #include "TH1.h"
// #include "TF1.h"
// #include "TH2.h"


const int textFont = 22; // TNewRoman
const bool divide = 0; // = 0 - each histo in separate file\screen. = 1 - all in one file\screen


void FitHistoGaus(TH1* hist, float* sigma = 0)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","gaus");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
    if (sigma) *sigma = fit->GetParameter(2);
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void FitHistoLine(TH1* hist)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","[0]");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void makeUpHisto(TH1* hist, TString title)
{
  if (hist && (hist->GetEntries() != 0)){

    hist->GetXaxis()->SetLabelFont(textFont);
    hist->GetXaxis()->SetTitleFont(textFont);
    hist->GetYaxis()->SetLabelFont(textFont);
    hist->GetYaxis()->SetTitleFont(textFont);
    
    hist->GetXaxis()->SetTitle(title);
    hist->GetXaxis()->SetTitleOffset(1);
    hist->GetYaxis()->SetTitle("Entries");
    hist->GetYaxis()->SetTitleOffset(1.05); // good then entries per bit <= 9999
  }
  else{
    std::cout << " E: Read hists error! Title:" << title << std::endl;
  }
}


void makeUpHisto(TH2* hist, TString title)
{
  if (hist && (hist->GetEntries() != 0)){
    // hist->GetXaxis()->SetLabelFont(textFont);
    // hist->GetXaxis()->SetTitleFont(textFont);
    // hist->GetYaxis()->SetLabelFont(textFont);
    // hist->GetYaxis()->SetTitleFont(textFont);
    
    // hist->GetXaxis()->SetTitle(title);
    // hist->GetXaxis()->SetTitleOffset(1);
    // hist->GetYaxis()->SetTitle("Entries");
    // hist->GetYaxis()->SetTitleOffset(1.05); // good then entries per bit <= 9999

    hist->SetOption("CONTZ");
  }
  else{
    std::cout << " E: Read hists error! Title:" << title << std::endl;
  }
}

void makeUpProfile(TProfile* prof, TString titleX, TString titleY)
{
  if (prof && (prof->GetEntries() != 0)){
    prof->SetMarkerColor(kRed);
    prof->SetLineColor(kRed);
    
    prof->GetXaxis()->SetTitle(titleX);
    prof->GetXaxis()->SetTitleOffset(1);
    prof->GetYaxis()->SetTitle(titleY);
    prof->GetYaxis()->SetTitleOffset(0.85);
      
    prof->GetXaxis()->SetLabelFont(textFont);
    prof->GetXaxis()->SetTitleFont(textFont);
    prof->GetYaxis()->SetLabelFont(textFont);
    prof->GetYaxis()->SetTitleFont(textFont);
  }
  else{
    std::cout << " E: Read profile error! TitleX:" << titleX << " TitleY:" << titleY << std::endl;
  }
}

void DrawHistos(TString suffix = "") { //  @suffix - addition to histo

    // ============================ Set Styles ============================
  TStyle *histoStyle = new TStyle("histoStyle","Plain Style(no colors/fill areas)");
//   TStyle *histoStyle = gStyle;
  
  histoStyle->SetTextFont(textFont);
  histoStyle->SetPadColor(0);
  histoStyle->SetCanvasColor(0);
  histoStyle->SetTitleColor(0);
  histoStyle->SetStatColor(0);
  
  histoStyle->SetOptTitle(0); // without main up title
  histoStyle->SetOptStat(1000111010);
//   The parameter mode can be = ksiourmen  (default = 000001111)
//   k = 1;  kurtosis printed
//   k = 2;  kurtosis and kurtosis error printed
//   s = 1;  skewness printed
//   s = 2;  skewness and skewness error printed
//   i = 1;  integral of bins printed
//   o = 1;  number of overflows printed
//   u = 1;  number of underflows printed
//   r = 1;  rms printed
//   r = 2;  rms and rms error printed
//   m = 1;  mean value printed
//   m = 2;  mean and mean error values printed
//   e = 1;  number of entries printed
//   n = 1;  name of histogram is printed
  
  histoStyle->SetOptFit(10001);
//   The parameter mode can be = pcev  (default = 0111)
//   p = 1;  print Probability
//   c = 1;  print Chisquare/Number of degress of freedom
//   e = 1;  print errors (if e=1, v must be 1)
//   v = 1;  print name/values of parameters
  
  histoStyle->SetStatW(0.175);
  histoStyle->SetStatH(0.02);
  histoStyle->SetStatX(0.95);
  histoStyle->SetStatY(0.97);
  histoStyle->SetStatFontSize(0.05);

  histoStyle->SetStatFont(textFont);
  histoStyle->SetPadBorderMode(0);
  histoStyle->SetFrameBorderMode(0);

  histoStyle->SetPalette(1); // for 2D
  
  TStyle *profStyle = new TStyle("profStyle","Plain Style(no colors/fill areas)");
//   TStyle *profStyle = gStyle;
  
  profStyle->SetTextFont(textFont);
  profStyle->SetPadColor(0);
  profStyle->SetCanvasColor(0);
  profStyle->SetTitleColor(0);
  profStyle->SetStatColor(0);
  
  profStyle->SetOptTitle(0); // without main up title
  profStyle->SetOptStat(0);
  profStyle->SetOptFit(0);
  profStyle->SetGridStyle(2);

  profStyle->SetPadBorderMode(0);
  profStyle->SetFrameBorderMode(0);

    // ============================ Open file ============================

  const TString fileName = "HLTTPCCATrackerPerformance.root";

  TFile *fileIn = new TFile(fileName.Data(),"read");

    // ------------ make histos -----------

      // ------------ tracks -----------
  
  TString dirFitName = "/Sector Performance/TrackFit";
  dirFitName = fileName + ":" + dirFitName;
  TDirectory *dirFit = reinterpret_cast<TDirectory *>(fileIn->Get(dirFitName));
  
  const int nHisto = 5;
  
  struct THistoData
  {
    char *name;  // one, which used in input file
    char *title; // for X-axis
  };
  const THistoData histoData[nHisto] =
  {
    // {"pullQPt",  "Pull q/pt"   },
    {"resXHit",  "Hit Resolution X [cm]"   },
    {"resYHit",  "Hit Resolution Y [cm]"   },
    {"resZHit",  "Hit Resolution Z [cm]"   },
    {"pullYHit",  "Hit Pull Y"   },
    {"pullZHit",  "Hit Pull Z"   }
  };
  
  TH1F *histo[nHisto];

  float charat[nHisto][2]; // 0 - sigma, 1 - RMS
  for (int i = 0; i < nHisto; i++){
    histo[i] = (TH1F*) dirFit->Get(histoData[i].name);
    makeUpHisto(histo[i], histoData[i].title);
    FitHistoGaus(histo[i], &(charat[i][0]));
    charat[i][1] = histo[i]->GetRMS();
  }

  cout.setf(std::ios::scientific,std::ios::floatfield);
  cout.precision(2);
  cout << endl << "Name   " << "   \t" << " Sigma   " << "\t" << " RMS " << endl;
  for (int i = 0; i < nHisto; i++){
    cout << histoData[i].name << "   \t" << charat[i][0] << "\t" << charat[i][1] << endl;
  }
      // ------------ hits, strips -----------
  
  // TString dirInputName = "/L1/Input";
  // dirInputName = fileName + ":" + dirInputName;
  // TDirectory *dirInput = reinterpret_cast<TDirectory *>(fileIn->Get(dirInputName));
  
  // const int nHistoInput = 0;
  
  // const THistoData histoDataInput[nHistoInput] =
  // {
  //   {"Px",  "Pull x" },
  //   {"Py",  "Pull y" },
  //   {"x",  "Residual (x^{hit}-x^{mc}) [#mum]" },
  //   {"y",  "Residual (y^{hit}-y^{mc}) [#mum]" }
  // };
  
  // TH1F *histoInput[nHistoInput];

  // for (int i = 0; i < nHistoInput; i++){
  //   histoInput[i] = (TH1F*) dirInput->Get(histoDataInput[i].name);
  //   makeUpHisto(histoInput[i],histoDataInput[i].title);
  //   FitHistoGaus(histoInput[i]);
  // }
        // ------------ other histo -----------


  const int nHistoOtherReco = 2;
  const int nHistoOtherGhost = 0;
  const int nHistoOtherAll = 2;
  const int nHistoOther = nHistoOtherReco + nHistoOtherGhost + nHistoOtherAll;
  const THistoData histoDataOther[nHistoOther] =
  {
      //reco
    {"recosLength",  "Reco Track Length" },
    {"nHitsRecoTOverNHitsMCT", "NHits^{reco} / NHits^{mc}" },
      // ghost
    
      // all
    {"mcTrackNRecoHits",  "N Reco Hits on MCTrack" },
    {"purity",  "Track Purity" }
  };
  
  TH1F *histoOther[nHistoOther];
    //reco
  {
    TString dirOtherName = "/Global Performance/RecoTracks";
    dirOtherName = fileName + ":" + dirOtherName;
    TDirectory *dirOther = reinterpret_cast<TDirectory *>(fileIn->Get(dirOtherName));

    for (int i = 0; i < nHistoOtherReco; i++){
      histoOther[i] = (TH1F*) dirOther->Get(histoDataOther[i].name);
      makeUpHisto(histoOther[i], histoDataOther[i].title);
        // FitHistoLine(histoOther[i]);
    }
  }
    // ghost
  {
    TString dirOtherName = "/Global Performance/Ghost";
    dirOtherName = fileName + ":" + dirOtherName;
    TDirectory *dirOther = reinterpret_cast<TDirectory *>(fileIn->Get(dirOtherName));

    for (int i = nHistoOtherReco; i < nHistoOtherReco + nHistoOtherGhost; i++){
      histoOther[i] = (TH1F*) dirOther->Get(histoDataOther[i].name);
      makeUpHisto(histoOther[i], histoDataOther[i].title);
        // FitHistoLine(histoOther[i]);
    }
  }
    // all
  {
    TString dirOtherName = "/Global Performance/AllTracks";
    dirOtherName = fileName + ":" + dirOtherName;
    TDirectory *dirOther = reinterpret_cast<TDirectory *>(fileIn->Get(dirOtherName));

    for (int i = nHistoOtherReco + nHistoOtherGhost; i < nHistoOther; i++){
      histoOther[i] = (TH1F*) dirOther->Get(histoDataOther[i].name);
      makeUpHisto(histoOther[i], histoDataOther[i].title);
        // FitHistoLine(histoOther[i]);
    }
  }
  
          // ------------ other 2D histo -----------
  
  TString dirOther2DName = "/Global Performance/AllTracks";
  dirOther2DName = fileName + ":" + dirOther2DName;
  TDirectory *dirOther2D = reinterpret_cast<TDirectory *>(fileIn->Get(dirOther2DName));
  
  const int nHistoOther2D = 1;
  
  const THistoData histoDataOther2D[nHistoOther2D] =
  {
    // {"resXHitVsZ",  "resXHitVsZ" }
    {"nMCPointsVsMCMom",  "NMCPoints vs Momentum" }
  };
  
  TH2F *histoOther2D[nHistoOther2D];

  for (int i = 0; i < nHistoOther2D; i++){
    histoOther2D[i] = (TH2F*) dirOther2D->Get(histoDataOther2D[i].name);
    makeUpHisto(histoOther2D[i], histoDataOther2D[i].title);
  }
    // ------------ make profiles -----------

  const int nProfSectorReco = 0;
  const int nProfGlobalReco = 2;
  const int nProfGlobalAll = 4;
  const int nProf = nProfSectorReco + nProfGlobalReco + nProfGlobalAll;
  struct TProfData
  {
    char *name;  // one, which used in input file
    char *titleX; // for X-axis
    char *titleY; // for Y-axis
  };
  const TProfData profData[nProf] =
  {

      // global reco
    {"recosEffVsMCMom",  "Momentum [GeV/c]",  "Efficiency [%]"  },
    {"recosEffVsNMCTracks",  "NMCTracks",  "Efficiency [%]"  },
      // global all
    {"nHitsOverNMCPointsVsRow",  "Row", "NHits / NMCPoints"  },
    {"nHitsOverNMCPointsVsMCMom",  "MCTrack Momentum", "NHits / NMCPoints"  },
    {"nHitsOverNMCPointsVsMCDzDs",  "MCTrack DzDs", "NHits / NMCPoints"  },
    {"nHitsOverNMCPointsVsNMCTracks",  "NMCTracks",  "NHits / NMCPoints"  }
    
  };
    
  TProfile *profile[nProf];
  int i = 0;
    // sector
  {
    TString dirName = "/Sector Performance/AllTracks";
    dirName = fileName + ":" + dirName;
    TDirectory *dir = reinterpret_cast<TDirectory *>(fileIn->Get(dirName));

    for (int ip = 0; ip < nProfSectorReco; i++, ip++){
      profile[i] = (TProfile*) dir->Get(profData[i].name);
      makeUpProfile(profile[i], profData[i].titleX, profData[i].titleY);
    }
  }
    // global reco
  {
    TString dirName = "/Global Performance/RecoTracks";
    dirName = fileName + ":" + dirName;
    TDirectory *dir = reinterpret_cast<TDirectory *>(fileIn->Get(dirName));

    for (int ip = 0; ip < nProfGlobalReco; i++, ip++){
      profile[i] = (TProfile*) dir->Get(profData[i].name);
      makeUpProfile(profile[i], profData[i].titleX, profData[i].titleY);
    }
  }
      // global all
  {
    TString dirName = "/Global Performance/AllTracks";
    dirName = fileName + ":" + dirName;
    TDirectory *dir = reinterpret_cast<TDirectory *>(fileIn->Get(dirName));

    for (int ip = 0; ip < nProfGlobalAll; i++, ip++){
      profile[i] = (TProfile*) dir->Get(profData[i].name);
      makeUpProfile(profile[i], profData[i].titleX, profData[i].titleY);
    }
  }
    
    // ============================ Draw  ============================
//   TCanvas *c1;
//   c1 = new TCanvas("c1","c1",0,0,1200,800);
//   c1 -> Divide(nHisto);
// 
//   c1->UseCurrentStyle();
// 
//   for (int i = 0; i < nHisto; i++){
//     c1->cd(i+1);
//     histo[i]->Draw();
//   }

//   c1->SaveAs("histo.png");

  
  system("mkdir Histos"+suffix+" -p");
  chdir( "Histos"+suffix );
  const int nParts = 2;
  int iPart = 1;
  histoStyle->cd();
  TCanvas *c2;
  if (!divide) c2 = new TCanvas("c2","c2",0,0,900,600);
          else c2 = new TCanvas("c2","c2",0,0,900*nParts,600*nParts);
  if (divide) c2->Divide(nParts, nParts);
  
  histoStyle->cd();
  c2->cd();
  for (int i = 0; i < nHisto; i++){
    if (divide) c2->cd(iPart++);
    histo[i]->Draw();
    TString name = TString(histoData[i].name)+suffix+".pdf";
    if (!divide) c2->SaveAs(name);
  }

  // for (int i = 0; i < nHistoInput; i++){
  //   if (divide) c2->cd(iPart++);
  //   histoInput[i]->Draw();
  //   TString name = TString(histoDataInput[i].name)+suffix+".pdf";
  //   if (!divide) c2->SaveAs(name);
  // }

  for (int i = 0; i < nHistoOther; i++){
    if (divide) c2->cd(iPart++);
    histoOther[i]->Draw();
    TString name = TString(histoDataOther[i].name)+suffix+".pdf";
    if (!divide) c2->SaveAs(name);
  }

  histoStyle->SetOptStat(1000000010);
  for (int i = 0; i < nHistoOther2D; i++){
    if (divide) c2->cd(iPart++);
    histoOther2D[i]->Draw();
    TString name = TString(histoDataOther2D[i].name)+suffix+".pdf";
    if (!divide) c2->SaveAs(name);
  }
  
  profStyle->cd();
  for (int i = 0; i < nProf; i++){
    if (divide) c2->cd(iPart++);
    c2->SetGridx();
    c2->SetGridy();
    profile[i]->Draw();
    TString name = TString(profData[i].name)+suffix+".pdf";
    if (!divide) c2->SaveAs(name);
  }

  if (divide) c2->SaveAs("Histos.pdf");
  chdir( ".." );
}
