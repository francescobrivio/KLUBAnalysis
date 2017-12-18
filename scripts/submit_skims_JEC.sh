OUTDIRR="Skims2017_26Lug_JEC_jecUnc"
AMESSAGE="Resub failed data with more jobs. Skims fixing the MuMu sideband SFs on both muons. Reweighted benchmarks 5D reweight. Introduced ele10 sideband. Data from 22Feb production, 7Feb2017 REminiaod. Data with fixed evts number. Added OR of non ER mu triggers to see if agreement is retrieved.Includes nBadMu filter applied. 11 Feb 2017 MC production (most but not all jobs are finished)."

INPUTDIR="inputFiles/JECproduction_Lug2017"
#SKIMDIR="/gwpool/users/brivio/Hhh_1718/SKIMMED_JEC"
SKIMDIR="/gwteraz/users/brivio/SKIMMED_JEC"

#source /opt/exp_soft/cms/t3/t3setup
#mkdir /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/
#mkdir $OUTDIRR
#touch /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/README.txt
#echo $AMESSAGE > /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/README.txt
#cp /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/

# source /opt/exp_soft/cms/t3/t3setup
# mkdir /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/
# mkdir $OUTDIRR
# touch /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/README.txt
# echo $AMESSAGE > /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/README.txt
# cp /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /data_CMS/cms/cadamuro/test_submit_to_tier3/$OUTDIRR/


### JEC ###
source /cvmfs/cms.cern.ch/cmsset_default.sh
source scripts/setup.sh
#mkdir /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/
mkdir /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/
mkdir $OUTDIRR
#touch /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/README.txt
#echo $AMESSAGE > /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/README.txt
cp /gwpool/users/brivio/Hhh_1718/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/ #/gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/



###################
# TT
# TT x section: 831.76 for inclusive sample, W->had 67,60% , W->l nu 3*10,8% = 32,4% (sum over all leptons)
# hh = 45.7%
# ll = 10.5%
# hl = 21.9% (x2 for permutation t-tbar)
# technical note: use -q long if splitting on ~180-350 jobs
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 100 -k True -o $SKIMDIR/$OUTDIRR/SKIM_TT_fullyHad  -i $INPUTDIR/TT_powheg_fullyHad.txt    -x 380.1143 -t True -b 1
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 100 -k True -o $SKIMDIR/$OUTDIRR/SKIM_TT_semiLep   -i $INPUTDIR/TT_powheg_semiLep.txt     -x 364.3108 -t True -b 5
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 100 -k True -o $SKIMDIR/$OUTDIRR/SKIM_TT_fullyLep  -i $INPUTDIR/TT_powheg_fullyLep.txt    -x 87.3348  -t True -b 4

# # ####################
# # ## DY
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 100 -k True -o $SKIMDIR/$OUTDIRR/SKIM_DY   -i $INPUTDIR/DY_mergedList.txt -g True -x 5765.4

# # ####################
# # ## ELECTROWEAK
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 50   -k True -o $SKIMDIR/$OUTDIRR/SKIM_EWKWPlus2Jets_WToLNu   -i $INPUTDIR/EWKWPlus2Jets_WToLNu_M-50.txt   -x 25.62
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 50   -k True -o $SKIMDIR/$OUTDIRR/SKIM_EWKWMinus2Jets_WToLNu  -i $INPUTDIR/EWKWMinus2Jets_WToLNu_M-50.txt  -x 20.25
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 50   -k True -o $SKIMDIR/$OUTDIRR/SKIM_EWKZ2Jets_ZToLL        -i $INPUTDIR/EWKZ2Jets_ZToLL_M-50.txt        -x 3.987


# #############################
# ##### ### W+jets :
# ##### ## note: currently I am chopping away the part with >= 1 jet in the inclusive sample, and writing the corresponding cross section.
# ##### ## But I can increase the stat with some stitching as done in DY if needed
# ##### python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_0jet     -i inputFiles/Files80X_25Nov2016/8_WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8__RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1.txt    -x 44149.01333 -e 0
# ##### python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_1jet     -i inputFiles/Files80X_25Nov2016/9_W1JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8__RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1.txt   -x 11778.36381
# ##### python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_2jet     -i inputFiles/Files80X_25Nov2016/10_W2JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8__RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1.txt  -x 3840.219744
# ##### python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_3jet     -i inputFiles/Files80X_25Nov2016/11_W3JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8__RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1.txt  -x 1166.047866
# ##### python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_4jet     -i inputFiles/Files80X_25Nov2016/12_W4JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8__RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1.txt  -x 593.055246

# #############################
# ##### ### W+jets in HT bins 
# ## WJets for HT bins -- chopping away HT > 70 in the inclusive sample is fine, as it is only ~10% of the total number of events
# ## tot xs NNLO: 61526.7 (+497.1 -264.6) (+/- 2312.7) pb
# ## individual xs are taken from : https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets
# ## and the one for 0-70 and 70-100 computed by difference and using the inclusive sample distribution
# ## xs LO : 50690 pb
# ## xs LO (HT < 100 ) = 50690 - sum (xs HT > 100) = 50690 - 1772.52216 = 48917.47784
# ## then using the inlcusive sample :
# ## ch->GetEntries("lheHt < 70")                : 186703 (97.09%) --> 47493.98 pb
# ## ch->GetEntries("lheHt > 70 && lheHt < 100") : 5589   (2.91%)  --> 1423.499 pb
# ## Finally, everything is scaled from the LO to the NNLO xs : (61526.7/50690.) = 1.213784
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_0_70       -i $INPUTDIR/WJets_incl.txt        -y 1.213784 -x 47493.98 -z 70
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_70_100     -i $INPUTDIR/WJetsHT-70-100.txt    -y 1.213784 -x 1423.499
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_100_200    -i $INPUTDIR/WJetsHT-100-200.txt   -y 1.213784 -x 1345
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_200_400    -i $INPUTDIR/WJetsHT-200-400.txt   -y 1.213784 -x 359.7
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_400_600    -i $INPUTDIR/WJetsHT-400-600.txt   -y 1.213784 -x 48.91
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_600_800    -i $INPUTDIR/WJetsHT-600-800.txt   -y 1.213784 -x 12.05
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_800_1200   -i $INPUTDIR/WJetsHT-800-1200.txt  -y 1.213784 -x 5.501
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_1200_2500  -i $INPUTDIR/WJetsHT-1200-2500.txt -y 1.213784 -x 1.329
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_2500_Inf   -i $INPUTDIR/WJetsHT-2500-Inf.txt  -y 1.213784 -x 0.03216

# #####################
# ### tW top :
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_ST_tW_antitop_5f_inclusiveDecays   -i $INPUTDIR/33_ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt   -x 35.6
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_ST_tW_top_5f_inclusiveDecays       -i $INPUTDIR/34_ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 35.6

# # #####################
# # ### DI-boson: - 12
# # # note2: all njobs listed here were increased by x10 to make them run on the short queue
# # #XS taken from here: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#Diboson
# ## for Z->4Q, I scaled from ZZ->2nu2Q using BR(2Nu2Q) = 2 * 20% * 69.91%
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWTo2L2Nu  -i $INPUTDIR/63_WWTo2L2Nu_13TeV-powheg__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt   -x 12.178
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWToLNuQQ  -i $INPUTDIR/WWToLNuQQ.txt                                                                                                  -x 49.997
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWTo4Q     -i $INPUTDIR/66_WWTo4Q_13TeV-powheg__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 51.723

 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2Q2Nu  -i $INPUTDIR/54_ZZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt -x 4.04
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2L2Nu  -i $INPUTDIR/55_ZZTo2L2Nu_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt               -x 0.564
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2L2Q   -i $INPUTDIR/56_ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt  -x 3.22
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo4L     -i $INPUTDIR/57_ZZTo4L_13TeV-amcatnloFXFX-pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 1.212
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo4Q     -i $INPUTDIR/60_ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt    -x 7.06091

 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo3LNu       -i $INPUTDIR/69_WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 4.42965
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo1L3Nu      -i $INPUTDIR/70_WZTo1L3Nu_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt    -x 3.033
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo1L1Nu2Q    -i $INPUTDIR/71_WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3.txt  -x 10.71
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo2L2Q       -i $INPUTDIR/72_WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt     -x 5.595

# # #####################
# # ### SM Higgs  - 3
# # ### HXSWG: xs(ZH) = 0.8839 pb
# # ### Z->qq : 69.91% , Z->ll : 3,3658% (x3 for all the leptons)
# # ### H->bb : 57.7%  , H->tautau : 6.32%
# # ##### ZH (Zall, Htautau) : 6.32%
# # ##### ZH (Zll, Hbb)      : 5.83%
# # ##### ZH (Zqq, Hbb)      : 40.34%
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HTauTau      -i $INPUTDIR/73_ZHToTauTau_M125_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 0.8839 -y 0.0632
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HBB_ZLL      -i $INPUTDIR/ZH_HToBB_ZToLL.txt                                                                                                              -x 0.8839 -y 0.0583
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HBB_ZQQ      -i $INPUTDIR/76_ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt  -x 0.8839 -y 0.4034


# # #####################
# # ### Radion signals - 13
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-250   -i $INPUTDIR/81_GluGluToRadionToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-270   -i $INPUTDIR/82_GluGluToRadionToHHTo2B2Tau_M-270_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-280   -i $INPUTDIR/83_GluGluToRadionToHHTo2B2Tau_M-280_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-300   -i $INPUTDIR/84_GluGluToRadionToHHTo2B2Tau_M-300_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt  -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-350   -i $INPUTDIR/85_GluGluToRadionToHHTo2B2Tau_M-350_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-400   -i $INPUTDIR/86_GluGluToRadionToHHTo2B2Tau_M-400_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v3.txt  -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-450   -i $INPUTDIR/87_GluGluToRadionToHHTo2B2Tau_M-450_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-500   -i $INPUTDIR/88_GluGluToRadionToHHTo2B2Tau_M-500_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-550   -i $INPUTDIR/89_GluGluToRadionToHHTo2B2Tau_M-550_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-600   -i $INPUTDIR/90_GluGluToRadionToHHTo2B2Tau_M-600_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-650   -i $INPUTDIR/91_GluGluToRadionToHHTo2B2Tau_M-650_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-750   -i $INPUTDIR/92_GluGluToRadionToHHTo2B2Tau_M-750_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-900   -i $INPUTDIR/93_GluGluToRadionToHHTo2B2Tau_M-900_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True


# #####################
# ### Graviton signals - 19
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-250   -i $INPUTDIR/94_GluGluToBulkGravitonToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-260   -i $INPUTDIR/95_GluGluToBulkGravitonToHHTo2B2Tau_M-260_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-270   -i $INPUTDIR/96_GluGluToBulkGravitonToHHTo2B2Tau_M-270_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-280   -i $INPUTDIR/97_GluGluToBulkGravitonToHHTo2B2Tau_M-280_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-300   -i $INPUTDIR/98_GluGluToBulkGravitonToHHTo2B2Tau_M-300_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-320   -i $INPUTDIR/99_GluGluToBulkGravitonToHHTo2B2Tau_M-320_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-340   -i $INPUTDIR/100_GluGluToBulkGravitonToHHTo2B2Tau_M-340_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-350   -i $INPUTDIR/101_GluGluToBulkGravitonToHHTo2B2Tau_M-350_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-400   -i $INPUTDIR/102_GluGluToBulkGravitonToHHTo2B2Tau_M-400_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-450   -i $INPUTDIR/103_GluGluToBulkGravitonToHHTo2B2Tau_M-450_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-500   -i $INPUTDIR/104_GluGluToBulkGravitonToHHTo2B2Tau_M-500_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-550   -i $INPUTDIR/105_GluGluToBulkGravitonToHHTo2B2Tau_M-550_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-600   -i $INPUTDIR/106_GluGluToBulkGravitonToHHTo2B2Tau_M-600_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-650   -i $INPUTDIR/107_GluGluToBulkGravitonToHHTo2B2Tau_M-650_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-750   -i $INPUTDIR/108_GluGluToBulkGravitonToHHTo2B2Tau_M-750_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-800   -i $INPUTDIR/109_GluGluToBulkGravitonToHHTo2B2Tau_M-800_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True


 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRSGravitonToHHTo2B2Tau_M-300   -i $INPUTDIR/110_GluGluToRSGravitonToHHTo2B2Tau_M-300_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRSGravitonToHHTo2B2Tau_M-650   -i $INPUTDIR/111_GluGluToRSGravitonToHHTo2B2Tau_M-650_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRSGravitonToHHTo2B2Tau_M-900   -i $INPUTDIR/112_GluGluToRSGravitonToHHTo2B2Tau_M-900_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True


# #####################
# #### HH dyn rew 12 benchmarks - 12 jobs
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_1   -i $INPUTDIR/allNonResClonesBench/filelist_bench_1.txt -x 1.0    --kl  7.5  --kt 1.0 --c2 -1.0 --cg  0.0 --c2g  0.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_2   -i $INPUTDIR/allNonResClonesBench/filelist_bench_2.txt -x 1.0    --kl  1.0  --kt 1.0 --c2  0.5 --cg -0.8 --c2g  0.6
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_3   -i $INPUTDIR/allNonResClonesBench/filelist_bench_3.txt -x 1.0    --kl  1.0  --kt 1.0 --c2 -1.5 --cg  0.0 --c2g -0.8
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_4   -i $INPUTDIR/allNonResClonesBench/filelist_bench_4.txt -x 1.0    --kl -3.5  --kt 1.5 --c2 -3.0 --cg  0.0 --c2g  0.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_5   -i $INPUTDIR/allNonResClonesBench/filelist_bench_5.txt -x 1.0    --kl  1.0  --kt 1.0 --c2  0.0 --cg  0.8 --c2g -1.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_6   -i $INPUTDIR/allNonResClonesBench/filelist_bench_6.txt -x 1.0    --kl  2.4  --kt 1.0 --c2  0.0 --cg  0.2 --c2g -0.2
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_7   -i $INPUTDIR/allNonResClonesBench/filelist_bench_7.txt -x 1.0    --kl  5.0  --kt 1.0 --c2  0.0 --cg  0.2 --c2g -0.2
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_8   -i $INPUTDIR/allNonResClonesBench/filelist_bench_8.txt -x 1.0    --kl  15.0 --kt 1.0 --c2  0.0 --cg -1.0 --c2g  1.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_9   -i $INPUTDIR/allNonResClonesBench/filelist_bench_9.txt -x 1.0    --kl  1.0  --kt 1.0 --c2  1.0 --cg -0.6 --c2g  0.6
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_10  -i $INPUTDIR/allNonResClonesBench/filelist_bench_10.txt -x 1.0   --kl 10.0  --kt 1.5 --c2 -1.0 --cg  0.0 --c2g  0.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_11  -i $INPUTDIR/allNonResClonesBench/filelist_bench_11.txt -x 1.0   --kl  2.4  --kt 1.0 --c2  0.0 --cg  1.0 --c2g -1.0
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_12  -i $INPUTDIR/allNonResClonesBench/filelist_bench_12.txt -x 1.0   --kl 15.0  --kt 1.0 --c2  1.0 --cg  0.0 --c2g  0.0



# #####################
# # ## HH din reweight
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_0   -i $INPUTDIR/allNonResClones/filelist_allNonRes_0.txt    -x 1.0 --kl -20     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_1   -i $INPUTDIR/allNonResClones/filelist_allNonRes_1.txt    -x 1.0 --kl -19     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_2   -i $INPUTDIR/allNonResClones/filelist_allNonRes_2.txt    -x 1.0 --kl -18     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_3   -i $INPUTDIR/allNonResClones/filelist_allNonRes_3.txt    -x 1.0 --kl -17     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_4   -i $INPUTDIR/allNonResClones/filelist_allNonRes_4.txt    -x 1.0 --kl -16     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_5   -i $INPUTDIR/allNonResClones/filelist_allNonRes_5.txt    -x 1.0 --kl -15     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_6   -i $INPUTDIR/allNonResClones/filelist_allNonRes_6.txt    -x 1.0 --kl -14     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_7   -i $INPUTDIR/allNonResClones/filelist_allNonRes_7.txt    -x 1.0 --kl -13     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_8   -i $INPUTDIR/allNonResClones/filelist_allNonRes_8.txt    -x 1.0 --kl -12     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_9   -i $INPUTDIR/allNonResClones/filelist_allNonRes_9.txt    -x 1.0 --kl -11     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_10   -i $INPUTDIR/allNonResClones/filelist_allNonRes_10.txt  -x 1.0 --kl -10     --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_11   -i $INPUTDIR/allNonResClones/filelist_allNonRes_11.txt  -x 1.0 --kl -9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_12   -i $INPUTDIR/allNonResClones/filelist_allNonRes_12.txt  -x 1.0 --kl -8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_13   -i $INPUTDIR/allNonResClones/filelist_allNonRes_13.txt  -x 1.0 --kl -7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_14   -i $INPUTDIR/allNonResClones/filelist_allNonRes_14.txt  -x 1.0 --kl -6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_15   -i $INPUTDIR/allNonResClones/filelist_allNonRes_15.txt  -x 1.0 --kl -5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_16   -i $INPUTDIR/allNonResClones/filelist_allNonRes_16.txt  -x 1.0 --kl -4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_17   -i $INPUTDIR/allNonResClones/filelist_allNonRes_17.txt  -x 1.0 --kl -3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_18   -i $INPUTDIR/allNonResClones/filelist_allNonRes_18.txt  -x 1.0 --kl -2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_19   -i $INPUTDIR/allNonResClones/filelist_allNonRes_19.txt  -x 1.0 --kl -1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_20   -i $INPUTDIR/allNonResClones/filelist_allNonRes_20.txt  -x 1.0 --kl 0.0001  --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_21   -i $INPUTDIR/allNonResClones/filelist_allNonRes_21.txt  -x 1.0 --kl 1       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_22   -i $INPUTDIR/allNonResClones/filelist_allNonRes_22.txt  -x 1.0 --kl 2       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_23   -i $INPUTDIR/allNonResClones/filelist_allNonRes_23.txt  -x 1.0 --kl 3       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_24   -i $INPUTDIR/allNonResClones/filelist_allNonRes_24.txt  -x 1.0 --kl 4       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_25   -i $INPUTDIR/allNonResClones/filelist_allNonRes_25.txt  -x 1.0 --kl 5       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_26   -i $INPUTDIR/allNonResClones/filelist_allNonRes_26.txt  -x 1.0 --kl 6       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_27   -i $INPUTDIR/allNonResClones/filelist_allNonRes_27.txt  -x 1.0 --kl 7       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_28   -i $INPUTDIR/allNonResClones/filelist_allNonRes_28.txt  -x 1.0 --kl 8       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_29   -i $INPUTDIR/allNonResClones/filelist_allNonRes_29.txt  -x 1.0 --kl 9       --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_30   -i $INPUTDIR/allNonResClones/filelist_allNonRes_30.txt  -x 1.0 --kl 10      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_31   -i $INPUTDIR/allNonResClones/filelist_allNonRes_31.txt  -x 1.0 --kl 11      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_32   -i $INPUTDIR/allNonResClones/filelist_allNonRes_32.txt  -x 1.0 --kl 12      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_33   -i $INPUTDIR/allNonResClones/filelist_allNonRes_33.txt  -x 1.0 --kl 13      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_34   -i $INPUTDIR/allNonResClones/filelist_allNonRes_34.txt  -x 1.0 --kl 14      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_35   -i $INPUTDIR/allNonResClones/filelist_allNonRes_35.txt  -x 1.0 --kl 15      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_36   -i $INPUTDIR/allNonResClones/filelist_allNonRes_36.txt  -x 1.0 --kl 16      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_37   -i $INPUTDIR/allNonResClones/filelist_allNonRes_37.txt  -x 1.0 --kl 17      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_38   -i $INPUTDIR/allNonResClones/filelist_allNonRes_38.txt  -x 1.0 --kl 18      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_39   -i $INPUTDIR/allNonResClones/filelist_allNonRes_39.txt  -x 1.0 --kl 19      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_40   -i $INPUTDIR/allNonResClones/filelist_allNonRes_40.txt  -x 1.0 --kl 20      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_41   -i $INPUTDIR/allNonResClones/filelist_allNonRes_41.txt  -x 1.0 --kl 21      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_42   -i $INPUTDIR/allNonResClones/filelist_allNonRes_42.txt  -x 1.0 --kl 22      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_43   -i $INPUTDIR/allNonResClones/filelist_allNonRes_43.txt  -x 1.0 --kl 23      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_44   -i $INPUTDIR/allNonResClones/filelist_allNonRes_44.txt  -x 1.0 --kl 24      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_45   -i $INPUTDIR/allNonResClones/filelist_allNonRes_45.txt  -x 1.0 --kl 25      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_46   -i $INPUTDIR/allNonResClones/filelist_allNonRes_46.txt  -x 1.0 --kl 26      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_47   -i $INPUTDIR/allNonResClones/filelist_allNonRes_47.txt  -x 1.0 --kl 27      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_48   -i $INPUTDIR/allNonResClones/filelist_allNonRes_48.txt  -x 1.0 --kl 28      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_49   -i $INPUTDIR/allNonResClones/filelist_allNonRes_49.txt  -x 1.0 --kl 29      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_50   -i $INPUTDIR/allNonResClones/filelist_allNonRes_50.txt  -x 1.0 --kl 30      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_51   -i $INPUTDIR/allNonResClones/filelist_allNonRes_51.txt  -x 1.0 --kl 31      --kt 1.0 -a True

## extra pts for a finer 1D plot
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99900   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99900.txt  -x 1.0 --kl 0.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99901   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99901.txt  -x 1.0 --kl 0.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99902   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99902.txt  -x 1.0 --kl 0.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99903   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99903.txt  -x 1.0 --kl 0.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99904   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99904.txt  -x 1.0 --kl 0.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99905   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99905.txt  -x 1.0 --kl 0.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99906   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99906.txt  -x 1.0 --kl 0.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99907   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99907.txt  -x 1.0 --kl 0.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99908   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99908.txt  -x 1.0 --kl 0.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99909   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99909.txt  -x 1.0 --kl 0.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99910   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99910.txt  -x 1.0 --kl 1.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99911   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99911.txt  -x 1.0 --kl 1.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99912   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99912.txt  -x 1.0 --kl 1.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99913   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99913.txt  -x 1.0 --kl 1.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99914   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99914.txt  -x 1.0 --kl 1.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99915   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99915.txt  -x 1.0 --kl 1.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99916   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99916.txt  -x 1.0 --kl 1.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99917   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99917.txt  -x 1.0 --kl 1.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99918   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99918.txt  -x 1.0 --kl 1.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99919   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99919.txt  -x 1.0 --kl 1.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99920   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99920.txt  -x 1.0 --kl 2.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99921   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99921.txt  -x 1.0 --kl 2.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99922   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99922.txt  -x 1.0 --kl 2.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99923   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99923.txt  -x 1.0 --kl 2.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99924   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99924.txt  -x 1.0 --kl 2.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99925   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99925.txt  -x 1.0 --kl 2.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99926   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99926.txt  -x 1.0 --kl 2.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99927   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99927.txt  -x 1.0 --kl 2.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99928   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99928.txt  -x 1.0 --kl 2.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99929   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99929.txt  -x 1.0 --kl 2.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99930   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99930.txt  -x 1.0 --kl 3.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99931   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99931.txt  -x 1.0 --kl 3.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99932   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99932.txt  -x 1.0 --kl 3.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99933   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99933.txt  -x 1.0 --kl 3.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99934   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99934.txt  -x 1.0 --kl 3.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99935   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99935.txt  -x 1.0 --kl 3.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99936   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99936.txt  -x 1.0 --kl 3.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99937   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99937.txt  -x 1.0 --kl 3.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99938   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99938.txt  -x 1.0 --kl 3.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99939   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99939.txt  -x 1.0 --kl 3.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99940   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99940.txt  -x 1.0 --kl 4.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99941   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99941.txt  -x 1.0 --kl 4.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99942   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99942.txt  -x 1.0 --kl 4.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99943   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99943.txt  -x 1.0 --kl 4.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99944   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99944.txt  -x 1.0 --kl 4.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99945   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99945.txt  -x 1.0 --kl 4.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99946   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99946.txt  -x 1.0 --kl 4.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99947   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99947.txt  -x 1.0 --kl 4.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99948   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99948.txt  -x 1.0 --kl 4.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99949   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99949.txt  -x 1.0 --kl 4.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99950   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99950.txt  -x 1.0 --kl 5.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99951   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99951.txt  -x 1.0 --kl 5.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99952   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99952.txt  -x 1.0 --kl 5.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99953   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99953.txt  -x 1.0 --kl 5.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99954   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99954.txt  -x 1.0 --kl 5.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99955   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99955.txt  -x 1.0 --kl 5.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99956   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99956.txt  -x 1.0 --kl 5.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99957   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99957.txt  -x 1.0 --kl 5.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99958   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99958.txt  -x 1.0 --kl 5.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99959   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99959.txt  -x 1.0 --kl 5.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99960   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99960.txt  -x 1.0 --kl 6.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99961   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99961.txt  -x 1.0 --kl 6.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99962   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99962.txt  -x 1.0 --kl 6.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99963   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99963.txt  -x 1.0 --kl 6.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99964   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99964.txt  -x 1.0 --kl 6.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99965   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99965.txt  -x 1.0 --kl 6.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99966   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99966.txt  -x 1.0 --kl 6.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99967   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99967.txt  -x 1.0 --kl 6.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99968   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99968.txt  -x 1.0 --kl 6.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99969   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99969.txt  -x 1.0 --kl 6.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99970   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99970.txt  -x 1.0 --kl 7.0      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99971   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99971.txt  -x 1.0 --kl 7.1      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99972   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99972.txt  -x 1.0 --kl 7.2      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99973   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99973.txt  -x 1.0 --kl 7.3      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99974   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99974.txt  -x 1.0 --kl 7.4      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99975   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99975.txt  -x 1.0 --kl 7.5      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99976   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99976.txt  -x 1.0 --kl 7.6      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99977   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99977.txt  -x 1.0 --kl 7.7      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99978   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99978.txt  -x 1.0 --kl 7.8      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99979   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99979.txt  -x 1.0 --kl 7.9      --kt 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_99980   -i $INPUTDIR/allNonResClones/filelist_allNonRes_99980.txt  -x 1.0 --kl 8.0      --kt 1.0 -a True



# # ## HH with reweighting --  a lot of files!
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_0   -i $INPUTDIR/allNonResClones/filelist_allNonRes_0.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_0.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_1   -i $INPUTDIR/allNonResClones/filelist_allNonRes_1.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_1.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_2   -i $INPUTDIR/allNonResClones/filelist_allNonRes_2.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_2.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_3   -i $INPUTDIR/allNonResClones/filelist_allNonRes_3.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_3.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_4   -i $INPUTDIR/allNonResClones/filelist_allNonRes_4.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_4.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_5   -i $INPUTDIR/allNonResClones/filelist_allNonRes_5.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_5.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_6   -i $INPUTDIR/allNonResClones/filelist_allNonRes_6.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_6.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_7   -i $INPUTDIR/allNonResClones/filelist_allNonRes_7.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_7.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_8   -i $INPUTDIR/allNonResClones/filelist_allNonRes_8.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_8.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_9   -i $INPUTDIR/allNonResClones/filelist_allNonRes_9.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_9.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_10   -i $INPUTDIR/allNonResClones/filelist_allNonRes_10.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_10.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_11   -i $INPUTDIR/allNonResClones/filelist_allNonRes_11.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_11.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_12   -i $INPUTDIR/allNonResClones/filelist_allNonRes_12.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_12.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_13   -i $INPUTDIR/allNonResClones/filelist_allNonRes_13.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_13.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_14   -i $INPUTDIR/allNonResClones/filelist_allNonRes_14.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_14.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_15   -i $INPUTDIR/allNonResClones/filelist_allNonRes_15.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_15.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_16   -i $INPUTDIR/allNonResClones/filelist_allNonRes_16.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_16.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_17   -i $INPUTDIR/allNonResClones/filelist_allNonRes_17.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_17.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_18   -i $INPUTDIR/allNonResClones/filelist_allNonRes_18.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_18.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_19   -i $INPUTDIR/allNonResClones/filelist_allNonRes_19.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_19.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_20   -i $INPUTDIR/allNonResClones/filelist_allNonRes_20.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_20.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_21   -i $INPUTDIR/allNonResClones/filelist_allNonRes_21.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_21.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_22   -i $INPUTDIR/allNonResClones/filelist_allNonRes_22.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_22.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_23   -i $INPUTDIR/allNonResClones/filelist_allNonRes_23.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_23.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_24   -i $INPUTDIR/allNonResClones/filelist_allNonRes_24.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_24.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_25   -i $INPUTDIR/allNonResClones/filelist_allNonRes_25.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_25.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_26   -i $INPUTDIR/allNonResClones/filelist_allNonRes_26.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_26.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_27   -i $INPUTDIR/allNonResClones/filelist_allNonRes_27.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_27.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_28   -i $INPUTDIR/allNonResClones/filelist_allNonRes_28.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_28.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_29   -i $INPUTDIR/allNonResClones/filelist_allNonRes_29.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_29.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_30   -i $INPUTDIR/allNonResClones/filelist_allNonRes_30.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_30.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_31   -i $INPUTDIR/allNonResClones/filelist_allNonRes_31.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_31.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_32   -i $INPUTDIR/allNonResClones/filelist_allNonRes_32.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_32.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_33   -i $INPUTDIR/allNonResClones/filelist_allNonRes_33.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_33.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_34   -i $INPUTDIR/allNonResClones/filelist_allNonRes_34.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_34.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_35   -i $INPUTDIR/allNonResClones/filelist_allNonRes_35.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_35.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_36   -i $INPUTDIR/allNonResClones/filelist_allNonRes_36.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_36.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_37   -i $INPUTDIR/allNonResClones/filelist_allNonRes_37.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_37.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_38   -i $INPUTDIR/allNonResClones/filelist_allNonRes_38.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_38.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_39   -i $INPUTDIR/allNonResClones/filelist_allNonRes_39.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_39.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_40   -i $INPUTDIR/allNonResClones/filelist_allNonRes_40.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_40.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_41   -i $INPUTDIR/allNonResClones/filelist_allNonRes_41.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_41.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_42   -i $INPUTDIR/allNonResClones/filelist_allNonRes_42.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_42.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_43   -i $INPUTDIR/allNonResClones/filelist_allNonRes_43.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_43.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_44   -i $INPUTDIR/allNonResClones/filelist_allNonRes_44.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_44.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_45   -i $INPUTDIR/allNonResClones/filelist_allNonRes_45.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_45.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_46   -i $INPUTDIR/allNonResClones/filelist_allNonRes_46.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_46.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_47   -i $INPUTDIR/allNonResClones/filelist_allNonRes_47.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_47.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_48   -i $INPUTDIR/allNonResClones/filelist_allNonRes_48.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_48.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_49   -i $INPUTDIR/allNonResClones/filelist_allNonRes_49.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_49.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_50   -i $INPUTDIR/allNonResClones/filelist_allNonRes_50.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_50.root -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_51   -i $INPUTDIR/allNonResClones/filelist_allNonRes_51.txt  -x 1.0 -w /home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/hhReweight/lambdaMap_51.root -a True


# ########
# ## SOME TESTS OF HH DYNAMIC REWEIGHT - IT WORKS!

# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 10 -k True -o $SKIMDIR/$OUTDIRR/SKIM_HH_node_SM   -i $INPUTDIR/38_GluGluToHHTo2B2Tau_node_SM_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt  -x 1.0 -a True

# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_dyn_21   -i $INPUTDIR/allNonResClones_dynRew/filelist_allNonRes_dr_21.txt  -x 1.0 --kl 1.0  --kt 1.0 -a True
# # python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 20 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_lambdarew_dyn_41   -i $INPUTDIR/allNonResClones_dynRew/filelist_allNonRes_dr_41.txt  -x 1.0 --kl 20.0 --kt 1.0 -a True