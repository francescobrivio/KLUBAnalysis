#OUTDIRR="Skims2017_26Lug_JEC_jecUnc_AbsoluteFlavMap_UP_nodeSM"
#OUTDIRR="Skims2017_26Lug_JEC_jecUnc_nodeSM"
OUTDIRR="Skims2017_26Lug_JEC_jecUnc_newSM"

INPUTDIR="inputFiles/JECproduction_Lug2017"
SKIMDIR="/gwteraz/users/brivio/SKIMMED_JEC"

source /cvmfs/cms.cern.ch/cmsset_default.sh
source scripts/setup.sh
mkdir /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/
mkdir $OUTDIRR
cp /gwpool/users/brivio/Hhh_1718/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/


echo "----- BEGIN -----" >> log.txt

echo "Submitting - newSM - "
echo "OUTDIR = $OUTDIRR"
echo "OUTDIR = $OUTDIRR" >> log.txt

# #### HH non resonant nodeSM
# python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_nodeSM  -i $INPUTDIR/113_GluGluToHHTo2B2Tau_node_SM_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt -x 1.0 -a True

 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 5 -k False -o $SKIMDIR/$OUTDIRR/SKIM_HH_benchmark_newSM   -i $INPUTDIR/allNonResClonesBench/filelist_bench_newSM.txt -x 1.0    --kl  1.0  --kt 1.0 --c2 0.0 --cg  0.0 --c2g  0.0

echo "---- FINISHED ---- " >> log.txt
 
