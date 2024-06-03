void draw_pad5(TH1F *h1,TH1F *h2,TH1F *h3,TH1F *h4,TH1F *h5,TCanvas *c)
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

void print_to_pdf(const char *file1="results.root",int run_no=0)
{
    TFile *f1 = new TFile(file1);

    //setup canvas
    TCanvas *c = new TCanvas("c","Xplane",1900,1000);
    c->Divide(3,2);
    TCanvas *c1 = new TCanvas("c1","YPlane",1900,1000);
    c1->Divide(3,2);
    TCanvas *c2 = new TCanvas("c2","2dPlane",1900,1000);
    c2->Divide(3,2);

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

    draw_pad5(h1,h2,h3,h4,h5,c);
    draw_pad5(h6,h7,h8,h9,h10,c1);
    draw_pad6(h11,h12,h13,h14,h15,h16,c2);

    c->Draw();
    c1->Draw();
    c2->Draw();
    
    char *filename = Form("cosmics_%d.pdf(",run_no);
    char *filename1 = Form("cosmics_%d.pdf",run_no);
    char *filename2 = Form("cosmics_%d.pdf)",run_no); 
   
    c->Print(filename);
    c1->Print(filename1);
    c2->Print(filename2);    
}
