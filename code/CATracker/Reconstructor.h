/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2009 Matthias Kretz <kretz@kde.org>
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

#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include "AliHLTTPCCATracker.h"

#ifdef USE_TBB
#include <tbb/task.h>

class AliHLTTPCCATracker::Reconstructor : public tbb::task
{
  public:
    Reconstructor( AliHLTTPCCATracker *tracker ) : d( tracker ) {}
    tbb::task *execute();

  private:
    AliHLTTPCCATracker *const d;
};
#else //USE_TBB
class AliHLTTPCCATracker::Reconstructor
{
  public:
    Reconstructor( AliHLTTPCCATracker *tracker ) : d( tracker ) {}
    int execute();

  private:
    AliHLTTPCCATracker *const d;
};
#endif //USE_TBB

#endif // RECONSTRUCTOR_H
