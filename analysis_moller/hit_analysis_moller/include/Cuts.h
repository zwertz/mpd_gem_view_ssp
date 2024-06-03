#ifndef CUTS_H
#define CUTS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "ValueType.h"

class GEMHit;
class GEMCluster;

class Cuts
{
    public:
        ~Cuts();

        static Cuts* instance() {
            if(_instance == nullptr)
                _instance = new Cuts();
            return _instance;
        }

        // members
        void SetFile(const char* path);
        void LoadFile();
        void Init(const char* path);
        void Print();

        // helpers
        float __arr_mean(const std::vector<float> &v);
        float __arr_sigma(const std::vector<float> &v);
        float __correlation_coefficient(const std::vector<float> &v1,
                const std::vector<float> &v2);

        void __parse_line(const std::string &line, 
                std::string &key, std::vector<std::string> &val);
        void __convert_map();

        // cuts on hits
        bool max_time_bin(const GEMHit &);
        bool strip_mean_time(const GEMHit &);
        bool reject_max_first_timebin(const GEMHit &);
        bool reject_max_last_timebin(const GEMHit &);

        // cuts on clusters
        bool seed_strip_min_peak_adc(const GEMCluster &);
        bool seed_strip_min_sum_adc(const GEMCluster &);
        bool qualify_for_seed_strip(const GEMHit &);
        // --between seed strip and any single constituent strip
        bool strip_mean_time_agreement(const GEMHit &, const GEMHit &);
        bool time_sample_correlation_coefficient(const GEMHit &, const GEMHit &);
        // --on total number of strips
        bool min_cluster_size(const GEMCluster &);

        // cuts on cluster matching
        bool cluster_adc_assymetry(const GEMCluster &c1, const GEMCluster &c2);

        // cuts on tracking
        bool track_chi2(const std::vector<GEMCluster> &);

    private:
        Cuts(){};

        static Cuts *_instance;
        std::string path;

        std::string tokens = " ,;-:";

        std::unordered_map<std::string, std::vector<std::string>> m_cache;
        std::unordered_map<std::string, ValueType> m_cut;
};

#endif
