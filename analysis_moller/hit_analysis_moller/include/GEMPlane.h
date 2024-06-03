#ifndef GEMPLANE_H
#define GEMPLANE_H

#include <vector>
#include "GEMCluster.h"
#include "GEMHit.h"

class GEMDetector;

class GEMPlane
{
    public:
        GEMPlane();
        ~GEMPlane();

        void AddHit(GEMHit & h);
        void AddCluster(const GEMCluster& c) {plane_clusters.push_back(c);}

        void SetID(const int &id) {plane_id = id;}
        void SetGEMDetector(GEMDetector* det) {gem_det = det;}
        void FormClusters();
        // a helper
        void __regroup_clusters(const std::vector<GEMHit> &, std::vector<GEMCluster> &);

        bool Cut(GEMHit &h);
        bool Cut(GEMCluster &c, std::vector<GEMCluster> &);
        void Sort();
        float GetStripPosition(GEMHit &h);
        float GetStripPosition(const int &strip_no);

        void Reset() {
            plane_clusters.clear();
            plane_hits.clear();
        }

        // getters
        int GetNumberOfClusters() {return plane_clusters.size();}
        std::vector<GEMCluster> &GetPlaneClusters() {return plane_clusters;}
        std::vector<GEMHit> &GetPlaneHits(){return plane_hits;}

    private:
        std::vector<GEMCluster> plane_clusters;
        std::vector<GEMHit> plane_hits;

        int plane_id; // 0 for x ,  1 for y
        GEMDetector *gem_det; 
};

#endif
