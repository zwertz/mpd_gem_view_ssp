#include "GEMResults.h"
#include "GEMPlane.h"
#include "GEMDetector.h"
#include "GEMAnalyzer.h"

#include <iostream>
#include <TH1F.h>
#include <TH2F.h>


GEMResult::GEMResult()
{
}

GEMResult::~GEMResult()
{

}

float GEMResult::UVtoX(float U, float V) const{
    //return U;

    float fPxU = cos(-166.75*M_PI/180);
    float fPyU = sin(-166.75*M_PI/180);
    float fPxV = cos(166.75*M_PI/180);
    float fPyV = sin(166.75*M_PI/180);

    float det = fPxU*fPyV-fPyU*fPxV;

    float X = (fPyV*U-fPyU*V)/det;
    //float Y = (fPxU*V-fPxV*U)/det;
    return X;
}

float GEMResult::UVtoY(float U, float V) const{
    //return V;

    float fPxU = cos(-166.75*M_PI/180);
    float fPyU = sin(-166.75*M_PI/180);
    float fPxV = cos(166.75*M_PI/180);
    float fPyV = sin(166.75*M_PI/180);
    
    float det = fPxU*fPyV-fPyU*fPxV;
    
    //float X = (fPyV*U-fPyU*V)/det;
    float Y = (fPxU*V-fPxV*U)/det;
    
    return Y;
}


void GEMResult::Init()
{
    // x plane
    h_cluster_adc_xplane = new TH1F("h_cluster_adc_xplane", "cluster adc x plane", 100, 0, 2000);
    h_cluster_size_xplane = new TH1F("h_cluster_size_xplane", "cluster size x plane", 15, 0, 15);
    h_cluster_pos_xplane = new TH1F("h_cluster_pos_xplane", "cluster position x plane", 600, -300, 300);
    h_strip_adc_xplane = new TH1F("h_strip_adc_xplane", "strip adc x plane", 100, 0, 2000);
    h_strip_pos_xplane = new TH1F("h_strip_pos_xplane", "strip pos x plane", 700, 0, 700);
    h_cluster_pos_xplane_clus1 = new TH1F("h_cluster_pos_xplane_clus1", "cluster position x plane, cluster size 1", 600, -300, 300);

    // y plane
    h_cluster_adc_yplane = new TH1F("h_cluster_adc_yplane", "cluster adc y plane", 100, 0, 2000);
    h_cluster_size_yplane = new TH1F("h_cluster_size_yplane", "cluster size y plane", 15, 0, 15);
    h_cluster_pos_yplane = new TH1F("h_cluster_pos_yplane", "cluster position y plane", 600, -300,300);
    h_strip_adc_yplane = new TH1F("h_strip_adc_yplane", "strip adc y plane", 100, 0, 2000);
    h_strip_pos_yplane = new TH1F("h_strip_pos_yplane", "strip pos y plane", 700, 0, 700);
    h_cluster_pos_yplane_clus1 = new TH1F("h_cluster_pos_yplane_clus1", "cluster position y plane,cluster size 1", 600, -300,300);

    // cluster charge
    h_charge_corr = new TH2F("h_charge_corr", "charge correlation(using maxadc of the cluster)", 200, 0, 2000, 200, 0, 2000);
    h_charge_corr_s = new TH2F("h_charge_corr_s", "charge correlation(using the sum of maxadc of each strip)", 300, 0, 3000, 300, 0, 3000 );
    h_charge_corr_c = new TH2F("h_charge_corr_c","charge correlation (cluster sum)", 500, 0, 10000, 500, 0, 10000);
    h_charge_ratio = new TH1F("h_charge_ratio","cluster charge ratio",200,0,2);

    //hitmap
    h_cluster_2dmap = new TH2F("h_cluster_2dmap", "cluster 2d map", 400, -400, 400, 300, -500, 100);
    
    // strip timing
    h_strip_timing = new TH1F("h_strip_timing", "max time sample", 10, -0.5, 9.5);
}

void GEMResult::FillEvent(GEMDetector* det)
{
    GEMPlane *x_plane = det -> GetXPlane();
    GEMPlane *y_plane = det -> GetYPlane();

    // x plane results
    auto & x_plane_clusters = x_plane -> GetPlaneClusters();
    //auto & x_plane_clusters_1 = x_plane -> GetPlaneClusters();

    auto & x_plane_strips = x_plane -> GetPlaneHits();
    
    // y plane results
    auto & y_plane_clusters = y_plane -> GetPlaneClusters();
    //auto & y_plane_clusters_1 = y_plane -> GetPlaneClusters();

    auto & y_plane_strips = y_plane -> GetPlaneHits();

    // do cluster cuts here...
    // to be implemented

    // fill x plane results
    for(auto &i: x_plane_clusters) {
        h_cluster_adc_xplane -> Fill(i.GetClusterADC());
        h_cluster_size_xplane -> Fill(i.GetClusterSize());
        h_cluster_pos_xplane -> Fill(i.GetClusterPosMoller());
	if(i.GetClusterSize()==1){
	h_cluster_pos_xplane_clus1->Fill(i.GetClusterPosMoller());	
	} 
   }
    for(auto &i: x_plane_strips) {
        h_strip_adc_xplane -> Fill(i.GetMaxADC());
        h_strip_pos_xplane -> Fill(i.GetStripNo());
        h_strip_timing -> Fill(i.GetMaxTSBin());
    }
    // fill y plane results
    for(auto &i: y_plane_clusters) {
        h_cluster_adc_yplane -> Fill(i.GetClusterADC());
        h_cluster_size_yplane -> Fill(i.GetClusterSize());
        h_cluster_pos_yplane -> Fill(i.GetClusterPosMoller());
	//std::cout << i.GetClusterSize() << std::endl;
	if(i.GetClusterSize()==1){
        h_cluster_pos_yplane_clus1->Fill(i.GetClusterPosMoller());
        }
    }
    for(auto &i: y_plane_strips) {
        h_strip_adc_yplane -> Fill(i.GetMaxADC());
        h_strip_pos_yplane -> Fill(i.GetStripNo());
        h_strip_timing -> Fill(i.GetMaxTSBin());
    }

    // fill 2d plots
    int n_clusters = x_plane_clusters.size();
    
    if( (size_t) n_clusters > y_plane_clusters.size()){
         n_clusters = y_plane_clusters.size();
    }

    if(n_clusters > 0)
    {
        for(int i=0; i< n_clusters; ++i) {
            float x_charge = x_plane_clusters[i].GetClusterADC();
            float x_charge_s = x_plane_clusters[i].GetClusterADCmaxsum();
            float x_charge_c = x_plane_clusters[i].GetClusterADCsum();
            float u_pos = x_plane_clusters[i].GetClusterPosMoller();

            float y_charge = y_plane_clusters[i].GetClusterADC();
            float y_charge_s = y_plane_clusters[i].GetClusterADCmaxsum();
            float y_charge_c = y_plane_clusters[i].GetClusterADCsum();
            float v_pos = y_plane_clusters[i].GetClusterPosMoller();

            float x_pos = UVtoX(u_pos,v_pos);
            float y_pos = UVtoY(u_pos,v_pos);

            float charge_ratio = x_charge_c/y_charge_c; 
	    if (y_pos<=0 and y_pos>=-480){
            	h_charge_corr -> Fill(x_charge, y_charge);
           	h_cluster_2dmap -> Fill(x_pos,y_pos);
            	h_charge_corr_s -> Fill(x_charge_s, y_charge_s);
            	h_charge_corr_c -> Fill(x_charge_c, y_charge_c);
            	h_charge_ratio -> Fill(charge_ratio);
	    }

        }
    }
}

void GEMResult::SaveResults()
{
    std::cout<<"Saving results to results.root"<<std::endl;
    
    TFile *f = new TFile("results.root", "recreate");
    //TTree *t = new TTree("result","result");

    //t->Branch("h_cluster_adc_xplane",&h_cluster_adc_xplane);
    //t->Branch("h_cluster_size_xplane",&h_cluster_size_xplane);
    //t->Branch("h_clsuter_pos_xplane",&h_cluster_pos_xplane);
    //t->Branch("h_strip_adc_xplane",&h_strip_adc_xplane);
    //t->Branch("h_strip_pos_xplane",&h_strip_pos_xplane);

    //t->Branch("h_cluster_adc_yplane",&h_cluster_adc_xplane);
    //t->Branch("h_cluster_size_yplane",&h_cluster_size_xplane);
    //t->Branch("h_clsuter_pos_yplane",&h_cluster_pos_xplane);
    //t->Branch("h_strip_adc_yplane",&h_strip_adc_xplane);
    //t->Branch("h_strip_pos_yplane",&h_strip_pos_xplane);

    //h_charge_corr -> SetOption("COLZ");
    //h_cluster_2dmap -> SetOption("COLZ");

    //t->Branch("h_charge_corr",&h_charge_corr);
    //t->Branch("h_cluster_2dmap",&h_cluster_2dmap);

    //t->Branch("h_strip_timing",&h_strip_timing);

    h_cluster_adc_xplane -> Write();
    h_cluster_size_xplane -> Write();
    h_cluster_pos_xplane -> Write();
    h_strip_adc_xplane -> Write();
    h_strip_pos_xplane -> Write();
    h_cluster_pos_xplane_clus1->Write();

    h_cluster_adc_yplane -> Write();
    h_cluster_size_yplane -> Write();
    h_cluster_pos_yplane -> Write();
    h_strip_adc_yplane -> Write();
    h_strip_pos_yplane -> Write();
    h_cluster_pos_yplane_clus1->Write();
    h_charge_corr -> SetOption("COLZ");
    h_charge_corr -> Write();

    h_charge_corr_s -> SetOption("COLZ");
    h_charge_corr_s -> Write();


    h_charge_corr_c -> SetOption("COLZ");
    h_charge_corr_c -> Write();

    h_charge_ratio -> Write();

    h_cluster_2dmap -> SetOption("COLZ");
    h_cluster_2dmap -> Write();

    h_strip_timing -> Write();

    //t->Write();
    f->Close();
}

