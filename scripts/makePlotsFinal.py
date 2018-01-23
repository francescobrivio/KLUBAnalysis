import os

inputDir = 'testPlot_TauTau_VBF/VBF_TauTau_12Gen18'

tag = '12Gen18'

#channels=['TauTau','MuTau','ETau']
channels=['TauTau']


#categories = ['VBFeta_baseline']
#categories = ['VBFeta_baseline', 'VBFeta_1b1j', 'VBFeta_2b0j', 'VBFeta_High_Eta', 'VBFeta_Low_Eta', 'VBFpt_baseline', 'VBFpt_1b1j', 'VBFpt_2b0j', 'VBFpt_High_Eta', 'VBFpt_Low_Eta']
#categories = ['VBFeta_baseline','VBFeta_1b1j','VBFeta_2b0j','VBFeta_High_Eta_1bM','VBFeta_Low_Eta_1bM','VBFeta_High_Eta_2bMM','VBFeta_Low_Eta_2bMM','VBFmjj_baseline','VBFmjj_1b1j','VBFmjj_2b0j','VBFmjj_High_Eta_1bM','VBFmjj_Low_Eta_1bM','VBFmjj_High_Eta_2bMM','VBFmjj_Low_Eta_2bMM']
categories = ['VBFeta_baseline', 'VBFeta_1b1j', 'VBFeta_2b0j', 'VBFeta_High_Eta_1bM', 'VBFeta_Low_Eta_1bM', 'VBFeta_High_Mass_1bM', 'VBFeta_Low_Mass_1bM', 'VBFmjj_baseline', 'VBFmjj_1b1j', 'VBFmjj_2b0j', 'VBFmjj_High_Eta_1bM', 'VBFmjj_Low_Eta_1bM', 'VBFmjj_High_Mass_1bM', 'VBFmjj_Low_Mass_1bM', 'VBFpt_baseline', 'VBFpt_1b1j', 'VBFpt_2b0j', 'VBFpt_High_Eta_1bM', 'VBFpt_Low_Eta_1bM', 'VBFpt_High_Mass_1bM', 'VBFpt_Low_Mass_1bM']


#vars = ['HHKin_mass_raw', 'MT2', 'VBFjj_mass_n', 'VBFjj_deltaEta_n']
#vars = ['MT2', 'HHKin_mass_raw', 'HH_pt', 'tauH_mass', 'bH_mass', 'bjet1_bID', 'bjet2_bID', 'VBFjet1_eta', 'VBFjet2_eta', 'VBFjet1n_eta', 'VBFjet2n_eta', 'VBFjj_mass', 'VBFjj_deltaEta', 'VBFjj_mass_n', 'VBFjj_deltaEta_n']
vars = ['MT2', 'HHKin_mass_raw', 'HH_pt', 'tauH_mass', 'bH_mass', 'bjet1_bID', 'bjet2_bID', 'VBFjet1_eta', 'VBFjet2_eta', 'VBFjet1n_eta', 'VBFjet2n_eta', 'VBFjj_mass', 'VBFjj_deltaEta', 'VBFjj_mass_n', 'VBFjj_deltaEta_n', 'jet3_eta', 'jet4_eta', 'jj_mass', 'jj_deltaEta']

# Lopp on Channels
for channel in channels:

    os.system('mkdir plots_'+channel+'VBF/'+tag)

    # Loop on baselines
    for category in categories:

        os.system('mkdir plots_'+channel+'VBF/'+tag+'/'+category)

        # Loop on variables
        for var in vars:

            if var is 'MT2':
                os.system('python scripts/makeFinalPlots.py --dir '+inputDir+' --var '+var+' --reg SR --blind-range 100 500 --sel '+category+' --channel '+channel+' --lymin 0.7 --lumi 35867 --quit --ratio --tag '+tag)
            elif var is 'HHKin_mass' or var is 'HHKin_mass_raw':
                os.system('python scripts/makeFinalPlots.py --dir '+inputDir+' --var '+var+' --reg SR --blind-range 250 400 --sel '+category+' --channel '+channel+' --lymin 0.7 --lumi 35867 --quit --ratio --tag '+tag)
            else:
                os.system('python scripts/makeFinalPlots.py --dir '+inputDir+' --var '+var+' --reg SR --sel '+category+' --channel '+channel+' --lymin 0.7 --lumi 35867 --quit --ratio --tag '+tag)


