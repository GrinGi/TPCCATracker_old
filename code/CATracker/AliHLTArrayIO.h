/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2009 Matthias Kretz <kretz@kde.org>
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

#ifndef ALIHLTARRAYIO_H
#define ALIHLTARRAYIO_H

#include <iostream>
#include "AliHLTArray.h"

namespace
{
    namespace AnsiColor
    {
        static const char *const green  = "\033[1;40;32m";
        static const char *const yellow = "\033[1;40;33m";
        static const char *const blue   = "\033[1;40;34m";
        static const char *const normal = "\033[0m";
    } // namespace AnsiColor
} // anonymous namespace

template<typename T>
inline std::ostream &operator<<( std::ostream &out, const AliHLTArray<T> &a )
{
  out << AnsiColor::blue << "{" << AnsiColor::normal;
  for ( int i = 0; i < a.Size(); ++i ) {
    if (i > 0) {
      out << AnsiColor::blue << ", " << AnsiColor::normal;
    }
    out << a[i];
  }
  out << AnsiColor::blue << "}" << AnsiColor::normal;
  return out;
}
#endif // ALIHLTARRAYIO_H
