#include "Cuts.h"
#include "GEMHit.h"
#include "GEMCluster.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using std::fstream;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::istringstream;

Cuts* Cuts::_instance = nullptr;

////////////////////////////////////////////////////////////////////////////////
//                                 members                                    //
////////////////////////////////////////////////////////////////////////////////

Cuts::~Cuts()
{
}

void Cuts::SetFile(const char* _path)
{
    path = _path;
}

void Cuts::Init(const char* path)
{
    SetFile(path);
    LoadFile();

    __convert_map();
}

void Cuts::LoadFile()
{
    fstream input_file(path.c_str(), fstream::in);
    if(!input_file.is_open()) {
        cout<<"Warning:: couldn't load config file for cuts"
            <<endl;
    }

    string line;
    while(std::getline(input_file, line))
    {
        string key;
        vector<string> val;

        __parse_line(line, key, val);

        if(key.size() > 0) {
            m_cache[key] = val;
        }
    }
}

void Cuts::Print()
{
    cout<<"============================================================="<<endl;
    cout<<"cuts applied: "<<endl;
    cout<<"-------------------------------------------------------------"<<endl;
    for(auto &i: m_cache) {
        cout<<std::setfill(' ')<<std::setw(36)<<i.first;
        cout<<" : ";
        for(auto &j: i.second)
            cout<<std::setfill(' ')<<std::setw(10)<<j;
        cout<<endl;
    }
    cout<<"============================================================="<<endl;
}

////////////////////////////////////////////////////////////////////////////////
//                                    cuts                                    //
////////////////////////////////////////////////////////////////////////////////

bool Cuts::max_time_bin(const GEMHit &hit)
{
    int timebin = hit.GetMaxTSBin();

    for(auto &i: m_cut["max time bin"].arr<int>())
    {
        if(i == timebin)
            return true;
    }

    return false;
}

bool Cuts::strip_mean_time(const GEMHit &hit)
{
    float mean_time = hit.GetMeanTime();

    auto mean_time_range = m_cut["strip mean time"].arr<float>();

    if(mean_time >= mean_time_range[0] && mean_time <= mean_time_range[1])
        return true;

    return false;
}

bool Cuts::reject_max_first_timebin(const GEMHit &hit)
{
    int max_bin = hit.GetMaxTSBin();

    if(max_bin != 0)
        return true;

    if(m_cut["reject max first bin"].val<bool>())
        return false;

    return true;
}

bool Cuts::reject_max_last_timebin(const GEMHit &hit)
{
    int max_bin = hit.GetMaxTSBin();

    int n_ts = hit.GetNTimeSample() - 1;

    if(max_bin != n_ts)
        return true;

    if(m_cut["reject max last bin"].val<bool>())
        return false;

    return true;
}

bool Cuts::seed_strip_min_peak_adc(const GEMCluster &cluster)
{
    auto & hit = cluster.GetMaxStrip();
    float adc = hit.GetMaxADC();

    float minimum_adc = m_cut["seed strip min peak ADC"].val<float>();

    if(adc >= minimum_adc)
        return true;

    return false;
}

bool Cuts::seed_strip_min_sum_adc(const GEMCluster &cluster)
{
    auto &hit = cluster.GetMaxStrip();
    float adc = hit.GetSumADC();

    float minimum_adc = m_cut["seed strip min sum ADC"].val<float>();

    if(adc >= minimum_adc)
        return true;
    return false;
}

bool Cuts::qualify_for_seed_strip(const GEMHit &hit)
{
    float peak_adc = hit.GetMaxADC();
    float sum_adc = hit.GetSumADC();

    float min_peak = m_cut["seed strip min peak ADC"].val<float>();
    float min_sum = m_cut["seed strip min sum ADC"].val<float>();

    if( (peak_adc >= min_peak) && (sum_adc >= min_sum))
        return true;

    return false;
}

bool Cuts::strip_mean_time_agreement(const GEMHit &hit1, const GEMHit &hit2)
{
    float m1 = hit1.GetMeanTime();
    float m2 = hit2.GetMeanTime();

    float diff = abs(m1 - m2);

    float criteria = m_cut["strip mean time agreement"].val<float>();

    if(diff <= criteria)
        return true;
    return false;
}

bool Cuts::time_sample_correlation_coefficient(const GEMHit &hit1, const GEMHit &hit2)
{
    auto & bin_charge1 = hit1.GetTimeBinADC();
    auto & bin_charge2 = hit2.GetTimeBinADC();

    float correlation = __correlation_coefficient(bin_charge1, bin_charge2);
    float criteria = m_cut["time sample correlation coefficient"].val<float>();

    if(correlation >= criteria)
        return true;
    return false;
}

bool Cuts::min_cluster_size(const GEMCluster &cluster)
{
    int cluster_size = cluster.GetClusterSize();

    int m = m_cut["min cluster size"].val<int>();

    if(cluster_size >= m)
        return true;
    return false;
}

bool Cuts::cluster_adc_assymetry(const GEMCluster &c1, const GEMCluster &c2)
{
    float c1_adc = c1.GetClusterADC();
    float c2_adc = c2.GetClusterADC();

    float assymetry = abs(c1_adc - c2_adc) / abs(c1_adc + c2_adc);
    
    float criteria = m_cut["2d cluster adc assymetry"].val<float>();

    if(assymetry <= criteria)
        return true;
    return false;
}

bool Cuts::track_chi2(const std::vector<GEMCluster> &vc)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//                                 helpers                                    //
////////////////////////////////////////////////////////////////////////////////

void Cuts::__parse_line(const string &line, string &key, vector<string> &val)
{
    // replace tokens with white space
    auto __replace_tokens = [&](const string & s) -> string
    {
        string l = s;
        char whitespace = ' ';
        for(char &c: tokens)
        {
            if(c == whitespace)
                continue;
            size_t pos = l.find(c);
            if(pos != string::npos) {
                l.replace(pos, 1, 1, whitespace);
            }
        }
        return l;
    };

    // trim leading and trailing spaces
    auto __trim_space = [&](const string &s) -> string
    {
        if(s.size() <= 0)
            return string("");

        size_t pos1 = s.find_first_not_of(tokens);
        size_t pos2 = s.find_last_not_of(tokens);

        if(pos2 < pos1)
            return string("");

        size_t length = pos2 - pos1;

        return s.substr(pos1, length+1);
    };

    // separate key and values
    auto __parse_key_value_string = [&](const string &s, string &key, string &val)
    {
        size_t length = s.size();
        if(length <= 0)
            return;

        size_t pos = s.find('=');

        if(pos == string::npos || pos == 0 || pos == (length -1) )
        {
            cout<<"Warning:: format is incorrect, must be: \"key = value\" format."
                <<endl;
            return;
        }

        key = s.substr(0, pos);
        val = s.substr(pos+1, length - pos + 1);
    };

    // remove comments
    auto __remove_comments = [&](const string &s) -> string
    {
        string res = s;

        size_t length = s.size();
        if(length <= 0)
            return res;

        // comments are leading by '#'
        size_t pos = s.find('#');

        if(pos == 0) {
            res.clear();
            return res;
        }

        if(pos == string::npos)
            return res;

        res = s.substr(0, pos);
        return res;
    };

    // parse value list
    auto __separate_vals = [&](const string &s, vector<string> &res)
    {
        res.clear();
        string temp = __trim_space(s);
        temp = __replace_tokens(temp);

        istringstream iss(temp);
        string t;
        while(iss >> t)
        {
            res.push_back(t);
        }
    };

    // parse line
    string temp1 = __remove_comments(line);
    string temp2;
    __parse_key_value_string(temp1, key, temp2);
    key = __trim_space(key);
    __separate_vals(temp2, val);
}

void Cuts::__convert_map()
{
    for(auto &i: m_cache)
    {
        ValueType val(i.second);
        m_cut[i.first] = val;
    }
}

float Cuts::__arr_mean(const vector<float> &v)
{
    float res = 0;

    if(v.size() <= 0)
        return res;

    for(auto &i: v)
        res += i;

    int n = (int)v.size();

    return res / n;
}

float Cuts::__arr_sigma(const vector<float> &v)
{
    float sigma = 0;

    if(v.size() <= 1)
        return sigma;

    float mean = __arr_mean(v);

    float square_sum = 0;
    for(auto &i: v)
        square_sum += pow(i - mean, 2);

    int n = (int)v.size();
    square_sum = square_sum / (n - 1);

    sigma = sqrt(square_sum);

    return sigma;
}

float Cuts::__correlation_coefficient(const vector<float> &v1,
        const vector<float> &v2)
{
    float coefficient = 0;

    if(v1.size() != v2.size()) {
        cout<<"Warning:: correlation coefficient must be between two arrays with same length"
            <<endl; 
        return coefficient;
    }

    if(v1.size() <= 1) {
        cout<<"Warning:: correlation coefficient must be between two arrays with length > 1"
            <<endl; 
        return coefficient;
    }

    float cross_product = 0;
    float x_avg = __arr_mean(v1);
    float y_avg = __arr_mean(v2);

    for(size_t i=0; i<v1.size(); ++i)
    {
        cross_product += (v1[i] - x_avg) * (v2[i] - y_avg);
    }

    float x_sigma = __arr_sigma(v1);
    float y_sigma = __arr_sigma(v2);

    cross_product = cross_product / (x_sigma * y_sigma);

    int n = (int)v1.size();
    coefficient = cross_product / (n - 1);
    return coefficient;
}
