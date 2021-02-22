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

#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAMath.h"

AliHLTTPCCAMCPoint::AliHLTTPCCAMCPoint()
    : fX( 0 ), fY( 0 ), fZ( 0 ), fSx( 0 ), fSy( 0 ), fSz( 0 ), fTime( 0 ), fISlice( 0 ), fTrackID( 0 )
{
  //* Default constructor
}

ostream& operator<<(ostream& out, const AliHLTTPCCALocalMCPoint &a)
{
  out << a.fX << " " << a.fY << " " << a.fZ << std::endl;
  out << a.fPx << " " << a.fPy << " " << a.fPz << " " << a.fQP << std::endl;
  return out << a.fISlice << " " << a.fIRow << " " << a.fTrackI << " " << a.fTrackID << std::endl;
}


istream& operator>>(istream& in, AliHLTTPCCALocalMCPoint &a)
{
  in >> a.fX >> a.fY >> a.fZ;
  in >> a.fPx >> a.fPy >> a.fPz >> a.fQP;
  return in >> a.fISlice >> a.fIRow >> a.fTrackI >> a.fTrackID;
}


void AliHLTTPCCALocalMCPoint::RotateXY( float alpha) {
  const float cA = CAMath::Cos( alpha );
  const float sA = CAMath::Sin( alpha );
  {
    const float x = X(), y = Y();
    SetX(  x*cA +  y*sA );
    SetY( -x*sA +  y*cA );
  }
  {
    const float x = Px(), y = Py();
    SetPx(  x*cA +  y*sA );
    SetPy( -x*sA +  y*cA );
  }
}
