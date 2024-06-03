#ifndef GEMDETECTOR_H
#define GEMDETECTOR_H

#include <vector>

class GEMPlane;
class GEMHit;

class GEMDetector
{
    public:
        GEMDetector();
        ~GEMDetector();

        void AddHit(GEMHit &h);

        void XYClusterMatch();

        void Reset(); // reset for next event

        GEMPlane *GetXPlane() {
            return plane[0];
        }
        GEMPlane *GetYPlane() {
            return plane[1];
        }

    private:
        GEMPlane* plane[2]; // one detector has 2 planes
};

#endif
