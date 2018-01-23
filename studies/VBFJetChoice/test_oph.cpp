#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include <tuple>
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TBranch.h"
#include "TString.h"
#include "TLorentzVector.h"

#include "bigTree.h" 
#include "OfflineProducerHelper.h"

using namespace std;

// Define jet_tupe type
typedef tuple<int, TLorentzVector, float, float> jetTuple;

enum sortingStrategy {
kLLRFramDefault = 0, // taking order from LLR framework <--> ordered by MVA ID
kHTauTau = 1,        // using HTauTau of lowest iso on 1st candidate, including (A,B) and (B,A)
kPtAndRawIso = 2     // order each pair leg by pt (ptA > ptB), then compare *raw* iso of first leg
};

// c++ -lm -o test_oph test_oph.cpp ../../src/OfflineProducerHelper.cc `root-config --glibs --cflags` -I ../../interface/

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

pair<int,int> getMJJpair(vector<jetTuple> reco_jets)
{
    pair<int,int> mjj_pair;
    float mass = -9999.;
    for (int i=0; i<reco_jets.size(); i++)
    {
        for (int j=0; j<reco_jets.size(); j++)
        {
            if ( (get<1>(reco_jets.at(i))+get<1>(reco_jets.at(j))).M()>mass )
            {
              mjj_pair = make_pair(get<0>(reco_jets.at(i)), get<0>(reco_jets.at(j)));
              mass = (get<1>(reco_jets.at(i))+get<1>(reco_jets.at(j))).M();
            }
        }
    }
    return mjj_pair;
}

pair<int,int> getETApair(vector<jetTuple> reco_jets)
{
    pair<int,int> eta_pair;
    float dEta = -999.;
    for (int i=0; i<reco_jets.size(); i++)
    {
        for (int j=0; j<reco_jets.size(); j++)
        {
            if ( TMath::Abs(get<1>(reco_jets.at(i)).Eta()-get<1>(reco_jets.at(j)).Eta()) > dEta)
            {
              eta_pair = make_pair(get<0>(reco_jets.at(i)), get<0>(reco_jets.at(j)));
              dEta = TMath::Abs(get<1>(reco_jets.at(i)).Eta()-get<1>(reco_jets.at(j)).Eta());
            }
        }
    }
    return eta_pair;

}


/// FUNCITONS FOR TAU PAIR CHOICE
int GetEventType (bigTree* theBigTree, OfflineProducerHelper* oph)
{
    ULong64_t debugEvent = -1;

    if (theBigTree->indexDau1->size () == 0) return -1;  //continue;
    
    int nmu = 0;
    int nmu10 = 0; // low pt muons for DY sideband, not entering in nmu
    int nele = 0;
    int nele10 = 0;

    for (unsigned int idau = 0; idau < theBigTree->daughters_px->size(); ++idau)
    {
        int dauType = theBigTree->particleType->at(idau);
        if (oph->isMuon(dauType))
        {
            bool passMu = oph->muBaseline (theBigTree, idau, 23., 2.1, 0.15, OfflineProducerHelper::MuTight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            bool passMu10 = oph->muBaseline(theBigTree, idau, 10., 2.4, 0.15, OfflineProducerHelper::MuTight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            if (passMu) ++nmu;
            else if (passMu10) ++nmu10;
        }
        else if (oph->isElectron(dauType))
        {
            bool passEle   = oph->eleBaseline (theBigTree, idau, 27., 2.1, 0.1, OfflineProducerHelper::EMVATight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            bool passEle10 = oph->eleBaseline (theBigTree, idau, 10., 2.5, 0.1, OfflineProducerHelper::EMVATight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            if (passEle) ++nele;
            else if (passEle10) ++nele10;
        }
    }
 
    int pairType = 2; // tau tau
    if (nmu > 0)
    {
      if (nmu == 1 && nmu10 == 0)
        pairType = 0 ; // mu tau
      else
        pairType = 3 ; // mu mu
    }
    else if (nele > 0)
    {
      if (nele == 1 && nele10 == 0)
        pairType = 1;  // ele tau
      else
        pairType = 4 ; // ele ele
    }

    return pairType;
}

pair<int,int> GetLeptonsPair (bigTree* theBigTree, OfflineProducerHelper* oph)
{
    ULong64_t debugEvent = -1;
    string leptonSelectionFlag = "Vertex-LepID-pTMin-etaMax-againstEle-againstMu";
    int sortStrategyThTh = 1;

    if (theBigTree->indexDau1->size () == 0) 
    {return make_pair(-1,-1);}  //continue;
    int nmu = 0;
    int nmu10 = 0; // low pt muons for DY sideband, not entering in nmu
    int nele = 0;
    int nele10 = 0;

    for (unsigned int idau = 0; idau < theBigTree->daughters_px->size(); ++idau)
    {
        int dauType = theBigTree->particleType->at(idau);
        if (oph->isMuon(dauType))
        {
            bool passMu = oph->muBaseline (theBigTree, idau, 23., 2.1, 0.15, OfflineProducerHelper::MuTight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            bool passMu10 = oph->muBaseline(theBigTree, idau, 10., 2.4, 0.15, OfflineProducerHelper::MuTight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            if (passMu) ++nmu;
            else if (passMu10) ++nmu10;
        }
        else if (oph->isElectron(dauType))
        {
            bool passEle   = oph->eleBaseline (theBigTree, idau, 27., 2.1, 0.1, OfflineProducerHelper::EMVATight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            bool passEle10 = oph->eleBaseline (theBigTree, idau, 10., 2.5, 0.1, OfflineProducerHelper::EMVATight, string("All") , (theBigTree->EventNumber == debugEvent ? true : false)) ;
            if (passEle) ++nele;
            else if (passEle10) ++nele10;
        }
    }
 
    int pairType = 2; // tau tau
    if (nmu > 0)
    {
      if (nmu == 1 && nmu10 == 0)
        pairType = 0 ; // mu tau
      else
        pairType = 3 ; // mu mu
    }
    else if (nele > 0)
    {
      if (nele == 1 && nele10 == 0)
        pairType = 1;
      else
        pairType = 4 ; // ele ele
    }
    // ----------------------------------------------------------
    // choose the first pair passing baseline and being of the right pair type

    int chosenTauPair = -1;

    if (pairType == 2 && sortStrategyThTh == kHTauTau)
    {
      chosenTauPair = oph->getBestPairHTauTau(theBigTree, leptonSelectionFlag, (theBigTree->EventNumber == debugEvent ? true : false));
    }

    else if (pairType == 2 && sortStrategyThTh == kPtAndRawIso)
    {
      chosenTauPair = oph->getBestPairPtAndRawIsoOrd(theBigTree, leptonSelectionFlag, (theBigTree->EventNumber == debugEvent ? true : false));
    }

    // (mu tauh), (e tauh), (tauhtauh && kLLRFramDefault)
    else
    {
      for (unsigned int iPair = 0 ; iPair < theBigTree->indexDau1->size () ; ++iPair)
      {
          int t_firstDaughterIndex  = theBigTree->indexDau1->at (iPair) ;
          int t_secondDaughterIndex = theBigTree->indexDau2->at (iPair) ;
          int t_type1 = theBigTree->particleType->at (t_firstDaughterIndex) ;
          int t_type2 = theBigTree->particleType->at (t_secondDaughterIndex) ;
          if ( oph->getPairType (t_type1, t_type2) != pairType ) continue ;
          string baselineSels = ( (pairType <= 2) ? leptonSelectionFlag : (leptonSelectionFlag + "-Iso")) ; // for ee, mumu, emu, ask isolation in baseline
          if ( oph->pairPassBaseline (theBigTree, iPair, baselineSels, (theBigTree->EventNumber == debugEvent ? true : false) ) ) // rlx izo to limit to tau iso < 7 -- good for sideband
          {
              chosenTauPair = iPair;
              break;          
          }
      }
    }

    if (chosenTauPair < 0) { return make_pair(-1,-1);} //if (chosenTauPair < 0) continue; // no pair found over baseline

    const int firstDaughterIndex  = theBigTree->indexDau1->at (chosenTauPair) ;
    const int secondDaughterIndex = theBigTree->indexDau2->at (chosenTauPair) ;
    
    return make_pair(firstDaughterIndex, secondDaughterIndex);
}


// open the first file in the input list, retrieve the histogram "Counters" for the trigger names and return a copy of it
TH1F* getFirstFileHisto (TString filename, bool isForTriggers=true)
{
    std::ifstream infile(filename.Data());
    std::string line;
    while (std::getline(infile, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) // skip empty lines
          break;
    }
    
    TFile* fIn = TFile::Open (line.c_str());
    TH1F* dummy = (TH1F*) fIn->Get ("HTauTauTree/Counters");
    TString name = "Counters_perTrigger";
    if(!isForTriggers) {
      dummy = (TH1F*) fIn->Get ("HTauTauTree/TauIDs");
      name = "Counters_pertauID";
    }
    TH1F* histo = new TH1F (*dummy);
    histo-> SetDirectory(0);
    histo->SetName (name.Data());
    fIn->Close();
    return histo;
}

// ---------- ---- ------------
// ---------- MAIN ------------
// ---------- ---- ------------

int main (int argc, char** argv)
{
  // DEBUG
  bool DEBUG = 0;
  
  // matching cones
  float genMatchCone = 0.1;

  // input and output setup
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/VBF_study/1_VBFHHTo2B2Tau_CV_1_C2V_1_C3_1_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt" ;
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/VBF_study/2_VBFHHTo2B2Tau_CV_1_C2V_2_C3_1_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/81_GluGluToRadionToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/TT_powheg_fullyHad.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/TT_powheg_semiLep.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/WJets_incl.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/DY_mergedList.txt";
  //TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/allNonResClonesBench/filelist_bench_newSM.txt";

  cout << ".. start" << endl;
  
  TH1F* hTriggers = getFirstFileHisto (inputFile);
  TH1F* hTauIDS = getFirstFileHisto (inputFile,false);
  
  OfflineProducerHelper oph (hTriggers, hTauIDS);

  TChain * bigChain = new TChain ("HTauTauTree/HTauTauTree") ;
  cout << ".. going to append to file" << endl;
  appendFromFileList (bigChain, inputFile);
  cout << ".. going to make the bigTree" << endl;
  bigTree theBigTree (bigChain) ;
  
  theBigTree.fChain->SetBranchStatus("*", 0);
  theBigTree.fChain->SetBranchStatus("genpart_*", 1);
  theBigTree.fChain->SetBranchStatus("genjet_*", 1);
  theBigTree.fChain->SetBranchStatus("jets_*", 1);
  theBigTree.fChain->SetBranchStatus("bCSVscore", 1);
  theBigTree.fChain->SetBranchStatus("PFjetID", 1);
  theBigTree.fChain->SetBranchStatus("index*", 1);
  theBigTree.fChain->SetBranchStatus("daughters*", 1);
  theBigTree.fChain->SetBranchStatus("combreliso", 1);
  theBigTree.fChain->SetBranchStatus("particleType", 1);
  theBigTree.fChain->SetBranchStatus("isOSCand", 1);
  theBigTree.fChain->SetBranchStatus("dz", 1);
  theBigTree.fChain->SetBranchStatus("dxy", 1);
  theBigTree.fChain->SetBranchStatus("discriminator", 1);
  theBigTree.fChain->SetBranchStatus("tauID", 1);

  
  cout << ".. going in the loop" << endl;

  int nGoodSelected = 0;
  int nGoodSelectedGenCuts = 0;
  
  int nAllJetsMatched = 0;
  int nAllJetsMatchedGenCuts = 0;
  
  int nTot = 0;
  int nTotGenCuts = 0;

  TH1F* nJetsB = new TH1F("nJetsB", "nJetsB", 20, 0, 20);
  TH1F* nJetsVBF = new TH1F("nJetsVBF", "nJetsVBF", 20, 0, 20);
  TH1F* nJetsB0p0 = new TH1F("nJetsB0p0", "nJetsB PU_id > 0", 20, 0, 20);
  TH1F* nJetsVBF0p0 = new TH1F("nJetsVBF0p0", "nJetsVBF PU_id > 0", 20, 0, 20);
  TH1F* nJetsB0p5 = new TH1F("nJetsB0p5", "nJetsB PU_id > 0.5", 20, 0, 20);
  TH1F* nJetsVBF0p5 = new TH1F("nJetsVBF0p5", "nJetsVBF PU_id > 0.5", 20, 0, 20);
  TH1F* nJetsB0p8 = new TH1F("nJetsB0p8", "nJetsB PU_id > 0.8", 20, 0, 20);
  TH1F* nJetsVBF0p8 = new TH1F("nJetsVBF0p8", "nJetsVBF PU_id > 0.8", 20, 0, 20);
  TH1F* nJetsB0p9 = new TH1F("nJetsB0p9", "nJetsB PU_id > 0.9", 20, 0, 20);
  TH1F* nJetsVBF0p9 = new TH1F("nJetsVBF0p9", "nJetsVBF PU_id > 0.9", 20, 0, 20);
  
  
  TH1F* h_2b_csvB1   = new TH1F("h_2b_csvB1",   "h_2b_csvB1", 20, -0.1, 1.1);
  TH1F* h_2b_csvB2   = new TH1F("h_2b_csvB2",   "h_2b_csvB2", 20, -0.1, 1.1);
  TH1F* h_2b_csvVBF1 = new TH1F("h_2b_csvVBF1", "h_2b_csvVBF1", 20, -0.1, 1.1);
  TH1F* h_2b_csvVBF2 = new TH1F("h_2b_csvVBF2", "h_2b_csvVBF2", 20, -0.1, 1.1);

  TH1F* h_2pt_csvB1   = new TH1F("h_2pt_csvB1",   "h_2pt_csvB1", 20, -0.1, 1.1);
  TH1F* h_2pt_csvB2   = new TH1F("h_2pt_csvB2",   "h_2pt_csvB2", 20, -0.1, 1.1);
  TH1F* h_2pt_csvVBF1 = new TH1F("h_2pt_csvVBF1", "h_2pt_csvVBF1", 20, -0.1, 1.1);
  TH1F* h_2pt_csvVBF2 = new TH1F("h_2pt_csvVBF2", "h_2pt_csvVBF2", 20, -0.1, 1.1);

  TH1F* h_2mjj_csvB1   = new TH1F("h_2mjj_csvB1",   "h_2mjj_csvB1", 20, -0.1, 1.1);
  TH1F* h_2mjj_csvB2   = new TH1F("h_2mjj_csvB2",   "h_2mjj_csvB2", 20, -0.1, 1.1);
  TH1F* h_2mjj_csvVBF1 = new TH1F("h_2mjj_csvVBF1", "h_2mjj_csvVBF1", 20, -0.1, 1.1);
  TH1F* h_2mjj_csvVBF2 = new TH1F("h_2mjj_csvVBF2", "h_2mjj_csvVBF2", 20, -0.1, 1.1);

  TH1F* h_2eta_csvB1   = new TH1F("h_2eta_csvB1",   "h_2eta_csvB1", 20, -0.1, 1.1);
  TH1F* h_2eta_csvB2   = new TH1F("h_2eta_csvB2",   "h_2eta_csvB2", 20, -0.1, 1.1);
  TH1F* h_2eta_csvVBF1 = new TH1F("h_2eta_csvVBF1", "h_2eta_csvVBF1", 20, -0.1, 1.1);
  TH1F* h_2eta_csvVBF2 = new TH1F("h_2eta_csvVBF2", "h_2eta_csvVBF2", 20, -0.1, 1.1);

  int totEvts = 0;

  int nBGen         = 0;
  int nBGenH        = 0;
  int nGenVBF       = 0;
  int nMothersMatch = 0;
  
  int nbandbH      = 0;
  int nbandVBF     = 0;
  int genBVBFmatch = 0;
  
  int genAcceptance = 0;
  
  int twoTaus = 0;
  int ntt = 0;
  int net = 0;
  int nmt = 0;
  int nee = 0;
  int nmm = 0;
  int bho = 0;
  int twoGoodTaus = 0;

  int lessThan2bJets   = 0;
  int moreThan2bJets   = 0;
  int exactly4recoJets = 0;
  int less4recoJets    = 0;
  int more4recoJets    = 0;
  int goodJets         = 0;
  
  int reco4jetsCSV = 0;
  int reco4jetsPT  = 0;
  int reco4jetsMJJ = 0;
  int reco4jetsETA = 0;

  int reco5jetsCSV = 0;
  int reco5jetsPT  = 0;
  int reco5jetsMJJ = 0;
  int reco5jetsETA = 0;

  int reco6jetsBB  = 0;
  int reco6jetsCSV = 0;
  int reco6jetsPT  = 0;
  int reco6jetsMJJ = 0;
  int reco6jetsETA = 0;
  
  int reco6jetsMB = 0;
  int reco6jetsPB = 0;
  int reco6jetsEB = 0;

  
  int dr_csv_b_1   = 0;
  int dr_csv_vbf_1 = 0;
  int dr_pt_b_1    = 0;
  int dr_pt_vbf_1  = 0;
  int dr_mjj_b_1   = 0;
  int dr_mjj_vbf_1 = 0;
  int dr_eta_b_1   = 0;
  int dr_eta_vbf_1 = 0;

  int dr_csv_b_2   = 0;
  int dr_csv_vbf_2 = 0;
  int dr_pt_b_2    = 0;
  int dr_pt_vbf_2  = 0;
  int dr_mjj_b_2   = 0;
  int dr_mjj_vbf_2 = 0;
  int dr_eta_b_2   = 0;
  int dr_eta_vbf_2 = 0;
  
  int dr_csv_b_3   = 0;
  int dr_csv_vbf_3 = 0;
  int dr_pt_b_3    = 0;
  int dr_pt_vbf_3  = 0;
  int dr_mjj_b_3   = 0;
  int dr_mjj_vbf_3 = 0;
  int dr_eta_b_3   = 0;
  int dr_eta_vbf_3 = 0;

  int nB1 = 0;
  int nB2 = 0;
  int nVBF1 = 0;
  int nVBF2 = 0;
  int n4jets = 0;

  int ngenB1    = 0;
  int ngenB2    = 0;
  int ngenVBF1  = 0;
  int ngenVBF2  = 0;
  int n4genjets = 0;
  
  int firstB1 =0;
  int firstB2 =0;
  int firstVBF1 =0;
  int firstVBF2 =0;
  int secondB1 =0;
  int secondB2 =0;
  int secondVBF1 =0;
  int secondVBF2 =0;

  int goodEvts = 0;


  for (uint iEv = 0; true; ++iEv)
  {
    int got = theBigTree.fChain->GetEntry(iEv);
    if (got == 0) break;
    if (iEv > 50000) break;

    if (iEv % 10000 == 0) cout << "------------------ Event: " << iEv << " -------------------------" << endl;

    totEvts +=1;

    // Gen Jets part
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

    // Counters to see how many times I don0t have exactly 2 gen b and 2 gen VBF
    if (vGenQuarks.size()  != 2) {nBGen  +=1; continue;}
    if (vGenQuarksH.size() != 2) {nBGenH +=1; continue;}


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
    
    // Match VBF jets to genB
    if ( (vgenVBF1.DeltaR(vgenb1H)<0.5 && vgenVBF2.DeltaR(vgenb2H)<0.5) || (vgenVBF1.DeltaR(vgenb2H)<0.5 && vgenVBF2.DeltaR(vgenb1H)<0.5) ) genBVBFmatch +=1;;
 
 
    // GenParticles in the detector acceptance
    if ( vgenVBF1.Pt()  < 20. ||  vgenVBF2.Pt()  < 20. || vgenb1H.Pt()  < 20. || vgenb2H.Pt()  < 20. ) continue; // genB   must have pt>20 and eta<2.4
    if ( vgenVBF1.Eta() > 5.  ||  vgenVBF2.Eta() > 5.  || vgenb1H.Eta() > 2.4 || vgenb2H.Eta() > 2.4 ) continue; // genVBF must have pt>20 and eta<5.0

    genAcceptance += 1;


    // Build the genJets
    bool genB1  =false;
    bool genB2  =false;
    bool genVBF1=false;
    bool genVBF2=false;
    for (uint igenjet = 0; igenjet<theBigTree.genjet_px->size(); igenjet++ )
    {
        TLorentzVector tlv_genjet( theBigTree.genjet_px->at (igenjet),theBigTree.genjet_py->at (igenjet),theBigTree.genjet_pz->at (igenjet),theBigTree.genjet_e->at (igenjet));
        if ( tlv_genjet.DeltaR(vgenb1H) < 0.5 )  genB1 = true;
        if ( tlv_genjet.DeltaR(vgenb2H) < 0.5 )  genB2 = true;
        if ( tlv_genjet.DeltaR(vgenVBF1) < 0.5 ) genVBF1 = true;
        if ( tlv_genjet.DeltaR(vgenVBF2) < 0.5 ) genVBF2 = true;
    }
    if (genB1)    ngenB1   +=1;
    if (genB2)    ngenB2   +=1;
    if (genVBF1)  ngenVBF1 +=1;
    if (genVBF2)  ngenVBF2 +=1;
    if (genB1 && genB2 && genVBF1 && genVBF2) n4genjets +=1;
    
     
    // Get the two TAU LEPTONS
    bool TausExist = false;
    pair<int,int> daus =  GetLeptonsPair (&theBigTree, &oph);
    int evtType = GetEventType(&theBigTree, &oph);
    if (daus.first == -1) continue;
    
    TausExist = true;
    twoTaus +=1;
    int firstDaughterIndex  = daus.first;
    int secondDaughterIndex = daus.second;
    TLorentzVector tlv_firstLepton (theBigTree.daughters_px->at (firstDaughterIndex),theBigTree.daughters_py->at (firstDaughterIndex),theBigTree.daughters_pz->at (firstDaughterIndex),theBigTree.daughters_e->at (firstDaughterIndex));
    TLorentzVector tlv_secondLepton (theBigTree.daughters_px->at (secondDaughterIndex),theBigTree.daughters_py->at (secondDaughterIndex),theBigTree.daughters_pz->at(secondDaughterIndex),theBigTree.daughters_e->at (secondDaughterIndex));
    
    if      (evtType == 0) nmt +=1; //mutau
    else if (evtType == 1) net +=1; //etau
    else if (evtType == 2) ntt +=1; //tautau
    else if (evtType == 3) nmm +=1; //mumu
    else if (evtType == 4) nee +=1; //ee
    else                   bho +=1; //undefined channel, should never happen

    // Keep only mutau, etau, tautau evts
    if( evtType != 0 && evtType != 1 && evtType != 2) continue;
    twoGoodTaus +=1;
    
    
    // Check if the selected tau matches with the gen Bs or VBFs
    if (vgenb1H.DeltaR  (tlv_firstLepton) < genMatchCone)  firstB1     +=1;
    if (vgenb2H.DeltaR  (tlv_firstLepton) < genMatchCone)  firstB2     +=1;
    if (vgenVBF1.DeltaR (tlv_firstLepton) < genMatchCone)  firstVBF1   +=1;
    if (vgenVBF2.DeltaR (tlv_firstLepton) < genMatchCone)  firstVBF2   +=1;

    if (vgenb1H.DeltaR  (tlv_secondLepton) < genMatchCone)  secondB1   +=1;
    if (vgenb2H.DeltaR  (tlv_secondLepton) < genMatchCone)  secondB2   +=1;
    if (vgenVBF1.DeltaR (tlv_secondLepton) < genMatchCone)  secondVBF1 +=1;
    if (vgenVBF2.DeltaR (tlv_secondLepton) < genMatchCone)  secondVBF2 +=1;
    
    // Jets tuple struture: (ijet, tlv, CSV, pt)
    vector<jetTuple> reco_jets;
    vector<jetTuple> reco_Bjets;

    // nJets counters
    int njB      = 0;
    int njVBF    = 0;
    int njB0p0   = 0;
    int njVBF0p0 = 0;
    int njB0p5   = 0;
    int njVBF0p5 = 0;
    int njB0p8   = 0;
    int njVBF0p8 = 0;
    int njB0p9   = 0;
    int njVBF0p9 = 0;
    
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

        // Minimum cuts: PFjetID and pT
        if (theBigTree.PFjetID->at (ijet) < 1) continue;    // 0 ; don't pass PF Jet ID; 1: loose, 2: tight, 3: tightLepVeto
        if (tlv_jet.Pt () < 20.) continue ;                 // jet_pt > 20 GeV
        
        // Cleaning from leptons
        if (tlv_jet.DeltaR (tlv_firstLepton) < 0.5)  continue;
        if (tlv_jet.DeltaR (tlv_secondLepton) < 0.5) continue;
        
        // CMS eta acceptance is 5. (for VBF jets) - 4.7 from AN2017-027 HTT
        if (TMath::Abs(tlv_jet.Eta()) > 5.) continue;
        reco_jets.push_back(make_tuple(ijet, tlv_jet, theBigTree.bCSVscore->at(ijet), tlv_jet.Pt()));
        
                                                      njVBF    +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.0) njVBF0p0 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.5) njVBF0p5 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.8) njVBF0p8 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.9) njVBF0p9 +=1;
        
        // 2.4 for a reasonable b-tag (2.4 is the tracker acceptance)
        if (TMath::Abs(tlv_jet.Eta()) > 2.4) continue;
        reco_Bjets.push_back(make_tuple(ijet, tlv_jet, theBigTree.bCSVscore->at(ijet), tlv_jet.Pt()));
        
                                                      njB    +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.0) njB0p0 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.5) njB0p5 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.8) njB0p8 +=1;
        if (theBigTree.jets_PUJetID->at(ijet) >= 0.9) njB0p9 +=1;
    }
    
    // histos with nJets
    nJetsB     ->Fill(njB);
    nJetsVBF   ->Fill(njVBF);
    nJetsB0p0  ->Fill(njB0p0);
    nJetsVBF0p0->Fill(njVBF0p0);
    nJetsB0p5  ->Fill(njB0p5);
    nJetsVBF0p5->Fill(njVBF0p5);
    nJetsB0p8  ->Fill(njB0p8);
    nJetsVBF0p8->Fill(njVBF0p8);
    nJetsB0p9  ->Fill(njB0p9);
    nJetsVBF0p9->Fill(njVBF0p9);
    
    if (reco_Bjets.size()<2 || reco_jets.size()<4) continue;
    goodJets += 1;

    if (reco_jets.size() == 4) exactly4recoJets +=1;
    if (reco_jets.size() >  4) more4recoJets    +=1;
    

    bool B1=false;
    bool B2=false;
    bool VBF1=false;
    bool VBF2=false;
    for (int n=0; n<reco_jets.size(); n++)
    {
        if( get<1>(reco_jets.at(n)).DeltaR(vgenb1H) <0.5) B1 = true;
        if( get<1>(reco_jets.at(n)).DeltaR(vgenb2H) <0.5) B2 = true;
        if( get<1>(reco_jets.at(n)).DeltaR(vgenVBF1)<0.5) VBF1 = true;
        if( get<1>(reco_jets.at(n)).DeltaR(vgenVBF2)<0.5) VBF2 = true;
    }
    
    if (B1)   nB1+=1;
    if (B2)   nB2+=1;
    if (VBF1) nVBF1+=1;
    if (VBF2) nVBF2+=1;
    if (B1 && B2 && VBF1 && VBF2) n4jets +=1;
    
    
    // ---------------------------------------------------------
    // ---------------- 4 jets in the event --------------------
    // ---------------------------------------------------------
    if (reco_jets.size()== 4)
    {
        // -- Getting the first two: Bs of VBFs --
        // First 2 by CSV - these are the Bs
        std::sort(reco_Bjets.begin(), reco_Bjets.end(), [](const jetTuple& a, jetTuple& b) -> bool { return get<2>(a) > get<2>(b);});
        int ijet1_b = get<0>(reco_Bjets.at(0));
        int ijet2_b = get<0>(reco_Bjets.at(1));
        
        // First 2 by PT - these are the VBFs
        std::sort(reco_jets.begin() , reco_jets.end() , [](const jetTuple& a, jetTuple& b) -> bool { return get<3>(a) > get<3>(b);});
        int ijet1_p = get<0>(reco_jets.at(0));
        int ijet2_p = get<0>(reco_jets.at(1));
        
        // First 2 by MJJ - these are the VBFs
        pair<int,int> mjj_pair = getMJJpair(reco_jets);
        int ijet1_m = mjj_pair.first;
        int ijet2_m = mjj_pair.second;
        
        // First 2 by ETA - these are the VBFs
        pair<int,int> eta_pair = getETApair(reco_jets);
        int ijet1_e = eta_pair.first;
        int ijet2_e = eta_pair.second;
        
        TLorentzVector jet1_b(theBigTree.jets_px->at(ijet1_b),theBigTree.jets_py->at(ijet1_b),theBigTree.jets_pz->at(ijet1_b),theBigTree.jets_e->at(ijet1_b));
        TLorentzVector jet2_b(theBigTree.jets_px->at(ijet2_b),theBigTree.jets_py->at(ijet2_b),theBigTree.jets_pz->at(ijet2_b),theBigTree.jets_e->at(ijet2_b));
        TLorentzVector jet1_p(theBigTree.jets_px->at(ijet1_p),theBigTree.jets_py->at(ijet1_p),theBigTree.jets_pz->at(ijet1_p),theBigTree.jets_e->at(ijet1_p));
        TLorentzVector jet2_p(theBigTree.jets_px->at(ijet2_p),theBigTree.jets_py->at(ijet2_p),theBigTree.jets_pz->at(ijet2_p),theBigTree.jets_e->at(ijet2_p));
        TLorentzVector jet1_m(theBigTree.jets_px->at(ijet1_m),theBigTree.jets_py->at(ijet1_m),theBigTree.jets_pz->at(ijet1_m),theBigTree.jets_e->at(ijet1_m));
        TLorentzVector jet2_m(theBigTree.jets_px->at(ijet2_m),theBigTree.jets_py->at(ijet2_m),theBigTree.jets_pz->at(ijet2_m),theBigTree.jets_e->at(ijet2_m));
        TLorentzVector jet1_e(theBigTree.jets_px->at(ijet1_e),theBigTree.jets_py->at(ijet1_e),theBigTree.jets_pz->at(ijet1_e),theBigTree.jets_e->at(ijet1_e));
        TLorentzVector jet2_e(theBigTree.jets_px->at(ijet2_e),theBigTree.jets_py->at(ijet2_e),theBigTree.jets_pz->at(ijet2_e),theBigTree.jets_e->at(ijet2_e));

        // -- Getting the second two: what is remaining --
        vector<jetTuple> csv_copy = reco_jets;
        for (int n=csv_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(csv_copy.at(n)) == ijet1_b || get<0>(csv_copy.at(n)) == ijet2_b ) csv_copy.erase(csv_copy.begin() +n);
        }
        int ijet3_b = get<0>(csv_copy.at(0));
        int ijet4_b = get<0>(csv_copy.at(1));
        TLorentzVector jet3_b(theBigTree.jets_px->at(ijet3_b),theBigTree.jets_py->at(ijet3_b),theBigTree.jets_pz->at(ijet3_b),theBigTree.jets_e->at(ijet3_b));
        TLorentzVector jet4_b(theBigTree.jets_px->at(ijet4_b),theBigTree.jets_py->at(ijet4_b),theBigTree.jets_pz->at(ijet4_b),theBigTree.jets_e->at(ijet4_b));
        bool dr_csv_b   = (jet1_b.DeltaR(vgenb1H) <0.5 && jet2_b.DeltaR(vgenb2H) <0.5) || (jet2_b.DeltaR(vgenb1H) <0.5 && jet1_b.DeltaR(vgenb2H) <0.5);
        bool dr_csv_vbf = (jet3_b.DeltaR(vgenVBF1)<0.5 && jet4_b.DeltaR(vgenVBF2)<0.5) || (jet4_b.DeltaR(vgenVBF1)<0.5 && jet3_b.DeltaR(vgenVBF2)<0.5);
        bool dr_csv     = dr_csv_b && dr_csv_vbf;
        if (dr_csv_b)   dr_csv_b_1   +=1;
        if (dr_csv_vbf) dr_csv_vbf_1 +=1;
        if (dr_csv)
        {
            reco4jetsCSV +=1;
            h_2b_csvB1->Fill(theBigTree.bCSVscore->at(ijet1_b));
            h_2b_csvB2->Fill(theBigTree.bCSVscore->at(ijet2_b));
            h_2b_csvVBF1->Fill(theBigTree.bCSVscore->at(ijet3_b));
            h_2b_csvVBF2->Fill(theBigTree.bCSVscore->at(ijet4_b));
        }
        
        // Clean from the already selected jets
        vector<jetTuple> pt_copy = reco_Bjets;
        for (int n=pt_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(pt_copy.at(n)) == ijet1_p || get<0>(pt_copy.at(n)) == ijet2_p ) pt_copy.erase(pt_copy.begin() +n);
        }
        
        vector<jetTuple> mjj_copy = reco_Bjets;
        for (int n=mjj_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(mjj_copy.at(n)) == ijet1_m || get<0>(mjj_copy.at(n)) == ijet2_m ) mjj_copy.erase(mjj_copy.begin() +n);
        }

        vector<jetTuple> eta_copy = reco_Bjets;
        for (int n=eta_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(eta_copy.at(n)) == ijet1_e || get<0>(eta_copy.at(n)) == ijet2_e ) eta_copy.erase(eta_copy.begin() +n);
        }

        // Get the 3rd and 4th tlv and do the matching
        if (pt_copy.size()==2)
        {
            int ijet3_p = get<0>(pt_copy.at(0));
            int ijet4_p = get<0>(pt_copy.at(1));
            TLorentzVector jet3_p(theBigTree.jets_px->at(ijet3_p),theBigTree.jets_py->at(ijet3_p),theBigTree.jets_pz->at(ijet3_p),theBigTree.jets_e->at(ijet3_p));
            TLorentzVector jet4_p(theBigTree.jets_px->at(ijet4_p),theBigTree.jets_py->at(ijet4_p),theBigTree.jets_pz->at(ijet4_p),theBigTree.jets_e->at(ijet4_p));
            bool dr_pt_b    = (jet3_p.DeltaR(vgenb1H) <0.5 && jet4_p.DeltaR(vgenb2H) <0.5) || (jet4_p.DeltaR(vgenb1H) <0.5 && jet3_p.DeltaR(vgenb2H) <0.5);
            bool dr_pt_vbf  = (jet1_p.DeltaR(vgenVBF1)<0.5 && jet2_p.DeltaR(vgenVBF2)<0.5) || (jet2_p.DeltaR(vgenVBF1)<0.5 && jet1_p.DeltaR(vgenVBF2)<0.5);
            bool dr_pt      = dr_pt_b  && dr_pt_vbf;
            if (dr_pt_b)   dr_pt_b_1   +=1;
            if (dr_pt_vbf) dr_pt_vbf_1 +=1;
            if (dr_pt )
            {
                reco4jetsPT +=1;
                h_2pt_csvB1->Fill(theBigTree.bCSVscore->at(ijet3_p));
                h_2pt_csvB2->Fill(theBigTree.bCSVscore->at(ijet4_p));
                h_2pt_csvVBF1->Fill(theBigTree.bCSVscore->at(ijet1_p));
                h_2pt_csvVBF2->Fill(theBigTree.bCSVscore->at(ijet2_p));
            }
        }
        else if (pt_copy.size()>2) cout << "More than two PT!!" <<endl;
        
        if (mjj_copy.size()==2)
        {
            int ijet3_m = get<0>(mjj_copy.at(0));
            int ijet4_m = get<0>(mjj_copy.at(1));
            TLorentzVector jet3_m(theBigTree.jets_px->at(ijet3_m),theBigTree.jets_py->at(ijet3_m),theBigTree.jets_pz->at(ijet3_m),theBigTree.jets_e->at(ijet3_m));
            TLorentzVector jet4_m(theBigTree.jets_px->at(ijet4_m),theBigTree.jets_py->at(ijet4_m),theBigTree.jets_pz->at(ijet4_m),theBigTree.jets_e->at(ijet4_m));
            bool dr_mjj_b   = (jet3_m.DeltaR(vgenb1H) <0.5 && jet4_m.DeltaR(vgenb2H) <0.5) || (jet4_m.DeltaR(vgenb1H) <0.5 && jet3_m.DeltaR(vgenb2H) <0.5);
            bool dr_mjj_vbf = (jet1_m.DeltaR(vgenVBF1)<0.5 && jet2_m.DeltaR(vgenVBF2)<0.5) || (jet2_m.DeltaR(vgenVBF1)<0.5 && jet1_m.DeltaR(vgenVBF2)<0.5);
            bool dr_mjj     = dr_mjj_b && dr_mjj_vbf;
            if (dr_mjj_b)   dr_mjj_b_1   +=1;
            if (dr_mjj_vbf) dr_mjj_vbf_1 +=1;
            if (dr_mjj)
            {
                reco4jetsMJJ +=1;
                h_2mjj_csvB1->Fill(theBigTree.bCSVscore->at(ijet3_m));
                h_2mjj_csvB2->Fill(theBigTree.bCSVscore->at(ijet4_m));
                h_2mjj_csvVBF1->Fill(theBigTree.bCSVscore->at(ijet1_m));
                h_2mjj_csvVBF2->Fill(theBigTree.bCSVscore->at(ijet2_m));
            }
        }
        else if (mjj_copy.size()>2) cout << "More than two MJJ!!" <<endl;


        if (eta_copy.size()==2)
        {
            int ijet3_e = get<0>(eta_copy.at(0));
            int ijet4_e = get<0>(eta_copy.at(1));
            TLorentzVector jet3_e(theBigTree.jets_px->at(ijet3_e),theBigTree.jets_py->at(ijet3_e),theBigTree.jets_pz->at(ijet3_e),theBigTree.jets_e->at(ijet3_e));
            TLorentzVector jet4_e(theBigTree.jets_px->at(ijet4_e),theBigTree.jets_py->at(ijet4_e),theBigTree.jets_pz->at(ijet4_e),theBigTree.jets_e->at(ijet4_e));
            bool dr_eta_b   = (jet3_e.DeltaR(vgenb1H) <0.5 && jet4_e.DeltaR(vgenb2H) <0.5) || (jet4_e.DeltaR(vgenb1H) <0.5 && jet3_e.DeltaR(vgenb2H) <0.5);
            bool dr_eta_vbf = (jet1_e.DeltaR(vgenVBF1)<0.5 && jet2_e.DeltaR(vgenVBF2)<0.5) || (jet2_e.DeltaR(vgenVBF1)<0.5 && jet1_e.DeltaR(vgenVBF2)<0.5);
            bool dr_eta     = dr_eta_b && dr_eta_vbf;
            if (dr_eta_b)   dr_eta_b_1   +=1;
            if (dr_eta_vbf) dr_eta_vbf_1 +=1;
            if (dr_eta)
            {
                reco4jetsETA +=1;
                h_2eta_csvB1->Fill(theBigTree.bCSVscore->at(ijet3_e));
                h_2eta_csvB2->Fill(theBigTree.bCSVscore->at(ijet4_e));
                h_2eta_csvVBF1->Fill(theBigTree.bCSVscore->at(ijet1_e));
                h_2eta_csvVBF2->Fill(theBigTree.bCSVscore->at(ijet2_e));
            }
        }
        else if (eta_copy.size()>2) cout << "More than two ETA!!" <<endl;
        
    }

    /*
    // ---------------------------------------------------------
    // ---------------- more than 4 reco jets ------------------
    // ---------------------------------------------------------
    else
    {
        // -- Getting the first two: Bs --
        // First 2 by CSV - these are the Bs
        std::sort(reco_Bjets.begin(), reco_Bjets.end(), [](const jetTuple& a, jetTuple& b) -> bool { return get<2>(a) > get<2>(b);});
        int ijet1_b = get<0>(reco_Bjets.at(0));
        int ijet2_b = get<0>(reco_Bjets.at(1));
        
        TLorentzVector jet1_b(theBigTree.jets_px->at(ijet1_b),theBigTree.jets_py->at(ijet1_b),theBigTree.jets_pz->at(ijet1_b),theBigTree.jets_e->at(ijet1_b));
        TLorentzVector jet2_b(theBigTree.jets_px->at(ijet2_b),theBigTree.jets_py->at(ijet2_b),theBigTree.jets_pz->at(ijet2_b),theBigTree.jets_e->at(ijet2_b));

        // -- After the two Bs (ijet1_b, ijet2_b), now getting the second two VBFs --
        // Clean from the already selected jets
        vector<jetTuple> pt_copy = reco_jets;
        for (int n=pt_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(pt_copy.at(n)) == ijet1_b || get<0>(pt_copy.at(n)) == ijet2_b ) pt_copy.erase(pt_copy.begin() +n);
        }
        
        vector<jetTuple> mjj_copy = reco_jets;
        for (int n=mjj_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(mjj_copy.at(n)) == ijet1_b || get<0>(mjj_copy.at(n)) == ijet2_b ) mjj_copy.erase(mjj_copy.begin() +n);
        }

        vector<jetTuple> eta_copy = reco_jets;
        for (int n=eta_copy.size()-1; n>=0; n--)
        {
            if ( get<0>(eta_copy.at(n)) == ijet1_b || get<0>(eta_copy.at(n)) == ijet2_b ) eta_copy.erase(eta_copy.begin() +n);
        }

        // Get the 3rd and 4th tlv and do the matching
        if (pt_copy.size()>=2)
        {
            std::sort(pt_copy.begin() , pt_copy.end() , [](const jetTuple& a, jetTuple& b) -> bool { return get<3>(a) > get<3>(b);});
            int ijet3_p = get<0>(pt_copy.at(0));
            int ijet4_p = get<0>(pt_copy.at(1));
            TLorentzVector jet3_p(theBigTree.jets_px->at(ijet3_p),theBigTree.jets_py->at(ijet3_p),theBigTree.jets_pz->at(ijet3_p),theBigTree.jets_e->at(ijet3_p));
            TLorentzVector jet4_p(theBigTree.jets_px->at(ijet4_p),theBigTree.jets_py->at(ijet4_p),theBigTree.jets_pz->at(ijet4_p),theBigTree.jets_e->at(ijet4_p));
            bool dr_pt_b    = (jet1_b.DeltaR(vgenb1H)<0.5 && jet2_b.DeltaR(vgenb2H)<0.5) || (jet2_b.DeltaR(vgenb1H)<0.5 && jet1_b.DeltaR(vgenb2H)<0.5);
            bool dr_pt_vbf  = (jet3_p.DeltaR(vgenVBF1)<0.5 && jet4_p.DeltaR(vgenVBF2)<0.5) || (jet4_p.DeltaR(vgenVBF1)<0.5 && jet3_p.DeltaR(vgenVBF2)<0.5);
            bool dr_pt      = dr_pt_b  && dr_pt_vbf;
            if (dr_pt )    reco5jetsPT +=1;
            if (dr_pt_b)   dr_pt_b_2   +=1;
            if (dr_pt_vbf) dr_pt_vbf_2 +=1;
        }
        else if (pt_copy.size()<2) cout << "SECOND: less than two PT!!" <<endl;
        
        if (mjj_copy.size()>=2)
        {
            pair<int,int> mjj_pair = getMJJpair(mjj_copy);
            int ijet3_m = mjj_pair.first;
            int ijet4_m = mjj_pair.second;
            TLorentzVector jet3_m(theBigTree.jets_px->at(ijet3_m),theBigTree.jets_py->at(ijet3_m),theBigTree.jets_pz->at(ijet3_m),theBigTree.jets_e->at(ijet3_m));
            TLorentzVector jet4_m(theBigTree.jets_px->at(ijet4_m),theBigTree.jets_py->at(ijet4_m),theBigTree.jets_pz->at(ijet4_m),theBigTree.jets_e->at(ijet4_m));
            bool dr_mjj_b   = (jet1_b.DeltaR(vgenb1H)<0.5 && jet2_b.DeltaR(vgenb2H)<0.5) || (jet2_b.DeltaR(vgenb1H)<0.5 && jet1_b.DeltaR(vgenb2H)<0.5);
            bool dr_mjj_vbf = (jet3_m.DeltaR(vgenVBF1)<0.5 && jet4_m.DeltaR(vgenVBF2)<0.5) || (jet4_m.DeltaR(vgenVBF1)<0.5 && jet3_m.DeltaR(vgenVBF2)<0.5);
            bool dr_mjj     = dr_mjj_b && dr_mjj_vbf;
            if (dr_mjj)     reco5jetsMJJ +=1;
            if (dr_mjj_b)   dr_mjj_b_2   +=1;
            if (dr_mjj_vbf) dr_mjj_vbf_2 +=1;
        }
        else if (mjj_copy.size()<2) cout << "SECOND: less than two MJJ!!" <<endl;


        if (eta_copy.size()>=2)
        {
            pair<int,int> eta_pair = getETApair(eta_copy);
            int ijet3_e = eta_pair.first;
            int ijet4_e = eta_pair.second;
            TLorentzVector jet3_e(theBigTree.jets_px->at(ijet3_e),theBigTree.jets_py->at(ijet3_e),theBigTree.jets_pz->at(ijet3_e),theBigTree.jets_e->at(ijet3_e));
            TLorentzVector jet4_e(theBigTree.jets_px->at(ijet4_e),theBigTree.jets_py->at(ijet4_e),theBigTree.jets_pz->at(ijet4_e),theBigTree.jets_e->at(ijet4_e));
            bool dr_eta_b   = (jet1_b.DeltaR(vgenb1H)<0.5 && jet2_b.DeltaR(vgenb2H)<0.5) || (jet2_b.DeltaR(vgenb1H)<0.5 && jet1_b.DeltaR(vgenb2H)<0.5);
            bool dr_eta_vbf = (jet3_e.DeltaR(vgenVBF1)<0.5 && jet4_e.DeltaR(vgenVBF2)<0.5) || (jet4_e.DeltaR(vgenVBF1)<0.5 && jet3_e.DeltaR(vgenVBF2)<0.5);
            bool dr_eta     = dr_eta_b && dr_eta_vbf;
            if (dr_eta)     reco5jetsETA +=1;
            if (dr_eta_b)   dr_eta_b_2   +=1;
            if (dr_eta_vbf) dr_eta_vbf_2 +=1;
        }
        else if (eta_copy.size()<2) cout << "SECOND: less than two ETA!!" <<endl;
    

        // -- Now get the VBF jets first --
        // First 2 by PT - these are the VBFs
        std::sort(reco_jets.begin() , reco_jets.end() , [](const jetTuple& a, jetTuple& b) -> bool { return get<3>(a) > get<3>(b);});
        int ijet1_p = get<0>(reco_jets.at(0));
        int ijet2_p = get<0>(reco_jets.at(1));
        
        // First 2 by MJJ - these are the VBFs
        pair<int,int> mjj_pair = getMJJpair(reco_jets);
        int ijet1_m = mjj_pair.first;
        int ijet2_m = mjj_pair.second;
        
        // First 2 by ETA - these are the VBFs
        pair<int,int> eta_pair = getETApair(reco_jets);
        int ijet1_e = eta_pair.first;
        int ijet2_e = eta_pair.second;

        TLorentzVector jet1_p(theBigTree.jets_px->at(ijet1_p),theBigTree.jets_py->at(ijet1_p),theBigTree.jets_pz->at(ijet1_p),theBigTree.jets_e->at(ijet1_p));
        TLorentzVector jet2_p(theBigTree.jets_px->at(ijet2_p),theBigTree.jets_py->at(ijet2_p),theBigTree.jets_pz->at(ijet2_p),theBigTree.jets_e->at(ijet2_p));
        TLorentzVector jet1_m(theBigTree.jets_px->at(ijet1_m),theBigTree.jets_py->at(ijet1_m),theBigTree.jets_pz->at(ijet1_m),theBigTree.jets_e->at(ijet1_m));
        TLorentzVector jet2_m(theBigTree.jets_px->at(ijet2_m),theBigTree.jets_py->at(ijet2_m),theBigTree.jets_pz->at(ijet2_m),theBigTree.jets_e->at(ijet2_m));
        TLorentzVector jet1_e(theBigTree.jets_px->at(ijet1_e),theBigTree.jets_py->at(ijet1_e),theBigTree.jets_pz->at(ijet1_e),theBigTree.jets_e->at(ijet1_e));
        TLorentzVector jet2_e(theBigTree.jets_px->at(ijet2_e),theBigTree.jets_py->at(ijet2_e),theBigTree.jets_pz->at(ijet2_e),theBigTree.jets_e->at(ijet2_e));

        // Clean from the already selected jets
        vector<jetTuple> csv_copy_p = reco_Bjets;
        for (int n=csv_copy_p.size()-1; n>=0; n--)
        {
            if ( get<0>(csv_copy_p.at(n)) == ijet1_p || get<0>(csv_copy_p.at(n)) == ijet2_p ) csv_copy_p.erase(csv_copy_p.begin() +n);
        }
        
        vector<jetTuple> csv_copy_m = reco_Bjets;
        for (int n=csv_copy_m.size()-1; n>=0; n--)
        {
            if ( get<0>(csv_copy_m.at(n)) == ijet1_m || get<0>(csv_copy_m.at(n)) == ijet2_m ) csv_copy_m.erase(csv_copy_m.begin() +n);
        }
        
        vector<jetTuple> csv_copy_e = reco_Bjets;
        for (int n=csv_copy_e.size()-1; n>=0; n--)
        {
            if ( get<0>(csv_copy_e.at(n)) == ijet1_e || get<0>(csv_copy_e.at(n)) == ijet2_e ) csv_copy_e.erase(csv_copy_e.begin() +n);
        }

        // Sort the copies by CSV:
        std::sort(csv_copy_p.begin(), csv_copy_p.end(), [](const jetTuple& a, jetTuple& b) -> bool { return get<2>(a) > get<2>(b);});
        std::sort(csv_copy_m.begin(), csv_copy_m.end(), [](const jetTuple& a, jetTuple& b) -> bool { return get<2>(a) > get<2>(b);});
        std::sort(csv_copy_e.begin(), csv_copy_e.end(), [](const jetTuple& a, jetTuple& b) -> bool { return get<2>(a) > get<2>(b);});

        if (csv_copy_p.size()>=2)
        {
            int ijet3_p_b = get<0>(csv_copy_p.at(0));
            int ijet4_p_b = get<0>(csv_copy_p.at(1));
            TLorentzVector jet3_p_b(theBigTree.jets_px->at(ijet3_p_b),theBigTree.jets_py->at(ijet3_p_b),theBigTree.jets_pz->at(ijet3_p_b),theBigTree.jets_e->at(ijet3_p_b));
            TLorentzVector jet4_p_b(theBigTree.jets_px->at(ijet4_p_b),theBigTree.jets_py->at(ijet4_p_b),theBigTree.jets_pz->at(ijet4_p_b),theBigTree.jets_e->at(ijet4_p_b));
            bool dr_pb_b    = (jet3_p_b.DeltaR(vgenb1H) <0.5 && jet4_p_b.DeltaR(vgenb2H) <0.5) || (jet4_p_b.DeltaR(vgenb1H) <0.5 && jet3_p_b.DeltaR(vgenb2H) <0.5);
            bool dr_pb_vbf  = (jet1_p.DeltaR(vgenVBF1)<0.5 && jet2_p.DeltaR(vgenVBF2)<0.5) || (jet2_p.DeltaR(vgenVBF1)<0.5 && jet1_p.DeltaR(vgenVBF2)<0.5);
            bool dr_pb      = dr_pb_b  && dr_pb_vbf;
            if (dr_pb )    reco6jetsPT +=1;
            if (dr_pb_b)   dr_pt_b_3   +=1;
            if (dr_pb_vbf) dr_pt_vbf_3 +=1;
        }
        //else if (csv_copy_p.size()<2) cout << "THIRD: less than two PT!!" <<endl;

        if (csv_copy_m.size()>=2)
        {
            int ijet3_m_b = get<0>(csv_copy_m.at(0));
            int ijet4_m_b = get<0>(csv_copy_m.at(1));
            TLorentzVector jet3_m_b(theBigTree.jets_px->at(ijet3_m_b),theBigTree.jets_py->at(ijet3_m_b),theBigTree.jets_pz->at(ijet3_m_b),theBigTree.jets_e->at(ijet3_m_b));
            TLorentzVector jet4_m_b(theBigTree.jets_px->at(ijet4_m_b),theBigTree.jets_py->at(ijet4_m_b),theBigTree.jets_pz->at(ijet4_m_b),theBigTree.jets_e->at(ijet4_m_b));
            bool dr_mb_b    = (jet3_m_b.DeltaR(vgenb1H) <0.5 && jet4_m_b.DeltaR(vgenb2H) <0.5) || (jet4_m_b.DeltaR(vgenb1H) <0.5 && jet3_m_b.DeltaR(vgenb2H) <0.5);
            bool dr_mb_vbf  = (jet1_p.DeltaR(vgenVBF1)<0.5 && jet2_p.DeltaR(vgenVBF2)<0.5) || (jet2_p.DeltaR(vgenVBF1)<0.5 && jet1_p.DeltaR(vgenVBF2)<0.5);
            bool dr_mb      = dr_mb_b  && dr_mb_vbf;
            if (dr_mb )    reco6jetsMJJ +=1;
            if (dr_mb_b)   dr_mjj_b_3   +=1;
            if (dr_mb_vbf) dr_mjj_vbf_3 +=1;
        }
        //else if (csv_copy_m.size()<2) cout << "THIRD: less than two MJJ!!" <<endl;

        if (csv_copy_e.size()>=2)
        {
            int ijet3_e_b = get<0>(csv_copy_e.at(0));
            int ijet4_e_b = get<0>(csv_copy_e.at(1));
            TLorentzVector jet3_e_b(theBigTree.jets_px->at(ijet3_e_b),theBigTree.jets_py->at(ijet3_e_b),theBigTree.jets_pz->at(ijet3_e_b),theBigTree.jets_e->at(ijet3_e_b));
            TLorentzVector jet4_e_b(theBigTree.jets_px->at(ijet4_e_b),theBigTree.jets_py->at(ijet4_e_b),theBigTree.jets_pz->at(ijet4_e_b),theBigTree.jets_e->at(ijet4_e_b));
            bool dr_eb_b    = (jet3_e_b.DeltaR(vgenb1H) <0.5 && jet4_e_b.DeltaR(vgenb2H) <0.5) || (jet4_e_b.DeltaR(vgenb1H) <0.5 && jet3_e_b.DeltaR(vgenb2H) <0.5);
            bool dr_eb_vbf  = (jet1_p.DeltaR(vgenVBF1)<0.5 && jet2_p.DeltaR(vgenVBF2)<0.5) || (jet2_p.DeltaR(vgenVBF1)<0.5 && jet1_p.DeltaR(vgenVBF2)<0.5);
            bool dr_eb      = dr_eb_b  && dr_eb_vbf;
            if (dr_eb )    reco6jetsETA +=1;
            if (dr_eb_b)   dr_eta_b_3   +=1;
            if (dr_eb_vbf) dr_eta_vbf_3 +=1;
        }
        //else if (csv_copy_e.size()<2) cout << "THIRD: less than two ETA!!" <<endl;

    }*/
    
    goodEvts +=1;
  }

  TFile* fOut = new TFile("fOut.root", "recreate");
  nJetsB     ->Write();
  nJetsB0p0  ->Write();
  nJetsB0p5  ->Write();
  nJetsB0p8  ->Write();
  nJetsB0p9  ->Write();
  nJetsVBF   ->Write();
  nJetsVBF0p0->Write();
  nJetsVBF0p5->Write();
  nJetsVBF0p8->Write();
  nJetsVBF0p9->Write();
  
  h_2b_csvB1  ->Write();
  h_2b_csvB2  ->Write();
  h_2b_csvVBF1->Write();
  h_2b_csvVBF2->Write();
  
  h_2pt_csvB1->Write();
  h_2pt_csvB2->Write();
  h_2pt_csvVBF1->Write();
  h_2pt_csvVBF2->Write();
  h_2mjj_csvB1->Write();
  h_2mjj_csvB2->Write();
  h_2mjj_csvVBF1->Write();
  h_2mjj_csvVBF2->Write();
  h_2eta_csvB1->Write();
  h_2eta_csvB2->Write();
  h_2eta_csvVBF1->Write();
  h_2eta_csvVBF2->Write();
  
  cout << endl;
  cout << "Total analyzed evts          : " << totEvts <<endl;
  cout << "---" << endl;
  cout << "Less/more than 2 genB, genBH, notSameH, genVBF: " << nBGen <<" ("<< 100.*nBGen/totEvts <<"%) - " << nBGenH <<" ("<< 100.*nBGenH/totEvts <<"%) - "
  << nMothersMatch <<" ("<< 100.*nMothersMatch/totEvts <<"%) - "<< nGenVBF <<" ("<< 100.*nGenVBF/totEvts <<"%)"<< endl;
  cout << "Gen in detect acceptance     : " << genAcceptance <<" ("<<100.*genAcceptance/totEvts << "%)"<<endl;
  cout << "---" <<endl;
  cout << " ngenB1                      : " << ngenB1 <<" ("<<100.*ngenB1/genAcceptance << "%)"<<endl;
  cout << " ngenB2                      : " << ngenB2 <<" ("<<100.*ngenB2/genAcceptance << "%)"<<endl;
  cout << " ngenVBF1                    : " << ngenVBF1 <<" ("<<100.*ngenVBF1/genAcceptance << "%)"<<endl;
  cout << " ngenVBF2                    : " << ngenVBF2 <<" ("<<100.*ngenVBF2/genAcceptance << "%)"<<endl;
  cout << " n4genjets                   : " << n4genjets <<" ("<<100.*n4genjets/genAcceptance << "%)"<<endl;
  cout << "---" <<endl;
  cout << "Evts with 2 good taus        : " << twoTaus <<" ("<< 100.*twoTaus/genAcceptance <<"%)"<<endl;
  cout << " mutau                       : " << nmt << " ("<< 100.*nmt/twoTaus <<"%)"<<endl;
  cout << " etau                        : " << net << " ("<< 100.*net/twoTaus <<"%)"<<endl;
  cout << " tautau                      : " << ntt << " ("<< 100.*ntt/twoTaus <<"%)"<<endl;
  cout << " mumu                        : " << nmm << " ("<< 100.*nmm/twoTaus <<"%)"<<endl;
  cout << " ee                          : " << nee << " ("<< 100.*nee/twoTaus <<"%)"<<endl;
  cout << " bho                         : " << bho << " ("<< 100.*bho/twoTaus <<"%)"<<endl;
  cout << "Good channels (mt,et,tt)     : " << twoGoodTaus << " ("<<100.*twoGoodTaus/genAcceptance <<"%)"<<endl;
  cout << "---" << endl;
  cout << " firstLept and B1            : " << firstB1 <<" ("<<100.*firstB1/twoGoodTaus << "%)"<<endl;
  cout << " firstLept and B2            : " << firstB2 <<" ("<<100.*firstB2/twoGoodTaus << "%)"<<endl;
  cout << " firstLept and VBF1          : " << firstVBF1 <<" ("<<100.*firstVBF1/twoGoodTaus << "%)"<<endl;
  cout << " firstLept and VBF2          : " << firstVBF2 <<" ("<<100.*firstVBF2/twoGoodTaus << "%)"<<endl;
  cout << " secondLept and B1           : " << secondB1 <<" ("<<100.*secondB1/twoGoodTaus << "%)"<<endl;
  cout << " secondLept and B2           : " << secondB2 <<" ("<<100.*secondB2/twoGoodTaus << "%)"<<endl;
  cout << " secondLept and VBF1         : " << secondVBF1 <<" ("<<100.*secondVBF1/twoGoodTaus << "%)"<<endl;
  cout << " secondLept and VBF2         : " << secondVBF2 <<" ("<<100.*secondVBF2/twoGoodTaus << "%)"<<endl;
  cout << "---" << endl;
  cout << "  - After Tau Cleaning -" << endl;
  cout << "Total good jets (2B+2jets)   : " << goodJets <<" ("<<100.*goodJets/twoGoodTaus << "%)"<<endl;
  cout << " = 4 reco jets               : " << exactly4recoJets <<" ("<<100.*exactly4recoJets/goodJets <<"%)"<<endl;
  cout << " > 4 reco jets               : " << more4recoJets <<" ("<<100.*more4recoJets/goodJets <<"%)"<<endl;
  cout << "---"<<endl;
  cout << " nB1                         : " << nB1 <<" ("<<100.*nB1/goodJets << "%)"<<endl;
  cout << " nB2                         : " << nB2 <<" ("<<100.*nB2/goodJets << "%)"<<endl;
  cout << " nVBF1                       : " << nVBF1 <<" ("<<100.*nVBF1/goodJets << "%)"<<endl;
  cout << " nVBF2                       : " << nVBF2 <<" ("<<100.*nVBF2/goodJets << "%)"<<endl;
  cout << " n4jets                      : " << n4jets <<" ("<<100.*n4jets/goodJets << "%)"<<endl;
  cout << "---" << endl;
  cout << "   - Only 4 reco jets -" << endl;
  cout << " chose 2b by CSV             : " << reco4jetsCSV << " ("<<100.*reco4jetsCSV/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_csv_b_1 << " ("<<100.*dr_csv_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_csv_vbf_1 << " ("<<100.*dr_csv_vbf_1/exactly4recoJets <<"%)"<<endl;
  cout << " chose 2vbf by PT            : " << reco4jetsPT << " ("<<100.*reco4jetsPT/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_pt_b_1 << " ("<<100.*dr_pt_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_pt_vbf_1 << " ("<<100.*dr_pt_vbf_1/exactly4recoJets <<"%)"<<endl;
  cout << " chose 2vbf by MJJ           : " << reco4jetsMJJ << " ("<<100.*reco4jetsMJJ/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_mjj_b_1 << " ("<<100.*dr_mjj_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_mjj_vbf_1 << " ("<<100.*dr_mjj_vbf_1/exactly4recoJets <<"%)"<<endl;
  cout << " chose 2vbf by ETA           : " << reco4jetsETA << " ("<<100.*reco4jetsETA/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_eta_b_1 << " ("<<100.*dr_eta_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_eta_vbf_1 << " ("<<100.*dr_eta_vbf_1/exactly4recoJets <<"%)"<<endl;
  cout << "---"<<endl;
/*
  cout << "   -- More than 4 reco jets --" << endl;
  cout << "Chose 2b first by CSV then 2vbf by:"<< endl;
  cout << "  PT                         : " << reco5jetsPT << " ("<<100.*reco5jetsPT/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_pt_b_2 << " ("<<100.*dr_pt_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_pt_vbf_2 << " ("<<100.*dr_pt_vbf_2/more4recoJets <<"%)"<<endl;
  cout << "  MJJ                        : " << reco5jetsMJJ << " ("<<100.*reco5jetsMJJ/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_mjj_b_2 << " ("<<100.*dr_mjj_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_mjj_vbf_2 << " ("<<100.*dr_mjj_vbf_2/more4recoJets <<"%)"<<endl;
  cout << "  ETA                        : " << reco5jetsETA << " ("<<100.*reco5jetsETA/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_eta_b_2 << " ("<<100.*dr_eta_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_eta_vbf_2 << " ("<<100.*dr_eta_vbf_2/more4recoJets <<"%)"<<endl;
  cout << "Chose 2vbf (csv,pt,mjj,eta) first then 2b (csv):"<< endl;
  cout << "  PT                         : " << reco6jetsPT << " ("<<100.*reco6jetsPT/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_pt_b_3 << " ("<<100.*dr_pt_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_pt_vbf_3 << " ("<<100.*dr_pt_vbf_3/more4recoJets <<"%)"<<endl;
  cout << "  MJJ                        : " << reco6jetsMJJ << " ("<<100.*reco6jetsMJJ/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_mjj_b_3 << " ("<<100.*dr_mjj_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_mjj_vbf_3 << " ("<<100.*dr_mjj_vbf_3/more4recoJets <<"%)"<<endl;
  cout << "  ETA                        : " << reco6jetsETA << " ("<<100.*reco6jetsETA/more4recoJets <<"%)"<<endl;
  cout << "    b-only                   :   " << dr_eta_b_3 << " ("<<100.*dr_eta_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                 :   " << dr_eta_vbf_3 << " ("<<100.*dr_eta_vbf_3/more4recoJets <<"%)"<<endl;
  cout << "---" << endl;
*/
  cout << "Total good events (>=4 jets) : " << goodJets <<" ("<< 100.*goodJets/totEvts <<"%)"<< endl;
}



