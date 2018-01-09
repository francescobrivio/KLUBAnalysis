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

// c++ -lm -o bJetChoice bJetChoice.cpp `root-config --glibs --cflags` -I ../../interface/

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

// Get two jets with higher mjj
pair<int,int> getHigerMjj(vector<pair<TLorentzVector, int> > jets_and_IDs)
{
    pair<int,int> mjj_pair;
    float mass = -99.;
    for (int i=0; i<jets_and_IDs.size(); i++)
    {
        for (int j=0; j<jets_and_IDs.size(); j++)
        {
            //cout << " --- mass: " << (jets_and_IDs.at(i).first+jets_and_IDs.at(j).first).M() << endl;
            if ( (jets_and_IDs.at(i).first+jets_and_IDs.at(j).first).M() > mass) 
            {
              mjj_pair = make_pair(jets_and_IDs.at(i).second,jets_and_IDs.at(j).second);
              mass = (jets_and_IDs.at(i).first+jets_and_IDs.at(j).first).M();
            }
        }
    }
    //cout << " ------ returning: " << mass << endl; 
    return mjj_pair;
}

pair<int,int> getHigerMjjBis(vector<int> reduced_jets, bigTree theBigTree)
{
    vector<pair<TLorentzVector, int> > jets;
    for (int n=0; n<reduced_jets.size(); n++)
    {
        TLorentzVector tmp_jet(theBigTree.genpart_px->at(reduced_jets.at(n)),theBigTree.genpart_py->at(reduced_jets.at(n)),theBigTree.genpart_pz->at(reduced_jets.at(n)),theBigTree.genpart_e->at(reduced_jets.at(n)));
        jets.push_back(make_pair(tmp_jet,reduced_jets.at(n)));
    }

    pair<int,int> mjj_pair;
    float mass = -999.;
    float pt;
    for (int i=0; i<jets.size(); i++)
    {
        for (int j=0; j<jets.size(); j++)
        {
            //cout << " --- mass: " << (jets.at(i).first+jets.at(j).first).M() << endl;
            if ( (jets.at(i).first+jets.at(j).first).M() > mass) 
            {
              mjj_pair = make_pair(jets.at(i).second,jets.at(j).second);
              mass = (jets.at(i).first+jets.at(j).first).M();
            }
        }
    }
    //cout << " ------ returning: " << mass << endl; 
    return mjj_pair;
}

// Get two jets with higher deltaEta
pair<int,int> getHigerDEta(vector<pair<TLorentzVector, int> > jets_and_IDs)
{
    float dEtaMax = -999.0;
    int idx1 = -1;
    int idx2 = -1;
    // double loop to find the highest DeltaEta between jet pairs
    for (int i = 0; i < jets_and_IDs.size(); i++ )
    {
        float eta1 = jets_and_IDs.at(i).first.Eta();
        for (int j = i+1; j < jets_and_IDs.size(); j++)
        {
            float eta2 = jets_and_IDs.at(j).first.Eta();
            float dEta = TMath::Abs(eta1 - eta2);
            //cout << " --- dEta: " << dEta << endl;
            if (dEta > dEtaMax)
            {
                dEtaMax = dEta;
                idx1 = jets_and_IDs.at(i).second;
                idx2 = jets_and_IDs.at(j).second;
            }
        }
    }
    //cout << "------ returning: " << dEtaMax << endl;
    return make_pair(idx1,idx2);
}

pair<int,int> getHigerDEtaBis(vector<int> reduced_jets, bigTree theBigTree)
{
    vector<pair<TLorentzVector, int> > jets;
    for (int n=0; n<reduced_jets.size(); n++)
    {
        TLorentzVector tmp_jet(theBigTree.genpart_px->at(reduced_jets.at(n)),theBigTree.genpart_py->at(reduced_jets.at(n)),theBigTree.genpart_pz->at(reduced_jets.at(n)),theBigTree.genpart_e->at(reduced_jets.at(n)));
        jets.push_back(make_pair(tmp_jet,reduced_jets.at(n)));
    }
    double dEtaMax = -999.0;
    int idx1 = -1;
    int idx2 = -1;
    // double loop to find the highest DeltaEta between jet pairs
    for (int i = 0; i < jets.size(); i++ )
    {
        //cout << " #-# Jet: " << i <<endl;
        double eta1 = jets.at(i).first.Eta();
        //cout << "first pt: " << jets.at(i).first.Pt() << endl;
        //cout << "first eta gotten" <<endl;
        for (int j = i+1; j < jets.size(); j++)
        {
            //cout << " #----# jet: " << j <<endl;
            double eta2 = jets.at(j).first.Pt(); //Eta()
            //cout << "second pt: " << jets.at(j).first.Pt() << endl;
            //cout << "second eta gotten"<<endl;
            double dEta = TMath::Abs(eta1 - eta2);
            //cout << " --- dEta: " << dEta << endl;
            if (dEta > dEtaMax)
            {
                dEtaMax = dEta;
                idx1 = jets.at(i).second;
                idx2 = jets.at(j).second;
            }
        }
    }
    //cout << "------ returning: " << dEtaMax << endl;
    return make_pair(idx1,idx2);
}


// ---------- MAIN ------------
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

  int nBGen = 0;
  int nBGenH = 0;
  int nMothersMatch = 0;
  int nGenVBF = 0;
  int nbandbH = 0;
  int nbandVBF = 0;
  int genBVBFmatch = 0;

  int atLeast4Jets = 0;

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

    if (DEBUG) debugGenPart(theBigTree);

    totEvts += 1;

    std::vector<int> vGenQuarks;
    std::vector<int> vGenQuarksH;
    std::vector<int> vGenVBF;
    for (uint igen = 0; igen < theBigTree.genpart_px->size(); ++igen)
    {
        // Ask for hard process
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 7)) continue; // 7: isHardProcess
        //if (abs(theBigTree.genpart_status->at(igen)) != 23) continue; // status==23..not sure!

        // Marked as "VBF" (should be checked in https://github.com/LLRCMS/LLRHiggsTauTau/blob/master/NtupleProducer/plugins/GenFiller.cc#L354)
        if (CheckBit(theBigTree.genpart_flags->at(igen), 15)) vGenVBF.push_back(igen);

        // Ask for the gen b from Higgs
        if (abs(theBigTree.genpart_pdg->at(igen)) != 5) continue; // pdgid==5 --> b quark
        //if (!CheckBit(theBigTree.genpart_flags->at(igen), 13)) continue; // 13: last copy
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 12)) continue; // 12: first copy
        vGenQuarks.push_back(igen);

        // Further check on the b from Higgs
        if (theBigTree.genpart_HMothInd->at(igen) > 0) vGenQuarksH.push_back(igen); // if > 0 => is from Higgs!, this is a further check
    }

    /*cout << "- isVBF size: " << vGenVBF.size() << endl;
    for (int n=0; n<vGenVBF.size(); n++) cout << " " << vGenVBF.at(n);
    cout << endl;

    cout << "- isbbH size: " << vGenQuarksH.size() << endl;
    for (int n=0; n<vGenQuarksH.size(); n++) cout << " " << vGenQuarksH.at(n);
    cout << endl;

    cout << "- isbb size: " << vGenQuarks.size() << endl;
    for (int n=0; n<vGenQuarks.size(); n++) cout << " " << vGenQuarks.at(n);
    cout << endl;*/

    if (vGenQuarks.size() != 2)
    {
        //cout << " ** 1) Event:" << iEv << endl;
        //cout << " ** 1) Wrong b quark size: " << vGenQuarks.size() << endl;
        //debugGenPart(theBigTree);
        //cout << "-----------------------------------------------------" << endl;
        nBGen+=1;
        //continue;
    }

    if (vGenQuarksH.size() != 2)
    {
        //cout << " ** 2) Event:" << iEv << endl;
        //cout << " ** 2) Wrong b quark size from H: " << vGenQuarksH.size() << endl;
        //cout << "-----------------------------------------------------" << endl;
        nBGenH+=1;
        //continue;
    }

    if (vGenVBF.size() != 2)
    {
        nGenVBF +=1;
        //continue;
    }

    if (theBigTree.genpart_HMothInd->at(vGenQuarksH.at(0)) != theBigTree.genpart_HMothInd->at(vGenQuarksH.at(1)))
    {
        //cout << " ** 3) Event:" << iEv << endl;
        //cout << " ** 3) No match between HMothers : " << theBigTree.genpart_HMothInd->at(0) << " , " << theBigTree.genpart_HMothInd->at(1) << endl;
        //cout << "-----------------------------------------------------" << endl;
        nMothersMatch+=1;
        //continue;
    }

    //Check that gen bb and gen bbH are the same
    if ( (vGenQuarksH.at(0)==vGenQuarks.at(0) && vGenQuarksH.at(1)==vGenQuarks.at(1)) || (vGenQuarksH.at(1)==vGenQuarks.at(0) && vGenQuarksH.at(0)==vGenQuarks.at(1)) )
    {
        nbandbH+=1;
    }

    //Check that genVBF and genBBH are different
    if ( vGenVBF.at(0)==vGenQuarksH.at(0) || vGenVBF.at(0)==vGenQuarksH.at(1) || vGenVBF.at(1)==vGenQuarksH.at(0) || vGenVBF.at(1)==vGenQuarksH.at(1) )
    {
        nbandVBF+=1;
        cout << "--------   *****   same bbH and VBF particle! " << endl;
    }

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
    if (false)
    {
        cout << "--- debug of a VBF event, n. " << iEv << endl;
        cout << "VBF 1 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenVBF1.Pt(), vgenVBF1.Eta(), vgenVBF1.Phi()) << endl;
        cout << "VBF 2 : " << Form("pt = %.1f , eta = %.2f, phi = %.2f" , vgenVBF2.Pt(), vgenVBF2.Eta(), vgenVBF2.Phi()) << endl;
    }

    // Match VBF jets to genB
    if ( (vgenVBF1.DeltaR(vgenb1H)<0.5 && vgenVBF2.DeltaR(vgenb2H)<0.5) || (vgenVBF1.DeltaR(vgenb2H)<0.5 && vgenVBF2.DeltaR(vgenb1H)<0.5) ) genBVBFmatch +=1;;

    // --- RECO JETS PART ---
    // select the jets - note: no overlap removal for simplicity, hopefully it is a small effect
    vector<pair<float, int> > jets_and_btag;
    vector<pair<float, int> > jets_and_pt;
    vector<pair<TLorentzVector, int> > jets_and_IDs;

    vector<int> matchedToGenB;

    vector<int> matchedTo1;
    vector<int> matchedTo2;

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

        // if (theBigTree.jets_PUJetID->at (iJet) < PUjetID_minCut) continue ;
        if (theBigTree.PFjetID->at (ijet) < 1) continue; // 0 ; don't pass PF Jet ID; 1: loose, 2: tight, 3: tightLepVeto
        if (tlv_jet.Pt () < 20. /*GeV*/) continue ; 
        //if (TMath::Abs(tlv_jet.Eta()) > 2.4) continue; // 2.4 for b-tag
        if (TMath::Abs(tlv_jet.Eta()) > 5.) continue; // 2.4 for b-tag

        // create vect of pairs (csv score, jetID)
        jets_and_btag.push_back(make_pair(theBigTree.bCSVscore->at(ijet), ijet));

        // create vect of pairs (pt, jetID)
        jets_and_pt.push_back(make_pair(tlv_jet.Pt(), ijet));
        
        // create vect of pairs (tlv_jet, jetID)
        jets_and_IDs.push_back(make_pair(tlv_jet, ijet));
    }
    
    //if (jets_and_btag.size() < 2) continue;
    if (jets_and_btag.size() < 4 ) 
    {
        //cout << " ** 4) Less than 4 reco jets: " << jets_and_btag.size() << endl;
        //cout << "-----------------------------------------------------" << endl;
        continue;
    }
    atLeast4Jets +=1;

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

    // Highest CSV jets 1 and 2 in order
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
    for (uint im1 = 0; im1 < matchedTo1.size(); ++im1)
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

    goodEvts +=1;

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
    }
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
  cout << "Less than 2 gen b            : " << nBGen <<" ("<< 100.*nBGen/totEvts <<"%)"<< endl;
  cout << "Less than 2 gen b from H     : " << nBGenH <<" ("<< 100.*nBGenH/totEvts <<"%)"<< endl;
  cout << "gen bs not from the same H   : " << nMothersMatch <<" ("<< 100.*nMothersMatch/totEvts <<"%)"<< endl;
  cout << "No 2 gen VBF parts           : " << nGenVBF <<" ("<< 100.*nGenVBF/totEvts <<"%)"<< endl;
  cout << "---" << endl;
  cout << "b and bH not matching        : " << totEvts-nbandbH <<" ("<< 100.*(totEvts-nbandbH)/totEvts <<"%)"<< endl;
  cout << "bH and VBF are the same      : " << nbandVBF <<" ("<< 100.*nbandVBF/totEvts <<"%)"<< endl;
  cout << "genVBF and genB matched      : " << genBVBFmatch <<" ("<< 100.*genBVBFmatch/totEvts <<"%)"<< endl;
  cout << "---" << endl;
  cout << "At least 4 reco jets         : " << atLeast4Jets <<" ("<< 100.*atLeast4Jets/totEvts <<"%)"<< endl;
  cout << "---" << endl;
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
  cout << "remaining dEta matched to VBF: " << VBFremaindEtamatch <<" ("<< 100.*VBFremaindEtamatch/atLeast4Jets <<"%)"<<endl;
  cout << "---" << endl;
  cout << "Total good events            : " << goodEvts <<" ("<< 100.*goodEvts/totEvts <<"%)"<< endl;
}



