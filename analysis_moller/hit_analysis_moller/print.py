import ROOT as R

tc1=R.TCanvas()
tc1.Divide(3,2)

tc2=R.TCanvas()
tc2.Divide(3,2)

tc3=R.TCanvas()
tc3.Divide(2,2)

tf1=R.TFile("results.root")

h1=tf1.Get("h_cluster_adc_xplane")
h2=tf1.Get("h_cluster_size_xplane")
h3=tf1.Get("h_cluster_pos_xplane")
h4=tf1.Get("h_strip_adc_xplane")
h5=tf1.Get("h_strip_pos_xplane")

h6=tf1.Get("h_cluster_adc_yplane")
h7=tf1.Get("h_cluster_size_yplane")
h8=tf1.Get("h_cluster_pos_yplane")
h9=tf1.Get("h_strip_adc_yplane")
h10=tf1.Get("h_strip_pos_yplane")

h11=tf1.Get("h_charge_corr")
h12=tf1.Get("h_cluster_2dmap")
h13=tf1.Get("h_strip_timing")

tc1.cd(1)
h1.Draw()
tc1.cd(2)
h2.Draw()
tc1.cd(3)
h3.Draw()
tc1.cd(4)
h4.Draw()
tc1.cd(5)
h5.Draw()

tc2.cd(1)
h6.Draw()
tc2.cd(2)
h7.Draw()
tc2.cd(3)
h8.Draw()
tc2.cd(4)
h9.Draw()
tc2.cd(5)
h10.Draw()

tc3.cd(1)
h11.Draw()
tc3.cd(2)
h12.Draw()
tc3.cd(3)
h13.Draw()


tc1.Draw()
tc2.Draw()
tc3.Draw()

tc1.Print("cosmics.pdf(","pdf")
tc2.Print("cosmics.pdf","pdf")
tc3.Print("cosmics.pdf)","pdf")


