OUTDIRR="Skims2017_26Lug_JEC_jecUnc_FlavorQCD_UP"
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


# # #####################
# # ### DI-boson: - 7*7 + 6*5 = 49 + 30 = 79
# # # note2: all njobs listed here were increased by x10 to make them run on the short queue
# # #XS taken from here: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#Diboson
# ## for Z->4Q, I scaled from ZZ->2nu2Q using BR(2Nu2Q) = 2 * 20% * 69.91%
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 6   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWTo2L2Nu  -i $INPUTDIR/63_WWTo2L2Nu_13TeV-powheg__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt   -x 12.178
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 6   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWToLNuQQ  -i $INPUTDIR/WWToLNuQQ.txt                                                                                                  -x 49.997
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 6   -k True -o $SKIMDIR/$OUTDIRR/SKIM_WWTo4Q     -i $INPUTDIR/66_WWTo4Q_13TeV-powheg__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 51.723

 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 6   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2Q2Nu  -i $INPUTDIR/54_ZZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt -x 4.04
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 6   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2L2Nu  -i $INPUTDIR/55_ZZTo2L2Nu_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt               -x 0.564
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo2L2Q   -i $INPUTDIR/56_ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt  -x 3.22
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo4L     -i $INPUTDIR/57_ZZTo4L_13TeV-amcatnloFXFX-pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 1.212
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7   -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZZTo4Q     -i $INPUTDIR/60_ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt    -x 7.06091

 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo3LNu       -i $INPUTDIR/69_WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 4.42965
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo1L3Nu      -i $INPUTDIR/70_WZTo1L3Nu_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt    -x 3.033
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo1L1Nu2Q    -i $INPUTDIR/71_WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3.txt  -x 10.71
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 7  -k True -o $SKIMDIR/$OUTDIRR/SKIM_WZTo2L2Q       -i $INPUTDIR/72_WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt     -x 5.595

