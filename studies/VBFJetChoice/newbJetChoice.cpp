#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TBranch.h"
#include "TString.h"
#include "TLorentzVector.h"

#include "bigTree.h" 

using namespace std;

// c++ -lm -o newbJetChoice newbJetChoice.cpp `root-config --glibs --cflags` -I ../../interface/

void appendFromFileList (TChain* chain, TString filename)
{
    //cout << "=== inizio parser ===" << endl;
    std::ifstream infile(filename.Data());
    std::string line;
    while (std::getline(infile, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()){ // skip empty lines
            chain->Add(line.c_str());
            // cout << ".. added file:" << line.c_str() << endl;
        }
    }
    return;
}

bool CheckBit (int number, int bitpos)
{
    bool res = number & (1 << bitpos);
    return res;
}

bool minGenCuts(TLorentzVector& vgen)
{
    if (vgen.Pt() < 20) return false;
    if (TMath::Abs(vgen.Eta()) > 2.4) return false;
    return true;
}


void debugGenPart(bigTree theBigTree)
{
  cout << "#### DEBUGGING ####"<< endl;
  for (uint igen = 0; igen < theBigTree.genpart_px->size(); ++igen)
    {
       if (abs(theBigTree.genpart_pdg->at(igen)) != 5) continue;
       //if (abs(theBigTree.genpart_status->at(igen)) != 23) continue;
       //if (theBigTree.genpart_HMothInd->at(igen) < 0) continue;
       cout << " - GenPart: " << igen << endl;
       cout << "    Flags(pdg,last,first,hard,HMoth,TopMoth): " << abs(theBigTree.genpart_pdg->at(igen)) << ", " << CheckBit(theBigTree.genpart_flags->at(igen), 13) << ", " << CheckBit(theBigTree.genpart_flags->at(igen), 12) << ", " << CheckBit(theBigTree.genpart_flags->at(igen), 7) << ", " << theBigTree.genpart_HMothInd->at(igen) << ", " << theBigTree.genpart_TopMothInd->at(igen) << endl;
       cout << "    Status: " << theBigTree.genpart_status->at(igen) << endl;
    }
  cout << "########" << endl;
}


pair<int,int> getCSVpair(vector<pair<TLorentzVector, int> > cleaned_jets, bigTree theBigTree)
{
    vector<pair<float, int> > csv_jets;
    for(int n=0; n<cleaned_jets.size(); n++)
    {
        csv_jets.push_back( make_pair( theBigTree.bCSVscore->at(cleaned_jets.at(n).second), cleaned_jets.at(n).second ) );
    }
    stable_sort(csv_jets.begin(), csv_jets.end());
    return make_pair(csv_jets.at(csv_jets.size()-1).second, csv_jets.at(csv_jets.size()-2).second);
}

pair<int,int> getPTpair(vector<pair<TLorentzVector, int> > cleaned_jets)
{
    vector<pair<float, int> > pt_jets;
    for(int n=0; n<cleaned_jets.size(); n++)
    {
        pt_jets.push_back(make_pair(cleaned_jets.at(n).first.Pt(), cleaned_jets.at(n).second ) );
    }
    stable_sort(pt_jets.begin(), pt_jets.end());
    return make_pair(pt_jets.at(pt_jets.size()-1).second, pt_jets.at(pt_jets.size()-2).second);
}

pair<int,int> getMJJpair(vector<pair<TLorentzVector, int> > cleaned_jets)
{
    pair<int,int> mjj_pair;
    float mass = -999.;
    for (int i=0; i<cleaned_jets.size(); i++)
    {
        for (int j=0; j<cleaned_jets.size(); j++)
        {
            //cout << " --- mass: " << (cleaned_jets.at(i).first+cleaned_jets.at(j).first).M() << endl;
            if ( (cleaned_jets.at(i).first+cleaned_jets.at(j).first).M() > mass)
            {
              mjj_pair = make_pair(cleaned_jets.at(i).second,cleaned_jets.at(j).second);
              mass = (cleaned_jets.at(i).first+cleaned_jets.at(j).first).M();
            }
        }
    }
    //cout << " ------ returning: " << mass << endl;
    return mjj_pair;
}

pair<int,int> getETApair(vector<pair<TLorentzVector, int> > cleaned_jets)
{
    pair<int,int> eta_pair;
    float dEta = -999.;
    for (int i=0; i<cleaned_jets.size(); i++)
    {
        for (int j=0; j<cleaned_jets.size(); j++)
        {
            //cout << " --- dEta: " << TMath::Abs(cleaned_jets.at(i).first.Eta()-cleaned_jets.at(j).first.Eta()) << endl;
            if ( TMath::Abs(cleaned_jets.at(i).first.Eta()-cleaned_jets.at(j).first.Eta()) > dEta)
            {
              eta_pair = make_pair(cleaned_jets.at(i).second,cleaned_jets.at(j).second);
              dEta = TMath::Abs(cleaned_jets.at(i).first.Eta()-cleaned_jets.at(j).first.Eta());
            }
        }
    }
    //cout << " ------ returning: " << dEta << endl;
    return eta_pair;
}


// ---------- ---- ------------
// ---------- MAIN ------------
// ---------- ---- ------------

int main (int argc, char** argv)
{
  // DEBUG
  bool DEBUG = 0;

  // input and output setup
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/VBF_study/1_VBFHHTo2B2Tau_CV_1_C2V_1_C3_1_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt" ;
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/VBF_study/2_VBFHHTo2B2Tau_CV_1_C2V_2_C3_1_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/81_GluGluToRadionToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/TT_powheg_fullyHad.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/TT_powheg_semiLep.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/WJets_incl.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/DY_mergedList.txt";

  cout << ".. start" << endl;

  TChain * bigChain = new TChain ("HTauTauTree/HTauTauTree") ;
  cout << ".. going to append to file" << endl;
  appendFromFileList (bigChain, inputFile);
  cout << ".. going to make the bigTree" << endl;
  bigTree theBigTree (bigChain) ;
  
  theBigTree.fChain->SetBranchStatus("*", 0);
  theBigTree.fChain->SetBranchStatus("genpart_*", 1);
  theBigTree.fChain->SetBranchStatus("jets_*", 1);
  theBigTree.fChain->SetBranchStatus("bCSVscore", 1);
  theBigTree.fChain->SetBranchStatus("PFjetID", 1);
  
  cout << ".. going in the loop" << endl;

  int nGoodSelected = 0;
  int nGoodSelectedGenCuts = 0;
  
  int nAllJetsMatched = 0;
  int nAllJetsMatchedGenCuts = 0;
  
  int nTot = 0;
  int nTotGenCuts = 0;

  TH1F* hMbbAll = new TH1F("hMbbAll", "hMbbAll", 200, 0, 200);

  TH1F* hMbbMatched = new TH1F("hMbbMatched", "hMbbMatched", 200, 0, 200);
  TH1F* hMbbHMatched = new TH1F("hMbbHMatched", "hMbbHMatched", 200, 0, 200);

  TH1F* dPhiMatched = new TH1F("dPhiMatched", "dPhiMatched", 100, -2.*TMath::Pi(), 2.*TMath::Pi());
  TH1F* dPhiHMatched = new TH1F("dPhiHMatched", "dPhiHMatched", 100, -2.*TMath::Pi(), 2.*TMath::Pi());

  int totEvts = 0;

  int nBGen         = 0;
  int nBGenH        = 0;
  int nGenVBF       = 0;
  int nMothersMatch = 0;
  
  int nbandbH      = 0;
  int nbandVBF     = 0;
  int genBVBFmatch = 0;

  int atLeast2bJets   = 0;
  int moreThan2bJets  = 0;
  int atLeast2cleanedJets  = 0;
  int moreThan2cleanedJets = 0;

  int twoBtoBH  = 0;
  int twoBtoVBF = 0;

  int twoCSVtoBH  = 0;
  int twoCSVtoVBF = 0;
  int twoPTtoBH   = 0;
  int twoPTtoVBF  = 0;
  int twoMJJtoBH  = 0;
  int twoMJJtoVBF = 0;
  int twoETAtoBH  = 0;
  int twoETAtoVBF = 0;
  
  int goodCSVtoBmatch = 0;
  int goodpTtoBmatch = 0;
  int goodMjjtoBmatch = 0;
  int gooddEtatoBmatch = 0;

  int VBFCSVmatch = 0;
  int VBFpTmatch = 0;
  int VBFMjjmatch = 0;
  int VBFdEtamatch = 0;

  int goodRemainingtoBmatch = 0;
  int VBFremainCSVmatch = 0;
  int VBFremainMjjmatch = 0;
  int VBFremaindEtamatch = 0;

  int goodEvts = 0;


  for (uint iEv = 0; true; ++iEv)
  {
    int got = theBigTree.fChain->GetEntry(iEv);
    if (got == 0) break;
    if (iEv > 1000) break;

    if (iEv % 100 == 0) cout << "------------------ Event: " << iEv << " -------------------------" << endl;

    totEvts +=1;

    if (DEBUG) debugGenPart(theBigTree);

    std::vector<int> vGenQuarks;
    std::vector<int> vGenQuarksH;
    std::vector<int> vGenVBF;
    for (uint igen = 0; igen < theBigTree.genpart_px->size(); ++igen)
    {
        // Ask for hard process
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 7)) continue; // 7: isHardProcess

        // Marked as "VBF" (it's ok! checked on https://github.com/LLRCMS/LLRHiggsTauTau/blob/master/NtupleProducer/plugins/GenFiller.cc#L354)
        if (CheckBit(theBigTree.genpart_flags->at(igen), 15)) vGenVBF.push_back(igen);

        // Ask for the gen b from Higgs
        if (abs(theBigTree.genpart_pdg->at(igen)) != 5) continue; // pdgid==5 --> b quark
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 12)) continue; // 12: first copy
        vGenQuarks.push_back(igen);

        // Further check on the b from Higgs
        if (theBigTree.genpart_HMothInd->at(igen) > 0) vGenQuarksH.push_back(igen); // if > 0 => is from Higgs!, this is a further check
    }

    // Counters to see how many times I don0t have exactly 2 gen b, 2 gen bH and 2 gen VBF
    if (vGenQuarks.size() != 2) nBGen+=1;
    if (vGenQuarksH.size() != 2) nBGenH+=1;
    if (vGenVBF.size() != 2) nGenVBF +=1;

    // gen b from H should be from the same gen H
    if (theBigTree.genpart_HMothInd->at(vGenQuarksH.at(0)) != theBigTree.genpart_HMothInd->at(vGenQuarksH.at(1))) nMothersMatch+=1;

    //Check that gen bb and gen bbH are the same
    if ( (vGenQuarksH.at(0)==vGenQuarks.at(0) && vGenQuarksH.at(1)==vGenQuarks.at(1)) || (vGenQuarksH.at(1)==vGenQuarks.at(0) && vGenQuarksH.at(0)==vGenQuarks.at(1)) ) nbandbH+=1;

    //Check that genVBF and genBBH are different
    if ( vGenVBF.at(0)==vGenQuarksH.at(0) || vGenVBF.at(0)==vGenQuarksH.at(1) || vGenVBF.at(1)==vGenQuarksH.at(0) || vGenVBF.at(1)==vGenQuarksH.at(1) ) nbandVBF+=1;

    // Two gen b
    TLorentzVector vgenb1(theBigTree.genpart_px->at(vGenQuarks.at(0)),theBigTree.genpart_py->at(vGenQuarks.at(0)),theBigTree.genpart_pz->at(vGenQuarks.at(0)),theBigTree.genpart_e->at(vGenQuarks.at(0)));
    TLorentzVector vgenb2(theBigTree.genpart_px->at(vGenQuarks.at(1)),theBigTree.genpart_py->at(vGenQuarks.at(1)),theBigTree.genpart_pz->at(vGenQuarks.at(1)),theBigTree.genpart_e->at(vGenQuarks.at(1)));

    // Two gen b from Higgs
    TLorentzVector vgenb1H(theBigTree.genpart_px->at(vGenQuarksH.at(0)),theBigTree.genpart_py->at(vGenQuarksH.at(0)),theBigTree.genpart_pz->at(vGenQuarksH.at(0)),theBigTree.genpart_e->at(vGenQuarksH.at(0)));
    TLorentzVector vgenb2H(theBigTree.genpart_px->at(vGenQuarksH.at(1)),theBigTree.genpart_py->at(vGenQuarksH.at(1)),theBigTree.genpart_pz->at(vGenQuarksH.at(1)),theBigTree.genpart_e->at(vGenQuarksH.at(1)));

    // Two gen VBF jets
    TLorentzVector vgenVBF1(theBigTree.genpart_px->at(vGenVBF.at(0)),theBigTree.genpart_py->at(vGenVBF.at(0)),theBigTree.genpart_pz->at(vGenVBF.at(0)),theBigTree.genpart_e->at(vGenVBF.at(0)));
    TLorentzVector vgenVBF2(theBigTree.genpart_px->at(vGenVBF.at(1)),theBigTree.genpart_py->at(vGenVBF.at(1)),theBigTree.genpart_pz->at(vGenVBF.at(1)),theBigTree.genpart_e->at(vGenVBF.at(1)));

    ///// for debuggin and understanding
    /*if (false)
    {
        cout << "--- debug of a VBF event, n. " << iEv << endl;
        cout << "VBF 1 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenVBF1.Pt(), vgenVBF1.Eta(), vgenVBF1.Phi()) << endl;
        cout << "VBF 2 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenVBF2.Pt(), vgenVBF2.Eta(), vgenVBF2.Phi()) << endl;
    }*/

    // Match VBF jets to genB
    if ( (vgenVBF1.DeltaR(vgenb1H)<0.5 && vgenVBF2.DeltaR(vgenb2H)<0.5) || (vgenVBF1.DeltaR(vgenb2H)<0.5 && vgenVBF2.DeltaR(vgenb1H)<0.5) ) genBVBFmatch +=1;;

    // --- RECO JETS PART ---
    // select the jets - note: no overlap removal for simplicity, hopefully it is a small effect
    vector<pair<float, int> > jets_and_btag;
    vector<pair<TLorentzVector, int> > jets_and_IDs;
    vector<pair<float, int> > jets_and_pt;

    // Loop on jets
    int nBtagMed = 0;
    for (uint ijet = 0; ijet < theBigTree.jets_px->size(); ++ijet)
    {
        TLorentzVector tlv_jet 
        (
            theBigTree.jets_px->at (ijet),
            theBigTree.jets_py->at (ijet),
            theBigTree.jets_pz->at (ijet),
            theBigTree.jets_e->at (ijet)
        );

        if (theBigTree.PFjetID->at (ijet) < 1) continue;    // 0 ; don't pass PF Jet ID; 1: loose, 2: tight, 3: tightLepVeto
        if (tlv_jet.Pt () < 20.) continue ;                 // jet_pt > 20 GeV
        if (TMath::Abs(tlv_jet.Eta()) > 5.) continue;       // 5. for VBF jets
        
        // VBF jets (eta<5)
        jets_and_IDs.push_back(make_pair(tlv_jet, ijet));
        
        if (TMath::Abs(tlv_jet.Eta()) > 2.4) continue;       // 2.4 for a reasonable b-tag (2.4 is the tracker acceptance)

        // create vect of pairs (csv score, jetID)
        jets_and_btag.push_back(make_pair(theBigTree.bCSVscore->at(ijet), ijet));

        // create vect of pairs (pt, jetID)
        jets_and_pt.push_back(make_pair(tlv_jet.Pt(), ijet));
    }
    
    // more than 2 b-jets
    if (jets_and_btag.size() > 2) moreThan2bJets +=1;
    
    // at least 2 b-jets
    if (jets_and_btag.size() < 2) continue;
    atLeast2bJets +=1;

    // Sort jets based on CSV value from smaller to bigger CSV
    if (DEBUG)
    {
      //cout << "Jets:";
      //for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).second;
      //cout << endl;
      cout << "Ordering by CSV..." << endl;
      stable_sort (jets_and_btag.begin(), jets_and_btag.end());
      cout << "Jets:";
      for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).second;
      cout << endl;
      cout << "CSV:";
      for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).first;
      cout << endl;
    }
    else stable_sort (jets_and_btag.begin(), jets_and_btag.end());

    // Sort jets based on pT value from smaller to bigger pT
    if (DEBUG)
    {
      //cout << "Jets:";
      //for (int n=0; n<jets_and_pt.size(); n++) cout << " " << jets_and_pt.at(n).second;
      //cout << endl;
      cout << "Ordering by pT..." << endl;
      stable_sort (jets_and_pt.begin(), jets_and_pt.end());
      cout << "Jets:";
      for (int n=0; n<jets_and_pt.size(); n++) cout << " " << jets_and_pt.at(n).second;
      cout << endl;
      cout << "pT:";
      for (int n=0; n<jets_and_pt.size(); n++) cout << " " << jets_and_pt.at(n).first;
      cout << endl;
    }
    else stable_sort (jets_and_pt.begin(), jets_and_pt.end());

    //cout << "jet_and_btag, jet_and_IDs, cleaned" << endl;
    //for (int n=0;n<jets_and_btag.size(); n++) cout<<jets_and_btag.at(n).second << " "; cout << endl;
    //for (int n=0;n<jets_and_IDs.size(); n++) cout<<jets_and_IDs.at(n).second << " "; cout << endl;
   
    // Get the two b jets (two highets CSV)
    int ijet1 = jets_and_btag.at(jets_and_btag.size()-1).second;
    int ijet2 = jets_and_btag.at(jets_and_btag.size()-2).second;
    TLorentzVector b_jet1(theBigTree.jets_px->at(ijet1),theBigTree.jets_py->at(ijet1),theBigTree.jets_pz->at(ijet1),theBigTree.jets_e->at(ijet1));
    TLorentzVector b_jet2(theBigTree.jets_px->at(ijet2),theBigTree.jets_py->at(ijet2),theBigTree.jets_pz->at(ijet2),theBigTree.jets_e->at(ijet2));

    if ( (b_jet1.DeltaR(vgenb1H)<0.5 && b_jet2.DeltaR(vgenb2H)) || (b_jet2.DeltaR(vgenb1H)<0.5 && b_jet1.DeltaR(vgenb2H)) )     twoBtoBH  +=1;
    if ( (b_jet1.DeltaR(vgenVBF1)<0.5 && b_jet2.DeltaR(vgenVBF2)) || (b_jet2.DeltaR(vgenVBF1)<0.5 && b_jet1.DeltaR(vgenVBF2)) ) twoBtoVBF +=1;

   
    // Clean jets from the bjets
    vector<pair<TLorentzVector, int> > cleaned_jets;
    for (int n=0; n<jets_and_IDs.size(); n++)
    {
        if ( jets_and_IDs.at(n).second != jets_and_btag.at(jets_and_btag.size()-1).second && jets_and_IDs.at(n).second != jets_and_btag.at(jets_and_btag.size()-2).second)
        cleaned_jets.push_back(jets_and_IDs.at(n));
    }
    
    //for (int n=0;n<cleaned_jets.size(); n++) cout<<cleaned_jets.at(n).second << " "; cout << endl;

    // at least 4 reco jets with wide eta
    if (cleaned_jets.size() > 2 ) moreThan2cleanedJets +=1;

    if (cleaned_jets.size() < 2 ) continue;
    atLeast2cleanedJets +=1;
    
    //Get 2 higher CSV, pT, mjj, dEta
    pair<int,int> csv_pair, pt_pair, mjj_pair, eta_pair;
    csv_pair = getCSVpair(cleaned_jets, theBigTree);
    pt_pair  = getPTpair (cleaned_jets);
    mjj_pair = getMJJpair(cleaned_jets);
    eta_pair = getETApair(cleaned_jets);
    
    TLorentzVector csv_jet1(theBigTree.jets_px->at(csv_pair.first),theBigTree.jets_py->at(csv_pair.first),theBigTree.jets_pz->at(csv_pair.first),theBigTree.jets_e->at(csv_pair.first));
    TLorentzVector csv_jet2(theBigTree.jets_px->at(csv_pair.second),theBigTree.jets_py->at(csv_pair.second),theBigTree.jets_pz->at(csv_pair.second),theBigTree.jets_e->at(csv_pair.second));
    
    TLorentzVector pt_jet1(theBigTree.jets_px->at(pt_pair.first),theBigTree.jets_py->at(pt_pair.first),theBigTree.jets_pz->at(pt_pair.first),theBigTree.jets_e->at(pt_pair.first));
    TLorentzVector pt_jet2(theBigTree.jets_px->at(pt_pair.second),theBigTree.jets_py->at(pt_pair.second),theBigTree.jets_pz->at(pt_pair.second),theBigTree.jets_e->at(pt_pair.second));

    TLorentzVector mjj_jet1(theBigTree.jets_px->at(mjj_pair.first),theBigTree.jets_py->at(mjj_pair.first),theBigTree.jets_pz->at(mjj_pair.first),theBigTree.jets_e->at(mjj_pair.first));
    TLorentzVector mjj_jet2(theBigTree.jets_px->at(mjj_pair.second),theBigTree.jets_py->at(mjj_pair.second),theBigTree.jets_pz->at(mjj_pair.second),theBigTree.jets_e->at(mjj_pair.second));

    TLorentzVector eta_jet1(theBigTree.jets_px->at(eta_pair.first),theBigTree.jets_py->at(eta_pair.first),theBigTree.jets_pz->at(eta_pair.first),theBigTree.jets_e->at(eta_pair.first));
    TLorentzVector eta_jet2(theBigTree.jets_px->at(eta_pair.second),theBigTree.jets_py->at(eta_pair.second),theBigTree.jets_pz->at(eta_pair.second),theBigTree.jets_e->at(eta_pair.second));

    
    // Matching with gen particles
    if ( (csv_jet1.DeltaR(vgenb1H)<0.5 && csv_jet2.DeltaR(vgenb2H)) || (csv_jet2.DeltaR(vgenb1H)<0.5 && csv_jet1.DeltaR(vgenb2H)) )     twoCSVtoBH  +=1;
    if ( (csv_jet1.DeltaR(vgenVBF1)<0.5 && csv_jet2.DeltaR(vgenVBF2)) || (csv_jet2.DeltaR(vgenVBF1)<0.5 && csv_jet1.DeltaR(vgenVBF2)) ) twoCSVtoVBF +=1;
    
    if ( (pt_jet1.DeltaR(vgenb1H)<0.5 && pt_jet2.DeltaR(vgenb2H)) || (pt_jet2.DeltaR(vgenb1H)<0.5 && pt_jet1.DeltaR(vgenb2H)) )         twoPTtoBH  +=1;
    if ( (pt_jet1.DeltaR(vgenVBF1)<0.5 && pt_jet2.DeltaR(vgenVBF2)) || (pt_jet2.DeltaR(vgenVBF1)<0.5 && pt_jet1.DeltaR(vgenVBF2)) )     twoPTtoVBF +=1;

    if ( (mjj_jet1.DeltaR(vgenb1H)<0.5 && mjj_jet2.DeltaR(vgenb2H)) || (mjj_jet2.DeltaR(vgenb1H)<0.5 && mjj_jet1.DeltaR(vgenb2H)) )      twoMJJtoBH  +=1;
    if ( (mjj_jet1.DeltaR(vgenVBF1)<0.5 && mjj_jet2.DeltaR(vgenVBF2)) || (mjj_jet2.DeltaR(vgenVBF1)<0.5 && mjj_jet1.DeltaR(vgenVBF2)) )  twoMJJtoVBF +=1;

    if ( (eta_jet1.DeltaR(vgenb1H)<0.5 && eta_jet2.DeltaR(vgenb2H)) || (eta_jet2.DeltaR(vgenb1H)<0.5 && eta_jet1.DeltaR(vgenb2H)) )      twoETAtoBH  +=1;
    if ( (eta_jet1.DeltaR(vgenVBF1)<0.5 && eta_jet2.DeltaR(vgenVBF2)) || (eta_jet2.DeltaR(vgenVBF1)<0.5 && eta_jet1.DeltaR(vgenVBF2)) )  twoETAtoVBF +=1;


    /*// Highest CSV jets 1 and 2 in order
    int ijet1 = jets_and_btag.at(jets_and_btag.size()-1).second;
    int ijet2 = jets_and_btag.at(jets_and_btag.size()-2).second;
    TLorentzVector tlv_jet1(theBigTree.jets_px->at(ijet1),theBigTree.jets_py->at(ijet1),theBigTree.jets_pz->at(ijet1),theBigTree.jets_e->at(ijet1));
    TLorentzVector tlv_jet2(theBigTree.jets_px->at(ijet2),theBigTree.jets_py->at(ijet2),theBigTree.jets_pz->at(ijet2),theBigTree.jets_e->at(ijet2));

    // Highest pT jets 3 and 4 in order
    int ijet3 = jets_and_pt.at(jets_and_pt.size()-1).second;
    int ijet4 = jets_and_pt.at(jets_and_pt.size()-2).second;
    TLorentzVector tlv_jet3(theBigTree.jets_px->at(ijet3),theBigTree.jets_py->at(ijet3),theBigTree.jets_pz->at(ijet3),theBigTree.jets_e->at(ijet3));
    TLorentzVector tlv_jet4(theBigTree.jets_px->at(ijet4),theBigTree.jets_py->at(ijet4),theBigTree.jets_pz->at(ijet4),theBigTree.jets_e->at(ijet4));

    // Get two jets with higher mjj
    pair<int,int> mjj_pair = getHigerMjj(jets_and_IDs);
    TLorentzVector tlv_jet5(theBigTree.jets_px->at(mjj_pair.first),theBigTree.jets_py->at(mjj_pair.first),theBigTree.jets_pz->at(mjj_pair.first),theBigTree.jets_e->at(mjj_pair.first));
    TLorentzVector tlv_jet6(theBigTree.jets_px->at(mjj_pair.second),theBigTree.jets_py->at(mjj_pair.second),theBigTree.jets_pz->at(mjj_pair.second),theBigTree.jets_e->at(mjj_pair.second));

    // Get two jets with higher deltaEta
    pair<int,int> deta_pair = getHigerDEta(jets_and_IDs);
    TLorentzVector tlv_jet7(theBigTree.jets_px->at(deta_pair.first),theBigTree.jets_py->at(deta_pair.first),theBigTree.jets_pz->at(deta_pair.first),theBigTree.jets_e->at(deta_pair.first));
    TLorentzVector tlv_jet8(theBigTree.jets_px->at(deta_pair.second),theBigTree.jets_py->at(deta_pair.second),theBigTree.jets_pz->at(deta_pair.second),theBigTree.jets_e->at(deta_pair.second));

    // CSV matched to genB
    if ( (tlv_jet1.DeltaR(vgenb1H)<0.5 && tlv_jet2.DeltaR(vgenb2H)<0.5) || (tlv_jet1.DeltaR(vgenb2H)<0.5 && tlv_jet2.DeltaR(vgenb1H)<0.5) ) goodCSVtoBmatch +=1;;

    // pT matched to genB
    if ( (tlv_jet3.DeltaR(vgenb1H)<0.5 && tlv_jet4.DeltaR(vgenb2H)<0.5) || (tlv_jet4.DeltaR(vgenb2H)<0.5 && tlv_jet3.DeltaR(vgenb1H)<0.5) ) goodpTtoBmatch +=1;;

    // mjj matched to genB
    if ( (tlv_jet5.DeltaR(vgenb1H)<0.5 && tlv_jet6.DeltaR(vgenb2H)<0.5) || (tlv_jet6.DeltaR(vgenb2H)<0.5 && tlv_jet5.DeltaR(vgenb1H)<0.5) ) goodMjjtoBmatch +=1;;

    // dEta matched to genB
    if ( (tlv_jet7.DeltaR(vgenb1H)<0.5 && tlv_jet8.DeltaR(vgenb2H)<0.5) || (tlv_jet7.DeltaR(vgenb2H)<0.5 && tlv_jet8.DeltaR(vgenb1H)<0.5) ) gooddEtatoBmatch +=1;;

    // Match gen VBF jets to CSV jets, pT jets, mjj jets, dEta jets
    if ( (tlv_jet1.DeltaR(vgenVBF1)<0.5 && tlv_jet2.DeltaR(vgenVBF2)<0.5) || (tlv_jet1.DeltaR(vgenVBF2)<0.5 && tlv_jet2.DeltaR(vgenVBF1)<0.5) ) VBFCSVmatch +=1;;
    if ( (tlv_jet3.DeltaR(vgenVBF1)<0.5 && tlv_jet4.DeltaR(vgenVBF2)<0.5) || (tlv_jet4.DeltaR(vgenVBF2)<0.5 && tlv_jet3.DeltaR(vgenVBF1)<0.5) ) VBFpTmatch +=1;;
    if ( (tlv_jet5.DeltaR(vgenVBF1)<0.5 && tlv_jet6.DeltaR(vgenVBF2)<0.5) || (tlv_jet6.DeltaR(vgenVBF2)<0.5 && tlv_jet5.DeltaR(vgenVBF1)<0.5) ) VBFMjjmatch +=1;;
    if ( (tlv_jet7.DeltaR(vgenVBF1)<0.5 && tlv_jet8.DeltaR(vgenVBF2)<0.5) || (tlv_jet7.DeltaR(vgenVBF2)<0.5 && tlv_jet8.DeltaR(vgenVBF1)<0.5) ) VBFdEtamatch +=1;;

    // Remove first two highest CSV jets (should be the bs from Higgs)
    //cout << " - Removing:" << endl;   
    //cout << "Jets:"; for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).second; cout << endl; 
    //cout << "CSV:";  for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).first;  cout << endl;
    jets_and_btag.erase(jets_and_btag.end()-2, jets_and_btag.end());
    //cout << "Jets:"; for (int n=0; n<jets_and_btag.size(); n++) cout << " " << jets_and_btag.at(n).second; cout << endl;

    vector<int> reduced_jets;
    for (int n=0; n<jets_and_btag.size() ; n++)
    {
       reduced_jets.push_back(jets_and_btag.at(n).second);
    }
    //cout << "Reduced:"; for (int n=0; n<reduced_jets.size(); n++) cout << " " << reduced_jets.at(n); cout << endl; 

    // remaining highest CSV jets
    int ijet9  = reduced_jets.at(reduced_jets.size()-1);
    int ijet10 = reduced_jets.at(reduced_jets.size()-2);
    TLorentzVector tlv_jet9(theBigTree.jets_px->at(ijet9),theBigTree.jets_py->at(ijet9),theBigTree.jets_pz->at(ijet9),theBigTree.jets_e->at(ijet9));
    TLorentzVector tlv_jet10(theBigTree.jets_px->at(ijet10),theBigTree.jets_py->at(ijet10),theBigTree.jets_pz->at(ijet10),theBigTree.jets_e->at(ijet10));

    // remaining CSV matched to genB
    if ( (tlv_jet9.DeltaR(vgenb1H)<0.5 && tlv_jet10.DeltaR(vgenb2H)<0.5) || (tlv_jet10.DeltaR(vgenb2H)<0.5 && tlv_jet9.DeltaR(vgenb1H)<0.5) ) goodRemainingtoBmatch +=1;;

    // remaining CSV matched to BVF
    if ( (tlv_jet9.DeltaR(vgenVBF1)<0.5 && tlv_jet10.DeltaR(vgenVBF2)<0.5) || (tlv_jet10.DeltaR(vgenVBF2)<0.5 && tlv_jet9.DeltaR(vgenVBF1)<0.5) ) VBFremainCSVmatch +=1;;

    // two remaining jets with higher mjj
    pair<int,int> mjj_reduced = getHigerMjjBis(reduced_jets, theBigTree);
    TLorentzVector tlv_jet11(theBigTree.jets_px->at(mjj_reduced.first),theBigTree.jets_py->at(mjj_reduced.first),theBigTree.jets_pz->at(mjj_reduced.first),theBigTree.jets_e->at(mjj_reduced.first));
    TLorentzVector tlv_jet12(theBigTree.jets_px->at(mjj_reduced.second),theBigTree.jets_py->at(mjj_reduced.second),theBigTree.jets_pz->at(mjj_reduced.second),theBigTree.jets_e->at(mjj_reduced.second));

    // remaining mjj matched to VBF
    if ( (tlv_jet11.DeltaR(vgenVBF1)<0.5 && tlv_jet12.DeltaR(vgenVBF2)<0.5) || (tlv_jet12.DeltaR(vgenVBF2)<0.5 && tlv_jet11.DeltaR(vgenVBF1)<0.5) ) VBFremainMjjmatch +=1;;
   
    // two remaining jets with higher dEta // giving problem with TLorentzVector
    pair<int,int> deta_reduced = getHigerDEtaBis(reduced_jets, theBigTree);
    TLorentzVector tlv_jet13(theBigTree.jets_px->at(deta_reduced.first),theBigTree.jets_py->at(deta_reduced.first),theBigTree.jets_pz->at(deta_reduced.first),theBigTree.jets_e->at(deta_reduced.first));
    TLorentzVector tlv_jet14(theBigTree.jets_px->at(deta_reduced.second),theBigTree.jets_py->at(deta_reduced.second),theBigTree.jets_pz->at(deta_reduced.second),theBigTree.jets_e->at(deta_reduced.second));

    // remaining dEta matched to BVF
    //if ( (tlv_jet13.DeltaR(vgenVBF1)<0.5 && tlv_jet14.DeltaR(vgenVBF2)<0.5) || (tlv_jet14.DeltaR(vgenVBF2)<0.5 && tlv_jet13.DeltaR(vgenVBF1)<0.5) ) VBFremaindEtamatch +=1;;



    // Filling histograms
    hMbbAll->Fill((tlv_jet1+tlv_jet2).M());

    bool drA1 = vgenb1.DeltaR(tlv_jet1) < 0.5;
    bool drA2 = vgenb1.DeltaR(tlv_jet2) < 0.5;
    bool drB1 = vgenb2.DeltaR(tlv_jet1) < 0.5;
    bool drB2 = vgenb2.DeltaR(tlv_jet2) < 0.5;

    bool drA1B2 = (drA1 && drB2);
    bool drA2B1 = (drA2 && drB1);

    bool drA1H = vgenb1H.DeltaR(tlv_jet1) < 0.5;
    bool drA2H = vgenb1H.DeltaR(tlv_jet2) < 0.5;
    bool drB1H = vgenb2H.DeltaR(tlv_jet1) < 0.5;
    bool drB2H = vgenb2H.DeltaR(tlv_jet2) < 0.5;

    bool drA1B2H = (drA1H && drB2H);
    bool drA2B1H = (drA2H && drB1H);

    nTot += 1;
    
    bool genCuts = minGenCuts(vgenb1) && minGenCuts(vgenb2) ;

    if (genCuts)
        nTotGenCuts += 1;
    
    if (drA1B2 || drA2B1)
    {
        ++nGoodSelected;
        if (genCuts)
        {
            nGoodSelectedGenCuts+=1;
        }
        hMbbMatched->Fill((tlv_jet1+tlv_jet2).M());
        dPhiMatched->Fill(tlv_jet1.DeltaPhi(tlv_jet2));
    }

    if (drA1B2H || drA2B1H)
    {
        hMbbHMatched->Fill((tlv_jet1+tlv_jet2).M());
        dPhiHMatched->Fill(tlv_jet1.DeltaPhi(tlv_jet2));
    }

    // check total events that matched one jet
    int allMatches = 0;
    /*for (uint im1 = 0; im1 < matchedTo1.size(); ++im1)
    {
        for (uint im2 = 0; im2 < matchedTo2.size(); ++im2)
        {
            if (matchedTo1.at(im1) != matchedTo2.at(im2))
            {
                allMatches += 1;
            }
        }
    }
    if (allMatches > 0)
    {
        nAllJetsMatched += 1;
        if (genCuts)
        {
            nAllJetsMatchedGenCuts += 1;            
        }
    }

    ///// for debuggin and understanding
    if (genCuts && !(drA1B2 || drA2B1) && allMatches > 0 && nBtagMed >= 2 && false)
    {
        cout << "--- debug of a non selected event " << iEv << endl;
        cout << endl;
        cout << "**** GEN INFO ****" << endl;
        cout << "Gen 1 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenb1.Pt(), vgenb1.Eta(), vgenb1.Phi()) << endl;
        cout << "Gen 2 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenb2.Pt(), vgenb2.Eta(), vgenb2.Phi()) << endl;
        cout << endl;
        cout << "**** JET INFO ****" << endl;
        for (uint ijet = 0; ijet< jets_and_btag.size(); ++ijet)
        {
            TLorentzVector tlv_deb
            (
                theBigTree.jets_px->at (jets_and_btag.at(ijet).second),
                theBigTree.jets_py->at (jets_and_btag.at(ijet).second),
                theBigTree.jets_pz->at (jets_and_btag.at(ijet).second),
                theBigTree.jets_e->at (jets_and_btag.at(ijet).second)
            );
            cout << "jet " << ijet << Form("pt = %.1f , eta = %.2f, phi = %.2f , CSV = %.3f" , tlv_deb.Pt(), tlv_deb.Eta(), tlv_deb.Phi(), jets_and_btag.at(ijet).first ) << endl;            
        }
    }*/
    
    goodEvts +=1;
  }

  TFile* fOut = new TFile("fOut.root", "recreate");
  //dPhiMatched->Write();
  //hMbbMatched->Write();
  //hMbbAll->Write();
  hMbbAll->Write();
  hMbbMatched->Write();
  hMbbHMatched->Write();
  dPhiMatched->Write();
  dPhiHMatched->Write();

  /*cout << "DONE" << endl;
  cout << "percentage good selected             : " << 100.*nGoodSelected/nTot << endl;
  cout << "percentage good selected w/ gen cuts : " << 100.*nGoodSelectedGenCuts/nTotGenCuts << endl;
  cout << "there was something good             : " << 100.*nAllJetsMatched/nTot << endl;
  cout << "there was something good w/ gen cuts : " << 100.*nAllJetsMatchedGenCuts/nTotGenCuts << endl;*/

  cout << endl;
  cout << "Total analyzed evts          : " << totEvts <<endl;
  cout << "---" << endl;
  cout << "Less than 2 genB, genBH, notSameH, genVBF: "
  << nBGen <<" ("<< 100.*nBGen/totEvts <<"%) - "
  << nBGenH <<" ("<< 100.*nBGenH/totEvts <<"%) - "
  << nMothersMatch <<" ("<< 100.*nMothersMatch/totEvts <<"%) - "
  << nGenVBF <<" ("<< 100.*nGenVBF/totEvts <<"%)"<< endl;
  cout << "---" << endl;
  cout << "genB and genBH matching      : " << nbandbH <<" ("<< 100.*nbandbH/totEvts <<"%)"<< endl;
  cout << "genB and genVBF are the same : " << nbandVBF <<" ("<< 100.*nbandVBF/totEvts <<"%)"<< endl;
  cout << "genVBF and genB matched      : " << genBVBFmatch <<" ("<< 100.*genBVBFmatch/totEvts <<"%)"<< endl;
  cout << "---" << endl;
  cout << "At least 2 bjets             : " << atLeast2bJets <<" ("<< 100.*atLeast2bJets/totEvts <<"%)"<< endl;
  cout << "More than 2 bjets            : " << moreThan2bJets <<" ("<< 100.*moreThan2bJets/totEvts <<"%)"<< endl;
  cout << "---" <<endl;
  cout << "At least 2 cleaned vbf-jets  : " << atLeast2cleanedJets <<" ("<< 100.*atLeast2cleanedJets/atLeast2bJets <<"%)"<< endl;
  cout << "More than 2 cleaned vbf-jets : " << moreThan2cleanedJets <<" ("<< 100.*moreThan2cleanedJets/atLeast2bJets <<"%)"<< endl;
  cout << "---" <<endl;
  cout << "Two B to BH                  :" << twoBtoBH <<" ("<< 100.*twoBtoBH/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two B to VBF                 :" << twoBtoVBF <<" ("<< 100.*twoBtoVBF/atLeast2cleanedJets <<"%)"<< endl;
  cout << "---" <<endl;
  cout << "Two CSV to BH                :" << twoCSVtoBH <<" ("<< 100.*twoCSVtoBH/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two CSV to VBF               :" << twoCSVtoVBF <<" ("<< 100.*twoCSVtoVBF/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two PT to BH                 :" << twoPTtoBH <<" ("<< 100.*twoPTtoBH/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two PT to VBF                :" << twoPTtoVBF <<" ("<< 100.*twoPTtoVBF/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two MJJ to BH                 :" << twoMJJtoBH <<" ("<< 100.*twoMJJtoBH/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two MJJ to VBF                :" << twoMJJtoVBF <<" ("<< 100.*twoMJJtoVBF/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two ETA to BH                 :" << twoETAtoBH <<" ("<< 100.*twoETAtoBH/atLeast2cleanedJets <<"%)"<< endl;
  cout << "Two ETA to VBF                :" << twoETAtoVBF <<" ("<< 100.*twoETAtoVBF/atLeast2cleanedJets <<"%)"<< endl;

  /*cout << "---" << endl;
  cout << "CSV matched to gen b         : " << goodCSVtoBmatch <<" ("<< 100.*goodCSVtoBmatch/atLeast4Jets <<"%)"<<endl;
  cout << "pT matched to gen b          : " << goodpTtoBmatch <<" ("<< 100.*goodpTtoBmatch/atLeast4Jets <<"%)"<<endl;
  cout << "mjj matched to gen b         : " << goodMjjtoBmatch <<" ("<< 100.*goodMjjtoBmatch/atLeast4Jets <<"%)"<<endl;
  cout << "dEta matched to gen b        : " << gooddEtatoBmatch <<" ("<< 100.*gooddEtatoBmatch/atLeast4Jets <<"%)"<<endl;
  cout << "---" << endl;
  cout << "CSV matched to VBF           : " << VBFCSVmatch <<" ("<< 100.*VBFCSVmatch/atLeast4Jets <<"%)"<<endl;
  cout << "pT matched to VBF            : " << VBFpTmatch <<" ("<< 100.*VBFpTmatch/atLeast4Jets <<"%)"<<endl;
  cout << "mjj matched to VBF           : " << VBFMjjmatch <<" ("<< 100.*VBFMjjmatch/atLeast4Jets <<"%)"<<endl;
  cout << "dEta matched to VBF          : " << VBFdEtamatch <<" ("<< 100.*VBFdEtamatch/atLeast4Jets <<"%)"<<endl;
  cout << "---" << endl;
  cout << "remaining CSV matched to b   : " << goodRemainingtoBmatch <<" ("<< 100.*goodRemainingtoBmatch/atLeast4Jets <<"%)"<<endl;
  cout << "remaining CSV matched to VBF : " << VBFremainCSVmatch <<" ("<< 100.*VBFremainCSVmatch/atLeast4Jets <<"%)"<<endl;
  cout << "remaining Mjj matched to VBF : " << VBFremainMjjmatch <<" ("<< 100.*VBFremainMjjmatch/atLeast4Jets <<"%)"<<endl;
  cout << "remaining dEta matched to VBF: " << VBFremaindEtamatch <<" ("<< 100.*VBFremaindEtamatch/atLeast4Jets <<"%)"<<endl;*/
  cout << "---" << endl;
  cout << "Total good events            : " << goodEvts <<" ("<< 100.*goodEvts/totEvts <<"%)"<< endl;
}



