#include <algorithm>
#include <utility>

//#define MOLLER_UV_CHAMBER
#define TOTAL_LAYERS 4
#define N_CHAMBERS_PER_LAYER 3

//variable that keeps track layer type
//static const int TOTAL_LAYERS = 4;
//std:string myLayers = {INFN,INFN,INFN,MOLLER};

#define N_CANVAS_PER_LAYER 9
TCanvas *LayerC[TOTAL_LAYERS][N_CANVAS_PER_LAYER];

vector<string> v_root_files = {
    //"../Rootfiles/cluster_0_e1209016_SBSGEMs_1452.root",
    //"../Rootfiles/cluster_0_e1209016_SBSGEMs_1450.root",
    //"../Rootfiles/cluster_0_e1209016_SBSGEMs_1449.root",
    //"../Rootfiles/cluster_0_uva_xray_1010.evio.0.root",
    //"../Rootfiles/cluster_0_uva_xray_1011.evio.0.root",
    //"../Rootfiles/cluster_0_e1209016_SBSGEMs-vtp2_1047.evio.0.0.root",
    //"../Rootfiles/cluster_0_uva_xray_1023.evio.0.root",
    //"../Rootfiles/cluster_0_hallc_fadc_ssp_3340..root",
    //"../Rootfiles/cluster_0_s1..root",
    //"../Rootfiles/cluster_0_e1209016_4899.root"
    //"../Rootfiles/cluster_0_ssp_gem_349.root"
    //"../Rootfiles/cluster_0_ssp_gem_apv_test_756.root"
    //"cluster_0_ssp_gem_apv_test_756.root"
    "../Rootfiles/cluster_0_ssp_gem_apv_test_770.root"
};

////////////////////////////////////////////////////////////////////////////////
// uv coordinate to xy coordinate transform
//
// uv_angle is the strip angle between u and v in the upper plane, 60 degree in sbs uv chambers
//
// viewing from the top of the chamber, with the chamber short side (has 8 APVs) on you right hand
//     u channel connects the upper half APV connectors
//     v channel connects the lower half APV connectors

template<typename T1, typename T2>
std::pair<T1, T1> convert_uv_to_xy(const T2& _u, const T2& _v, T2 uv_angle)
{
    //uv_angle = 26.5;
    T1 u = static_cast<T1>(_u);
    T1 v = static_cast<T1>(_v);

    T1 angle = static_cast<T1>(uv_angle) * TMath::Pi() / 180.;

    T1 x, y;

    // 1558, 406
    // 1766.4, 406

    u += 20;
    v -= 406 * TMath::Sin(angle/2.);

    y = -0.5 * ( (u-v)/TMath::Tan(angle/2.) - 406);
    x = 0.5*(u+v);
    //
    //   x = (u+v)*TMath::Cos(angle/2.);
    //   y = 4*(v-u)*TMath::Sin(angle/2.);

   //x = (u*TMath::Sin(angle) - v*TMath::Sin(-angle))/(TMath::Cos(-angle)*TMath::Sin(angle) - TMath::Sin(-angle)*TMath::Cos(angle));
   //y = (v*TMath::Cos(-angle) - u*TMath::Cos(angle))/(TMath::Cos(-angle)*TMath::Sin(angle) - TMath::Sin(-angle)*TMath::Cos(angle));  

    return std::make_pair<T1&, T1&>(x, y);
}


////////////////////////////////////////////////////////////////////////////////
// gem cluster struct

struct GEMCluster
{
    int layerID, detID, layerPosIndex, plane, size;
    float adc, pos;

    GEMCluster():
        layerID(-1), detID(-1), layerPosIndex(-1), plane(-1), size(-1),
        adc(0), pos(-9999)
    {
    }

    GEMCluster(const GEMCluster &that):
        layerID(that.layerID), detID(that.detID), layerPosIndex(that.layerPosIndex),
        plane(that.plane), size(that.size), adc(that.adc), pos(that.pos)
    {
    }

    GEMCluster(GEMCluster &&that):
        layerID(that.layerID), detID(that.detID), layerPosIndex(that.layerPosIndex),
        plane(that.plane), size(that.size), adc(that.adc), pos(that.pos)
    {
    }

    // copy assignment
    GEMCluster& operator=(const GEMCluster &rhs)
    {
        if(this == &rhs)
            return *this;
        GEMCluster c(rhs);
        *this = move(c);
        return *this;
    }

    // move assignment
    GEMCluster & operator=(GEMCluster &&rhs) {
        if(this == &rhs)
            return *this;
        layerID = rhs.layerID;
        detID = rhs.detID;
        layerPosIndex = rhs.layerPosIndex;
        plane = rhs.plane;
        size = rhs.size;
        adc = rhs.adc;
        pos = rhs.pos;
        return *this;
    }
};

// print cluster
ostream& operator<<(ostream& out, const GEMCluster& c) {
    out<<" layer id: "<<c.layerID
        <<" det id: "<<c.detID
        <<" layer pos index: "<<c.layerPosIndex
        <<" plane id: "<<c.plane
        <<" size: "<<c.size
        <<" adc: "<<c.adc
        <<" pos: "<<c.pos
        <<endl;
    return out;
}

// reconstructed gem 2d hit point
struct GEM2DHit 
{
    GEMCluster xc, yc;
    GEM2DHit()
    {}

    GEM2DHit(const GEMCluster &a, const GEMCluster &b)
        :xc(a), yc(b)
    {}
};


// clusters on one plane
struct PlaneCluster
{
    int planeID;
    vector<GEMCluster> clusters;

    PlaneCluster():
        planeID(-1)
    {
        clusters.clear();
    }

    void addCluster(const GEMCluster& c)
    {
        clusters.push_back(c);
    }

    // must be in desending order
    void Sort() {
        sort(clusters.begin(), clusters.end(), 
                [](const GEMCluster &c1, const GEMCluster &c2)
                {
                return c1.adc > c2.adc; // desending order
                });
    }

    void Clear(){
        clusters.clear();
    }
};

// clusters on one chamber
struct ChamberCluster
{
    int chamberPosIndex;
    PlaneCluster plane_cluster[2]; // 2 planes: x/y, u/v

    vector<GEM2DHit> Hits2D;

    ChamberCluster():
        chamberPosIndex(-1)
    {
    }

    void addCluster(const GEMCluster &c)
    {
        int id = c.plane;
        if(id < 0 || id > 1) {
            cout<<__func__<<": plane index is incorrect: "<<id<<endl;
            return;
        }
        plane_cluster[id].addCluster(c);
    }

    void Sort() {
        plane_cluster[0].Sort();
        plane_cluster[1].Sort();
    }

    void Match() 
    {
        Sort();

        // get 2d hits
        int nCluster = plane_cluster[0].clusters.size();
        if(nCluster > plane_cluster[1].clusters.size())
            nCluster = plane_cluster[1].clusters.size();

        for(int i=0;i<nCluster;i++) {
            GEM2DHit hit(plane_cluster[0].clusters[i],
                    plane_cluster[1].clusters[i]);
            Hits2D.push_back(hit);
        }

        // clear this event, get ready for next event
        for(auto &i: plane_cluster)
            i.Clear();
    }
};

// clusters on one layer
struct LayerCluster
{
    int layerID;
    ChamberCluster chamber_cluster[N_CHAMBERS_PER_LAYER];

    LayerCluster():
        layerID(-1)
    {
    }

    void addCluster(const GEMCluster &c) 
    {
        int id = c.layerPosIndex;
        if(id < 0 || id > 3) {
            cout<<"incorrect chamber pos index: "<< id<<" on layer: "<<layerID<<endl;
            return;
        }
        chamber_cluster[id].addCluster(c);
    }

    void Match() {
        for(auto &i: chamber_cluster)
            i.Match();
    }
};

// show 2d cluster map for 1 layer
TH2F* GetLayerCluster2DMap(const LayerCluster &layer_cluster, const int &nlayer)
{
   TH2F *hClusterMap = new TH2F(Form("hClusterMap%d", nlayer), 
            Form("cluster 2d map layer %d", nlayer), 
            400, -800, 800,
           900, -900, 900);

    for(auto &chamber : layer_cluster.chamber_cluster) {
        for(auto &c: chamber.Hits2D) {
//#ifdef MOLLER_UV_CHAMBER
if (nlayer == 3){
            // convert uv to xy
            std::pair<float, float> res = convert_uv_to_xy<float, float>(c.yc.pos, c.xc.pos, 26.5);
            hClusterMap -> Fill(res.first, res.second);
		}
//#else
else{
            //hClusterMap -> Fill(c.xc.pos, c.yc.pos);
            //hClusterMap -> Fill(c.yc.pos, c.xc.pos); // TCS coordinate in Hall, 12-slot plane is the x axis
            hClusterMap -> Fill(c.yc.pos, -c.xc.pos); // flip the vertical axis, make module 0 on top side
}
//#endif
        }
    }

    hClusterMap -> GetXaxis() -> SetLabelSize(0.06);
    hClusterMap -> GetXaxis() -> SetLabelOffset(-0.03);
    hClusterMap -> GetXaxis() -> SetTitle("X [mm]");
    hClusterMap -> GetXaxis() -> CenterTitle();
    hClusterMap -> GetXaxis() -> SetTitleOffset(0.2);

    hClusterMap -> GetYaxis() -> SetLabelSize(0.06);
    hClusterMap -> GetYaxis() -> SetLabelOffset(0.01);
    hClusterMap -> GetYaxis() -> SetTitle("Y [mm]");
    hClusterMap -> GetYaxis() -> CenterTitle();
    hClusterMap -> GetYaxis() -> SetTitleOffset(0.8);

    return hClusterMap;
}

// show results for each layer after matching
// chamber by chamber, one layer has N_CHAMBERS_PER_LAYER chambers
// contents include (for each chamber):
//      cluster size(x and y), adc(x and y), cluster_pos(x and y)
//      charge correlation (x vs y), size correlation (x vs y), cluster 2d map (x vs y)
void ShowLayerResults(const LayerCluster &layer_cluster, const int &nlayer)
{
    //TCanvas *c[9];

    int chamber_id = 0;
    for(auto &chamber: layer_cluster.chamber_cluster) 
    {
        // get y position
        float x_size = 60; // mm
        float x_min = -x_size/2.;
        float x_max =  x_size/2.;
        float y_size = 60; // mm
        float y_min = -y_size/2.;
        float y_max = y_size/2.;
        // hists
        TH1F *cluster_size_x = new TH1F(Form("x_cluster_size_chamber%d/%d", chamber_id, nlayer),
                Form("x cluser size chamber_%d layer_%d", chamber_id, nlayer),
                20, -0.5, 19.5);
        cluster_size_x -> GetXaxis() -> SetTitle("cluster size");
        cluster_size_x -> GetXaxis() -> CenterTitle();
        TH1F *cluster_size_y = new TH1F(Form("y_cluster_size_chamber%d/%d", chamber_id, nlayer),
                Form("y cluser size chamber_%d layer_%d", chamber_id, nlayer),
                20, -0.5, 19.5);
        cluster_size_y -> GetXaxis() -> SetTitle("cluster size");
        cluster_size_y -> GetXaxis() -> CenterTitle();

        TH1F *cluster_adc_x = new TH1F(Form("x_cluster_adc_chamber%d/%d", chamber_id, nlayer),
                Form("x cluser adc chamber_%d layer_%d", chamber_id, nlayer),
                100, 0, 2500);
        cluster_adc_x -> GetXaxis() -> SetTitle("X channel ADC");
        cluster_adc_x -> GetXaxis() -> CenterTitle();

        TH1F *cluster_adc_y = new TH1F(Form("y_cluster_adc_chamber%d/%d", chamber_id, nlayer),
                Form("y cluser adc chamber_%d layer_%d", chamber_id, nlayer),
                100, 0, 2500);
        cluster_adc_y -> GetXaxis() -> SetTitle("Y channel ADC");
        cluster_adc_y -> GetXaxis() -> CenterTitle();

        TH1F *cluster_pos_x = new TH1F(Form("x_cluster_pos_chamber%d/%d", chamber_id, nlayer),
                Form("x cluser pos chamber_%d layer_%d", chamber_id, nlayer),
                100, x_min, x_max);
        cluster_pos_x -> GetXaxis() -> SetTitle("strip index");
        cluster_pos_x -> GetXaxis() -> CenterTitle();

        TH1F *cluster_pos_y = new TH1F(Form("y_cluster_pos_chamber%d/%d", chamber_id, nlayer),
                Form("y cluser pos chamber_%d layer_%d", chamber_id, nlayer),
                100, x_min, x_max);
        cluster_pos_y -> GetXaxis() -> SetTitle("strip index");
        cluster_pos_y -> GetXaxis() -> CenterTitle();

        TH2F *charge_correlation = new TH2F(Form("charge_correlation_chamber%d/%d", chamber_id, nlayer),
                Form("charge_correlation chamber_%d layer_%d", chamber_id, nlayer),
                1000, 0, 2500, 1000, 0, 2500);
        charge_correlation -> GetXaxis() -> SetTitle("X channel cluster charge");
        charge_correlation -> GetXaxis() -> CenterTitle();
        charge_correlation -> GetYaxis() -> SetTitle("Y channel cluster charge");
        charge_correlation -> GetYaxis() -> CenterTitle();

        TH2F *size_correlation = new TH2F(Form("size_correlation_chamber%d/%d", chamber_id, nlayer),
                Form("size_correlation chamber_%d layer_%d", chamber_id, nlayer),
                20, -0.5, 19.5, 20, -0.5, 19.5);
        TH2F *position_correlation = new TH2F(Form("position_correlation_chamber%d/%d", chamber_id, nlayer),
                Form("pos_correlation chamber_%d layer_%d", chamber_id, nlayer),
                100, -60, 60, 100, -60, 60);

        for(auto &hit2d: chamber.Hits2D) 
        {
            cluster_size_x -> Fill(hit2d.xc.size);
            cluster_size_y -> Fill(hit2d.yc.size);
            cluster_adc_x ->  Fill(hit2d.xc.adc);
            cluster_adc_y ->  Fill(hit2d.yc.adc);
            cluster_pos_x ->  Fill(hit2d.xc.pos);
            cluster_pos_y ->  Fill(hit2d.yc.pos);
            charge_correlation -> Fill(hit2d.xc.adc, hit2d.yc.adc);
            size_correlation   -> Fill(hit2d.xc.size, hit2d.yc.size);
            position_correlation -> Fill(hit2d.xc.pos, hit2d.yc.pos);
        }
        // show histos
        gStyle -> SetTitleFontSize(0.1);

        LayerC[nlayer][0] = new TCanvas(Form("c_layer%d_chamber%d_page0", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        position_correlation->Draw("col");

        LayerC[nlayer][1] = new TCanvas(Form("c_layer%d_chamber%d_page_1", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_size_x -> Draw();

        LayerC[nlayer][2] = new TCanvas(Form("c_layer%d_chamber%d_page_2", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_size_y -> Draw();

        LayerC[nlayer][3] = new TCanvas(Form("c_layer%d_chamber%d_page_3", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_adc_x -> Draw(); 

        LayerC[nlayer][4] = new TCanvas(Form("c_layer%d_chamber%d_page_4", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_adc_y -> Draw(); 

        LayerC[nlayer][5] = new TCanvas(Form("c_layer%d_chamber%d_page_5", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_pos_x -> Draw(); 

        LayerC[nlayer][6] = new TCanvas(Form("c_layer%d_chamber%d_page_6", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        cluster_pos_y -> Draw(); 

        LayerC[nlayer][7] = new TCanvas(Form("c_layer%d_chamber%d_page_7", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        charge_correlation -> Draw("col");

        LayerC[nlayer][8] = new TCanvas(Form("c_layer%d_chamber%d_page_8", nlayer, chamber_id), Form("layer %d chamber %d page %d", nlayer, chamber_id, 0), 900, 600);
        size_correlation -> Draw("col");

        chamber_id++;
    }

    //return c;
}

// main 
void show_cluster_2d_map(const char* root_input_path = "")
{
    const int NLayer = TOTAL_LAYERS;
    std::map<int, LayerCluster> layer_cluster;

    int evtID;
    int nCluster;

    vector<int> *layerID = nullptr;
    vector<int> *detID = nullptr;
    vector<int> *layerPosIndex = nullptr;
    vector<int> *plane = nullptr;
    vector<int> *size = nullptr;
    vector<double> *adc = nullptr;
    vector<double> *pos = nullptr;
    TChain *T = new TChain("GEMCluster");

    //cout<<"adding file: "<<root_input_path<<endl;
    //T -> Add(root_input_path);
    for(auto &i: v_root_files)
    {
        cout<<"adding file: "<<i<<endl;
        T -> Add(i.c_str());
    }

    T->SetBranchAddress("evtID", &evtID);
    T->SetBranchAddress("nCluster", &nCluster);
    T->SetBranchAddress("planeID", &layerID);
    T->SetBranchAddress("prodID", &detID);
    T->SetBranchAddress("moduleID", &layerPosIndex);
    T->SetBranchAddress("axis", &plane);
    T->SetBranchAddress("size", &size);
    T->SetBranchAddress("adc", &adc);
    T->SetBranchAddress("pos", &pos);

    int Entries = T->GetEntries();
    cout<<"total enries: "<<Entries<<endl;

    // decode
    for(int entry = 0;entry<Entries;entry++) 
    {
        if(entry > 300000) break;
        std::cout<<entry<<endl;
        T->GetEntry(entry);
        //std::cout<<"debug1"<<endl;
        // loop for each cluster
        for(int i=0; i<nCluster; i++) {
            //std::cout<<"debug - "<<i<<endl;
            GEMCluster cluster;
            //std::cout<<"debug a "<<i<<endl;
            cluster.layerID = layerID->at(i);
            //std::cout<<"debug b "<<i<<endl;
            cluster.detID = detID->at(i);
            //std::cout<<"debug c "<<i<<endl;
            cluster.layerPosIndex = layerPosIndex->at(i);
            //std::cout<<"debug d "<<i<<endl;
            cluster.plane = plane->at(i);
            //std::cout<<"debug e "<<i<<endl;
            cluster.size = size->at(i);
            //std::cout<<"debug f "<<i<<endl;
            cluster.adc = adc->at(i);
            //std::cout<<"debug g "<<i<<endl;
            cluster.pos = pos->at(i);
            //std::cout<<"debug h "<<i<<endl;
            layer_cluster[layerID->at(i)].addCluster(cluster);//this is where the seg fault occurs
            //std::cout<<"debug :: "<<i<<endl;
        }
        //std::cout<<"debug2"<<endl;
        // match clusters
        for(auto &i: layer_cluster)
            i.second.Match();
    }
    //std::cout<<"debug3"<<endl;

    // show results
    // cluster 2d map
    //TCanvas *c_layer_2d = new TCanvas("c_layer_2d", "layer 2d hit map", 1800, 950);
    //c_layer_2d -> Divide(NLayer, 1);
    TCanvas *c_layer_2d[NLayer];
    TH2F *hClusterMap[NLayer];
    for(auto &i: layer_cluster) 
    {
        hClusterMap[i.first] = GetLayerCluster2DMap(i.second, i.first);

        c_layer_2d[i.first] = new TCanvas(Form("c_layer_2d%d", i.first), "layer 2d hit map", 1800, 950); 
        hClusterMap[i.first] -> Draw("colz");
    }
    bool first_page = true;
    for(auto &i: layer_cluster)
    {
        if(first_page) {
            c_layer_2d[i.first] -> Print("results.pdf(");
            first_page = false;
        } else {
            c_layer_2d[i.first] -> Print("results.pdf");
        }
    }

    for(int i=0; i<TOTAL_LAYERS; i++) {
        for(int j=0; j<N_CANVAS_PER_LAYER; j++)
            LayerC[i][j] = nullptr;
    }

    // cluster size
    for(auto &i: layer_cluster)
    {
        ShowLayerResults(i.second, i.first);
    }

    for(int i=0; i<TOTAL_LAYERS; i++) {
        for(int j=0; j<N_CANVAS_PER_LAYER; j++)
        {
            if(LayerC[i][j] != nullptr)
                LayerC[i][j] -> Print("results.pdf");
        }
    }

    // close the pdf file
    TCanvas *c_blank = new TCanvas("c_blank", "blank", 900, 900);
    c_blank -> Print("results.pdf)");
}
