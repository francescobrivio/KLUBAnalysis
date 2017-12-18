OUTDIRR="Skims2017_26Lug_JEC_jecUnc_TimePtEta_DOWN_ZH"

INPUTDIR="inputFiles/JECproduction_Lug2017"
SKIMDIR="/gwteraz/users/brivio/SKIMMED_JEC"

source /cvmfs/cms.cern.ch/cmsset_default.sh
source scripts/setup.sh
mkdir /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/
mkdir $OUTDIRR
cp /gwpool/users/brivio/Hhh_1718/CMSSW_7_4_7/src/KLUBAnalysis/scripts/haddAll.sh /gwteraz/users/brivio/SKIMMED_JEC/$OUTDIRR/


echo "----- BEGIN -----" >> log.txt

echo "Submitting - ZH - "
echo "OUTDIR = $OUTDIRR"
echo "OUTDIR = $OUTDIRR" >> log.txt

# # ### SM Higgs  (ZH) - 3*15 = 45
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 15  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HTauTau      -i $INPUTDIR/73_ZHToTauTau_M125_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt      -x 0.8839 -y 0.0632
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 30  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HBB_ZLL      -i $INPUTDIR/ZH_HToBB_ZToLL.txt                                                                                                              -x 0.8839 -y 0.0583
 python scripts/skimNtuple.py -T $OUTDIRR -s True -c  config/skim_2017.cfg  -n 15  -k True -o $SKIMDIR/$OUTDIRR/SKIM_ZH_HBB_ZQQ      -i $INPUTDIR/76_ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt  -x 0.8839 -y 0.4034

echo "---- FINISHED ---- " >> log.txt
 
