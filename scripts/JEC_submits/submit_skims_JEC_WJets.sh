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
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_0_70       -i $INPUTDIR/WJets_incl.txt        -y 1.213784 -x 47493.98 -z 70
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_70_100     -i $INPUTDIR/WJetsHT-70-100.txt    -y 1.213784 -x 1423.499
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_100_200    -i $INPUTDIR/WJetsHT-100-200.txt   -y 1.213784 -x 1345
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_200_400    -i $INPUTDIR/WJetsHT-200-400.txt   -y 1.213784 -x 359.7
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_400_600    -i $INPUTDIR/WJetsHT-400-600.txt   -y 1.213784 -x 48.91
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_600_800    -i $INPUTDIR/WJetsHT-600-800.txt   -y 1.213784 -x 12.05
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 9 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_800_1200   -i $INPUTDIR/WJetsHT-800-1200.txt  -y 1.213784 -x 5.501
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 8 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_1200_2500  -i $INPUTDIR/WJetsHT-1200-2500.txt -y 1.213784 -x 1.329
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 8 -k True -o $SKIMDIR/$OUTDIRR/SKIM_WJets_HT_2500_Inf   -i $INPUTDIR/WJetsHT-2500-Inf.txt  -y 1.213784 -x 0.03216

