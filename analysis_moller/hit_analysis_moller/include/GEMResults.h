#ifndef GEMRESULT_H
#define GEMRESULT_H

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

class GEMDetector;

class GEMResult
{
public:
    GEMResult();
    ~GEMResult();
    
    float UVtoX(float,float) const;
    float UVtoY(float,float) const;

    void FillEvent(GEMDetector *);

    void SaveResults();

    void Init();

private:
    // x palne
    TH1F *h_cluster_adc_xplane; // cluster adc
    TH1F *h_cluster_size_xplane; // clusetr size
    TH1F *h_cluster_pos_xplane; // cluster position
    TH1F *h_strip_adc_xplane; // strip adc
    TH1F *h_strip_pos_xplane; // strip position
    TH1F *h_cluster_pos_xplane_clus1;

    // y palen
    TH1F *h_cluster_adc_yplane; // cluster adc
    TH1F *h_cluster_size_yplane; // clusetr size
    TH1F *h_cluster_pos_yplane; // cluster position
    TH1F *h_strip_adc_yplane; // strip adc
    TH1F *h_strip_pos_yplane; // strip position
    TH1F *h_cluster_pos_yplane_clus1;


    TH2F *h_charge_corr; // cluster charge correlation
    TH2F *h_charge_corr_s;
    TH2F *h_charge_corr_c;
    TH1F *h_charge_ratio;

    TH2F *h_cluster_2dmap; // cluster 2d map

    // timing distribution
    TH1F *h_strip_timing; // max timing distribution

};
#endif
