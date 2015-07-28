# DoubleHiggs

repo for the h->hh analysis within the LLR framework

## Installing
``` 
cd /data_CMS/cms/govoni/CMSSW_7_4_3/src
cmsenv
cd -
git clone git@github.com:bvormwald/HHKinFit.git
ls
cd HHKinFit/
``` 
change 0.000001 into 0.000002 at the following line:
https://github.com/bvormwald/HHKinFit/blob/master/src/HHKinFit.cpp#L519
``` 
./compile.sh
cd ..
git clone git@github.com:LLRCMS/DoubleHiggs.git
cd DoubleHiggs
source scripts/setup.sh
make
make exe
```
 
## Big ntuples skimming

Example on how to submit a skimming.
 * the "-i" option indicates the folder where the LLR ntuples are stored
 * the "-x" option indicates the cross-section of the sample, which will be multiplied to the MC weight variable
 * jobs to be executed are saved in the folder SKIM_XXX in the current directory, where XXX is the name of the folder containing the LLR ntuples
   * this means that for a re-skimming the same folder is used, which means that the resubmit can be done only on the last skim performed

```
python scripts/skimNtuple.py \
-i /data_CMS/cms/govoni/test_submit_to_tier3/HiggsTauTauOutput_DY_-1Events_0Skipped_1436202480.82 \
-x 6025.2
``` 

Example on how to check if all the jobs run successfully 
(based on the presence of the "done" file for any failures at cluster level,
and on the presence of "Error" in the log file for root problems,
which sometimes arise.
 * The flag "-i" is the same as the submission job
 * The "-r" option triggers the checking, always overcomes the initial submission (but if set to "none") and by default lists the problematic jobs.
 * The "-r run" option triggers the resubmission of the jobs identified as failed
 * -s True: introduce a sleep of 0.1 s between each submission, to avoid output saving problems
 * -I True: run the inclusive skim, i.e. does not cut on number of jets

To skim one sample:
```
python scripts/skimNtuple.py -d True -s True -o /data_CMS/cms/govoni/test_submit_to_tier3/Skims3/SKIM_Data_SingleMuon     -i /data_CMS/cms/cadamuro/test_submit_to_tier3/HiggsTauTauOutput_Data_SingleMuon_-1Events_0Skipped_1437412858.02
```
Once jobs are run to check whether there's files to be resubmitted, 
```
python scripts/skimNtuple.py -r list -d True -s True -o /data_CMS/cms/govoni/test_submit_to_tier3/Skims3/SKIM_Data_SingleMuon     -i /data_CMS/cms/cadamuro/test_submit_to_tier3/HiggsTauTauOutput_Data_SingleMuon_-1Events_0Skipped_1437412858.02
```
If there's any, actually resubmit, 
```
python scripts/skimNtuple.py -r run -d True -s True -o /data_CMS/cms/govoni/test_submit_to_tier3/Skims3/SKIM_Data_SingleMuon     -i /data_CMS/cms/cadamuro/test_submit_to_tier3/HiggsTauTauOutput_Data_SingleMuon_-1Events_0Skipped_1437412858.02
```
Once all jobs finished successfully, hadd the results (this is necessary for the analysis to run)
```
python scripts/skimNtuple.py -H doit -d True -s True -o /data_CMS/cms/govoni/test_submit_to_tier3/Skims3/SKIM_Data_SingleMuon     -i /data_CMS/cms/cadamuro/test_submit_to_tier3/HiggsTauTauOutput_Data_SingleMuon_-1Events_0Skipped_1437412858.02
```

## Plotting

Example on how to draw plots of variables already existing in the skimmed ntuples.
Plots are produced in the "plotter" folder, divided into a set with the proper normalisation
according to the XS inserted in the processing, and one with shapes, where the backgrounds
are combined according to the XS and then the total result is normalised, to compare to 
the signal in shapes.
 * the config file ```plotter_muTau.cfg``` contains all the relevant information for the run
 * an additional ```plotter_muTau.cut``` contains the selections to be applied in the plots generation

```
./bin/plotNEW.exe config/plotter_muTau.cfg 
```

## MVA training

Example on how to run a MVA training on the reduced flat trees.
The TMVATraining.cpp executable uses the TMVATrainingClass to interface to the TMVA package.
 * preselections are applied according to the cfg file
 * BDT training methods available
 * more training methods could be added copyting them from the TMVATrainingClass, which unfortunately does not work properly, since it would need a artisanal root patch which we don't want to do
 
 ```
./bin/TMVATrainingZero.exe ./config/TMVATraining.cfg
```
The MVA info is then added to the SKIM tree with:
```
./bin/addTMVA.exe config/addTMVA.cfg 
```
