/*
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/


#ifndef ENABLE_ARRAY_BOUNDS_CHECKING
#define ENABLE_ARRAY_BOUNDS_CHECKING
#endif

#include "unittest.h"
#include <AliHLTArray.h>

void test1DimArray()
{
    AliHLTResizableArray<int> a(10);
    for (int i = 0; i < 10; ++i) {
        a[i] = 2;
    }
    for (int i = 0; i < 10; ++i) {
        VERIFY(a[i] == 2);
    }
    EXPECT_ASSERT_FAILURE(a[10] = 2);
    EXPECT_ASSERT_FAILURE(a[-1] = 2);

    a.Resize(2);
    for (int i = 0; i < 2; ++i) {
        a[i] = 2;
    }
    for (int i = 0; i < 2; ++i) {
        VERIFY(a[i] == 2);
    }
    EXPECT_ASSERT_FAILURE(a[2] = 2);
    EXPECT_ASSERT_FAILURE(a[-1] = 2);
}

void test2DimArray()
{
    AliHLTResizableArray<int, 2> a(3, 3);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            a(i, j) = 1;
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            VERIFY(a(i, j) == 1);
        }
    }
    EXPECT_ASSERT_FAILURE(a(2, 3) = 2);
    EXPECT_ASSERT_FAILURE(a(3, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(-1, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(0, -1) = 2);

    a.Resize(2, 2);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            a(i, j) = 1;
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            VERIFY(a(i, j) == 1);
        }
    }
    EXPECT_ASSERT_FAILURE(a(1, 2) = 2);
    EXPECT_ASSERT_FAILURE(a(2, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(-1, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(0, -1) = 2);
}

void test2DimArraySubArray()
{
    AliHLTResizableArray<int, 2> a(3, 3);
    for (int i = 0; i < 3; ++i) {
        AliHLTArray<int> b = a[i];
        for (int j = 0; j < 3; ++j) {
            b[j] = i + (j << 4);
            VERIFY(&b[j] == &a(i, j));
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            VERIFY(a(i, j) == (i + (j << 4)));
            VERIFY(a(i, j) == a[i][j]);
        }
    }
}

void testArrayArithmetic()
{
    AliHLTResizableArray<int> a(10);
    for (int i = 0; i < 10; ++i) {
        *(a + i) = i;
        VERIFY(a[i] == i);
    }
    AliHLTArray<int> b = a + 10;
    VERIFY(b[-1] == a[9]);
    EXPECT_ASSERT_FAILURE(b[0]);
}

void test1DimFixedArray()
{
    AliHLTFixedArray<int, AliHLTArraySize<10> > a;
    for (int i = 0; i < 10; ++i) {
        a[i] = 2;
    }
    for (int i = 0; i < 10; ++i) {
        VERIFY(a[i] == 2);
    }
    EXPECT_ASSERT_FAILURE(a[10] = 2);
    EXPECT_ASSERT_FAILURE(a[-1] = 2);
}

void test2DimFixedArray()
{
    AliHLTFixedArray<int, AliHLTArraySize<3, 3> > a;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            a(i, j) = 1;
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            VERIFY(a(i, j) == 1);
        }
    }
    EXPECT_ASSERT_FAILURE(a(2, 3) = 2);
    EXPECT_ASSERT_FAILURE(a(3, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(-1, 0) = 2);
    EXPECT_ASSERT_FAILURE(a(0, -1) = 2);
}

void test2DimFixedArraySubArray()
{
    AliHLTFixedArray<int, AliHLTArraySize<3, 3> > a;
    for (int i = 0; i < 3; ++i) {
        AliHLTArray<int> b = a[i];
        for (int j = 0; j < 3; ++j) {
            b[j] = i + (j << 4);
            VERIFY(&b[j] == &a(i, j));
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            VERIFY(a(i, j) == (i + (j << 4)));
            VERIFY(a(i, j) == a[i][j]);
        }
    }
}

void testFixedArrayArithmetic()
{
    AliHLTFixedArray<int, AliHLTArraySize<10> > a;
    for (int i = 0; i < 10; ++i) {
        *(a + i) = i;
        VERIFY(a[i] == i);
    }
    AliHLTArray<int> b = a + 10;
    VERIFY(b[-1] == a[9]);
    EXPECT_ASSERT_FAILURE(b[0]);
}

void testArrayAlignments()
{
  {
    unsigned char i = 0;
    AliHLTResizableArray<int, 1, 8> b( 10 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 8 - 1 ) ) == 0 );
    AliHLTFixedArray<int, AliHLTArraySize<10>, 8> a;
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 8 - 1 ) ) == 0 );
  } {
    unsigned char i = 0;
    AliHLTResizableArray<int, 1, 16> b( 10 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 16 - 1 ) ) == 0 );
    AliHLTFixedArray<int, AliHLTArraySize<10>, 16> a;
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 16 - 1 ) ) == 0 );
  } {
    unsigned char i = 0;
    AliHLTResizableArray<int, 1, 32> b( 10 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 32 - 1 ) ) == 0 );
    AliHLTFixedArray<int, AliHLTArraySize<10>, 32> a;
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 32 - 1 ) ) == 0 );
  } {
    unsigned char i = 0;
    AliHLTResizableArray<int, 1, 64> b( 10 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 64 - 1 ) ) == 0 );
    AliHLTFixedArray<int, AliHLTArraySize<10>, 64> a;
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 64 - 1 ) ) == 0 );
  } {
    unsigned char i = 0;
    AliHLTResizableArray<int, 1, 128> b( 10 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 128 - 1 ) ) == 0 );
    AliHLTFixedArray<int, AliHLTArraySize<10>, 128> a;
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 128 - 1 ) ) == 0 );
  }
}

template<typename T> void testCacheLineAlignment()
{
  //std::cout << sizeof( T ) << " - " << sizeof( AliHLTInternal::CacheLineSizeHelper<T> ) << std::endl;
  AliHLTFixedArray<T, AliHLTArraySize<20>, AliHLTFullyCacheLineAligned> a;
  AliHLTResizableArray<T, 1, AliHLTFullyCacheLineAligned> b( 20 );

  for ( int i = 0; i < 20; ++i ) {
    VERIFY( ( reinterpret_cast<unsigned long>( &a[i] ) & ( 64 - 1 ) ) == 0 );
    VERIFY( ( reinterpret_cast<unsigned long>( &b[i] ) & ( 64 - 1 ) ) == 0 );
  }
}

struct CtorDtorCounter
{
  static int count;
  CtorDtorCounter() { ++count; }
  ~CtorDtorCounter() { --count; }
};

int CtorDtorCounter::count = 0;

void testCtorDtor()
{
  CtorDtorCounter::count = 0;
  {
    AliHLTResizableArray<CtorDtorCounter> a( 200 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTResizableArray<CtorDtorCounter, 2> a( 20, 10 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTResizableArray<CtorDtorCounter, 3> a( 5, 10, 4 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );

  {
    AliHLTResizableArray<CtorDtorCounter, 1, 16> a( 200 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTResizableArray<CtorDtorCounter, 2, 16> a( 20, 10 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTResizableArray<CtorDtorCounter, 3, 16> a( 5, 10, 4 );
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
}

void testCtorDtorFixed()
{
  CtorDtorCounter::count = 0;
  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<200> > a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<20, 10> > a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<2, 10, 10> > a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );

  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<200>, 16> a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<20, 10>, 16> a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
  {
    AliHLTFixedArray<CtorDtorCounter, AliHLTArraySize<2, 10, 10>, 16> a;
    COMPARE( CtorDtorCounter::count, 200 );
  }
  COMPARE( CtorDtorCounter::count, 0 );
}

template<int Size> struct StructOfSize { char mem[Size]; };

int main()
{
    // ResizableArray
    runTest(test1DimArray);
    runTest(test2DimArray);
    runTest(test2DimArraySubArray);
    runTest(testArrayArithmetic);

    // FixedArray
    runTest(test1DimFixedArray);
    runTest(test2DimFixedArray);
    runTest(test2DimFixedArraySubArray);
    runTest(testFixedArrayArithmetic);

    runTest(testCtorDtor);
    runTest(testCtorDtorFixed);

    // Alignments
    runTest(testArrayAlignments);
    runTest(testCacheLineAlignment<int>);
    runTest(testCacheLineAlignment<double>);
    runTest(testCacheLineAlignment<StructOfSize<63> >);
    runTest(testCacheLineAlignment<StructOfSize<64> >);
    runTest(testCacheLineAlignment<StructOfSize<65> >);
    runTest(testCacheLineAlignment<StructOfSize<103> >);
    runTest(testCacheLineAlignment<StructOfSize<127> >);
    runTest(testCacheLineAlignment<StructOfSize<128> >);
    runTest(testCacheLineAlignment<StructOfSize<129> >);
    runTest(testCacheLineAlignment<StructOfSize<255> >);
    runTest(testCacheLineAlignment<StructOfSize<256> >);
    runTest(testCacheLineAlignment<StructOfSize<257> >);
    return 0;
}
