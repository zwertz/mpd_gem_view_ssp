double myXMinVal_Finder(TH1F *histo){
int da_bin= -1;
double my_num =-1;
	for(int i=1 ; i <= histo->GetNbinsX();i++){
	double dat_bin_val = histo->GetBinContent(i);
		if(dat_bin_val >0){
		//We found the bin and we want the first one
		da_bin=i;
		break;
		}
	}
my_num =(histo->GetXaxis())->GetBinCenter(da_bin);
//cout << my_num << endl;
return my_num;
}

void draw_pad5(TH1F *h1,TH1F *h2,TH1F *h3,TH1F *h4,TH1F *h5,TCanvas *c)
{
    gStyle->SetOptFit(0101);
    c->cd(1);
    h1->Draw();
    int h1_max = h1->GetMaximumBin();
    double h1_max_val = h1->GetXaxis()->GetBinCenter(h1_max);
    double h1_std = h1->GetStdDev();
    double h1_peak = h1->GetBinContent(h1_max);
    //cout << h1_max_val << " " << h1_std << " "<< h1_peak << endl;
    TF1  *fit1 = new TF1("fit1","landau",115,2000);
    //fit1->SetParameter(0,h1_peak);
    //fit1->SetParameter(1,h1_max_val);
    fit1->SetLineColor(kRed);
    fit1->SetLineWidth(3);
    h1->Fit("fit1","qRMS");
    c->Modified();
    c->cd(2);
    h2->Draw();
    c->cd(3);
    h3->Draw();
    c->Modified();
    c->cd(4);
    h4->Draw();
    double h4_min = myXMinVal_Finder(h4);
    double min_4 = h4_min+(2*h4_min)/30;
    TF1 *fit4 = new TF1("fit4","landau",min_4,2000);
    fit4->SetLineColor(kRed);
    fit4->SetLineWidth(3);
    h4->Fit("fit4","qRMS");
    c->cd(5);
    h5->Draw();
    gPad->Update();
}

void draw_pad6(TH2F *h1,TH2F *h2,TH2F *h3,TH2F *h4,TH1F *h5,TH1F *h6,TCanvas *c)
{
    c->cd(1);
    h1->Draw();
    c->cd(2);
    h2->Draw();
    c->cd(3);
    h3->Draw();
    c->cd(4);
    h4->Draw();
    c->cd(5);
    h5->Draw();
    c->cd(6);
    h6->Draw();
    gPad->Update();
}

void draw_pad2(TH1F *h1,TH1F *h2,TCanvas *c){
	c->cd(1);
	h1->Draw();
	c->cd(2);
	h2->Draw();
	gPad->Update();
}

void print_Zeke(const char *file1="results.root",int run_no=0)
{
    TFile *f1 = new TFile(file1);

    //setup canvas
    TCanvas *c = new TCanvas("c","Xplane",1900,1000);
    c->Divide(3,2);
    TCanvas *c1 = new TCanvas("c1","YPlane",1900,1000);
    c1->Divide(3,2);
    TCanvas *c2 = new TCanvas("c2","2dPlane",1900,1000);
    c2->Divide(3,2);
    TCanvas *c3 = new TCanvas("c3","Cluster Postion, Cluster Size 1",1900,1000);
    c3->Divide(2,1);

    TH1F *h1 = (TH1F*) f1 -> Get("h_cluster_adc_xplane");
    TH1F *h2 = (TH1F*) f1 -> Get("h_cluster_size_xplane");
    TH1F *h3 = (TH1F*) f1 -> Get("h_cluster_pos_xplane");
    TH1F *h4 = (TH1F*) f1 -> Get("h_strip_adc_xplane");
    TH1F *h5 = (TH1F*) f1 -> Get("h_strip_pos_xplane");

    TH1F *h6 = (TH1F*) f1 -> Get("h_cluster_adc_yplane");     
    TH1F *h7 = (TH1F*) f1 -> Get("h_cluster_size_yplane");
    TH1F *h8 = (TH1F*) f1 -> Get("h_cluster_pos_yplane");      
    TH1F *h9 = (TH1F*) f1 -> Get("h_strip_adc_yplane");
    TH1F *h10 = (TH1F*) f1 -> Get("h_strip_pos_yplane");

    TH2F *h11 = (TH2F*) f1 -> Get("h_charge_corr");
    TH2F *h12 = (TH2F*) f1 -> Get("h_charge_corr_s");
    TH2F *h13 = (TH2F*) f1 -> Get("h_charge_corr_c");
    TH2F *h14 = (TH2F*) f1 -> Get("h_cluster_2dmap");
    TH1F *h15 = (TH1F*) f1 -> Get("h_charge_ratio");
    TH1F *h16 = (TH1F*) f1 -> Get("h_strip_timing");

    TH1F *h17 = (TH1F*) f1 -> Get("h_cluster_pos_xplane_clus1");
    TH1F *h18 = (TH1F*) f1 -> Get("h_cluster_pos_yplane_clus1");

    draw_pad5(h1,h2,h3,h4,h5,c);
    draw_pad5(h6,h7,h8,h9,h10,c1);
    draw_pad6(h11,h12,h13,h14,h15,h16,c2);
    draw_pad2(h17,h18,c3);

    c->Draw();
    c1->Draw();
    c2->Draw();
    c3->Draw();
    
    char *filename = Form("cosmics_Zeke_%d.pdf(",run_no);
    char *filename1 = Form("cosmics_Zeke_%d.pdf",run_no);
    char *filename2 = Form("cosmics_Zeke_%d.pdf)",run_no); 
   
    c->Print(filename);
    c1->Print(filename1);
    c2->Print(filename1);
    c3->Print(filename2);    
}
