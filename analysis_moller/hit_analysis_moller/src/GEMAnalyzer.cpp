#include "GEMAnalyzer.h"
#include "GEMHit.h"

#include <iostream>
#include <TTree.h>
#include <TFile.h>

GEMAnalyzer::GEMAnalyzer()
{
    gem_det = new GEMDetector();
    gem_result = new GEMResult();
}

GEMAnalyzer::~GEMAnalyzer()
{

}

void GEMAnalyzer::analyze()
{
    std::cout<<"Analyzing file: "<<file<<std::endl;
    std::cout<<"Getting tree with name: GEMHit"<<std::endl;
    TFile *_f = new TFile(file.c_str());
    TTree *T = (TTree*)_f->Get("GEMHit");

    // init tree
    Init(T);
    // init results
    gem_result -> Init();

    int entries = fChain -> GetEntries();
    std::cout<<"total entries: "<<entries<<std::endl;

    for(int entry = 0; entry < entries; ++entry)
    {
        // get event
        fChain -> GetEntry(entry);

        // loop all fired strips in this event
        for(int i=0; i<nch; ++i) {
            GEMHit hit(adc0[i], adc1[i], adc2[i], adc3[i], adc4[i], adc5[i]);
            hit.SetPlaneID(axis[i]);
            hit.SetStripNo(strip[i]);

            gem_det -> AddHit(hit);
        }

        // calculate clusters for this event
        gem_det -> XYClusterMatch();

        // save result
        gem_result -> FillEvent(gem_det);

        // reset for next event
        gem_det -> Reset();
    }
}

void GEMAnalyzer::SetFileName(const char* str)
{
    file = str;
    //Vimukthi : Add this segment to get the run number isolated from the filename
    std::string s(str);
    std::string delimiter1 = ".";
    std::string delimiter2 = "_";
    size_t pos=0;

    std::string token1 = s.substr(0,s.find(delimiter1));
    //std::cout<<"token 1 : "<<token1<<std::endl;
    
    std::string token2;
    std::string run_no; 
    while ((pos = token1.find(delimiter2)) != std::string::npos){
        token2 = token1.substr(0,pos);
        token1.erase(0,pos+delimiter2.length());
    }
    run_no = token1;
    std::cout<<"run number : "<<run_no<<std::endl;
}


void GEMAnalyzer::SetFileNameList(const char* list)
{
    // to be implemented
}

void GEMAnalyzer::save()
{
    gem_result -> SaveResults();
}
