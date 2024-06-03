#include "GEMPlane.h"
#include "Config.h"
#include "Cuts.h"

#include <iostream>
#include <algorithm>

GEMPlane::GEMPlane()
{
}

GEMPlane::~GEMPlane()
{
}

float GEMPlane::GetStripPosition(GEMHit &h)
{
    int strip_no = h.GetStripNo();

    return GetStripPosition(strip_no);
}

float GEMPlane::GetStripPosition(const int &strip_no)
{
    // origin is in plane center
    float plane_length = 0;

    if(plane_id == 0)
        plane_length = config::apv_length * config::n_apvs_x_plane;
    else
        plane_length = config::apv_length * config::n_apvs_y_plane;

    float strip_pos = -plane_length / 2. + strip_no * config::strip_pitch;

    //std::cout<<"x apvs: "<<config::n_apvs_x_plane<<", y apvs: "<<config::n_apvs_y_plane<<", apv length: "<<config::apv_length<<std::endl;
    //std::cout<<"plane length: "<<plane_length<<", strip position: "<<strip_pos<<std::endl;
    return strip_pos;
}


// sort clusters in desending order
void GEMPlane::Sort()
{
    std::sort(plane_clusters.begin(), plane_clusters.end(),
            [](const GEMCluster &c1, const GEMCluster &c2) 
            {
            if(c1.GetClusterADC() > c2.GetClusterADC())
            return true;
            return false;
            }
            );
}

// group clusters
void GEMPlane::FormClusters()
{
    if(plane_hits.size() <= 0)
        return;

    // sort according to strip index
    std::sort(plane_hits.begin(), plane_hits.end(),
            [](const GEMHit &h1, const GEMHit &h2)
            {
            if(h1.GetStripNo() < h2.GetStripNo())
            return true;
            return false;
            }
            );

    auto beg = plane_hits.begin();
    auto end = plane_hits.end();

    auto cbeg = beg;
    for(auto it = beg; it != end; ++it)
    {
        auto it_n = it + 1;

        if( (it_n == end) || ((it_n->GetStripNo() - it->GetStripNo()) != 1) )
        {
            // find cluster range: (c_beg -> it_n)
            GEMCluster cluster;
            for(auto cit=cbeg; cit != it_n; ++cit)
                cluster.AddStrip(*cit);

            std::vector<GEMCluster> tmp;

            if(Cut(cluster, tmp)) {
                for(auto &cc: tmp)
                    AddCluster(cc);
            }

            cbeg = it_n;
        }
    }
}

bool GEMPlane::Cut(GEMHit &h)
{
    if(!Cuts::instance() -> max_time_bin(h))
        return false;
    if(!Cuts::instance() -> strip_mean_time(h))
        return false;
    if(!Cuts::instance() -> reject_max_first_timebin(h))
        return false;
    if(!Cuts::instance() -> reject_max_last_timebin(h))
        return false;

    return true;
}

bool GEMPlane::Cut(GEMCluster &cluster, std::vector<GEMCluster> &v_c)
{
    if(!Cuts::instance() -> seed_strip_min_peak_adc(cluster))
        return false;
    if(!Cuts::instance() -> seed_strip_min_sum_adc(cluster))
        return false;

    // filter for each strips in cluster
    auto & strips = cluster.GetStrips();

    __regroup_clusters(strips, v_c);

    if(v_c.size() >= 0)
        return true;

    return false;
}

void GEMPlane::__regroup_clusters(const std::vector<GEMHit> &hits, std::vector<GEMCluster> &vclusters)
{
    // get seed strip index from a vector of strips
    auto get_seed_strip = [&](const std::vector<GEMHit> &vhits, int &index) -> bool
    {
        index = -1;
        if(vhits.size() <= 0)
            return false;

        index = 0;
        float max_adc = vhits[index].GetMaxADC();
        for(size_t i=1; i<vhits.size(); i++)
        {
            if(max_adc < vhits[i].GetMaxADC()) {
                max_adc = vhits[i].GetMaxADC();
                index = i;
            }
        }

        if(Cuts::instance() -> qualify_for_seed_strip(vhits[index]))
            return true;

        return false;
    };

    int seed_strip_id;
    if(!get_seed_strip(hits, seed_strip_id))
        return;

    // re-form cluster around seed strip
    GEMCluster tmp_cluster;
    if(!tmp_cluster.AddStrip(hits[seed_strip_id]))
        return;

    std::vector<GEMHit> temp;
    for(size_t i=0; i<hits.size(); ++i)
    {
        if(i == (size_t)seed_strip_id)
            continue;

        if( (Cuts::instance() -> strip_mean_time_agreement(hits[i], hits[seed_strip_id])) &&
                (Cuts::instance() -> time_sample_correlation_coefficient(hits[i], hits[seed_strip_id]))
          )
        {
            if(!tmp_cluster.AddStrip(hits[i]))
                temp.push_back(hits[i]);
        }
        else
        {
            temp.push_back(hits[i]);
        }
    }

    if(Cuts::instance() -> min_cluster_size(tmp_cluster))
        vclusters.push_back(tmp_cluster);

    __regroup_clusters(temp, vclusters);
}

void GEMPlane::AddHit(GEMHit &h)
{
    // first calculate strip position
    float pos = GetStripPosition(h);
    h.SetStripPos(pos);

    // second, filter clusters
    if(!Cut(h))
        return;

    plane_hits.push_back(h);
}
