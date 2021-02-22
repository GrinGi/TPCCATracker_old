// $Id: AliHLTTPCCAInputSettings.cxx,v 1.12 2010/09/01 10:38:27 ikulakov Exp $
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
