
from ROOT import *
import re
import os
import sys
import argparse
import fnmatch
import math
from array import array

def findInFolder (folder, pattern):
        ll = []
        for ff in os.listdir(folder):
            if fnmatch.fnmatch(ff, pattern): ll.append(ff)
        if len (ll) == 0:
            print "*** WARNING: No valid" , pattern , "found in " , folder
            return None
        if len (ll) > 1:
            print "*** WARNING: Too many files found in " , folder , ", using first one : " , ll
        return ll[0]


# prototype: TT_s2b0jresolvedMcut_SR_VBFjj_mass
def retrieveHistos (rootFile, namelist, var, sel, reg):
    res = {}
    for name in namelist:
        fullName = name + "_" + sel + "_" + reg + "_" + var
        if not rootFile.GetListOfKeys().Contains(fullName):
            print "*** WARNING: histo " , fullName , " not available"
            continue
        h = rootFile.Get(fullName)
        res[name] = h
    return res


def getHisto (histoName,inputList):
        for idx, name in enumerate(inputList):
                if (name.startswith(histoName)):
                        h = inputList[name].Clone (histoName)
                        break
        return h

## QCD is special and has a strange name, need data to recontruct it
## CORR_DDQCD_SS_DATA_HHKin_mass_SS_defaultBtagMMNoIsoBBTTCut_DsingleMuRunD
def retrieveQCD (rootFile, var, sel, dataNameList):
    #name = "CORR_DDQCD_SS_DATA_" + var + "_SS_" + sel + "_" + dataNameList[-1]
    name = "QCD_" + sel + "_SR_" + var
    if not rootFile.GetListOfKeys().Contains(name):
        print "*** WARNING: QCD histo " , name , " not available"
        return None
    hQCD = rootFile.Get(name)
    return hQCD

# makes an histogram by adding together all those in the input list ; inputList: names, histoList: histograms
def makeStack (stackName, histoList):
    s = THStack (stackName, stackName)
    for h in histoList:
        s.Add(h)
    return s

def setPlotStyle ():
    #Styles are: Plain Bold Video Pub Classic Default Modern
    #Modern is the default one
    #ss = gROOT.GetListOfStyles()
    #for s in ss: sys.stdout.write(s.GetName() + " ")
    
    gROOT.SetStyle("Modern")
    #LucaStyle = TStyle ("LucaStyle", "LucaStyle")
    #LucaStyle

# tranform an histo into a TGraphAsymmErrors, with 
def makeTGraphFromHist (histo, newName):
    nPoints  = hData.GetNbinsX()
    fX       = []
    fY       = []
    feYUp    = []
    feYDown  = []
    feXRight = []
    feXLeft  = []

    for ibin in range (1, nPoints+1):
        x = hData.GetBinCenter(ibin);
        y = hData.GetBinContent(ibin);
        dxRight = hData.GetBinLowEdge(ibin+1) - hData.GetBinCenter(ibin);
        dxLeft  = hData.GetBinCenter(ibin) - hData.GetBinLowEdge(ibin);
        dyUp    = hData.GetBinErrorUp(ibin);
        dyLow   = hData.GetBinErrorLow(ibin);

        #if (!drawGrass && (int) y == 0) continue;
        fY.append(y)
        fX.append(x)
        feYUp.append(dyUp)
        feYDown.append(dyLow)
        feXRight.append(dxRight)
        feXLeft.append(dxLeft)

    afX       = array ("d", fX      )
    afY       = array ("d", fY      )
    afeYUp    = array ("d", feYUp   )
    afeYDown  = array ("d", feYDown )
    afeXRight = array ("d", feXRight)
    afeXLeft  = array ("d", feXLeft )

    gData = TGraphAsymmErrors (len(afX), afX, afY, afeXLeft, afeXRight, afeYDown, afeYUp);
    gData.SetMarkerStyle(8);
    gData.SetMarkerSize(1.);
    gData.SetMarkerColor(kBlack);
    gData.SetLineColor(kBlack);
    gData.SetName(newName)
    return gData;

# set all horizontal bar errors to 0
def removeHErrors (graph):
    for ipt in range (0, graph.GetN()):
        graph.SetPointEXlow(ipt, 0)
        graph.SetPointEXhigh(ipt, 0)

# remove all points with content = 0
def removeEmptyPoints (graph):
    zeroes = []
    for ipt in range (0, graph.GetN()):
        x = Double(0.0)
        y = Double(0.0)
        graph.GetPoint(ipt,x,y)
        if y == 0:
            zeroes.append(ipt)
    for i in reversed (zeroes):
        graph.RemovePoint(i) 

# NB!! need to be called BEFORE removeHErrors or cannot know bin width
def scaleGraphByBinWidth (graph):
    for ipt in range (0, graph.GetN()):
        bwh = graph.GetErrorXhigh(ipt)
        bwl = graph.GetErrorXlow(ipt)
        bw = bwl + bwh

        eyh = graph.GetErrorYhigh(ipt)
        eyl = graph.GetErrorYlow(ipt)
        x = Double(0.0)
        y = Double(0.0)
        graph.GetPoint (ipt, x, y)
        graph.SetPoint (ipt, x, y/bw)
        graph.SetPointEYlow(ipt, eyl/bw)
        graph.SetPointEYhigh(ipt, eyh/bw)

## do ratio of Data/MC
# horErrs : do horizonta errors
def makeDataOverMCRatioPlot (hData, hMC, newName, horErrs=False):
    nPoints = hData.GetNbinsX()
    fX       = []
    fY       = []
    feYUp    = []
    feYDown  = []
    feXRight = []
    feXLeft  = []

    for ibin in range (1, nPoints+1):
        num = hData.GetBinContent(ibin)
        den = hMC.GetBinContent(ibin)
        if den > 0:
            # Y
            fY.append(num/den)
            feYUp.append(hData.GetBinErrorUp(ibin) / den)
            feYDown.append(hData.GetBinErrorLow(ibin) / den)

            # X
            fX.append (hData.GetBinCenter(ibin))
            if horErrs:
                feXRight.append(hData.GetBinLowEdge(ibin+1) - hData.GetBinCenter(ibin))
                feXLeft.append(hData.GetBinCenter(ibin) - hData.GetBinLowEdge(ibin))
            else:
                feXLeft.append(0.0)
                feXRight.append(0.0)

    afX       = array ("d", fX      )
    afY       = array ("d", fY      )
    afeYUp    = array ("d", feYUp   )
    afeYDown  = array ("d", feYDown )
    afeXRight = array ("d", feXRight)
    afeXLeft  = array ("d", feXLeft )

    gRatio = TGraphAsymmErrors (len(afX), afX, afY, afeXLeft, afeXRight, afeYDown, afeYUp);
    
    gRatio.SetMarkerStyle(8);
    gRatio.SetMarkerSize(1.);
    gRatio.SetMarkerColor(kBlack);
    gRatio.SetLineColor(kBlack);
    gRatio.SetName(newName)

    return gRatio;

## find maximum of tgraph, including error
def findMaxOfGraph (graph):
    uppers = []
    for i in range (0, graph.GetN()):
        x = Double(0.0)
        y = Double(0.0)
        graph.GetPoint(i, x, y)
        uppers.append (y + graph.GetErrorYhigh(i))
    return max(uppers)

# def makeSystUpDownStack (bkgList, systList, newNamePart):
#     for i, name in bkgList:
#         scale = systList[i] # error on nominal histo
#         hUp = None   # the histograms with up/down syst
#         hDown = None #
#         if i == 0:
#             hUp = bkgList[i].Clone (newNamePart + "_up")
#             hDown = bkgList[i].Clone (newNamePart + "_up")
#             hUp.Scale (1.0 + scale)
#             hDown.Scale (1.0 - scale)
#         else:
#             hTempUp   = bkgList[i].Clone (newNamePart + "_tmp_up" + i)
#             hTempDown = bkgList[i].Clone (newNamePart + "_tmp_up" + i)
#             hTempUp.Scale (1.0 + scale)
#             hTempDown.Scale (1.0 - scale)

## remove negative bins and reset yield accordingly
## NB: must be done BEFORE bin width division
def makeNonNegativeHistos (hList):
    for h in hList:
        integral = h.Integral()
        for b in range (1, h.GetNbinsX()+1):
            if (h.GetBinContent(b) < 0):
               h.SetBinContent (b, 0)
        integralNew = h.Integral()        
        if (integralNew != integral):
            print "** INFO: removed neg bins from histo " , h.GetName() 
        
        # print h.GetName() , integral , integralNew
        if integralNew == 0:
            h.Scale(0)
        else:
            h.Scale(integral/integralNew) 


### script body ###

if __name__ == "__main__" :
    TH1.AddDirectory(0)

    titleSize = 30
    labelSize = 22

    # gStyle.SetLabelFont(43)
    # gStyle.SetTitleFont(43)

    parser = argparse.ArgumentParser(description='Command line parser of plotting options')
    
    #string opts
    parser.add_argument('--var', dest='var', help='variable name', default=None)
    parser.add_argument('--sel', dest='sel', help='selection name', default=None)
    parser.add_argument('--dir', dest='dir', help='analysis output folder name', default="./")
    parser.add_argument('--tag', dest='tag', help='plots output folder name', default="./")
    parser.add_argument('--reg', dest='reg', help='region name', default=None)
    parser.add_argument('--title', dest='title', help='plot title', default=None)
    parser.add_argument('--channel', dest='channel', help='channel = (MuTau, ETau, TauTau)', default=None)
    parser.add_argument('--siglegextratext', dest='siglegextratext', help='additional optional text to be plotted in legend after signal block', default=None)
    
    #bool opts
    parser.add_argument('--log',     dest='log', help='use log scale',  action='store_true', default=False)
    parser.add_argument('--no-data', dest='dodata', help='disable plotting data',  action='store_false', default=True)
    parser.add_argument('--no-sig',  dest='dosig',  help='disable plotting signal',  action='store_false', default=True)
    parser.add_argument('--no-legend',   dest='legend',   help = 'disable drawing legend',       action='store_false', default=True)
    parser.add_argument('--no-binwidth', dest='binwidth', help = 'disable scaling by bin width', action='store_false', default=True)
    parser.add_argument('--ratio',    dest='ratio', help = 'do ratio plot at the botton', action='store_true', default=False)
    parser.add_argument('--no-print', dest='printplot', help = 'no pdf output', action='store_false', default=True)
    parser.add_argument('--quit',    dest='quit', help = 'quit at the end of the script, no interactive window', action='store_true', default=False)

    # par list opt
    parser.add_argument('--blind-range',   dest='blindrange', nargs=2, help='start and end of blinding range', default=None)

    #float opt
    parser.add_argument('--lymin', dest='lymin', type=float, help='legend min y position in pad fraction', default=None)
    parser.add_argument('--ymin', dest='ymin', type=float, help='min y range of plots', default=None)
    parser.add_argument('--ymax', dest='ymax', type=float, help='max y range of plots', default=None)
    parser.add_argument('--sigscale', dest='sigscale', type=float, help='scale to apply to all signals', default=None)
    parser.add_argument('--lumi', dest='lumi_num', type=float, help='lumi in fb-1', default=None)

    #parser.add_argument('integers', metavar='N', type=int, nargs='+', help='an integer for the accumulator')
    #parser.add_argument('--sum', dest='accumulate', action='store_const',
    #               const=sum, default=max,
    #               help='sum the integers (default: find the max)')
    
    args = parser.parse_args()

    if args.quit : gROOT.SetBatch(True)
    
    ######################### CANVASES #################################

    c1 = TCanvas ("c1", "c1", 600, 600)
    # c1.SetLeftMargin(0.15);
    # c1.SetBottomMargin(0.12);
    # c1.SetTopMargin(0.055);

    pad1 = None
    pad2 = None

    if args.ratio:
        pad1 = TPad ("pad1", "pad1", 0, 0.25, 1, 1.0)
        pad1.SetFrameLineWidth(3)
        pad1.SetLeftMargin(0.15);
        pad1.SetBottomMargin(0.02);
        pad1.SetTopMargin(0.055);
        
        pad1.Draw()
    else:
        pad1 = TPad ("pad1", "pad1", 0, 0.0, 1.0, 1.0)
        pad1.SetFrameLineWidth(3)
        pad1.SetLeftMargin(0.15);
        pad1.SetBottomMargin(0.12);
        pad1.SetTopMargin(0.055);
        pad1.Draw()


    pad1.cd()



    ######################### PUT USER CONFIGURATION HERE ####################

    # signals to plot
    #signalList = ["Radion300", "Radion450", "Radion800"]
    
    #sigList = ["Lambda1", "Lambda20"]
    #sigNameList = ["100 x #lambda_{hhh}/#lambda_{hhh}^{SM} = 1", "10 x #lambda_{hhh}/#lambda_{hhh}^{SM} = 20"]
    #sigNameList = ["100 x #lambda/#lambda^{SM} = 1", "10 x #lambda/#lambda^{SM} = 20"]

    #sigList = ["VBFC2V1","Radion300","HH_SM"]
    #sigNameList = ["VBFC2V1","Radion300","HH_SM"]
    sigList = ["VBFC2V1","HH_SM"]
    sigNameList = ["VBFC2V1","ggHH (scaled)"]


    sigScale = [100.]
    sigColors = {}
    sigColors["VBFC2V1"] = kRed #2
    #sigColors["Radion300"] = kBlack
    sigColors["HH_SM"] = kBlack

    bkgColors = {}
    #bkgColors["TT"] = kBlue+2
    #bkgColors["WplusToLNuWminusTo2JJJ_QCD"] = kOrange
    #bkgColors["WplusTo2JWminusToLNuJJ_QCD"] = kOrange +1
    #bkgColors["WplusToLNuWplusTo2JJJ_QCD"] = kOrange +3
    #bkgColors["WminusToLNuWminusTo2JJJ_QCD"] = kOrange +4
    #bkgColors["WplusToLNuZTo2JJJ_QCD"] = kOrange +7
    #bkgColors["WplusTo2JZTo2LJJ_QCD"] = kOrange +9
    #bkgColors["WminusToLNuZTo2JJJ_QCD"] = kPink
    #bkgColors["WminusTo2JZTo2LJJ_QCD"] = kPink+1
    #bkgColors["ZTo2LZTo2JJJ_QCD"] = kPink+10
    #bkgColors["VVJJ"] = kOrange+1
    bkgColors["EWKW"]       = kYellow    # kPink+10
    bkgColors["TTfullyHad"] = kGreen     # kCyan
    bkgColors["TTsemiLep"]  = kGreen +1  # kCyan +2
    bkgColors["TTfullyLep"] = kGreen +2  # kBlue
    bkgColors["VV"]         = kBlue      # kOrange+1
    bkgColors["DY"]         = kOrange +6 # kSpring+2
    bkgColors["WJets"]      = kRed+2     # OK
    bkgColors["TW"]         = kCyan      # kOrange+2
    bkgColors["QCD"]        = kRed -9    # kPink+9
    

    # sigList = ["Radion300", "Radion450", "Radion800"]
    # sigNameList = [
    #     "m_{H} = 300 GeV",
    #     "m_{H} = 450 GeV",
    #     "m_{H} = 800 GeV" ]
    
    # sigScale = [1.0, 1.0, 1.0]
    #sigScale = [0.1, 0.1, 0.1]
    if args.sigscale:
        for i in range(0,len(sigScale)): sigScale[i] = args.sigscale

    #plotTitle = ";m_{#tau#tau} [GeV];dN/dm_{#tau#tau} [1/GeV]"
    plotTitle = ""

    if args.title:
        plotTitle = args.title
    dataList = ["data_obs"]
    #dataList = ["DTau_6"]
    #bkgList =  ["VVJJ","TT","DY","WJets"]
    #bkgList =  ["VVJJ","TTfullyHad","TTsemiLep","TTfullyLep","DY","WJets"]
    bkgList =  ["TTfullyHad", "TTfullyLep", "TTsemiLep", "DY", "VV", "WJets", "TW", "EWKW", "QCD"]
    #bkgList =  ["WplusToLNuWminusTo2JJJ_QCD","WplusTo2JWminusToLNuJJ_QCD","WplusToLNuWplusTo2JJJ_QCD","WminusToLNuWminusTo2JJJ_QCD","WplusToLNuZTo2JJJ_QCD","WplusTo2JZTo2LJJ_QCD","WminusToLNuZTo2JJJ_QCD","WminusTo2JZTo2LJJ_QCD","ZTo2LZTo2JJJ_QCD","TTfullyHad","TTsemiLep","TTfullyLep","DY","WJets"]
    #Dylist   = ["DYIncl", "DY100200", "DY200400", "DY400600", "DY600Inf"]
    #Wjetlist = ["WJetsIncl", "WJets100200", "WJets200400", "WJets400600", "WJets600Inf"]
    #TTlist   = ["TT"]
    #tWlist   = ["TWtop", "TWantitop"]
    #VVlist   = ["WWTo2L2Nu", "WWToLNuQQ", "WZTo1L1Nu2Q", "WZTo1L3Nu", "WZTo2L2Q", "WZTo3LNu", "ZZTo2L2Nu", "ZZTo2L2Q", "ZZTo4L"]

    ###########################################################################
    #setPlotStyle()


    
    #outplotterName = findInFolder  (args.dir+"/", 'outPlotter.root')
    outplotterName = findInFolder  (args.dir+"/", 'analyzedOutPlotter.root')
    

    rootFile = TFile.Open (args.dir+"/"+outplotterName)
    hSigs = retrieveHistos (rootFile, sigList, args.var, args.sel,args.reg)
    hBkgs = retrieveHistos  (rootFile, bkgList, args.var, args.sel,args.reg)
    hDatas = retrieveHistos  (rootFile, dataList, args.var, args.sel,args.reg)

    #hDY    = groupTogether ("DY", DYlist, hBkgs)
    #hWJets = groupTogether ("WJets", Wjetlist, hBkgs)
    #hTT    = getHisto ("TT", hBkgs)

    #hWplusToLNuWminusTo2JJJ_QCD =getHisto("WplusToLNuWminusTo2JJJ_QCD",hBkgs)
    #hWplusTo2JWminusToLNuJJ_QCD =getHisto("WplusTo2JWminusToLNuJJ_QCD",hBkgs)
    #hWplusToLNuWplusTo2JJJ_QCD =getHisto("WplusToLNuWplusTo2JJJ_QCD",hBkgs)
    #hWminusToLNuWminusTo2JJJ_QCD =getHisto("WminusToLNuWminusTo2JJJ_QCD",hBkgs)
    #hWplusToLNuZTo2JJJ_QCD =getHisto("WplusToLNuZTo2JJJ_QCD",hBkgs)
    #hWplusTo2JZTo2LJJ_QCD =getHisto("WplusTo2JZTo2LJJ_QCD",hBkgs)
    #hWminusToLNuZTo2JJJ_QCD =getHisto("WminusToLNuZTo2JJJ_QCD",hBkgs)
    #hWminusTo2JZTo2LJJ_QCD  =getHisto("WminusTo2JZTo2LJJ_QCD",hBkgs)
    #hZTo2LZTo2JJJ_QCD =getHisto("ZTo2LZTo2JJJ_QCD",hBkgs)
    #hVVJJ    = getHisto ("VVJJ", hBkgs)
    hTTfullyHad    = getHisto ("TTfullyHad", hBkgs)
    hTTsemiLep    = getHisto ("TTsemiLep", hBkgs)
    hTTfullyLep    = getHisto ("TTfullyLep", hBkgs)
    hDY    = getHisto ("DY", hBkgs)
    hWJets    = getHisto ("WJets", hBkgs)
    htW     = getHisto ("TW", hBkgs)
    hVV    = getHisto("VV", hBkgs)
    hEWKW  = getHisto("EWKW", hBkgs)
    #htW    = groupTogether ("tW", tWlist, hBkgs)
    #hVV    = groupTogether ("VV", VVlist, hBkgs)
    hQCD   = retrieveQCD (rootFile, args.var, args.sel, dataList)
    
    #hBkgList = [hTT, hDY, hWJets, hQCD, hVV, htW] ## full list for stack
    #hBkgNameList = ["t#bar{t}", "Drell-Yann", "W+jets", "QCD", "di-boson", "single top"] # list for legend
    #hBkgList = [hTT,hVVJJ,hDY,hWJets] ## full list for stack
    #hBkgList = [hVVJJ, hTTfullyHad,hTTsemiLep,hTTfullyLep,hDY,hWJets] ## full list for stack
    #hBkgList = [hTTfullyHad,hDY] ## full list for stack
    hBkgList = [hTTfullyHad, hTTfullyLep, hTTsemiLep, hDY, hVV, hWJets, htW, hEWKW, hQCD]
    #hBkgList = [hWplusToLNuWminusTo2JJJ_QCD,hWplusTo2JWminusToLNuJJ_QCD,hWplusToLNuWplusTo2JJJ_QCD,hWminusToLNuWminusTo2JJJ_QCD,hWplusToLNuZTo2JJJ_QCD,hWplusTo2JZTo2LJJ_QCD,hWminusToLNuZTo2JJJ_QCD,hWminusTo2JZTo2LJJ_QCD ,hZTo2LZTo2JJJ_QCD,hTTfullyHad,hTTsemiLep,hTTfullyLep,hDY,hWJets] ## full list for stack
    #hBkgNameList = ["VV+JJ""t#bar{t}","DY+jets","WJets"] # list for legend
    #hBkgNameList = ["VVjj","t#bar{t} hh","t#bar{t} hl","t#bar{t} ll","DY+jets","WJets"] # list for legend
    hBkgNameList = ["t#bar{t} hh","DY+jets"] # list for legend
    hBkgNameList = ["t#bar{t} hh","t#bar{t} ll","t#bar{t} hl","DY+Jets","VV","WJets","singleTop","EWKW", "QCD"]
    #hBkgNameList = ["W^{+}ToLNuW^{-}To2JJJ","W^{+}To2JW^{-}ToLNuJJ","W^{+}ToLNuW^{+}To2JJJ","W^{-}ToLNuW^{-}To2JJJ","W^{+}ToLNuZTo2JJJ","W^{+}To2JZTo2LJJ","W^{-}ToLNuZTo2JJJ","W^{-}To2JZTo2LJJ ","ZTo2LZTo2JJJ","t#bar{t} hh","t#bar{t} hl","t#bar{t} ll","DY+jets","WJets"] # list for legend
    hData = getHisto  ("data_obs", hDatas)

    # remove all data from blinding region before creating tgraph etc...
    if args.blindrange:
        blow = float (args.blindrange[0]) 
        bup = float (args.blindrange[1]) 
        for ibin in range (1, hData.GetNbinsX()+1):
            center = hData.GetBinCenter(ibin)
            if center > blow and center < bup:
                hData.SetBinContent(ibin, 0)

    hDataNonScaled = hData.Clone("hDataNonScaled")   #pyroot fa schifo!!! senza questa cosa hData not defined dopo la creazione del testo . E se chiamo tutto hData2 non va. MAH!
    gData = makeTGraphFromHist (hData, "grData")

    # apply sig scale
    for i, scale in enumerate (sigScale):
        histo = hSigs[sigList[i]]
        histo.Scale(scale)

    # apply sig color if available
    for key in hSigs:
        hSigs[key].SetLineWidth(2)    
        if key in sigColors:
            thecolor = int(sigColors[key])
            hSigs[key].SetLineColor(thecolor)


    # apply bkg color if available
    for h in hBkgList:
            histoname =h.GetName()
            for key,value in bkgColors.items():
                if key in histoname:    
                        thecolor = int(bkgColors[key])
                        h.SetLineColor(thecolor)
                        h.SetFillColor(thecolor)
                        h.SetFillStyle(1001)

            
    #################### REMOVE NEGARIVE BINS #######################
    print "** INFO: removing all negative bins from bkg histos"
    makeNonNegativeHistos (hBkgList)


    #################### PERFORM DIVISION BY BIN WIDTH #######################
    #clones non scaled (else problems with graph ratio because I pass data evt hist)
    bkgStackNS = makeStack ("bkgStackNS", hBkgList)
    hBkgEnvelopeNS = bkgStackNS.GetStack().Last().Clone("hBkgEnvelopeNS")

    if args.binwidth:
        scaleGraphByBinWidth (gData)
        for h in hBkgList:
            h.Scale(1., "width")
        for i, name in enumerate (sigNameList):
            histo = hSigs[sigList[i]]
            histo.Scale(1., "width")


    #################### DO STACK AND PLOT #######################

    bkgStack = makeStack ("bkgStack", hBkgList)

    # c1.SetLeftMargin(0.15);
    # c1.SetBottomMargin(0.12);
    # c1.SetTopMargin(0.055);
    
    if args.log: pad1.SetLogy()

    #pad1.SetFrameLineWidth(3)


    ################## TITLE AND AESTETICS ############################
    bkgStack.Draw("HIST")

    bkgStack.GetXaxis().SetTitleFont(43)
    bkgStack.GetYaxis().SetTitleFont(43)
    bkgStack.GetXaxis().SetLabelFont(43)
    bkgStack.GetYaxis().SetLabelFont(43)

    bkgStack.GetXaxis().SetTitleOffset(1.0)
    bkgStack.GetYaxis().SetTitleOffset(1.2)

    bkgStack.GetXaxis().SetTitleSize(titleSize)
    bkgStack.GetYaxis().SetTitleSize(titleSize)

    bkgStack.GetXaxis().SetLabelSize(labelSize)
    bkgStack.GetYaxis().SetLabelSize(labelSize)

    #intBkg = bkgStack.GetHistogram().Integral()
    intBkg = bkgStack.GetStack().Last().Integral()
    bkgStack.SetTitle(plotTitle)


    for key in hSigs:
        intSig = hSigs[key].Integral()
        if intSig > 0:
            hSigs[key].Scale(intBkg/intSig)
            #if key is 'HH_SM':
            #    xsecRatio = 19.56/10   ## giusto?? mi sa di no...
            #    ggHH_scale = xsecRatio*hSigs["HH_SM"].GetEntries()/float(hSigs["VBFC2V1"].GetEntries())
            #    hSigs[key].Scale(ggHH_scale)

    ################## LEGEND ######################################

    legmin = 0.45
    if args.lymin: legmin = args.lymin
    legminx = 0.50
    if (len(hBkgNameList) +len(hSigs)>6): legminx = 0.3
    leg = TLegend (legminx, legmin, 0.85, 0.93)
    if (len(hBkgNameList) +len(hSigs)> 6): leg.SetNColumns(2)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextFont(43)
    leg.SetTextSize(20)

    # add element in same order as stack --> top-bottom
    for i, name in reversed(list(enumerate(hBkgNameList))):
        leg.AddEntry(hBkgList[i], name, "f")

    if args.dosig:
        #for i, name in enumerate (sigNameList):
        for i, name in reversed(list(enumerate (sigNameList))):
            histo = hSigs[sigList[i]]
            leg.AddEntry (histo, name, "l")
        # null entry to complete signal Xsection
        if args.siglegextratext:
            leg.AddEntry(None, args.siglegextratext, "")

    if args.dodata:
        leg.AddEntry(gData, "data", "pe")


    ################## Y RANGE SETTINGS ############################
    ymin = 0
    if args.log: ymin = 1.E-2

    maxs = []
    
    # bkgmax = 0
    # for h in hBkgList: bkgmax+= h.GetMaximum()
    # maxs.append(bkgmax)
    maxs.append(bkgStack.GetStack().Last().GetMaximum())

    if args.dodata:
        maxs.append(findMaxOfGraph(gData))
        #if not args.binwidth:
        #    maxs.append(hData.GetMaximum() + math.sqrt(hData.GetMaximum()))
        
    if args.dosig :
        for key in hSigs: maxs.append(hSigs[key].GetMaximum())

    ymax = max(maxs)

    # scale max to leave some space (~10%)
    extraspace = 0.3

    if not args.log:
        ymax += extraspace* (ymax-ymin)
    
    else:
        new = extraspace * (math.log(ymax, 10) - math.log(ymin, 10)) + math.log(ymax, 10)
        ymax = math.pow(10, new)


    #print "limits: " , ymin, ymax

    ## override form args
    if args.ymin: ymin = args.ymin
    if args.ymax: ymax = args.ymax

    bkgStack.SetMinimum(ymin)
    bkgStack.SetMaximum(ymax)

    # interactive display
    bkgStack.Draw("HIST")
    if args.dosig:
        for key in hSigs: hSigs[key].Draw("hist same")
    if args.dodata:
        removeHErrors(gData)
        removeEmptyPoints(gData)
        gData.Draw("P Z same") # Z: no small line at the end of error bar

    ###################### OTHER TEXT ON PLOT #########################

    # extraText = "preliminary"
    # CMStext = "CMS"

    cmsTextFont     = 61  # font of the "CMS" label
    cmsTextSize   = 0.05  # font size of the "CMS" label
    extraTextFont   = 52     # for the "preliminary"
    extraTextSize   = 0.76 * cmsTextSize # for the "preliminary"
    xpos  = 0.177
    ypos  = 0.94
    #yoffset = 0.05 # fractional shift   

    CMSbox       = TLatex  (xpos, ypos         , "CMS")       
    extraTextBox = TLatex  (xpos, ypos - 0.05 , "preliminary")
    CMSbox.SetNDC()
    extraTextBox.SetNDC()
    CMSbox.SetTextSize(cmsTextSize)
    CMSbox.SetTextFont(cmsTextFont)
    CMSbox.SetTextColor(kBlack)
    CMSbox.SetTextAlign(13)
    extraTextBox.SetTextSize(extraTextSize)
    extraTextBox.SetTextFont(extraTextFont)
    extraTextBox.SetTextColor(kBlack)
    extraTextBox.SetTextAlign(13)


    
    lumi = "%.1f pb^{-1} (13 TeV)" % args.lumi_num
    lumibox = TLatex  (0.9, 0.964, lumi)       
    lumibox.SetNDC()
    lumibox.SetTextAlign(31)
    lumibox.SetTextSize(extraTextSize)
    lumibox.SetTextFont(42)
    lumibox.SetTextColor(kBlack)

    chName = None
    chBox = None
    if args.channel:
        if args.channel == "MuTau":
            chName = "bb #mu#tau_{h}"
        elif args.channel == "ETau":
            chName = "bb e#tau_{h}"
        elif args.channel == "TauTau":
            chName = "bb #tau_{h}#tau_{h}"
        else:
            print "*** Warning: channel name must be MuTau, ETau, TauTau, you wrote: " , args.channel

        if chName:
            chBox = TLatex  (xpos, ypos + 0.025, chName)
            chBox.SetNDC()
            chBox.SetTextSize(cmsTextSize+20)
            chBox.SetTextFont(43)
            chBox.SetTextColor(kBlack)
            chBox.SetTextAlign(11)
    CMSbox.Draw()
    extraTextBox.Draw()
    lumibox.Draw()
    if args.legend: leg.Draw()
    if chBox: chBox.Draw()

    ###################### BLINDING BOX ###############################
    if args.blindrange:
        blow = float(args.blindrange[0])
        bup = float(args.blindrange[1])
        bBox = TBox (blow, ymin, bup, 0.93*ymax)
        bBox.SetFillStyle(3002) # NB: does not appear the same in displayed box and printed pdf!!
        bBox.SetFillColor(kGray+2) # NB: does not appear the same in displayed box and printed pdf!!
        bBox.Draw()


    ###################### RATIO PLOT #################################
    if args.ratio:
        bkgStack.GetXaxis().SetTitleSize(0.00);
        bkgStack.GetXaxis().SetLabelSize(0.00);

        c1.cd()
        pad2 = TPad ("pad2", "pad2", 0, 0.0, 1, 0.2496)
        pad2.SetLeftMargin(0.15);
        pad2.SetTopMargin(0.05);
        pad2.SetBottomMargin(0.35);
        pad2.SetGridy(True);
        pad2.SetFrameLineWidth(3)
        #pad2.SetGridx(True);
        pad2.Draw()
        pad2.cd()

        grRatio = makeDataOverMCRatioPlot (hDataNonScaled, hBkgEnvelopeNS, "grRatio")
        hRatio = hDataNonScaled.Clone("hRatioAxis") # for ranges only

        hRatio.GetXaxis().SetTitleFont(43) # so that size is in pixels
        hRatio.GetYaxis().SetTitleFont(43) # so that size is in pixels
        hRatio.GetXaxis().SetLabelFont(43) # so that size is in pixels
        hRatio.GetYaxis().SetLabelFont(43) # so that size is in pixels
        hRatio.GetYaxis().SetNdivisions(505)

        #hRatio.GetXaxis().SetTitle(bkgStack.GetXaxis().GetName())
        hRatio.SetTitle(plotTitle)
        hRatio.GetYaxis().SetTitle ("Data/MC")
        hRatio.GetXaxis().SetTitleOffset(2.8)
        hRatio.GetYaxis().SetTitleOffset(1.2)

        hRatio.GetXaxis().SetTitleSize(titleSize);
        hRatio.GetXaxis().SetLabelSize(labelSize);
        hRatio.GetYaxis().SetTitleSize(titleSize);
        hRatio.GetYaxis().SetLabelSize(labelSize);

        hRatio.GetXaxis().SetTickSize(0.10)
        hRatio.GetYaxis().SetTickSize(0.05)

        hRatio.SetStats(0)
        # hRatio.SetMinimum(0.5)
        # hRatio.SetMaximum(1.5)
        hRatio.SetMinimum(0.1)
        hRatio.SetMaximum(1.9)

        removeEmptyPoints (grRatio)
        hRatio.Draw("axis")
        grRatio.Draw("P Z same") # Z : no small limes at the end of points
        pad2.RedrawAxis();
        pad2.RedrawAxis("g"); #otherwise no grid..
    ###################### DISPLAY ###################################
    if not args.quit:
        # pad1.Update() # necessary to show plot
        # pad2.Update() # necessary to show plot
        c1.Update()
        pad1.Update()
        if pad2: pad2.Update()
        raw_input() # to prevent script from closing

    if args.printplot:
        tagch = ""
        if args.channel:
            tagch = "_" + args.channel
        saveName = "./plots_"+args.channel+"VBF/"+args.tag+"/"+args.sel+"/plot_" + args.var + "_" + args.sel + tagch 
        if args.log:
            saveName = saveName+"_log"
        c1.Print (saveName+".pdf", "pdf")
        c1.SaveAs (saveName+".png")
