/*
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef CYCLECOUNTER_H
#define CYCLECOUNTER_H

class CycleCounter
{
    public:
        CycleCounter();
        void Start();
        void Stop();
        unsigned int Cycles() const;

    private:
        unsigned int tsc;
};

// the ctor puts tsc into the cache which should give a better first measurement
inline CycleCounter::CycleCounter()
    : tsc(0)
{
}

inline void CycleCounter::Start()
{
    asm volatile("rdtsc\n\t"
           "mov %%eax,%0"
           : "=m"(tsc) : : "eax", "edx");
}

inline void CycleCounter::Stop()
{
    asm volatile("rdtsc\n\t"
            "sub %1,%%eax"
            : "=a"(tsc)
            : "m"(tsc)
            : "edx");
}

inline unsigned int CycleCounter::Cycles() const
{
    return tsc;
}

#endif // CYCLECOUNTER_H
