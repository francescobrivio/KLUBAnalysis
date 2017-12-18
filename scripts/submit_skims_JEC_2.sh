OUTDIRR="Skims2017_26Lug_JEC_jecUnc_TEST"
AMESSAGE="Resub failed data with more jobs. Skims fixing the MuMu sideband SFs on both muons. Reweighted benchmarks 5D reweight. Introduced ele10 sideband. Data from 22Feb production, 7Feb2017 REminiaod. Data with fixed evts number. Added OR of non ER mu triggers to see if agreement is retrieved.Includes nBadMu filter applied. 11 Feb 2017 MC production (most but not all jobs are finished)."

INPUTDIR="inputFiles/JECproduction_Lug2017"
SKIMDIR="/gwteraz/users/brivio/SKIMMED_JEC"


### JEC ###
source /cvmfs/cms.cern.ch/cmsset_default.sh
source scripts/setup.sh
#mkdir /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/
mkdir /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/
mkdir $OUTDIRR
#touch /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/README.txt
#echo $AMESSAGE > /gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/README.txt
cp /gwpool/users/brivio/Hhh_1718/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/ #/gwpool/users/brivio/Hhh_1718/SKIMMED_JEC/$OUTDIRR/



# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-260   -i $INPUTDIR/95_GluGluToBulkGravitonToHHTo2B2Tau_M-260_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToBulkGravitonToHHTo2B2Tau_M-270   -i $INPUTDIR/96_GluGluToBulkGravitonToHHTo2B2Tau_M-270_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True



 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 1 -k True -o $SKIMDIR/$OUTDIRR/SKIM_GluGluToRadionToHHTo2B2Tau_M-250   -i $INPUTDIR/81_GluGluToRadionToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt       -x 1.0 -a True


