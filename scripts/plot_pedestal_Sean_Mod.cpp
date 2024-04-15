////////////////////////////////////////////////////////////////////////////////
//// UVa XY GEM Detector
//// a strip mapping from danning, combines three stages conversion
//// this is original and believed to be the right one, Jan 09 2017
//// 1) apv internal tree structure mapping:
////    strip = 32*(ch%4) + 8*(ch/4) - 31*(ch/16);
//// 2) apv chip to on-board panasonic connector mapping:
////    strip = ch + 1 + ch%4 - 5 * ( (ch/4) % 2 );
//// 3) panasonic connector to detector strip mapping:
////    strip = ch
//
const int mapped_strip_uva_xy[128] = {
     1,  33, 65,  97,  9,  41, 73, 105, 17,  49, 
     81, 113, 25,  57, 89, 121,  3,  35, 67,  99, 
     11,  43, 75, 107, 19,  51, 83, 115, 27,  59, 
     91, 123,  5,  37, 69, 101, 13,  45, 77, 109, 
     21,  53, 85, 117, 29,  61, 93, 125,  7,  39, 
     71, 103, 15,  47, 79, 111, 23,  55, 87, 119, 
     31,  63, 95, 127,  0,  32, 64,  96,  8,  40, 
     72, 104, 16,  48, 80, 112, 24,  56, 88, 120, 
     2,  34, 66,  98, 10,  42, 74, 106, 18,  50, 
     82, 114, 26,  58, 90, 122,  4,  36, 68, 100, 
     12,  44, 76, 108, 20,  52, 84, 116, 28,  60, 
     92, 124,  6,  38, 70, 102, 14,  46, 78, 110, 
     22,  54, 86, 118, 30,  62, 94, 126
};

// a helper
TH1F* plot_apv(const vector<float> &v, int crate, int slot, int mpd, int adc, const char* prefix="noise")
{
    if(v.size() != 128) {
        cout<<"Error reading apv data."<<endl;
        return nullptr;
    }

    TH1F *h = new TH1F(Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
		       Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
            138, -5, 133);

    for(size_t i=0;i<v.size();i++)
    {
	h->SetBinContent(mapped_strip_uva_xy[i],v[i]);
        //h->SetBinContent(i+5, v[i]);
    }

    return h;
}

// plot pedestal
void plot_pedestal_Sean_Mod(int run)
{

  string InPath = Form("../database/gem_ped_%i.dat",run);
  
  const char* path = InPath.c_str();
    //const char* path = "../gui/gem_ped.dat";

    TString outDir = "../plots/pedestals/";

    fstream f(path, fstream::in);
    vector<TH1F*> res;

    vector<float> apv_offset;
    vector<float> apv_noise;

    TH1F *h_res = new TH1F("h_res","RMS of All Channels;ADC RMS;",200,0,60);
    TH1F *h_off = new TH1F("h_off","Offset of All Channels;ADC Offset;",200,-200,200);
    TH2F *h_resVoff = new TH2F("h_resVoff",";RMS;Offset",200,0,60,200,-200,200);
    string line;
    int mpd = -1;
    int adc = -1;
    int crate = -1;
    int slot = -1;
    int current_MPD = -1;
    int nMPD = 0;
    while(getline(f, line)) 
    {
      istringstream iss(line);
        if(line.find("APV") != string::npos) {
	  string tmp;
	  if(mpd != -1){
            if(apv_offset.size() > 0) {
	      int temp_MPD = mpd;
	      if(temp_MPD != current_MPD){
		nMPD++;
		current_MPD = temp_MPD;
	      }
	      res.push_back(plot_apv(apv_offset, crate, slot, mpd, adc, "offset"));
	      res.push_back(plot_apv(apv_noise, crate, slot, mpd, adc, "noise"));
            }
	  }
	  
	  iss >> tmp >> crate >> slot >> mpd >> adc;
	  
	  apv_offset.clear();
	  apv_noise.clear();
        } else {
            int strip;
            float offset, noise;
            iss >> strip >> offset >> noise;
	  
            apv_offset.push_back(offset);
            apv_noise.push_back(noise);
	    //if(mpd>=20&&adc<3){
	    h_resVoff->Fill(noise,offset);
	    h_res->Fill(noise);
	    h_off->Fill(offset);}
        //}
    }

    //Do one last time for last APV
    res.push_back(plot_apv(apv_offset, crate, slot, mpd, adc, "offset"));
    res.push_back(plot_apv(apv_noise, crate, slot, mpd, adc, "noise"));
    

    // save histos
    TFile *froot = new TFile(Form(outDir + "pedestal_%i.root",run), "recreate");
    for(auto &i: res)
        i->Write();
    h_res->Write();
    h_off->Write(); 
    h_resVoff->Write();
   froot->Close();	

    // plot histos
    TCanvas *c_all = new TCanvas("c_all","",1000,800);
    c_all->Divide(2,1);

    c_all->cd(1);
    h_res->Draw();
    c_all->cd(2);
    h_off->Draw();

    TCanvas *c_rms[nMPD];
    for(int k = 0;k<nMPD;k++) {
        c_rms[k] = new TCanvas(Form("c_rms%d", k), Form("c_rms%d", k), 1000, 800);
        c_rms[k] -> Divide(4, 4);
    }

    TCanvas *c_off[nMPD];
    for(int k = 0;k<nMPD;k++) {
      c_off[k] = new TCanvas(Form("c_off%d", k), Form("c_off%d", k), 1000, 800);
      c_off[k] -> Divide(4, 4);
    }
    
    TFile *f_pedestal = new TFile(Form(outDir + "pedestal_%i.root",run));
    TIter keyList(f_pedestal->GetListOfKeys());
    TKey *key;
    int nCanvas = 0;
    int nPad = 0;
    current_MPD = -1;
    while( (key = (TKey*)keyList()) ){
        TClass *cl = gROOT -> GetClass(key->GetClassName());
        if(!cl->InheritsFrom("TH1")) continue;

        TH1F* h = (TH1F*)key->ReadObj();
        string title = h->GetTitle();
	
	auto start = title.find("fiber_");
	auto end = title.find("_adc");

	if(title.find("fiber_") == std::string::npos) continue;

	string test = title.substr(start + 6, end - start - 6);
	int temp_MPD = stoi(test);

	if(temp_MPD != current_MPD){
	  if(current_MPD != -1) nCanvas++;
	  current_MPD = temp_MPD;
	  
	  nPad = 1;;
	}
	
        if(title.find("noise") != string::npos){
	  if(h->GetBinContent(10) == 5000 || h->GetBinContent(10) == 0) continue;
	  
	  c_rms[nCanvas] -> cd(nPad);
	  h->Draw();

	  nPad++;
	}
	if(title.find("offset") != string::npos){
          if(h->GetBinContent(10) == 5000 || h->GetBinContent(10) == 0) continue;

          c_off[nCanvas] -> cd(nPad);
          h->Draw();
         
	}

    }

    
    TString output = Form(outDir + "pedestal_%i.pdf",run);
      
    c_all->Print(output + "(");


    for( int i_can = 0; i_can < nMPD; i_can++)
      c_rms[i_can]->Print(output);
    

    for( int i_can = 0; i_can < nMPD; i_can++){
      if(i_can == nMPD - 1) c_off[i_can]->Print(output + ")");
      else c_off[i_can]->Print(output);
    }

}

