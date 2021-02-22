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
#if 1//def DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef ALIHLTTPCCA_SLICES_LINKS_PERFORMANCE_H
#define ALIHLTTPCCA_SLICES_LINKS_PERFORMANCE_H


#include "AliHLTTPCCASlicesPerformance.h"

#include "AliHLTArray.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAGBTracker.h"

  /**
   * @class AliHLTTPCCASlicesLinksPerformance
   */
class AliHLTTPCCASlicesLinksPerformance: public AliHLTTPCCASlicesPerformance
{
 public:

  AliHLTTPCCASlicesLinksPerformance():AliHLTTPCCASlicesPerformance(){};
  virtual ~AliHLTTPCCASlicesLinksPerformance(){};

  virtual void SetNewEvent(const AliHLTTPCCAGBTracker * const Tracker,
  AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
  AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
  AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);

    /// Histograms. don't need them
//  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0){};
//  virtual void FillHistos(){};
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
