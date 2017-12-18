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



###################
# TT
# TT x section: 831.76 for inclusive sample, W->had 67,60% , W->l nu 3*10,8% = 32,4% (sum over all leptons)
# hh = 45.7%
# ll = 10.5%
# hl = 21.9% (x2 for permutation t-tbar)
# technical note: use -q long if splitting on ~180-350 jobs
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 100 -k True -o $SKIMDIR/$OUTDIRR/SKIM_TT_fullyLep  -i $INPUTDIR/TT_powheg_fullyLep.txt    -x 87.3348  -t True -b 4