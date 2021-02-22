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

#include "../code/AliHLTTPCCATrackParamVector.h"
#include "random.h"
#include "../code/tsc.h"
#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;

void print(sfloat_m mask)
{
    cerr << mask << endl;
}
void print(sfloat_v v)
{
    cerr << v << endl;
}
void print(const TrackParamVector &param)
{
    cerr << param << endl;
}

struct Point { float x, y, z, errY, errZ; };
static const Point points[] = {
    { 135.1, 22.0896, 57.1441, 0.478042, 37 },
    { 136.1, 21.8976, 57.8748, 0.401248, 10 },
    { 137.1, 21.6292, 58.411, 0.337578, 12 },
    { 138.1, 21.211, 58.517, 0.41703, 34 },
    { 139.1, 21.1311, 58.7534, 0.401294, 29 },
    { 140.1, 20.2856, 59.5319, 0.321724, 25 },
    { 141.1, 20.227, 59.7307, 0.386096, 29 },
    { 142.1, 19.6322, 60.1898, 0.3565, 52 },
    { 143.1, 19.327, 60.4999, 0.382641, 25 },
    { 144.1, 18.8093, 61.1843, 0.394991, 30 },
    { 145.1, 18.3264, 61.6037, 0.38332, 59 },
    { 146.1, 18.1171, 61.8398, 0.368713, 93 },
    { 147.1, 17.431, 62.4709, 0.447406, 57 },
    { 148.1, 17.1294, 62.7788, 0.38138, 169 },
    { 149.1, 16.8025, 63.1084, 0.405781, 37 },
    { 150.1, 16.1993, 63.6425, 0.396973, 18 },
    { 151.1, 15.6599, 64.2074, 0.423864, 23 },
    { 152.1, 15.1118, 64.3623, 0.399163, 60 },
    { 153.1, 14.5673, 64.9115, 0.458708, 57 },
    { 154.1, 14.2634, 65.3569, 0.434, 53 },
    { 155.1, 13.7927, 65.8411, 0.3812, 31 },
    { 156.1, 12.8615, 66.4416, 0.447161, 13 },
    { 157.1, 12.6824, 66.5843, 0.395614, 14 },
    { 158.1, 11.8679, 67.1872, 0.456317, 29 },
    { 159.1, 11.4111, 67.799, 0.429961, 25 },
    { 160.1, 10.7652, 68.1114, 0.372699, 36 },
    { 161.1, 10.3243, 68.3566, 0.427548, 28 },
    { 162.1, 9.63181, 68.8766, 0.433095, 35 },
    { 163.1, 9.09223, 69.4113, 0.365334, 66 },
    { 164.1, 8.70789, 69.6686, 0.386338, 31 },
    { 165.1, 7.66849, 70.3711, 0.338374, 21 },
    { 166.1, 6.77626, 71.0484, 0.402308, 42 },
    { 167.1, 6.47169, 71.3256, 0.401572, 121 },
    { 168.1, 6.12489, 71.5467, 0.435544, 29 },
    { 169.1, 5.00854, 72.2651, 0.456057, 20 },
    { 170.1, 4.33758, 72.7472, 0.33781, 18 },
    { 171.1, 3.63814, 73.2676, 0.429722, 27 },
    { 172.1, 2.95788, 73.6961, 0.377169, 36 },
    { 173.1, 2.42343, 73.9797, 0.451176, 48 },
    { 174.1, 1.47768, 74.678, 0.391769, 61 },
    { 175.1, 0.997154, 75.0336, 0.309559, 52 },
    { 176.1, -0.194864, 75.7658, 0.423141, 46 },
    { 177.1, -0.78548, 76.1002, 0.405591, 82 },
    { 178.1, -1.70181, 76.6887, 0.49373, 44 },
    { 179.1, -2.43273, 77.1976, 0.433543, 26 },
    { 180.1, -3.5269, 77.8249, 0.365949, 71 },
    { 181.1, -3.89783, 78.0077, 0.391864, 43 },
    { 182.1, -5.07642, 78.7063, 0.342308, 78 },
    { 183.1, -5.67738, 79.1308, 0.364185, 27 },
    { 184.1, -7.09874, 79.9793, 0.413012, 17 },
    { 185.1, -7.74874, 80.2651, 0.390857, 78 },
    { 186.1, -8.72684, 80.7763, 0.435554, 63 },
    { 187.1, -9.94512, 81.5842, 0.458442, 52 },
    { 188.1, -10.3914, 81.8053, 0.39452, 51 },
    { 189.1, -11.6131, 82.5479, 0.406414, 44 },
    { 190.1, -12.5526, 83.168, 0.471748, 30 },
    { 191.1, -13.6604, 83.7105, 0.415798, 37 },
    { 192.1, -14.8006, 84.254, 0.363932, 89 },
    { 193.1, -15.2956, 84.5091, 0.34097, 78 },
    { 194.1, -17.1412, 85.468, 0.43038, 43 },
    { 195.1, -18.3906, 86.1656, 0.432597, 96 },
    { 196.1, -18.661, 86.3193, 0.418414, 85 },
    { 197.1, -20.6867, 87.2684, 0.444132, 32 },
    { 198.1, -20.9574, 87.465, 0.4423, 23 },
    { 199.35, -23.7709, 88.8723, 0.424679, 139 },
    { 200.85, -24.0818, 89.0219, 0.412616, 139 },
    { 202.35, -26.3313, 90.1934, 0.38898, 344 },
    { 203.85, -28.5423, 91.2925, 0.407156, 136 },
    { 205.35, -30.4051, 92.0181, 0.442859, 59 },
    { 206.85, -32.8783, 93.3577, 0.432989, 60 },
    { 208.35, -34.5433, 94.1707, 0.385841, 69 }
};
static const int pointsCount = sizeof(points)/sizeof(Point);

#define COMPARE( a, b ) \
  if ( ( a ) != ( b ) ) { \
    std::cerr << std::setprecision( 40 ); \
    std::cerr << "Failed test on " << __FILE__ << ':' << __LINE__ << " where " << #a << '(' << ( a ) << ") != " << #b << '(' << ( b ) << ")" << std::endl; \
  } \
do {} while ( false )
int main()
{
    sfloat_m mask(Vc::One);
    sfloat_v one(Vc::One);
    sfloat_v zero(Vc::Zero);
    TimeStampCounter tsc;

    const Point *const endP = &points[pointsCount];
    TrackParamVector param;

    tsc.Start();
    for (unsigned int i = 0; i < 100000; ++i) {
        const Point *p = &points[0];
        sfloat_v x(p->x), y(p->y), z(p->z);
        ++p;
        sfloat_v dx(p->x - x), dy(p->y - y), dz(p->z - z);
        sfloat_v ri = sfloat_v(Vc::One) / Vc::sqrt(dx * dx + dy * dy);
        sfloat_v sinPhi = dy * ri;
        sfloat_v dzDs   = dz * ri;

        param.SetX(x);
        param.SetY(y);
        param.SetZ(z);
        param.SetSinPhi(sinPhi);
        param.SetDzDs(dzDs);

        param.SetQPt(     zero );
        param.SetSignCosPhi( one );
        param.SetChi2(    zero );
        param.SetNDF(       -3 );
        param.SetCov(  0, p->errY );
        param.SetCov(  2, p->errZ );
        param.SetCov(  3, zero );
        param.SetCov(  4, zero );
        param.SetCov(  5,  one );
        param.SetCov(  6, zero );
        param.SetCov(  7, zero );
        param.SetCov(  8, zero );
        param.SetCov(  9,  one );
        param.SetCov( 10, zero );
        param.SetCov( 11, zero );
        param.SetCov( 12, zero );
        param.SetCov( 13, zero );
        param.SetCov( 14, 10.f );

        do {
            mask = param.TransportToX(p->x, param.SinPhi(), -5.f, -1.f, mask);
            dx = p->x - param.X();
            dy = p->y - param.Y();
            dz = p->z - param.Z();
            mask = param.FilterDelta(mask, dy, dz, p->errY, p->errZ, .99f);
            ++p;
        } while (p < endP);
    }
    tsc.Stop();
    COMPARE( param.X(), sfloat_v( 208.35f ) );
    COMPARE( param.Y(), sfloat_v( -34.97408294677734375f ) );
    COMPARE( param.Z(), sfloat_v( 94.34102630615234375f ) );
    COMPARE( param.SinPhi(), sfloat_v( -0.83448994159698486328125f ) );
    COMPARE( param.DzDs(), sfloat_v( 0.3905226290225982666015625f ) );
    COMPARE( param.QPt(), sfloat_v( 0.00136145087890326976776123046875f ) );
    COMPARE( param.Cov()[ 0], sfloat_v( 0.092910252511501312255859375f ) );
    COMPARE( param.Cov()[ 1], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[ 2], sfloat_v( 3.4566605091094970703125f ) );
    COMPARE( param.Cov()[ 3], sfloat_v( 0.001459264196455478668212890625f ) );
    COMPARE( param.Cov()[ 4], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[ 5], sfloat_v( 2.817280983435921370983123779296875e-05f ) );
    COMPARE( param.Cov()[ 6], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[ 7], sfloat_v( 0.0480997264385223388671875f ) );
    COMPARE( param.Cov()[ 8], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[ 9], sfloat_v( 0.0007769078947603702545166015625f ) );
    COMPARE( param.Cov()[10], sfloat_v( -7.51373090679408051073551177978515625e-06f ) );
    COMPARE( param.Cov()[11], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[12], sfloat_v( -1.673531500046010478399693965911865234375e-07f ) );
    COMPARE( param.Cov()[13], sfloat_v( 0.f ) );
    COMPARE( param.Cov()[14], sfloat_v( 1.108250491377305024798261001706123352051e-09f ) );
    std::cout << tsc.Cycles() / sfloat_v::Size << endl;
    return 0;
}
