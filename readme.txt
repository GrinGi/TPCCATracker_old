====== Instalation ======
1. mkdir build
2. cd build
3. cmake ..
4. cmake -i  (set CMAKE_BUILD_TYPE in Release)
5. make -j16

====== Run ======
CA        - procces all events from "build/Events" directory
CA -perf  - print performances
CA N      - procces only event with number N
CA -ev N  -          -//-//-
CA -ev N1 N2 - procces only events with numbers N1,N1+1,..,N2
CA -dir [dirPath] - procces all events from [directory] directory

ex: CA     0   -dir "/d/cbm02/ikulakov/STAR/Data/dataAuAu" -perf
ex: CA -ev 0 9 -dir "/d/cbm02/ikulakov/STAR/Data/dataAuAu" -perf

======= QA =======
in build directory:

root -l DrawHistos.C+   - will create dir Histos with number of pictures
root -l Pulls.C         - will show and create pulls pictures: GlobalTrackParamQuality.pdf;	 SectorTrackParamQuality.pdf

=======

tested with:
2 x X5550  @ 2.67GHz
gcc (Debian 4.4.4-11) 4.4.5 20100824 (prerelease)

------------------
0. geometry file - settings.data
The following information has to be there:

NSlices
slice0
slice1
...

slice[j] is info for slice[j] in the next format:

ISlice
NRows
Angle dAngle
cosAngle sinAngle
angleMin angleMax
rMin rMax
minZ maxZ
ErrX ErrY ErrZ
PadPitch
Bz
1
4
3.5
3.5
12.25
row0X
row1X
...
ParamS0Par000
...
ParamS0Par126

ISlice - index of the slice (starting from 0)
NRows - number of rows in slice
Angle - the angle of simmetry line of slice, in radian
dAngle - angle between left and right edges of the slice
cosAngle, sinAngle - cos and sin of angle.
angleMin, angleMax - angle of edges of the slice
rMin, rMax - min and max radius of the slice
minZ, maxZ - min and max z coor
ErrX ErrY ErrZ - errors of hits, see hits format.
PadPitch - pitch
Bz - field, in kGauss
row[j]X - x coordinate of j-th row
ParamS0Par - errors approximation (can be 42 zeros)

1. The hits are stored in the files with names "event[eventNumber]_hits.data"

each file has format

NHits
hit0
hit1
…

where NHits is number of hits
hit0 is info for hit0 in the next format:

X Y Z
ErrX ErrY ErrZ
0 ISlice IRow HitId 0

here xyz - coordinates of the hit in Global coordinate system (same for all detectors). The global z is along beam line, x is horizontal, y is vertical. The global c.s is righthand c.s.
ErrX ErrY ErrZ - covariance matrix for hit position in Local coordinate system. The local c.s. is connected with detector module (row) on which hit is placed, x is normal to the detector, y is parallel and perpendicular to beam, z is parallel both to the module and beam line. The local c.s is righthand c.s. (Usual value is 0, 0.12, 0.16)
ISlice, IRow - slice and row index (starting from zero)
HitId - index of the hit in hit array


Also we store MC information to do efficiency evaluation.

2. MC Points are stored in event[NEvent]_MCPoints.data files

each file contains:

NMCPoints
mcPoint0
mcPoint1
mcPoint2
…


mcPoint is:

X Y Z
Px Py Pz QP
ISlice IRow TrackI TrackID

xyz px py pz - coordinates and momentum of the MCTrack in the MCPoint position (in Global c.s.). So xyz is MCPoint position itself.
QP = q/p - charge over momentum
TrackI - index of the MCTrack in the tracks array (or MCTracks file, see below).
TrackID - simulation id of the MCTrack.

MCPoints are sorter by MCTracks such that TrackID is increased

3. MCTracks are stored in event[NEvent]_MCTracks.data

format is same:
NMCTracks
MCTrack0
MCTrack1
…

MCTrack0 is:
MotherId PDG
x y z ex ey ez QP
0 0 0 0 0 0 0
p pt
NHits NMCPoints FirstMCPointI
0 0 1

MotherId is simulation ID of the MCtrack of the mother particle. For primary tracks it is -1
PDG is PDG code
x y z - global coordinates
ex ey ez is ort, so ex = px/fabs( p ) for example
QP = q/p
p - momentum, pt - is transverse momentum
NHits - number of hits created by the particle
NMCPoints - number of mc points created by the particle (that are stored in the MCpoints-file)
FirstMCPointI - position (index) of the first MCPoint, which belong to this mcTrack, in the MCPoints-file.

4. event[EventNumber]_hitLabels.data - File, which set correspondence between hits and MCTracks. Format is:

NLabels
MCId0_0 MCId1_0 MCId2_0
MCId0_1 MCId1_1 MCId2_1
MCId0_2 MCId1_2 MCId2_2
…

where MCId[j]_[i] is index of the MCTrack, which created i-th hit. We suppose that maximum 3 mcTracks can contribute considerable in a hit. If only one track contributes than MCId1_[i] and MCId2_[i] = -1


======= Output information ===========

RecoTracks are stored in event[NEvent]_tracks.data

format is:
NHits
NTracks
Track0
Track1
…

Track[i] is:
NHits
hitId0 hitId1 ...
alpha nDeDx DeDx
x signCosPhi chi2 ndf
y z sinPhi dzDs q/Pt
Cyy
Czy Czz
CsPy CsPz CsPsP
CdDy CdDz CdDsP CdDdD
CqPy CqPz CqPsP CqPdD CqPqP

where:
alpha - angle of local c.s.
nDeDx - number of hits used in DeDx calculation
DeDx = dE/dX
q - sign of change in assuption that pt > 0
C[i][j] - covariance matrix
chi2 - chi2 after Kaman Filter, equal to -1 in case the fit has diverged
ndf - number degree of freedom



