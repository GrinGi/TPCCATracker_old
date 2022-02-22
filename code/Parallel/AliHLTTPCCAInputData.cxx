/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2007-2019 Sergey Gorbunov
 *               2007-2019 Maksym Zyzak
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
