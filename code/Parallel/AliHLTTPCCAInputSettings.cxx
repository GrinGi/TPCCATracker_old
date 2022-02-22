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


#include "AliHLTTPCCAInputSettings.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAOutTrack.h"
#include "AliHLTArray.h"
#include <algorithm>
#include <fstream>
using std::ofstream;
using std::ifstream;

AliHLTTPCCAInputSettings::AliHLTTPCCAInputSettings()
    :
    fNSlices( 0 )
{
}

void AliHLTTPCCAInputSettings::Init()
{
  fNSlices = 0;
}

AliHLTTPCCAInputSettings::AliHLTTPCCAInputSettings( const AliHLTTPCCAInputSettings& a)
{
  StartEvent();
  SetSettings( a.GetSettings() );
}

const AliHLTTPCCAInputSettings& AliHLTTPCCAInputSettings::operator=( const AliHLTTPCCAInputSettings& a)
{
  StartEvent();
  SetSettings( a.GetSettings() );
  return a;
}

AliHLTTPCCAInputSettings::~AliHLTTPCCAInputSettings()
{
  //* destructor
  StartEvent();
}

void AliHLTTPCCAInputSettings::SetNSlices( int N )
{
  //* set N of slices
  StartEvent();
  fNSlices = N;
  fSettings.resize( N );
}

void AliHLTTPCCAInputSettings::StartEvent()
{
  //* clean up track and hit arrays
  fNSlices = 0;
}

void AliHLTTPCCAInputSettings::ReadSettings( std::istream &in )
{
  //* Read settings from the file
  int nSlices = 0;
  in >> nSlices;
  SetNSlices( nSlices );
  fSettings.resize(nSlices);
  for ( int iSlice = 0; iSlice < NSlices(); iSlice++ ) {
    AliHLTTPCCAParam param;
    in >> param;

    fSettings[iSlice] = param;
  }
}

void AliHLTTPCCAInputSettings::SetSettings( const std::vector<AliHLTTPCCAParam>& settings )
{
  SetNSlices( settings.size() );
  fSettings.resize( NSlices() );
  for ( int iSlice = 0; iSlice < NSlices(); iSlice++ ) {
    fSettings[iSlice] = settings[iSlice];
  }
}

bool AliHLTTPCCAInputSettings::ReadSettingsFromFile(string prefix)
{
  ifstream ifile((prefix+"settings.data").data());
  if ( !ifile.is_open() ) return 0;
  ReadSettings(ifile);
  return 1;
}
