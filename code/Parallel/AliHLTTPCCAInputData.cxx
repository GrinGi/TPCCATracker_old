// $Id: AliHLTTPCCAInputData.cxx,v 1.12 2010/09/01 10:38:27 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
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


#include "AliHLTTPCCAInputData.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAOutTrack.h"
#include "AliHLTArray.h"
#include <algorithm>
#include <fstream>
using std::ofstream;
using std::ifstream;

AliHLTTPCCAInputData::AliHLTTPCCAInputData()
    :
    fNHits( 0 )
{
}

void AliHLTTPCCAInputData::Init()
{
  fNHits = 0;
}

AliHLTTPCCAInputData::AliHLTTPCCAInputData( const AliHLTTPCCAInputData& a)
{
  StartEvent();
  SetHits( a.Hits(), a.NHits() );
}

const AliHLTTPCCAInputData& AliHLTTPCCAInputData::operator=( const AliHLTTPCCAInputData& a)
{
  StartEvent();
  SetHits( a.Hits(), a.NHits() );
  return a;
}

AliHLTTPCCAInputData::~AliHLTTPCCAInputData()
{
  //* destructor
  StartEvent();
}

void AliHLTTPCCAInputData::StartEvent()
{
  //* clean up track and hit arrays
  fNHits = 0;
}

void AliHLTTPCCAInputData::SetNHits( int nHits )
{
  //* set the number of hits
  fHits.Resize( nHits );
  fNHits = nHits;
}

void AliHLTTPCCAInputData::SetHits( std::vector<AliHLTTPCCAGBHit> &hits)
{
  const int NHits2 = hits.size();

  SetNHits(NHits2);

  fHits.Resize(NHits2);
  for (int iH = 0; iH < NHits2; iH++){
    fHits[iH] = hits[iH];
  }
} // need for StRoot

void AliHLTTPCCAInputData::SetHits( const AliHLTTPCCAGBHit *hits, int nHits )
{
  const int NHits2 = nHits;

  SetNHits(NHits2);

  fHits.Resize(NHits2);
  for (int iH = 0; iH < NHits2; iH++){
    fHits[iH] = hits[iH];
  }
} // need for StRoot

bool AliHLTTPCCAInputData::ReadHitsFromFile(string prefix)
{
    ifstream ifile((prefix+"hits.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    fHits.Resize(Size);
    SetNHits(Size);
    for (int i = 0; i < Size; i++){
      AliHLTTPCCAGBHit &l = fHits[i];
      ifile >> l;
    }
    ifile.close();
    return 1;
}
