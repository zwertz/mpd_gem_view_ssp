#ifndef GEMCLUSTER_H
#define GEMCLUSTER_H

#include "GEMHit.h"

#include "TMath.h"
#include "TVector2.h"
#include <vector>

class GEMCluster
{
public:
    GEMCluster();
    ~GEMCluster();

    bool AddStrip(const GEMHit & hit);

    int GetClusterSize() const;
    float GetClusterADC() const;
    float GetClusterADCmaxsum() const;
    float GetClusterADCsum() const;
    float GetClusterPos() const;
    float GetClusterPosMoller() const;
    //float UVtoX(float, float) const;
    //float UVtoY(float,float) const;


    const std::vector<GEMHit> &GetStrips() const;
    const GEMHit & GetMaxStrip() const;

private:
    std::vector<GEMHit> cluster_strips;
};

#endif
