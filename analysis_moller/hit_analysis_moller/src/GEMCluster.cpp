#include "GEMCluster.h"

#include <iostream>
#include <cmath>

GEMCluster::GEMCluster()
{
}

GEMCluster::~GEMCluster()
{
}

bool GEMCluster::AddStrip(const GEMHit &hit)
{
    int s = (int)cluster_strips.size();

    if(s <= 0) {
        cluster_strips.push_back(hit);
        return true;
    }

    if(s == 1) {
        int strip_id = cluster_strips[0].GetStripNo();
        if( abs(strip_id - hit.GetStripNo()) == 1)
        {
            cluster_strips.push_back(hit);
            return true;
        }
        return false;
    }

    int strip_id_lhs = cluster_strips[0].GetStripNo();
    int strip_id_rhs = cluster_strips[s-1].GetStripNo();
    int add_id = hit.GetStripNo();

    if( (abs(strip_id_lhs - add_id) == 1) ||
            (abs(strip_id_rhs - add_id) == 1) ) {
        cluster_strips.push_back(hit);
        return true;
    }
    return false;
}

int GEMCluster::GetClusterSize() const
{
    return cluster_strips.size();
}

float GEMCluster::GetClusterADC() const
{
    // using max adc
    if(cluster_strips.size() <= 0)
	return -9999;

    float adc = cluster_strips[0].GetMaxADC();

    for(auto &i: cluster_strips)
	if(adc < i.GetMaxADC())
	    adc = i.GetMaxADC();

    return adc;
}

float GEMCluster::GetClusterADCmaxsum() const
{
    //using the max adc sum
    if(cluster_strips.size()<=0)
        return -9999;
    
    float sum = 0;

    for(auto &i: cluster_strips){
        sum += i.GetMaxADC();
    }
    
    return sum;
}


float GEMCluster::GetClusterADCsum() const
{
        //using the max adc sum
    if(cluster_strips.size()<=0)
        return -9999;
        
    float sum = 0;
    for(auto &i: cluster_strips){
        sum += i.GetSumADC();
    }
        
    return sum;
}
        

float GEMCluster::GetClusterPos() const
{
    if(cluster_strips.size() <= 0)
	return -9999;

    float pos = 0;
    float charge = 0;

    // charge gravity center
    for(auto &i: cluster_strips)
    {
	pos += i.GetMaxADC() * i.GetStripPos();
	charge += i.GetMaxADC();
    }

    if(charge == 0)
	return -9999.;

    pos /= charge;

    return pos;
}

float GEMCluster::GetClusterPosMoller() const
{
    if(cluster_strips.size() <= 0)
        return -9999;
    /*
    float pitch = 0.80; //correct these values later
    float offset = 0; //correct these values later
    float Nstrips = 640;
    float hitpos = 0;
    */

    float charge = 0;
    float pos = 0; 

    // charge gravity center
    for(auto &i: cluster_strips)
    {   
        //hitpos = (i.GetStripPos()+0.5-0.5*Nstrips)*pitch+offset;

        //hitpos = i.GetStripPos()+0.5-0.5*Nstrips + offset;
        float hitpos = i.GetStripPos();

        pos += i.GetMaxADC() * hitpos;
        charge += i.GetMaxADC();
    }

    if(charge == 0)
        return -9999.;
    pos /= charge;
    return pos;
}

/*float GEMCluster::UVtoX(float U, float V) const{
    float fPxU = cos(165*M_PI/180);
    float fPyU = sin(165*M_PI/180);
    float fPxV = cos(-165*M_PI/180);
    float fPyV = sin(-165*M_PI/180);

    float det = fPxU*fPyV-fPyU*fPxV;

    float X = (fPyV*U-fPyU*V)/det;
    //float Y = (fPxU*V-fPxV*U)/det;
    
    return X;
}

float GEMCluster::UVtoY(float U, float V) const{
    float fPxU = cos(165*M_PI/180);
    float fPyU = sin(165*M_PI/180);
    float fPxV = cos(-165*M_PI/180);
    float fPyV = sin(-165*M_PI/180);

    float det = fPxU*fPyV-fPyU*fPxV;

    //float X = (fPyV*U-fPyU*V)/det;
    float Y = (fPxU*V-fPxV*U)/det;

    return Y;
}
*/

const std::vector<GEMHit> & GEMCluster::GetStrips() const
{
    return cluster_strips;
}

const GEMHit & GEMCluster::GetMaxStrip() const
{
    if(cluster_strips.size() <= 0)
    {
        std::cout<<"Error: empty cluster."<<std::endl;
        exit(0);
    }

    size_t pos = 0;
    float adc = cluster_strips[pos].GetMaxADC();
    for(size_t i=1; i<cluster_strips.size(); ++i)
    {
        float tmp = cluster_strips[i].GetMaxADC();
        if(adc < tmp) {
            pos = i;
            adc = tmp;
        }
    }

    return cluster_strips[pos];
}
