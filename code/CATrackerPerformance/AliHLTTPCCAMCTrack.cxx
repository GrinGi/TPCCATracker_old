/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2007-2019 Sergey Gorbunov
 *               2007-2019 Maksym Zyzak
 *               2007-2014 Igor Kulakov
 *               2014-2020 Grigory Kozlov
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


#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMath.h"

#ifndef HLTCA_STANDALONE
#include "TParticle.h"
#include "TDatabasePDG.h"
#endif

AliHLTTPCCAMCTrack::AliHLTTPCCAMCTrack()
    : fPDG( 0 ), fP( 0 ), fPt( 0 ), fNHits( 0 ), fNMCPoints( 0 ), fFirstMCPointID( 0 ), fNReconstructed( 0 ), fSet( 0 ), fNTurns( 0 )
{
  //* Default constructor
}

#ifndef HLTCA_STANDALONE
AliHLTTPCCAMCTrack::AliHLTTPCCAMCTrack( const TParticle *part )
    : fPDG( 0 ), fP( 0 ), fPt( 0 ), fNHits( 0 ), fNMCPoints( 0 ), fFirstMCPointID( 0 ), fNReconstructed( 0 ), fSet( 0 ), fNTurns( 0 )
{
  //* Constructor from TParticle

  for ( int i = 0; i < 7; i++ ) fPar[i] = 0;
  for ( int i = 0; i < 7; i++ ) fTPCPar[i] = 0;
  fP = 0;
  fPt = 0;

  if ( !part ) return;
  TLorentzVector mom, vtx;
  part->ProductionVertex( vtx );
  part->Momentum( mom );
  fPar[0] = part->Vx();
  fPar[1] = part->Vy();
  fPar[2] = part->Vz();
  fP = part->P();
  fPt = part->Pt();
  float pi = ( fP > 1.e-4 ) ? 1. / fP : 0;
  fPar[3] = part->Px() * pi;
  fPar[4] = part->Py() * pi;
  fPar[5] = part->Pz() * pi;
  fPar[6] = 0;
  fPDG  = part->GetPdgCode();
  if ( CAMath::Abs( fPDG ) < 100000 ) {
    TParticlePDG *pPDG = TDatabasePDG::Instance()->GetParticle( fPDG );
    if ( pPDG ) fPar[6] = pPDG->Charge() / 3.0 * pi;
  }
}
#endif

// void AliHLTTPCCAMCTrack::SetTPCPar( float X, float Y, float Z,
//                                     float Px, float Py, float Pz )
// {
//   //* Set parameters at TPC entrance

//   for ( int i = 0; i < 7; i++ ) fTPCPar[i] = 0;

//   fTPCPar[0] = X;
//   fTPCPar[1] = Y;
//   fTPCPar[2] = Z;
//   float p = CAMath::Sqrt( Px * Px + Py * Py + Pz * Pz );
//   float pi = ( p > 1.e-4 ) ? 1. / p : 0;
//   fTPCPar[3] = Px * pi;
//   fTPCPar[4] = Py * pi;
//   fTPCPar[5] = Pz * pi;
//   fTPCPar[6] = 0;
//   if ( CAMath::Abs( fPDG ) < 100000 ) {
//     TParticlePDG *pPDG = TDatabasePDG::Instance()->GetParticle( fPDG );
//     if ( pPDG ) fTPCPar[6] = pPDG->Charge() / 3.0 * pi;
//   }
// }

ostream& operator<<(ostream& out, const AliHLTTPCCAMCTrack &a)
{
  out << a.fMotherId << " " << a.fPDG << std::endl;
  for (int i = 0; i < 7; i++) out << a.fPar[i] << std::endl;
  for (int i = 0; i < 7; i++) out << a.fTPCPar[i] << std::endl;
  out << a.fP << " " << a.fPt << std::endl;
  out << a.fNHits << " " << a.fNMCPoints << " " << a.fFirstMCPointID << std::endl;
  return out << a.fNReconstructed << " " << a.fSet << " " << a.fNTurns << std::endl;
}


istream& operator>>(istream& in, AliHLTTPCCAMCTrack &a)
{
  in >> a.fMotherId >> a.fPDG;
  for (int i = 0; i < 7; i++) in >> a.fPar[i];
  for (int i = 0; i < 7; i++) in >> a.fTPCPar[i];
  in >> a.fP >> a.fPt;
  in >> a.fNHits >> a.fNMCPoints >> a.fFirstMCPointID;
  return in >> a.fNReconstructed >> a.fSet >> a.fNTurns;
}

