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


# #####################
# ### tW top :
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 40 -k True -o $SKIMDIR/$OUTDIRR/SKIM_ST_tW_antitop_5f_inclusiveDecays   -i $INPUTDIR/33_ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt   -x 35.6
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 39 -k True -o $SKIMDIR/$OUTDIRR/SKIM_ST_tW_top_5f_inclusiveDecays       -i $INPUTDIR/34_ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt       -x 35.6

