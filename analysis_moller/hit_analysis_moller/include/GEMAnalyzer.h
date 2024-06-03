#ifndef GEMANALYZER_H
#define GEMANALYZER_H

#include <string>

#include "GEMDetector.h"
#include "GEMHitTree.h"
#include "GEMResults.h"

class GEMAnalyzer : public GEMHitTree
{
    public:
        GEMAnalyzer();
        ~GEMAnalyzer();

        void analyze();
        void save();

        void SetFileName(const char* str);
        void SetFileNameList(const char* file_list);

    private:
        GEMDetector *gem_det;
        std::string file;

        GEMResult *gem_result;
};

#endif
