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

#ifndef ALIHLTTPCCAMCPOINT_H
#define ALIHLTTPCCAMCPOINT_H

#include <vector>

#include <iostream>
using std::ostream;
using std::istream;



/**
 * @class AliHLTTPCCAMCPoint
 * store MC point information for AliHLTTPCCAPerformance
 */
class AliHLTTPCCAMCPoint
{
  public:

    AliHLTTPCCAMCPoint();

    float  X()           const { return fX; }
    float  Y()           const { return fY; }
    float  Z()           const { return fZ; }
    float  Sx()          const { return fSx; }
    float  Sy()          const { return fSy; }
    float  Sz()          const { return fSz; }
    float  Time()        const { return fTime; }
    int    ISlice()      const { return fISlice; }
    int    TrackID()     const { return fTrackID; }

    void SetX( float v )           { fX = v; }
    void SetY( float v )           { fY = v; }
    void SetZ( float v )           { fZ = v; }
    void SetSx( float v )          { fSx = v; }
    void SetSy( float v )          { fSy = v; }
    void SetSz( float v )          { fSz = v; }
    void SetTime( float v )        { fTime = v; }
    void SetISlice( int v )      { fISlice = v; }
    void SetTrackID( int v )     { fTrackID = v; }

    static bool Compare( const AliHLTTPCCAMCPoint &p1, const AliHLTTPCCAMCPoint &p2 ) {
      return ( p1.fTrackID < p2.fTrackID );
    }

  protected:

    float fX;         //* global X position
    float fY;         //* global Y position
    float fZ;         //* global Z position
    float fSx;        //* slice X position
    float fSy;        //* slice Y position
    float fSz;        //* slice Z position
    float fTime;      //* time
    int   fISlice;    //* slice number
    int   fTrackID;   //* mc track number
};


/**
 * @class AliHLTTPCCALocalMCPoint
 * store MC point information for AliHLTTPCCAPerformance
 */
class AliHLTTPCCALocalMCPoint
{
  public:

    AliHLTTPCCALocalMCPoint(){};
    ~AliHLTTPCCALocalMCPoint(){};

    float  X()           const { return fX; }
    float  Y()           const { return fY; }
    float  Z()           const { return fZ; }
    float  Px()          const { return fPx; }
    float  Py()          const { return fPy; }
    float  Pz()          const { return fPz; }
    float  QP()          const { return fQP; }
    int    ISlice()      const { return fISlice; }
    int    IRow()      const { return fIRow; }
    int    TrackI()     const { return fTrackI; }
    int    TrackID()     const { return fTrackID; }

    void SetX( float v )           { fX = v; }
    void SetY( float v )           { fY = v; }
    void SetZ( float v )           { fZ = v; }
    void SetPx( float v )          { fPx = v; }
    void SetPy( float v )          { fPy = v; }
    void SetPz( float v )          { fPz = v; }
    void SetQP( float v )        { fQP = v; }
    void SetISlice( int v )      { fISlice = v; }
    void SetIRow( int v )        { fIRow = v; }
    void SetTrackI( int v )     { fTrackI = v; }
    void SetTrackID( int v )     { fTrackID = v; }

    void RotateXY( float alpha );
  
    static bool Compare( const AliHLTTPCCALocalMCPoint &p1, const AliHLTTPCCALocalMCPoint &p2 ) {
      return ( p1.fTrackID < p2.fTrackID );
    }

    friend ostream& operator<<(ostream& out, const AliHLTTPCCALocalMCPoint &a);
    friend istream& operator>>(istream& in, AliHLTTPCCALocalMCPoint &a);


  protected:

    float fX;         //* local X position
    float fY;         //* local Y position
    float fZ;         //* local Z position
    float fPx;        //* local momentum projection - Px
    float fPy;
    float fPz;
    float fQP;        //* Q/P

    int   fISlice;   //* slice number
    int   fIRow;
    int   fTrackI;   //* mc track index
    int   fTrackID;  //* mc track simulation ID
  public:
    std::vector<int> fHits;
};

#endif
