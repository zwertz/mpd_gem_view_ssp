#include <iostream>
#include "GEMAnalyzer.h"
#include "Cuts.h"
#include "ValueType.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cout<<"How to use the program: "<<std::endl;
        std::cout<<"./main <hit_root_file_name.root>"<<std::endl;
        exit(0);
    }

    // init cuts
    Cuts::instance() -> Init("config/cuts_Zeke.conf");
    Cuts::instance() -> Print();

    GEMAnalyzer *ana =new GEMAnalyzer();
    ana -> SetFileName(argv[1]);
    ana -> analyze();
    ana -> save();

    return 0;
}
