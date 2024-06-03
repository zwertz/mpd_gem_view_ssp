
// a helper
TH1F* plot_apv(const vector<float> &v, int crate, int slot, int mpd, int adc, const char* prefix="noise")
{
    if(v.size() != 121) {
        cout<<"Error reading apv data."<<endl;
        return nullptr;
    }

    TH1F *h = new TH1F(Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
		       /*Form("%s_APV_%d", prefix, adc),*/
                       Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
            138, -5, 133);

    for(size_t i=0;i<v.size();i++)
    {
        h->SetBinContent(i+5, v[i]);
    }

    return h;
}

// plot pedestal
void plot_pedestal_no7chan(int run, const char *cable_length)
{

  string InPath = Form("../database/gem_ped_%i.dat",run);
  
  const char* path = InPath.c_str();
    //const char* path = "../gui/gem_ped.dat";

    TString outDir = "../plots/pedestals/";

    fstream f(path, fstream::in);
    vector<TH1F*> res;

    vector<float> apv_offset;
    vector<float> apv_noise;

    TH1F *h_res = new TH1F("h_res","RMS of All Channels;ADC RMS;",167,0,50);
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
	    //change the strips to ingnore the first 7 so that way not included in the plot. We know this comes from cable mismatch. Will be resolved by not reading out those strips. Strip starts counting at 0.
            if(strip <= 6){
            continue;
            
            }
            else{
            apv_offset.push_back(offset);
            apv_noise.push_back(noise);
	    //if(mpd>=20&&adc<3){
	    h_resVoff->Fill(noise,offset);
	    h_res->Fill(noise);
	    h_off->Fill(offset);
            }
            }
        //}
    }

    //Do one last time for last APV
    res.push_back(plot_apv(apv_offset, crate, slot, mpd, adc, "offset"));
    res.push_back(plot_apv(apv_noise, crate, slot, mpd, adc, "noise"));
    

    // save histos
    
    TFile *froot = new TFile(Form(outDir + "pedestal_no7chan_%i.root",run), "recreate");
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
  
    //Add a legend for the histogram
    auto rms_legend = new TLegend(0.5,0.2,0.9,0.3);
    rms_legend->SetTextSize(0.035);
    rms_legend->AddEntry(h_res,cable_length,"l");
    rms_legend->Draw();   

    c_all->cd(2);
    h_off->Draw();

    //Add a legend for the histogram
    auto off_legend = new TLegend(0.1,0.8,0.38,0.9);
    off_legend->SetTextSize(0.03);
    off_legend->AddEntry(h_off,cable_length,"l");
    off_legend->Draw();




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
    
    TFile *f_pedestal = new TFile(Form(outDir + "pedestal_no7chan_%i.root",run));
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
        TString last = title.back();
        //cout << title[27] << endl;
	if(title.find("fiber_") == std::string::npos) continue;

	string test = title.substr(start + 6, end - start - 6);
	int temp_MPD = stoi(test);

	if(temp_MPD != current_MPD){
	  if(current_MPD != -1) nCanvas++;
	  current_MPD = temp_MPD;
	  
	  nPad = 1;
	}
	
        if(title.find("noise") != string::npos){
	  if(h->GetBinContent(10) == 5000 || h->GetBinContent(10) == 0) continue;
	 // cout << last << endl;
          h->SetTitle(Form("pedestal_fiber_%i_rms_APV_%s", current_MPD,last.Data()));
	  c_rms[nCanvas] -> cd(nPad);
          h->GetYaxis()->SetRangeUser(0,50);
	  h->Draw();

          //Give histogram a legend
          auto legend_rms = new TLegend(0.3,0.80,0.7,0.9);
           legend_rms->SetTextSize(0.035);
           legend_rms->AddEntry(h,cable_length,"l");
           legend_rms->Draw();

     

	  nPad++;
	}
	if(title.find("offset") != string::npos){
          if(h->GetBinContent(10) == 5000 || h->GetBinContent(10) == 0) continue;
          //cout << last << endl;
          c_off[nCanvas] -> cd(nPad);
          h->GetYaxis()->SetRangeUser(-125,150);
          h->SetTitle(Form("pedestal_offset_fiber_%i_APV_%s",current_MPD,last.Data()));
          h->Draw();

          //Give histogram a legend
          auto legend_off = new TLegend(0.3,0.80,0.7,0.9);
           legend_off->SetTextSize(0.035);
           legend_off->AddEntry(h,cable_length,"l");
           legend_off->Draw();

         
	}

    }

    
    TString output = Form(outDir + "pedestal_no7chan_%i.pdf",run);
      
    c_all->Print(output + "(");


    for( int i_can = 0; i_can < nMPD; i_can++)
      c_rms[i_can]->Print(output);
    

    for( int i_can = 0; i_can < nMPD; i_can++){
      if(i_can == nMPD - 1) c_off[i_can]->Print(output + ")");
      else c_off[i_can]->Print(output);
    }

}

