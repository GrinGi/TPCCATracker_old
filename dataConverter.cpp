  /// delete hits on rows
  /// to compile: cmake dataConverter
  /// to run:  ./dataConverter NEvents InputDir OutputDir.

#define DEBUG
#define MUTE1
#define MUTE2
 
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <stdio.h> // for atoi
#include <stdlib.h>

using namespace std;


typedef  float fscal; 

#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCPerformanceBase.h"
#include "AliHLTTPCCAParam.h"



char work_dir[100] = "/u/akishina/STT/Data_Panda/1GeV_10tr/";
char out_dir[100] = "/u/akishina/STT/Data_Panda/Valia/";

class TData{
 public:
  void ReadData();
  void WriteData() const;
  
  void Process();

 private:
  bool ReadMCDataFromFiles(string prefix);
  bool ReadHitsFromFile(string prefix);
  bool ReadSettingsFromFile(string prefix);
  bool WriteMCDataToFiles(string prefix) const;
  bool WriteHitsToFile(string prefix) const;
  bool WriteSettingsToFile(string prefix) const;
  
  int NewIRow( int ir ) const {
    if (ir%2 == 1) return -1;
    else           return ir/2; }

  // int NewIRow( int ir ) const {
  //   if (ir%4 != 0) return -1;
  //   else           return ir/4; }

  // int NewIRow( int ir ) const {
  //   if (ir < 13)        return ir;
  //   if ((ir-13)%2 == 1) return -1;
  //   else                return (ir-13)/2+13; }

  vector<AliHLTTPCCAGBHit> vHits;
  vector<AliHLTTPCCALocalMCPoint> vMCPoints;  
  vector<AliHLTTPCCAMCTrack> vMCTracks; 
  vector<AliHLTTPCCAHitLabel> vHitLabel;
  vector<AliHLTTPCCAParam> mSliceParam;
};

void TData::Process()
{
  int id = 0;
  for (unsigned int i = 0; i < vHits.size(); i++) {
    const AliHLTTPCCAGBHit &p = vHits[i];

    const int ir = NewIRow(p.IRow());
    if ( ir < 0 ) continue;
         
    vHits[id] = p;
    vHits[id].SetID( id );
    vHits[id].SetIRow( ir );
    vHitLabel[id] = vHitLabel[i];
    id++;
  }
  vHits.resize(id);
  vHitLabel.resize(id);


  for (unsigned int i = 0; i < vMCTracks.size(); i++) {
    AliHLTTPCCAMCTrack& tr = vMCTracks[i];
    tr.SetFirstMCPointID( 0 );
    tr.SetNMCPoints( 0 );
  }
  id = 0;
  int lastTrI = -1;
  for (unsigned int i = 0; i < vMCPoints.size(); i++) {
    const AliHLTTPCCALocalMCPoint &p = vMCPoints[i];

    const int ir = NewIRow(p.IRow());
    if ( ir < 0 ) continue;
    
    const int trI = p.TrackI();
    AliHLTTPCCAMCTrack& tr = vMCTracks[trI];
    if( trI != lastTrI ) {
      lastTrI = trI;
      tr.SetFirstMCPointID( id );
    }
    tr.SetNMCPoints(tr.NMCPoints()+1);

    vMCPoints[id] = p;
    vMCPoints[id].SetIRow( ir );
    id++;
  }
  vMCPoints.resize(id);


  for (unsigned int iS = 0; iS < mSliceParam.size(); iS++) {
    AliHLTTPCCAParam &p = mSliceParam[iS];

    int nRows = 0;
    int nInnerRows = 0;
    for( int i = 0; i < p.NRows(); i++ ) {
      const int ir = NewIRow(i);
      if ( ir >= 0 ) {
        nRows++;
        if (i < p.NInnerRows()) nInnerRows++;
      }

      p.SetRowX( ir, p.RowX(i) );
    }
    p.SetNRows(nRows);
    p.SetNInnerRows(nInnerRows);
  }
}

void TData::ReadData()
{
  
  vHits.clear();
  vMCPoints.clear();
  vMCTracks.clear();
  vHitLabel.clear();
    
#ifndef MUTE0
  cout <<  " ReadData"<<endl;
#endif
  static int iEvent = 1;

  { 
    char buf[6];
    sprintf( buf, "%d", iEvent-1 );
    string fileName = work_dir;

    if ( iEvent == 1 )
      if (!( ReadSettingsFromFile(fileName) ))
        cout << " Settings can't be read from file " << fileName << endl;

    fileName += string("event") + string(buf) + string("_");
      
    if (!( ReadHitsFromFile(fileName) &&
           ReadMCDataFromFiles(fileName) ))
      cout << " Data can't be read from file " << fileName << endl;

  }

  iEvent++;
}

     

void TData::WriteData() const
    
{
  static int iEvent = 1;
  { 
    char buf[6];
    sprintf( buf, "%d", iEvent-1 );
    string fileName = out_dir;
    
    if ( iEvent == 1 )
      if (!( WriteSettingsToFile(fileName) ))
        cout << " Settings can't be write to file " << fileName << endl;
    
    fileName += string("event") + string(buf) + string("_");
    
    if (!( WriteHitsToFile(fileName) &&
           WriteMCDataToFiles(fileName)  ))
      cout << " Data can't be write to file " << fileName << endl;

  }
  iEvent++;
}

  
int main(int argc, char *argv[])
{
  int NEvents = 1;

  if (argc >= 2){
    NEvents = atoi(argv[1]);
  }

  if (argc >= 3){
    strcpy(work_dir,argv[2]);
  }

  if (argc >= 4){
    strcpy(out_dir,argv[3]);
  }
  std::cout << " Create " << NEvents << " events." << std::endl;
  std::cout << " Input  Dir = " << work_dir << std::endl;
  std::cout << " Output Dir = " << out_dir << std::endl;

  for (int igEv = 0; igEv < NEvents; igEv++ ){
    cout << " Event " << igEv << endl;
    TData data;
    
    data.ReadData();

    data.Process();
    
      // std::sort( data_out.vHits.begin(), data_out.vHits.end(), AliHLTTPCCAGBHit::Compare );
      // vector<AliHLTTPCPerformanceBase::AliHLTTPCCAHitLabel> vHitLabelSorted(data_out.vHitLabel.size());
      // for(int iH=0; iH<data_out.vHits.size(); iH++){
      //   int currentHitID = data_out.vHits[iH].ID();
      //   vHitLabelSorted[iH] = data_out.vHitLabel[currentHitID];
      //   data_out.vHits[iH].SetID(iH);
      // }
      // data_out.vHitLabel = vHitLabelSorted;

    data.WriteData();
    
  } // igEv
  
  return 1;
}


   

bool TData::ReadHitsFromFile(string prefix)
{
  static ifstream ifile;
  ifile.open((prefix+"hits.data").data());
  if ( !ifile.is_open() ) return 0;
  int Size;
	
  ifile >> Size;
  cout<<Size<<"SizeHitsRead"<<endl;
  vHits.resize(Size);   
  for (int i = 0; i < Size; i++){
    AliHLTTPCCAGBHit &l = vHits[i];
    ifile >> l;
	
  }
  ifile.close();
  return 1;
}

bool TData::ReadSettingsFromFile(string prefix)
{
  ifstream in((prefix+"settings.data").data());
  if ( !in.is_open() ) return 0;
  int nSlices = 0;
  in >> nSlices;
  cout<<"SliceParamRead"<<endl;
  mSliceParam.resize( nSlices );
  for ( int iSlice = 0; iSlice < nSlices; iSlice++ ) {
    in >> mSliceParam[iSlice];
  }
  return 1;
}

bool TData::ReadMCDataFromFiles(string prefix)
{
  
  {
    ifstream ifile((prefix+"hitLabels.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<Size<<"SizeLabRead"<<endl;
    vHitLabel.resize(Size);
    for (int i = 0; i < Size; i++){
      AliHLTTPCCAHitLabel &l = vHitLabel[i];
      ifile >> l;
    }
    ifile.close();
  }

  {
    ifstream ifile((prefix+"MCTracks.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<Size<<"SizeMCTracksRead"<<endl;
    vMCTracks.resize(Size);
    for (int i = 0; i < Size; i++){
      AliHLTTPCCAMCTrack &l = vMCTracks[i];
      ifile >> l;
    }
    ifile.close();
  }


  {
    ifstream ifile((prefix+"MCPoints.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<Size<<"SizeMCPointRead"<<endl;
    vMCPoints.resize(Size);
    for (int i = 0; i < Size; i++){
      AliHLTTPCCALocalMCPoint &l = vMCPoints[i];
      ifile >> l;
    }
    ifile.close();
  }
  return 1;
}

bool TData::WriteHitsToFile(string prefix) const
{
  ofstream ifile((prefix+"hits.data").data());
  if ( !ifile.is_open() ) return 0;
  int Size = vHits.size();
  ifile << Size<<endl;
  cout<<Size<<"SizeHitsWriteHits"<<endl;

  for (int i = 0; i < Size; i++){
    const AliHLTTPCCAGBHit &l = vHits[i];
    ifile << l;
  }

  ifile.close();
  return 1;
}

bool TData::WriteMCDataToFiles(string prefix) const
{
 

  {
    ofstream ifile((prefix+"hitLabels.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size = vHitLabel.size();
    ifile << Size<<endl;
    cout<<Size<<"SizeLabWrite"<<endl;
   
    for (int i = 0; i < Size; i++){
      const AliHLTTPCCAHitLabel &l = vHitLabel[i];
      ifile << l;
    }
    ifile.close();
  }

  {
    ofstream ifile((prefix+"MCTracks.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size = vMCTracks.size();
    ifile << Size<<endl;
    cout<<Size<<"MCTrackWrite"<<endl;
    for (int i = 0; i < Size; i++){
      const AliHLTTPCCAMCTrack &l = vMCTracks[i];
      ifile << l;
    }
    ifile.close();
  }


  {
    ofstream ifile((prefix+"MCPoints.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size = vMCPoints.size();
    ifile << Size<<endl;
    cout<<Size<<"MCPointWrite"<<endl;
    for (int i = 0; i < Size; i++){
      const AliHLTTPCCALocalMCPoint &l = vMCPoints[i];
      ifile << l;
    }
    ifile.close();
  }
  
  return 1;
}

bool TData::WriteSettingsToFile(string prefix) const
{
  ofstream out((prefix+"settings.data").data());
  if ( !out.is_open() ) return 0;
  out << mSliceParam.size() << std::endl;
  cout<<"SliceParamWrite"<<endl;
  for ( unsigned int iSlice = 0; iSlice < mSliceParam.size(); iSlice++ ) {
    out << mSliceParam[iSlice];
  }
  return 1;
}
