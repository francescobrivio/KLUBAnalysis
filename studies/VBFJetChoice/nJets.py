import ROOT

fin = ROOT.TFile('fOut.root')

# B Jets
hnjB = ROOT.gDirectory.FindObjectAny('nJetsB')
hnjB0p0 = ROOT.gDirectory.FindObjectAny('nJetsB0p0')
hnjB0p5 = ROOT.gDirectory.FindObjectAny('nJetsB0p5')
hnjB0p8 = ROOT.gDirectory.FindObjectAny('nJetsB0p8')
hnjB0p9 = ROOT.gDirectory.FindObjectAny('nJetsB0p9')

hnjB.SetLineColor(ROOT.kRed)
hnjB0p0.SetLineColor(ROOT.kGreen)
hnjB0p5.SetLineColor(ROOT.kBlue)
hnjB0p8.SetLineColor(ROOT.kBlack)
hnjB0p9.SetLineColor(ROOT.kCyan)

leg1 = ROOT.TLegend(0.7,0.7,0.9,0.9)
leg1.AddEntry(hnjB,    'no cut', 'l')
leg1.AddEntry(hnjB0p0, 'PUjetID > 0.0', 'l')
leg1.AddEntry(hnjB0p5, 'PUjetID > 0.5', 'l')
leg1.AddEntry(hnjB0p8, 'PUjetID > 0.8', 'l')
leg1.AddEntry(hnjB0p9, 'PUjetID > 0.9', 'l')

c1 = ROOT.TCanvas()
c1.cd()
hnjB0p8.Draw()
hnjB0p9.Draw('same')
hnjB0p5.Draw('same')
hnjB0p0.Draw('same')
hnjB.Draw('same')
leg1.Draw('same')
c1.SaveAs('nJetsB.pdf','pdf')



# VBF Jets
hnjVBF = ROOT.gDirectory.FindObjectAny('nJetsVBF')
hnjVBF0p0 = ROOT.gDirectory.FindObjectAny('nJetsVBF0p0')
hnjVBF0p5 = ROOT.gDirectory.FindObjectAny('nJetsVBF0p5')
hnjVBF0p8 = ROOT.gDirectory.FindObjectAny('nJetsVBF0p8')
hnjVBF0p9 = ROOT.gDirectory.FindObjectAny('nJetsVBF0p9')

hnjVBF.SetLineColor(ROOT.kRed)
hnjVBF0p0.SetLineColor(ROOT.kGreen)
hnjVBF0p5.SetLineColor(ROOT.kBlue)
hnjVBF0p8.SetLineColor(ROOT.kBlack)
hnjVBF0p9.SetLineColor(ROOT.kCyan)

leg2 = ROOT.TLegend(0.7,0.7,0.9,0.9)
leg2.AddEntry(hnjVBF,    'no cut', 'l')
leg2.AddEntry(hnjVBF0p0, 'PUjetID > 0.0', 'l')
leg2.AddEntry(hnjVBF0p5, 'PUjetID > 0.5', 'l')
leg2.AddEntry(hnjVBF0p8, 'PUjetID > 0.8', 'l')
leg2.AddEntry(hnjVBF0p9, 'PUjetID > 0.9', 'l')

c2 = ROOT.TCanvas()
c2.cd()
hnjVBF0p0.Draw()
hnjVBF0p8.Draw('same')
hnjVBF0p9.Draw('same')
hnjVBF0p5.Draw('same')
hnjVBF.Draw('same')
leg2.Draw('same')
c2.SaveAs('nJetsVBF.pdf','pdf')


fin.Close()
