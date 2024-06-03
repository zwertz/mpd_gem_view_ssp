#include "GEMHit.h"
#include "Cuts.h"

GEMHit::GEMHit()
{
}

GEMHit::~GEMHit()
{
}

GEMHit::GEMHit(const float & a1, const float &a2, const float &a3, 
        const float &a4, const float &a5, const float &a6)
{
    strip_adc.clear();
    strip_adc.push_back(a1);
    strip_adc.push_back(a2);
    strip_adc.push_back(a3);
    strip_adc.push_back(a4);
    strip_adc.push_back(a5);
    strip_adc.push_back(a6);
}

int GEMHit::GetNTimeSample() const
{
    return strip_adc.size();
}

int GEMHit::GetMaxTSBin() const
{
    if(strip_adc.size() <= 0)
        return -1;

    float Max = strip_adc[0];
    int bin = 0;
    int index = 0;
    for(auto &i: strip_adc)
    {
        if(i > Max) {
            Max = i;
            bin = index;
        }
        index++;
    }

    return bin;
}

int GEMHit::GetMinTSBin() const
{
    if(strip_adc.size() <= 0)
        return -1;

    float Min = strip_adc[0];
    int bin = 0;
    int index = 0;
    for(auto &i: strip_adc)
    {
        if(i < Min) {
            Min = i;
            bin = index;
        }
        index++;
    }
    return bin;
}

float GEMHit::GetMaxADC() const
{
    if(strip_adc.size() <= 0)
        return -9999;

    float max = strip_adc[0];
    for(auto &i: strip_adc) {
        if(max < i)
            max = i;
    }

    return max;
}

float GEMHit::GetSumADC() const
{
    if(strip_adc.size() <= 0)
        return -9999;

    float sum = 0;
    for(auto &i: strip_adc)
        sum += i;

    return sum;
}

float GEMHit::GetAvgADC() const
{
    float sum = GetSumADC();
    int n = GetNTimeSample();

    return sum/(float)n;
}

void GEMHit::SetStripNo(const int &s)
{
    strip_no = s;
}

void GEMHit::SetStripPos(const float &s)
{
    strip_pos = s;
}

int GEMHit::GetStripNo() const
{
    return strip_no;
}

float GEMHit::GetStripPos() const
{
    return strip_pos;
}

void GEMHit::SetTimeBinADC(int i, float adc)
{
    if(strip_adc.size() < (size_t)i)
        strip_adc.resize(i);

    strip_adc[i] = adc;
}

void GEMHit::SetPlaneID(int i)
{
    plane_id = i;
}

int GEMHit::GetPlaneID() const
{
    return plane_id;
}

const std::vector<float> & GEMHit::GetTimeBinADC() const
{
    return strip_adc;
}

float GEMHit::GetMeanTime() const
{
    float charge_sum = GetSumADC();

    float mean_time = 0;

    for(size_t i=0; i<strip_adc.size(); ++i)
    {
        mean_time += strip_adc[i] * (float)(i+1.) * 25.0;
    }

    if(charge_sum != 0)
        mean_time /= charge_sum;
    else
        mean_time = 0.;

    return mean_time;
}

bool GEMHit::operator==(const GEMHit& h) const
{
    if(strip_no != h.GetStripNo())
        return false;

    if(plane_id != h.GetPlaneID())
        return false;

    auto & rhs_strip_adc = h.GetTimeBinADC();

    if(strip_adc.size() != rhs_strip_adc.size())
        return false;

    for(size_t i=0; i<strip_adc.size(); ++i)
    {
        if(strip_adc[i] != rhs_strip_adc[i])
            return false;
    }

    return true;
}
