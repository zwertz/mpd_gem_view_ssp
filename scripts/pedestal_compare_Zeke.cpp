
// a helper
TH1F* plot_apv(const vector<float> &v, int crate, int slot, int mpd, int adc, const char* prefix="noise")
{
    if(v.size() != 122) {
        cout<<"Error reading apv data."<<endl;
        return nullptr;
    }

    TH1F *h = new TH1F(Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
                       Form("%s_crate_%d_slot_%d_fiber_%d_adc_%d", prefix, crate, slot, mpd, adc),
            138, -5, 133);

    for(size_t i=0;i<v.size();i++)
    {
        h->SetBinContent(i+5, v[i]);
    }

    return h;
}


// plot pedestal
void pedestal_compare_Zeke(int run1,int run2)
{

  string InPath = Form("../database/gem_ped_%i.dat",run1);
  string datPath =Form("../database/gem_ped_%i.dat",run2);
  
  const char* path = InPath.c_str();
    //const char* path = "../gui/gem_ped.dat";

    TString outDir = "../plots/pedestals/";

    fstream f(path, fstream::in);


    vector<TH1F*> res1;
    

    vector<float> apv_offset1;
    vector<float> apv_noise1;

    TH1F *h_res1 = new TH1F("h_res1","RMS of All Channels;ADC RMS;",267,0,80);
    TH1F *h_off1 = new TH1F("h_off1","Offset of All Channels;ADC Offset;",200,-200,200);

    string line1;
    int mpd1 = -1;
    int adc1 = -1;
    int slot1 = -1;
    int crate1 = -1;
    int current_MPD1 = -1;
    int nMPD1 = 0;
    while(getline(f, line1)) 
    {
      istringstream iss1(line1);
        if(line1.find("APV") != string::npos) {
	  string tmp1;
	  if(mpd1 != -1){
            if(apv_offset1.size() > 0) {
	      int temp_MPD1 = mpd1;
	      if(temp_MPD1 != current_MPD1){
		nMPD1++;
		current_MPD1 = temp_MPD1;
	      }
	      res1.push_back(plot_apv(apv_offset1, crate1,slot1, mpd1, adc1, "offset"));
	      res1.push_back(plot_apv(apv_noise1, crate1,slot1, mpd1, adc1, "noise"));

            }
	  }
	  
	  iss1 >> tmp1 >> crate1 >> slot1 >> mpd1 >> adc1;
	  
	  apv_offset1.clear();
	  apv_noise1.clear();
        } else {
            int strip1;
            float offset1, noise1;
            iss1 >> strip1 >> offset1 >> noise1;
	     
            //change the strips to ingnore the first 6 so that way not included in the plot. We know this comes from cable mismatch. Will be resolved by not reading out those strips. Strip starts counting at 0.
	    if(strip1 <= 5){
            continue;

            }
            else{
	  
            apv_offset1.push_back(offset1);
            apv_noise1.push_back(noise1);
	    
	    h_res1->Fill(noise1);
	    h_off1->Fill(offset1);
     	    }
	   }
    }

    //Do one last time for last APV
    res1.push_back(plot_apv(apv_offset1, crate1, slot1,  mpd1, adc1, "offset"));
    res1.push_back(plot_apv(apv_noise1, crate1, slot1, mpd1, adc1, "noise"));

 const char* mypath = datPath.c_str();

 fstream g(mypath, fstream::in);


    vector<TH1F*> res2;

    vector<float> apv_offset2;
    vector<float> apv_noise2;

    TH1F *h_res2 = new TH1F("h_res2","RMS of All Channels;ADC RMS;",267,0,80);
    TH1F *h_off2 = new TH1F("h_off2","Offset of All Channels;ADC Offset;",200,-200,200);

    string line2;
    int mpd2 = -1;
    int adc2 = -1;
    int slot2 = -1;
    int crate2 = -1;
    int current_MPD2 = -1;
    int nMPD2 = 0;
    while(getline(g, line2)) 
    {
      istringstream iss2(line2);
        if(line2.find("APV") != string::npos) {
	  string tmp2;
	  if(mpd2 != -1){
            if(apv_offset2.size() > 0) {
	      int temp_MPD2 = mpd2;
	      if(temp_MPD2 != current_MPD2){
		nMPD2++;
		current_MPD2 = temp_MPD2;
	      }
	      res2.push_back(plot_apv(apv_offset2, crate2, slot2, mpd2, adc2, "offset"));
	      res2.push_back(plot_apv(apv_noise2, crate2, slot2, mpd2, adc2, "noise"));
            }
	  }
	  
	  iss2 >> tmp2 >> crate2 >> slot2 >> mpd2 >> adc2;
	  
	  apv_offset2.clear();
	  apv_noise2.clear();
        } else {
            int strip2;
            float offset2, noise2;
            iss2 >> strip2 >> offset2 >> noise2;

	    
            //change the strips to ingnore the first 6 so that way not included in the plot. We know this comes from cable mismatch. Will be resolved by not reading out those strips. Strip starts counting at 0.
            if(strip2 <= 5){
            continue;

            }
            else{	  
            apv_offset2.push_back(offset2);
            apv_noise2.push_back(noise2);
	    
	    h_res2->Fill(noise2);
	    h_off2->Fill(offset2);
     	    }
	   }
    }

    //Do one last time for last APV
    res2.push_back(plot_apv(apv_offset2, crate2, slot2, mpd2, adc2, "offset"));
    res2.push_back(plot_apv(apv_noise2, crate2, slot2, mpd2, adc2, "noise"));
   

    // save histos
    TFile *froot1 = new TFile(Form(outDir + "pedestal_%i.root",run1), "recreate");
    for(auto &i: res1)
        i->Write();

    froot1->Close();

// save histos
    TFile *froot2 = new TFile(Form(outDir + "pedestal_%i.root",run2), "recreate");
    for(auto &j: res2)
        j->Write();

    froot2->Close();

    // plot histos
    TCanvas *c_all = new TCanvas("c_all","",1000,800);
    c_all->Divide(2,1);

    //For total pedestal rms histo
    c_all->cd(1);
    //Draw the first set of data in blue
    h_res1->SetLineColor(kBlue); 
    h_res1->Draw("same");
    gPad->Update();   

    //Draw the stat box for first data also in blue
    TPaveStats *stats_rms1 = (TPaveStats*)h_res1->GetListOfFunctions()->FindObject("stats");
    stats_rms1->Draw();
    stats_rms1->SetName("h_res1");
    stats_rms1->SetY1NDC(0.8);
    stats_rms1->SetY2NDC(0.95);
    stats_rms1->SetTextColor(kBlue);
 
    //Draw the second set of data in red
    h_res2->SetLineColor(kRed);
    h_res2->Draw("sames");
    gPad->Update();

    //Draw the stat box for second data also in red
    TPaveStats *stats_rms2 = (TPaveStats*)h_res2->GetListOfFunctions()->FindObject("stats");
    stats_rms2->Draw();
    stats_rms2->SetName("h_res2");
    stats_rms2->SetY1NDC(0.6);
    stats_rms2->SetY2NDC(0.75);
    stats_rms2->SetTextColor(kRed);

    //Add a legend for the histogram
    auto rms_legend = new TLegend(0.5,0.2,0.9,0.3);
    rms_legend->SetTextSize(0.035);
    rms_legend->AddEntry(h_res1,"Some Cable Length","l");
    rms_legend->AddEntry(h_res2,"A diff cable length","l");
    rms_legend->Draw();

    //For total offset histo
    c_all->cd(2);
    //Draw the first set of data in blue
    h_off1->SetLineColor(kBlue);
    h_off1->Draw("same");

    gPad->Update();

    //Draw the stat box for first data also in blue
    TPaveStats *stats_off1 = (TPaveStats*)h_off1->GetListOfFunctions()->FindObject("stats");
    stats_off1->Draw();
    stats_off1->SetName("h_off1");
    stats_off1->SetY1NDC(0.8);
    stats_off1->SetY2NDC(0.95);
    stats_off1->SetTextColor(kBlue);

    //Draw the second set of data in red
    h_off2->SetLineColor(kRed);
    h_off2->Draw("sames");
    gPad->Update();

    //Draw the stat box for second data also in red   
    TPaveStats *stats_off2 = (TPaveStats*)h_off2->GetListOfFunctions()->FindObject("stats");
    stats_off2->Draw();
    stats_off2->SetName("h_off2");
    stats_off2->SetY1NDC(0.6);
    stats_off2->SetY2NDC(0.75);
    stats_off2->SetTextColor(kRed);
 
    //Add a legend for the histogram
    auto off_legend = new TLegend(0.1,0.8,0.38,0.9);
    off_legend->SetTextSize(0.03);
    off_legend->AddEntry(h_off1,"Some Cable Length","l");
    off_legend->AddEntry(h_off2,"A diff cable length","l");
    off_legend->Draw();


    TCanvas *c_rms[nMPD1];
    for(int k = 0;k<nMPD1;k++) {
        c_rms[k] = new TCanvas(Form("c_rms%d", k), Form("c_rms%d", k), 1000, 800);
        c_rms[k] -> Divide(4, 4);
    }

    TCanvas *c_off[nMPD1];
    for(int k = 0;k<nMPD1;k++) {
      c_off[k] = new TCanvas(Form("c_off%d", k), Form("c_off%d", k), 1000, 800);
      c_off[k] -> Divide(4, 4);
    }
    
    TFile *f_pedestal1 = new TFile(Form(outDir + "pedestal_%i.root",run1));
    TFile *f_pedestal2 = new TFile(Form(outDir + "pedestal_%i.root",run2));
    TIter keyList1(f_pedestal1->GetListOfKeys());
    TIter keyList2(f_pedestal2->GetListOfKeys());
    TKey *key1;
    TKey *key2;
    int nCanvas = 0;
    int nPad1 = 0;
    int nPad2 = 0;
    current_MPD1 = -1;
    while( (key1 = (TKey*)keyList1()) && (key2 = (TKey*)keyList2()) ){
        TClass *cl1 = gROOT -> GetClass(key1->GetClassName());
	TClass *cl2 = gROOT -> GetClass(key2->GetClassName());
        if(!cl1->InheritsFrom("TH1") ||!cl2->InheritsFrom("TH1") ) continue;
         
        
        TH1F* h1 = (TH1F*)key1->ReadObj();	
	TH1F* h2 = (TH1F*)key2->ReadObj();
	
        string title = h1->GetTitle();
	//Draw the first set of data in blue
        h1 ->SetLineColor(kBlue);
	//Draw the second set of data in red
        h2 ->SetLineColor(kRed);
	auto start = title.find("fiber_");
	auto end = title.find("_adc");
	TString last = title.back();
	string test = title.substr(start + 6, end - start - 6);
	int temp_MPD = stoi(test);

	if(temp_MPD != current_MPD1){
	  if(current_MPD1 != -1) nCanvas++;
	  current_MPD1 = temp_MPD;
	  
	  nPad1 = 1;
          nPad2 = 1;
	}
	
        if(title.find("noise") != string::npos){
	  if(h1->GetBinContent(10) == 5000 || h1->GetBinContent(10) == 0) continue;
	  //Give histogram more reasonable title for not expert audience
          h1->SetTitle(Form("pedestal_rms_APV_%s", last.Data()));	  
	  //Handle invdividual APV rms plots
          c_rms[nCanvas] -> cd(nPad1);
           //Draw the first set of data in blue
          h1->GetYaxis()->SetRangeUser(0,80);
	  h1->Draw("same");
          gPad->Update();

          //Draw the stat box for first data also in blue     
          TPaveStats *rms1_stats = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
          rms1_stats->Draw();
          rms1_stats->SetName("h1");
          rms1_stats->SetY1NDC(0.8);
          rms1_stats->SetY2NDC(0.95);
          rms1_stats->SetTextColor(kBlue);     
	  
          //Draw the second set of data in red
          h2->GetYaxis()->SetRangeUser(0,80);
          h2->Draw("sames");
          gPad->Update();

          //Draw the stat box for second data also in red
          TPaveStats *rms2_stats = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
          rms2_stats->Draw();
          rms2_stats->SetName("h2");
          rms2_stats->SetY1NDC(0.6);
          rms2_stats->SetY2NDC(0.75);
          rms2_stats->SetTextColor(kRed);

          //Give histogram a legend
           auto legend_rms = new TLegend(0.3,0.80,0.7,0.9);
           legend_rms->SetTextSize(0.035);
           legend_rms->AddEntry(h1,"Some Cable Length","l");
           legend_rms->AddEntry(h2,"A diff cable length","l");
           legend_rms->Draw();


	  nPad1++;
	}
	if(title.find("offset") != string::npos){
          if(h1->GetBinContent(10) == 5000 || h1->GetBinContent(10) == 0) continue;
          //Give histogram more reasonable title for not expert audience
          h1->SetTitle(Form("pedestal_offset_APV_%s", last.Data()));
          //Handle invdividual APV offset plot
          c_off[nCanvas] -> cd(nPad2);
          //Draw the first set of data in blue
	  h1->GetYaxis()->SetRangeUser(-125,150);
          h1->Draw("same");
          gPad->Update();

          //Draw the stat box for first data also in blue
          TPaveStats *off1_stats = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
          off1_stats->Draw();
          off1_stats->SetName("h1");
          off1_stats->SetY1NDC(0.8);
          off1_stats->SetY2NDC(0.95);
          off1_stats->SetTextColor(kBlue);

          //Draw the second set of data in red
	  h2->GetYaxis()->SetRangeUser(-125,150);
	  h2->Draw("sames");
          gPad->Update();

          //Draw the stat box for second data also in red
          TPaveStats *off2_stats = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
          off2_stats->Draw();
          off2_stats->SetName("h2");
          off2_stats->SetY1NDC(0.6);
          off2_stats->SetY2NDC(0.75);
          off2_stats->SetTextColor(kRed);

          //Give histogram a legend
          auto legend_off = new TLegend(0.3,0.80,0.7,0.9);
           legend_off->SetTextSize(0.035);
           legend_off->AddEntry(h1,"Some Cable Length","l");
           legend_off->AddEntry(h2,"A diff cable length","l");
           legend_off->Draw();



          nPad2++;
	}

    }

   
    TString output = Form(outDir + "pedestal_Zeke_%i_%i.pdf",run1,run2);
      
    c_all->Print(output + "(");

    for( int i_can = 0; i_can < nMPD1; i_can++){
      c_rms[i_can]->Print(output);
    }

    for( int i_can = 0; i_can < nMPD1; i_can++){
      if(i_can == nMPD1 - 1) c_off[i_can]->Print(output + ")");
      else c_off[i_can]->Print(output);
    }

}

