#include <iostream>

#include "GEMDetector.h"
#include "GEMPlane.h"
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

GEMDetector::GEMDetector()
{
    // init all GEM Planes
    plane[0] = new GEMPlane();
    plane[0] -> SetID(0);
    plane[0] -> SetGEMDetector(this);

    plane[1] = new GEMPlane();
    plane[1] -> SetID(1);
    plane[1] -> SetGEMDetector(this);

    plane[2] = new GEMPlane();
    plane[2] -> SetID(2);
    plane[2] -> SetGEMDetector(this);

    plane[3] = new GEMPlane();
    plane[3] -> SetID(3);
    plane[3] -> SetGEMDetector(this);


}


GEMDetector::~GEMDetector()
{

}

void GEMDetector::AddHit(GEMHit &h)
{
    int plane_id = h.GetPlaneID();
    if(plane_id != 0 && plane_id != 1) {
        std::cout<<"Error: plane id is not set in GEMHit."<<std::endl;
    }

    plane[plane_id] -> AddHit(h);
}

void GEMDetector::XYClusterMatch()
{
    for(auto &i: plane)
    {
        i -> FormClusters();
        i -> Sort();
    }
}

void GEMDetector::Reset()
{
    for(auto &i: plane)
        i -> Reset();
}
