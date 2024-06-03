#ifndef GEMHIT_H
#define GEMHIT_H

#include <vector>

class GEMHit
{
public:
    GEMHit();
    GEMHit(const float & a1, const float &a2, const float &a3, 
	    const float &a4, const float &a5, const float &a6);
    ~GEMHit();

    // members
    bool operator==(const GEMHit &h) const;

    // setters
    void SetStripNo(const int &s);
    void SetStripPos(const float &s);
    void SetTimeBinADC(int i, float adc);
    void SetPlaneID(int i);

    // getters
    int GetNTimeSample() const;
    int GetMaxTSBin() const;
    int GetMinTSBin() const;
    int GetStripNo() const;
    float GetStripPos() const;
    float GetMaxADC() const;
    float GetSumADC() const;
    float GetAvgADC() const;
    int GetPlaneID() const;
    float GetMeanTime() const;

    const std::vector<float>& GetTimeBinADC() const;

private:
    int strip_no;
    float strip_pos;
    int plane_id = -1;
    std::vector<float> strip_adc;
};

#endif
