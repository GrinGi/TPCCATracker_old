// **************************************************************************
//                                                                          *
// Primary Author: Igor Kulakov <I.Kulakov@gsi.de>                          *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

// converts txt tracking output in the root-historgrams
// compile to run it. Ex: root -l 'QA.C+("/SSD2/Data/ALICE/TPC/dataAuAu",1)' -q

#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::abs;

#include "TMath.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TCanvas.h"
#include "TStyle.h"
const int textFont = 22; // TNewRoman
const float PI2 = 3.1415926535897f*2;

struct AliHLTTPCCAGBHit {
  float fX; //* X position
  float fY; //* Y position
  float fZ; //* Z position

  float fErrX; //* X position error
  float fErrY; //* Y position error
  float fErrZ; //* Z position error

  float fAmp;   //* Maximal amplitude
  int fISlice; //* slice number
  int fIRow;   //* row number
};

std::istream &operator>>( std::istream &in, AliHLTTPCCAGBHit &a )
{
  float tmp;
  in >> a.fX >> a.fY >> a.fZ
     >> a.fErrX >> a.fErrY >> a.fErrZ
     >> a.fAmp >> a.fISlice >> a.fIRow >> tmp >> tmp;
  return in;
}

struct AliHLTTPCCATrackParam {

  float Pt() const  { return abs(1.f/fP[4]); };
  float Phi() const { return asin(fP[2]);    }; // local phi
  float Lambda() const { return atan(fP[3]); };
  float ErrY() const { return sqrt(fC[0]); };
  float ErrZ() const { return sqrt(fC[2]); };
  float ErrSinPhi() const { return sqrt(fC[5]); };
  float ErrDzDs() const { return sqrt(fC[9]); };
  float ErrQPt() const { return sqrt(fC[14]); };
  
  float fX;      // x position
  float fSignCosPhi; // sign of cosPhi   // phi = arctg (Dy/Dx)
  float fP[5];   // 'active' track parameters: Y, Z, SinPhi = dy/ds, Dz/Ds (ds = sqrt( dx^2 + dy^2 )), q/Pt
  float fC[15];  // the covariance matrix for Y,Z,SinPhi,..
  float fChi2;   // the chi^2 value
  int   fNDF;    // the Number of Degrees of Freedom
};

std::istream &operator>>( std::istream &in, AliHLTTPCCATrackParam &t )
{
  in >> t.fX;
  in >> t.fSignCosPhi;
  in >> t.fChi2;
  in >> t.fNDF;
  for ( int i = 0; i < 5; i++ ) in >> t.fP[i];
  for ( int i = 0; i < 15; i++ ) in >> t.fC[i];
  return in;
}

struct AliHLTTPCCAGBTrack {
  vector <int> fHits;
  AliHLTTPCCATrackParam fParam; // fitted track parameters
  float fAlpha;             //* Alpha angle of the parametrerisation
  int   fNDeDx;
  float fDeDx;              //* DE/DX
};

std::istream &operator>>( std::istream &in, AliHLTTPCCAGBTrack &t ) {
  int nHits;
  in >> nHits;
  t.fHits.resize(nHits);
  for ( int ih = 0; ih < nHits; ih++ ) {
    in >> t.fHits[ih];
  }
  in >> t.fAlpha;
  in >> t.fNDeDx;
  in >> t.fDeDx;
  in >> t.fParam;
  return in;
}

struct AliHLTTPCCAEvent {
  AliHLTTPCCAEvent():fNHits(-1) {};
  int fNHits;
  vector<AliHLTTPCCAGBTrack> fTracks;
  vector<AliHLTTPCCAGBHit> fHits;
};

std::istream &operator>>( std::istream &in, AliHLTTPCCAEvent &e ) { // read hits first!
  if ( e.fNHits == -1 ) { // hits
    in >> e.fNHits;
    e.fHits.resize(e.fNHits);
    for ( int i = 0; i < e.fNHits; i++ ) {
      in >> e.fHits[i];
    }
  }
  else { // tracks
    int size;
    in >> size; // nhits
    assert( size == e.fNHits );
    in >> size; // ntracks
    e.fTracks.resize(size);
    for ( int i = 0; i < size; i++ ) {
      in >> e.fTracks[i];
    }
  }
  return in;
}

//////////////////

void makeUpHisto(TH1* hist, TString title)
{
  if (hist){

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
    std::cout << " E: Read hists error! Title: " << title << std::endl;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

void QA( string dir = "", int nEv = 10 ) {
  const int NHistos = 26; // all
  const string Names[NHistos] = { "NHitsInEvent",
                                  
                                  "HitX","HitY","HitZ",
                                  "HitErrX","HitErrY","HitErrZ",
                                  
                                  "NTracksInEvent","AttachedHitsRatioInEvent","NHitsInTrack",
                                  "Pt","Phi","Lambda","prob","q",
                                  
                                  "Y","Z","SinPhi","DzDs","QPt",
                                  "ErrY","ErrZ","ErrSinPhi","ErrDzDs","ErrQPt",
                                  
                                  "NHitsOnSectorRow"};
  TH1 *histos[NHistos];

  int i = -1;
  histos[++i] = new TH1I(Names[i].data(), Names[i].data(), 100, 0, 250000 );
  const int IFirstHPull = i+1;
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 50, 250 ); // x
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -50, 50 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -250, 250 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, .1 ); // err x
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 1 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 2.8 );
  const int NHPulls = i+1 - IFirstHPull;
  histos[++i] = new TH1I(Names[i].data(), Names[i].data(), 100, 0, 5000 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 1 );
  histos[++i] = new TH1I(Names[i].data(), Names[i].data(), 101, 0, 100 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 10 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, PI2 ); // phi
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -3, 3 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 1 );
  histos[++i] = new TH1I(Names[i].data(), Names[i].data(), 5, -2, 2 ); // q
  const int IFirstTPull = i+1;
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -50, 50 ); // y
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -250, 250 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -1, 1 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -5, 5 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, -10, 10 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 1 ); // err y
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 2.8 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, .12 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, .08 );
  histos[++i] = new TH1F(Names[i].data(), Names[i].data(), 100, 0, 1.2 );
  const int NTPulls = i+1 - IFirstTPull;
  const int IFirst2DHisto = i+1;
  histos[++i] = new TH2I(Names[i].data(), Names[i].data(), 24, 0, 24, 45, 0, 45 ); // n hits on sector row
  const int N2DHistos = i+1 - IFirst2DHisto;


  for (int i = 0; i < NHistos; i++){
    makeUpHisto( histos[i], Names[i].data() );
  }
  for (int i = NHistos-N2DHistos; i < NHistos; i++){
    histos[i]->SetOption("COLZ");
  }
  
  for ( int kEvents = 0; kEvents < nEv; kEvents++ ) {
      // read data
    AliHLTTPCCAEvent ev;
      
    char buf[6];
    sprintf( buf, "%d", kEvents );
    const string filePrefix = dir + "/event" + string(buf) + "_";

    const string hitFileName = filePrefix+"hits.data";
    cout << "Read data from " << hitFileName << endl;
    ifstream hIn(hitFileName.data());
    hIn >> ev;
    
    const string trFileName = filePrefix+"tracks.data";
    cout << "Read data from " << trFileName << endl;
    ifstream tIn(trFileName.data());
    tIn >> ev;
    
      // ----- analyse data -----

      // -- fill 1D --
    int ii = -1;
      // NHitsInEvent
    histos[++ii]->Fill( ev.fNHits );
          // Hits pulls
    for ( unsigned int iH = 0; iH < ev.fHits.size(); iH++ ) {
      const AliHLTTPCCAGBHit& h = ev.fHits[iH];
      int i = ii;
      histos[++i]->Fill( h.fX );
      histos[++i]->Fill( h.fY );
      histos[++i]->Fill( h.fZ );
      histos[++i]->Fill( h.fErrX );
      histos[++i]->Fill( h.fErrY );
      histos[++i]->Fill( h.fErrZ );
    }
    ii += 6;
      // NTracksInEvent
    histos[++ii]->Fill( ev.fTracks.size() );
      // NAttachedHitsRatioInEvent
    int nAttachedHits = 0;
    for ( unsigned int iT = 0; iT < ev.fTracks.size(); iT++ ) {
      const AliHLTTPCCAGBTrack& t = ev.fTracks[iT];
      nAttachedHits += t.fHits.size();
    }
    histos[++ii]->Fill( double(nAttachedHits)/ev.fNHits );
    
    for ( unsigned int iT = 0; iT < ev.fTracks.size(); iT++ ) {
      const AliHLTTPCCAGBTrack& t = ev.fTracks[iT];

      int i = ii;
        // NHitsInTrack
      histos[++i]->Fill( t.fHits.size() );
        // pt
      histos[++i]->Fill( t.fParam.Pt() );
        // phi
      float phi = t.fParam.Phi() + t.fAlpha; // CHECKME i'm not sure how to get global angle
      histos[++i]->Fill( phi-floor(phi/PI2)*PI2 );
        // lambda
      histos[++i]->Fill( t.fParam.Lambda() );
        // prob
      histos[++i]->Fill( TMath::Prob( t.fParam.fChi2, t.fParam.fNDF ) );
        // q
      histos[++i]->Fill( (t.fParam.fP[4] > 0) ? 1 : -1 );
        // params
      for( int k = 0; k < 5; k++ )
        histos[++i]->Fill( t.fParam.fP[k] );
        // errors
      histos[++i]->Fill( t.fParam.ErrY() );
      histos[++i]->Fill( t.fParam.ErrZ() );
      histos[++i]->Fill( t.fParam.ErrSinPhi() );
      histos[++i]->Fill( t.fParam.ErrDzDs() );
      histos[++i]->Fill( t.fParam.ErrQPt() );
    }
      // -- fill 2D --
      // NHitsOnSectorRow
    ii = NHistos - N2DHistos;
    for ( unsigned int iH = 0; iH < ev.fHits.size(); iH++ ) {
      const AliHLTTPCCAGBHit& h = ev.fHits[iH];
      histos[ii]->Fill( h.fISlice, h.fIRow );
    }

  }

    // ====================== make pictures ======================
 
  TStyle *histoStyle = new TStyle("histoStyle","Plain Style(no colors/fill areas)");
  
  histoStyle->SetTextFont(textFont);
  histoStyle->SetPadColor(0);
  histoStyle->SetCanvasColor(0);
  histoStyle->SetTitleColor(0);
  histoStyle->SetStatColor(0);
  
  histoStyle->SetOptTitle(0); // without main up title
  histoStyle->SetOptStat(1000111110);
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

  histoStyle->cd();

  
  TCanvas *c0 = new TCanvas("c0","c0",0,0,1200,800);
  c0->UseCurrentStyle();
  TCanvas *c1 = new TCanvas("c1","c1",0,0,1200,800);
  c1->Divide(NTPulls/2,2);
  c1->UseCurrentStyle();
  TCanvas *c2 = new TCanvas("c2","c2",0,0,1200,800);
  c2->Divide(NHPulls/2,2);
  c2->UseCurrentStyle();
  
  for (int i = 0; i < NHistos; i++){
    if ( i == IFirst2DHisto ) 
      histoStyle->SetOptStat(1000000010);
    else
      histoStyle->SetOptStat(1000111110);
    if ( i >= IFirstTPull && i < IFirstTPull + NTPulls ) {
      c1->cd( i - IFirstTPull + 1 );
      histos[i]->Draw();
    }
    else if ( i >= IFirstHPull && i < IFirstHPull + NHPulls ) {
      c2->cd( i - IFirstHPull + 1 );
      histos[i]->Draw();
    }
    else {
      c0->cd();
      histos[i]->Draw();
      c0->SaveAs( (Names[i] + ".pdf").data() );
    }
  }
  c1->SaveAs( "TrackPulls.pdf" );
  c2->SaveAs( "HitPulls.pdf" );
}
