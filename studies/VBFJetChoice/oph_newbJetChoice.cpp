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
#include "OfflineProducerHelper.h"

using namespace std;

enum sortingStrategy {
kLLRFramDefault = 0, // taking order from LLR framework <--> ordered by MVA ID
kHTauTau = 1,        // using HTauTau of lowest iso on 1st candidate, including (A,B) and (B,A)
kPtAndRawIso = 2     // order each pair leg by pt (ptA > ptB), then compare *raw* iso of first leg
};

// c++ -lm -o oph_newbJetChoice oph_newbJetChoice.cpp ../../src/OfflineProducerHelper.cc `root-config --glibs --cflags` -I ../../interface/

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



/// FUNCITONS FOR TAU PAIR CHOICE
/*
bool CheckPassTau (bigTree* theBigTree, int iDau, float ptMin, float etaMax, int againstEleWP, int againstMuWP, float isoRaw3Hits)
{
    TLorentzVector p4 (theBigTree->daughters_px->at(iDau), theBigTree->daughters_py->at(iDau), theBigTree->daughters_pz->at(iDau), theBigTree->daughters_e->at(iDau));

    if (againstEleWP < 0 || againstEleWP > 4) {
        cout << " ** OfflineProducerHelper::tauBaseline: againstEleWP must be between 0 and 4 --> using 0" << endl;
        againstEleWP = 0;
    } 

    if (againstMuWP < 0 || againstMuWP > 1) {
        cout << " ** OfflineProducerHelper::tauBaseline: againstMuWP must be between 0 and 1 --> using 0" << endl;
        againstMuWP = 0;
    }
    
    bool dmfS = (theBigTree->daughters_decayModeFindingOldDMs->at(iDau) == 1);
    bool vertexS = (fabs(theBigTree->dz->at(iDau)) < 0.2);
    bool isoS = (theBigTree->daughters_byCombinedIsolationDeltaBetaCorrRaw3Hits->at(iDau) < isoRaw3Hits);
    bool ptS = (p4.Pt() > ptMin);
    bool etaS = (fabs(p4.Eta()) < etaMax);
    bool totalS = (dmfS && vertexS && isoS && ptS && etaS);

    return totalS;
}



bool CheckPassMu (bigTree* theBigTree, int iDau)
{
    TLorentzVector p4 (theBigTree->daughters_px->at(iDau), theBigTree->daughters_py->at(iDau), theBigTree->daughters_pz->at(iDau), theBigTree->daughters_e->at(iDau));
    int discr = theBigTree->daughters_muonID->at(iDau);

    bool idS = CheckBit (discr, 3); // bit 0 is LOOSE id, bit 2 is MEDIUM mu id, bit 3 is TIGHT mu id
    bool isoS = (theBigTree->combreliso->at(iDau) < 0.15);
    bool ptS = (p4.Pt() > 23.);
    bool etaS = (fabs(p4.Eta()) < 2.1);
    
    return (idS && isoS && ptS && etaS);
}

bool CheckPassMu10 (bigTree* theBigTree, int iDau)
{
    TLorentzVector p4 (theBigTree->daughters_px->at(iDau), theBigTree->daughters_py->at(iDau), theBigTree->daughters_pz->at(iDau), theBigTree->daughters_e->at(iDau));
    int discr = theBigTree->daughters_muonID->at(iDau);

    bool idS = CheckBit (discr, 3); // bit 0 is LOOSE id, bit 2 is MEDIUM mu id, bit 3 is TIGHT mu id
    bool isoS = (theBigTree->combreliso->at(iDau) < 0.15);
    bool ptS = (p4.Pt() > 10.);
    bool etaS = (fabs(p4.Eta()) < 2.4);
    
    return (idS && isoS && ptS && etaS);
}

bool CheckPassEle (bigTree* theBigTree, int iDau)
{
    TLorentzVector p4 (theBigTree->daughters_px->at(iDau), theBigTree->daughters_py->at(iDau), theBigTree->daughters_pz->at(iDau), theBigTree->daughters_e->at(iDau));

    bool isoS = (theBigTree->combreliso->at(iDau) < 0.1);
    bool ptS = (p4.Pt() > 27.);
    bool etaS = (fabs(p4.Eta()) < 2.1);
    bool idS = theBigTree->daughters_iseleBDT->at(iDau); // use it in ntuples produced after 11 June 2015, contains tight WP bool
    
    return (idS && isoS && ptS && etaS);
}

bool CheckPassEle10 (bigTree* theBigTree, int iDau)
{
    TLorentzVector p4 (theBigTree->daughters_px->at(iDau), theBigTree->daughters_py->at(iDau), theBigTree->daughters_pz->at(iDau), theBigTree->daughters_e->at(iDau));

    bool isoS = (theBigTree->combreliso->at(iDau) < 0.1);
    bool ptS = (p4.Pt() > 10.);
    bool etaS = (fabs(p4.Eta()) < 2.5);
    bool idS = theBigTree->daughters_iseleBDT->at(iDau); // use it in ntuples produced after 11 June 2015, contains tight WP bool
    
    return (idS && isoS && ptS && etaS);
}


int getPairType (int type1, int type2)
{
    int nmu = 0;
    int nele = 0;
    int ntau = 0;
    
    if (type1 == 0) nmu++;
    if (type1 == 1) nele++;
    if (type1 == 2) ntau++;

    if (type2 == 0) nmu++;
    if (type2 == 1) nele++;
    if (type2 == 2) ntau++;

    if (nmu == 1 && nele == 0 && ntau == 1) return 0; //(int) pairType::MuHad;
    if (nmu == 0 && nele == 1 && ntau == 1) return 1; //(int) pairType::EHad;
    if (nmu == 0 && nele == 0 && ntau == 2) return 2; //(int) pairType::HadHad;
    if (nmu == 2 && nele == 0 && ntau == 0) return 3; //(int) pairType::MuMu;
    if (nmu == 0 && nele == 2 && ntau == 0) return 4; //(int) pairType::EE;
    if (nmu == 1 && nele == 1 && ntau == 0) return 5; //(int) pairType::EMu;
    
    return -1;
}

float DeltaRDau(bigTree* tree, int dau1idx, int dau2idx)
{
  TLorentzVector v1, v2;
  v1.SetPxPyPzE (tree->daughters_px->at(dau1idx), tree->daughters_py->at(dau1idx), tree->daughters_pz->at(dau1idx), tree->daughters_e->at(dau1idx));
  v2.SetPxPyPzE (tree->daughters_px->at(dau2idx), tree->daughters_py->at(dau2idx), tree->daughters_pz->at(dau2idx), tree->daughters_e->at(dau2idx));
  return v1.DeltaR(v2);
}


bool pairPassBaseline (bigTree* tree, int iPair)
{
    int dau1index = tree->indexDau1->at(iPair);
    int dau2index = tree->indexDau2->at(iPair);
    int type1 = tree->particleType->at(dau1index);
    int type2 = tree->particleType->at(dau2index);
    int pairType = getPairType (type1, type2);
    
    float dR = DeltaRDau(tree, dau1index, dau2index);
    bool drMin = (dR > 0.1);

    bool isOS = tree->isOSCand->at(iPair);

    // pairs are always ordered as: e mu | e tau | mu tau  (e < mu < tau)
    // if same type of particle, highest pt one is the first
    bool leg1=false;
    bool leg2=false;
    if (pairType == 0)
    {
        //float tauIso = whatApply.Contains("TauRlxIzo") ? 7.0 : 3.0 ;
        leg1 = CheckPassMu (tree, dau1index);
        leg2 = CheckPassTau (tree, dau2index, 20., 2.3, 0, 1, 3.0);
        //leg2 = tauBaseline  (tree, dau2index, 20., 2.3, aeleVLoose, amuTight, tauIso, whatApply, debug);
    }

    if (pairType == 1)
    {
        //float tauIso = whatApply.Contains("TauRlxIzo") ? 7.0 : 3.0 ;
        leg1 = CheckPassEle (tree, dau1index);
        leg2 = CheckPassTau (tree, dau2index, 20., 2.3, 0, 1, 3.0);
        //leg2 = tauBaseline  (tree, dau2index, 20., 2.3, aeleTight, amuLoose, tauIso, whatApply, debug);
    }

    // ordered by pT and not by most isolated, but baseline asked in sync is the same...
    if (pairType == 2)
    {
        //float tauIso = whatApply.Contains("TauRlxIzo") ? 7.0 : 2.0 ;
        leg1 = CheckPassTau (tree, dau2index, 40., 2.1, 0, 1, 2.0);
        //leg1 = tauBaseline (tree, dau1index, 40., 2.1, aeleVLoose, amuLoose, tauIso, whatApply, debug);
        leg2 = CheckPassTau (tree, dau2index, 40., 2.1, 0, 1, 2.0);
        //leg2 = tauBaseline (tree, dau2index, 40., 2.1, aeleVLoose, amuLoose, tauIso, whatApply, debug);
    }

    if (pairType == 5)
    {
        leg1 = CheckPassEle  (tree, dau1index);
        leg2 = CheckPassMu10 (tree, dau2index);
    }

    // e e, mu mu are still preliminary (not from baseline)
    if (pairType == 4)
    {
      leg1 = CheckPassEle (tree, dau1index);
      leg2 = CheckPassEle (tree, dau2index);
    }

    if (pairType == 3)
    {
      leg1 = CheckPassMu10 (tree, dau1index);
      leg2 = CheckPassMu10 (tree, dau2index);
      bool leg1ER = CheckPassMu (tree, dau1index);
      bool leg2ER = CheckPassMu (tree, dau2index);
      
      return ( ((leg1ER && leg2) || (leg2ER && leg1)) && drMin );
    }

    bool result = (leg1 && leg2);
    result = (result && drMin);

    return result;
}


pair<int,int> GetLeptonsPair (bigTree* theBigTree)
{

    if (theBigTree->indexDau1->size () == 0) 
    { return make_pair(-1,-1);}  //continue;

    int nmu = 0;
    int nmu10 = 0;
    int nele = 0;
    int nele10 = 0;
    for (unsigned int idau = 0; idau < theBigTree->daughters_px->size(); ++idau)
    {
        int dauType = theBigTree->particleType->at(idau);
        if (dauType == 0)
        {
            bool passMu   = CheckPassMu   (theBigTree, idau);
            bool passMu10 = CheckPassMu10 (theBigTree, idau);
            if (passMu) ++nmu;
            else if (passMu10) ++nmu10;
        }
        else if (dauType == 1)
        {
            bool passEle   = CheckPassEle   (theBigTree, idau);
            bool passEle10 = CheckPassEle10 (theBigTree, idau);
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

    int chosenTauPair = -1;

    // (mu tauh), (e tauh), (tauhtauh && kLLRFramDefault)
    for (unsigned int iPair = 0 ; iPair < theBigTree->indexDau1->size () ; ++iPair)
    {
      int t_firstDaughterIndex  = theBigTree->indexDau1->at (iPair) ;
      int t_secondDaughterIndex = theBigTree->indexDau2->at (iPair) ;
      int t_type1 = theBigTree->particleType->at (t_firstDaughterIndex) ;
      int t_type2 = theBigTree->particleType->at (t_secondDaughterIndex) ;        
      if ( getPairType (t_type1, t_type2) != pairType ) continue ;
      if ( pairPassBaseline (theBigTree, iPair) ) // rlx izo to limit to tau iso < 7 -- good for sideband
      {
          chosenTauPair = iPair;
          break;          
      }
    }
    if (chosenTauPair < 0) { return make_pair(-1,-1);} //continue; // no pair found over baseline

    const int firstDaughterIndex  = theBigTree->indexDau1->at (chosenTauPair) ;
    const int secondDaughterIndex = theBigTree->indexDau2->at (chosenTauPair) ;
    
    return make_pair(firstDaughterIndex, secondDaughterIndex);
}*/
/// END functions for tau pair choice




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

  TH1F* hMbbAll = new TH1F("hMbbAll", "hMbbAll", 200, 0, 200);

  TH1F* hMbbMatched = new TH1F("hMbbMatched", "hMbbMatched", 200, 0, 200);
  TH1F* hMbbHMatched = new TH1F("hMbbHMatched", "hMbbHMatched", 200, 0, 200);

  TH1F* dPhiMatched = new TH1F("dPhiMatched", "dPhiMatched", 100, -2.*TMath::Pi(), 2.*TMath::Pi());
  TH1F* dPhiHMatched = new TH1F("dPhiHMatched", "dPhiHMatched", 100, -2.*TMath::Pi(), 2.*TMath::Pi());
  
  TH1F* hGenVBFMjj = new TH1F("hGenVBFMjj", "hGenVBFMjj", 100, 0, 5000);
  TH1F* hGenBMjj   = new TH1F("hGenBMjj", "hGenBMjj", 125, 50, 175);
  TH1F* hGenVBFdEta = new TH1F("hGenVBFdEta", "hGenVBFdEta", 24, 0, 12);
  TH1F* hGenBdEta   = new TH1F("hGenBdEta", "hGenBdEta", 24, 0, 12);

    

  int totEvts = 0;

  int twoTaus = 0;

  int nBGen         = 0;
  int nBGenH        = 0;
  int nGenVBF       = 0;
  int nMothersMatch = 0;
  
  int nbandbH      = 0;
  int nbandVBF     = 0;
  int genBVBFmatch = 0;

  int lessThan2bJets   = 0;
  int moreThan2bJets  = 0;
  int atLeast2cleanedJets  = 0;
  int moreThan2cleanedJets = 0;

  int exactly4recoJets = 0;
  int less4recoJets    = 0;
  int more4recoJets    = 0;
  
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
  
  int reco6jetsMB = 0;
  int reco6jetsPB = 0;
  int reco6jetsEB = 0;

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

  int dr_csv_b_1 = 0;
  int dr_csv_vbf_1 = 0;
  int dr_pt_b_1  = 0;
  int dr_pt_vbf_1  = 0;
  int dr_mjj_b_1 = 0;
  int dr_mjj_vbf_1 = 0;
  int dr_eta_b_1 = 0;
  int dr_eta_vbf_1 = 0;

  int dr_csv_b_2 = 0;
  int dr_csv_vbf_2 = 0;
  int dr_pt_b_2  = 0;
  int dr_pt_vbf_2  = 0;
  int dr_mjj_b_2 = 0;
  int dr_mjj_vbf_2 = 0;
  int dr_eta_b_2 = 0;
  int dr_eta_vbf_2 = 0;
  
  int dr_csv_b_3 = 0;
  int dr_csv_vbf_3 = 0;
  int dr_pt_b_3  = 0;
  int dr_pt_vbf_3  = 0;
  int dr_mjj_b_3 = 0;
  int dr_mjj_vbf_3 = 0;
  int dr_eta_b_3 = 0;
  int dr_eta_vbf_3 = 0;


  for (uint iEv = 0; true; ++iEv)
  {
    int got = theBigTree.fChain->GetEntry(iEv);
    if (got == 0) break;
    if (iEv > 20000) break;

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

    // Counters to see how many times I don0t have exactly 2 gen b, 2 gen bH and 2 gen VBF
    //if (vGenQuarks.size() != 2) nBGen+=1;
    //if (vGenQuarksH.size() != 2) nBGenH+=1;
    //if (vGenVBF.size() != 2) nGenVBF +=1;
    if (vGenQuarks.size()  != 2) {nBGen  +=1; continue;}
    if (vGenQuarksH.size() != 2) {nBGenH +=1; continue;}
    //if (vGenVBF.size()     != 2) {nGenVBF+=1; continue;}


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
 
    if ( vgenVBF1.Pt()<20. ||  vgenVBF2.Pt()<20. || vgenb1H.Pt()<20.  || vgenb2H.Pt()<20. ) continue;
    if ( vgenVBF1.Eta()>5. ||  vgenVBF2.Eta()>5. || vgenb1H.Eta()>2.4 || vgenb2H.Eta()>2.4 ) continue;


    hGenVBFMjj->Fill((vgenVBF1+vgenVBF2).M());
    hGenBMjj->Fill((vgenb1H+vgenb2H).M());
    hGenVBFdEta->Fill(TMath::Abs(vgenVBF1.Eta()-vgenVBF2.Eta()));
    hGenBdEta->Fill(TMath::Abs(vgenb1H.Eta()-vgenb2H.Eta()));

    
    // Get the two TAU LEPTONS
    bool TausExist = false;
    //pair<int,int> daus = GetLeptonsPair (&theBigTree);
    pair<int,int> daus =  GetLeptonsPair (&theBigTree, &oph);
    if (daus.first != -1)
    {
        TausExist = true;
        twoTaus +=1;
        int firstDaughterIndex  = daus.first;
        int secondDaughterIndex = daus.second;
        //cout << " Evt: "<< iEv <<" / Daus idxs: " << firstDaughterIndex << " - " << secondDaughterIndex << endl;
        TLorentzVector tlv_firstLepton (theBigTree.daughters_px->at (firstDaughterIndex),theBigTree.daughters_py->at (firstDaughterIndex),theBigTree.daughters_pz->at (firstDaughterIndex),theBigTree.daughters_e->at (firstDaughterIndex));
        TLorentzVector tlv_secondLepton (theBigTree.daughters_px->at (secondDaughterIndex),theBigTree.daughters_py->at (secondDaughterIndex),theBigTree.daughters_pz->at(secondDaughterIndex),theBigTree.daughters_e->at (secondDaughterIndex));
    }
    else
        continue;
    

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
        //if (TMath::Abs(tlv_jet.Eta()) > 5.) continue;      // 5. for VBF jets
        if (TMath::Abs(tlv_jet.Eta()) > 4.7) continue;       // 5. for VBF jets - 4.7 from AN2017-027 HTT
        
        // VBF jets (eta<5)
        jets_and_IDs.push_back(make_pair(tlv_jet, ijet));

        // create vect of pairs (csv score, jetID)
        jets_and_btag.push_back(make_pair(theBigTree.bCSVscore->at(ijet), ijet));

        // create vect of pairs (pt, jetID)
        jets_and_pt.push_back(make_pair(tlv_jet.Pt(), ijet));

        //if (TMath::Abs(tlv_jet.Eta()) > 2.4) continue;       // 2.4 for a reasonable b-tag (2.4 is the tracker acceptance)
    }
    
    // more than 2 b-jets
    if (jets_and_btag.size() > 2) moreThan2bJets +=1;
    
    // at least 2 b-jets
    if (jets_and_btag.size() < 2) lessThan2bJets +=1;

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

    // Clean jets from leptons dR < 0.3
    if (TausExist)
    {
	vector<int> toBeRemoved;
        for(int n=0; n<jets_and_IDs.size(); n++)
	{
        int firstDaughterIndex  = daus.first;
        int secondDaughterIndex = daus.second;
        //cout << " Evt: "<< iEv <<" / Daus idxs: " << firstDaughterIndex << " - " << secondDaughterIndex << endl;
        TLorentzVector tlv_firstLepton (theBigTree.daughters_px->at (firstDaughterIndex),theBigTree.daughters_py->at (firstDaughterIndex),theBigTree.daughters_pz->at (firstDaughterIndex),theBigTree.daughters_e->at (firstDaughterIndex));
        TLorentzVector tlv_secondLepton (theBigTree.daughters_px->at (secondDaughterIndex),theBigTree.daughters_py->at (secondDaughterIndex),theBigTree.daughters_pz->at(secondDaughterIndex),theBigTree.daughters_e->at (secondDaughterIndex));
	    bool dr1 = jets_and_IDs.at(n).first.DeltaR(tlv_firstLepton)  < 0.3;
	    bool dr2 = tlv_secondLepton.DeltaR(jets_and_IDs.at(n).first) < 0.3;
	    if (dr1 || dr2) 
	    {
		toBeRemoved.push_back(jets_and_IDs.at(n).second);
		jets_and_IDs.erase(jets_and_IDs.begin() + n);
	    }
	}

	if (toBeRemoved.size() != 0)
	{
	    for (int i=0; i<toBeRemoved.size(); i++)
	    {
		for (int j=0; j<jets_and_btag.size(); j++)
		{
		    if (jets_and_btag.at(j).second == toBeRemoved.at(i)) jets_and_btag.erase(jets_and_btag.begin() + j);
		}
		for (int k=0; k<jets_and_pt.size(); k++)
		{
		    if (jets_and_pt.at(k).second == toBeRemoved.at(i)) jets_and_pt.erase(jets_and_pt.begin() + k);
		}

	    }
	}

    }

    // ---------------------------------------------------------
    // ---------------- 4 jets in the event --------------------
    // ---------------------------------------------------------
    if (jets_and_IDs.size() < 4)        // less than 4 reco jets
    {
        less4recoJets +=1;
        continue;
    }
    else if (jets_and_IDs.size() == 4)  // exactly 4 reco jets
    {
        exactly4recoJets +=1;
        
        // First 2 by CSV, pT, Mjj, dEta
        int ijet1_b = jets_and_btag.at(jets_and_btag.size()-1).second;
        int ijet2_b = jets_and_btag.at(jets_and_btag.size()-2).second;
        int ijet1_p = jets_and_pt.at(jets_and_pt.size()-1).second;
        int ijet2_p = jets_and_pt.at(jets_and_pt.size()-2).second;
        pair<int,int> mjj_pair;
        mjj_pair = getMJJpair(jets_and_IDs);
        int ijet1_m = mjj_pair.first;
        int ijet2_m = mjj_pair.second;
        pair<int,int> eta_pair;
        eta_pair = getETApair(jets_and_IDs);
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
        
        // the other two jets
        int ijet3_b = jets_and_btag.at(jets_and_btag.size()-3).second;
        int ijet4_b = jets_and_btag.at(jets_and_btag.size()-4).second;
        int ijet3_p = jets_and_pt.at(jets_and_pt.size()-3).second;
        int ijet4_p = jets_and_pt.at(jets_and_pt.size()-4).second;
        
        vector<pair<TLorentzVector, int> > mjj_copy = jets_and_IDs;
        for (int n=mjj_copy.size()-1; n>=0; n--)
        {
            if ( mjj_copy.at(n).second == ijet1_m || mjj_copy.at(n).second == ijet2_m ) mjj_copy.erase(mjj_copy.begin() + n);
        }
        int ijet3_m = mjj_copy.at(0).second;
        int ijet4_m = mjj_copy.at(1).second;

        vector<pair<TLorentzVector, int> > eta_copy = jets_and_IDs;
        for (int n=eta_copy.size()-1; n>=0; n--)
        {
            if ( eta_copy.at(n).second == ijet1_e || eta_copy.at(n).second == ijet2_e ) eta_copy.erase(eta_copy.begin() + n);
        }
        int ijet3_e = eta_copy.at(0).second;
        int ijet4_e = eta_copy.at(1).second;

        TLorentzVector jet3_b(theBigTree.jets_px->at(ijet3_b),theBigTree.jets_py->at(ijet3_b),theBigTree.jets_pz->at(ijet3_b),theBigTree.jets_e->at(ijet3_b));
        TLorentzVector jet4_b(theBigTree.jets_px->at(ijet4_b),theBigTree.jets_py->at(ijet4_b),theBigTree.jets_pz->at(ijet4_b),theBigTree.jets_e->at(ijet4_b));
        TLorentzVector jet3_p(theBigTree.jets_px->at(ijet3_p),theBigTree.jets_py->at(ijet3_p),theBigTree.jets_pz->at(ijet3_p),theBigTree.jets_e->at(ijet3_p));
        TLorentzVector jet4_p(theBigTree.jets_px->at(ijet4_p),theBigTree.jets_py->at(ijet4_p),theBigTree.jets_pz->at(ijet4_p),theBigTree.jets_e->at(ijet4_p));
        TLorentzVector jet3_m(theBigTree.jets_px->at(ijet3_m),theBigTree.jets_py->at(ijet3_m),theBigTree.jets_pz->at(ijet3_m),theBigTree.jets_e->at(ijet3_m));
        TLorentzVector jet4_m(theBigTree.jets_px->at(ijet4_m),theBigTree.jets_py->at(ijet4_m),theBigTree.jets_pz->at(ijet4_m),theBigTree.jets_e->at(ijet4_m));
        TLorentzVector jet3_e(theBigTree.jets_px->at(ijet3_e),theBigTree.jets_py->at(ijet3_e),theBigTree.jets_pz->at(ijet3_e),theBigTree.jets_e->at(ijet3_e));
        TLorentzVector jet4_e(theBigTree.jets_px->at(ijet4_e),theBigTree.jets_py->at(ijet4_e),theBigTree.jets_pz->at(ijet4_e),theBigTree.jets_e->at(ijet4_e));


        // Matching part
        bool dr_csv_b   = (jet1_b.DeltaR(vgenb1H) <0.5 && jet2_b.DeltaR(vgenb2H) <0.5) || (jet2_b.DeltaR(vgenb1H) <0.5 && jet1_b.DeltaR(vgenb2H) <0.5);
        bool dr_csv_vbf = (jet3_b.DeltaR(vgenVBF1)<0.5 && jet4_b.DeltaR(vgenVBF2)<0.5) || (jet4_b.DeltaR(vgenVBF1)<0.5 && jet3_b.DeltaR(vgenVBF2)<0.5);

        bool dr_pt_b    = (jet3_p.DeltaR(vgenb1H) <0.5 && jet4_p.DeltaR(vgenb2H) <0.5) || (jet4_p.DeltaR(vgenb1H) <0.5 && jet3_p.DeltaR(vgenb2H) <0.5);
        bool dr_pt_vbf  = (jet1_p.DeltaR(vgenVBF1)<0.5 && jet2_p.DeltaR(vgenVBF2)<0.5) || (jet2_p.DeltaR(vgenVBF1)<0.5 && jet1_p.DeltaR(vgenVBF2)<0.5);
        
        bool dr_mjj_b   = (jet3_m.DeltaR(vgenb1H) <0.5 && jet4_m.DeltaR(vgenb2H) <0.5) || (jet4_m.DeltaR(vgenb1H) <0.5 && jet3_m.DeltaR(vgenb2H) <0.5);
        bool dr_mjj_vbf = (jet1_m.DeltaR(vgenVBF1)<0.5 && jet2_m.DeltaR(vgenVBF2)<0.5) || (jet2_m.DeltaR(vgenVBF1)<0.5 && jet1_m.DeltaR(vgenVBF2)<0.5);
        
        bool dr_eta_b   = (jet3_e.DeltaR(vgenb1H) <0.5 && jet4_e.DeltaR(vgenb2H) <0.5) || (jet4_e.DeltaR(vgenb1H) <0.5 && jet3_e.DeltaR(vgenb2H) <0.5);
        bool dr_eta_vbf = (jet1_e.DeltaR(vgenVBF1)<0.5 && jet2_e.DeltaR(vgenVBF2)<0.5) || (jet2_e.DeltaR(vgenVBF1)<0.5 && jet1_e.DeltaR(vgenVBF2)<0.5);
        
        bool dr_csv = dr_csv_b && dr_csv_vbf;
        bool dr_pt  = dr_pt_b  && dr_pt_vbf;
        bool dr_mjj = dr_mjj_b && dr_mjj_vbf;
        bool dr_eta = dr_eta_b && dr_eta_vbf;
        
        if (dr_csv) reco4jetsCSV +=1;
        if (dr_pt ) reco4jetsPT  +=1;
        if (dr_mjj) reco4jetsMJJ +=1;
        if (dr_eta) reco4jetsETA +=1;

        if (dr_csv_b)   dr_csv_b_1 +=1;
        if (dr_csv_vbf) dr_csv_vbf_1 +=1;
        if (dr_pt_b)    dr_pt_b_1  +=1;
        if (dr_pt_vbf)  dr_pt_vbf_1  +=1;
        if (dr_mjj_b)   dr_mjj_b_1 +=1;
        if (dr_mjj_vbf) dr_mjj_vbf_1 +=1;
        if (dr_eta_b)   dr_eta_b_1 +=1;
        if (dr_eta_vbf) dr_eta_vbf_1 +=1;
    }
    
    // ---------------------------------------------------------
    // ---------------- more than 4 reco jets ------------------
    // ---------------------------------------------------------

    //if (jets_and_IDs.size() > 4)  more4recoJets +=1;
    else  // more than 4 reco jets
    {
        // evts counter
        more4recoJets +=1;
        
        // First 2 by CSV, pT, Mjj, dEta
        int ijet1_b = jets_and_btag.at(jets_and_btag.size()-1).second;
        int ijet2_b = jets_and_btag.at(jets_and_btag.size()-2).second;
        int ijet1_p = jets_and_pt.at(jets_and_pt.size()-1).second;
        int ijet2_p = jets_and_pt.at(jets_and_pt.size()-2).second;
        pair<int,int> mjj_pair = getMJJpair(jets_and_IDs);
        int ijet1_m = mjj_pair.first;
        int ijet2_m = mjj_pair.second;
        pair<int,int> eta_pair = getETApair(jets_and_IDs);
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

        // 2bjets first (ijet1_b, ijet2_b), then the other two jets by CSV, PT, MJJ, ETA
        int ijet3_b_b = jets_and_btag.at(jets_and_btag.size()-3).second;
        int ijet4_b_b = jets_and_btag.at(jets_and_btag.size()-4).second;

        vector<pair<TLorentzVector, int> > pt_copy = jets_and_IDs;
        for (int n=pt_copy.size()-1; n>=0; n--)
        {
            if ( pt_copy.at(n).second == ijet1_b || pt_copy.at(n).second == ijet2_b ) pt_copy.erase(pt_copy.begin() + n);
        }
        pair<int,int> pt_pair_b = getPTpair(pt_copy);
        int ijet3_b_p = pt_pair_b.first;
        int ijet4_b_p = pt_pair_b.second;
        
        vector<pair<TLorentzVector, int> > mjj_copy = jets_and_IDs;
        for (int n=mjj_copy.size()-1; n>=0; n--)
        {
            if ( mjj_copy.at(n).second == ijet1_b || mjj_copy.at(n).second == ijet2_b ) mjj_copy.erase(mjj_copy.begin() + n);
        }
        pair<int,int> mjj_pair_b = getMJJpair(mjj_copy);
        int ijet3_b_m = mjj_pair_b.first;
        int ijet4_b_m = mjj_pair_b.second;
        //cout << "mass: " << ijet1_b << ijet2_b <<ijet3_b_m<<ijet4_b_m<<endl;

        vector<pair<TLorentzVector, int> > eta_copy = jets_and_IDs;
        for (int n=eta_copy.size()-1; n>=0; n--)
        {
            if ( eta_copy.at(n).second == ijet1_b || eta_copy.at(n).second == ijet2_b ) eta_copy.erase(eta_copy.begin() + n);
        }
        pair<int,int> eta_pair_b = getETApair(eta_copy);
        int ijet3_b_e = eta_pair_b.first;
        int ijet4_b_e = eta_pair_b.second;
        //cout << "eta : " << ijet1_b << ijet2_b <<ijet3_b_e<<ijet4_b_e<<endl;

        TLorentzVector jet3_b_b(theBigTree.jets_px->at(ijet3_b_b),theBigTree.jets_py->at(ijet3_b_b),theBigTree.jets_pz->at(ijet3_b_b),theBigTree.jets_e->at(ijet3_b_b));
        TLorentzVector jet4_b_b(theBigTree.jets_px->at(ijet4_b_b),theBigTree.jets_py->at(ijet4_b_b),theBigTree.jets_pz->at(ijet4_b_b),theBigTree.jets_e->at(ijet4_b_b));
        TLorentzVector jet3_b_p(theBigTree.jets_px->at(ijet3_b_p),theBigTree.jets_py->at(ijet3_b_p),theBigTree.jets_pz->at(ijet3_b_p),theBigTree.jets_e->at(ijet3_b_p));
        TLorentzVector jet4_b_p(theBigTree.jets_px->at(ijet4_b_p),theBigTree.jets_py->at(ijet4_b_p),theBigTree.jets_pz->at(ijet4_b_p),theBigTree.jets_e->at(ijet4_b_p));
        TLorentzVector jet3_b_m(theBigTree.jets_px->at(ijet3_b_m),theBigTree.jets_py->at(ijet3_b_m),theBigTree.jets_pz->at(ijet3_b_m),theBigTree.jets_e->at(ijet3_b_m));
        TLorentzVector jet4_b_m(theBigTree.jets_px->at(ijet4_b_m),theBigTree.jets_py->at(ijet4_b_m),theBigTree.jets_pz->at(ijet4_b_m),theBigTree.jets_e->at(ijet4_b_m));
        TLorentzVector jet3_b_e(theBigTree.jets_px->at(ijet3_b_e),theBigTree.jets_py->at(ijet3_b_e),theBigTree.jets_pz->at(ijet3_b_e),theBigTree.jets_e->at(ijet3_b_e));
        TLorentzVector jet4_b_e(theBigTree.jets_px->at(ijet4_b_e),theBigTree.jets_py->at(ijet4_b_e),theBigTree.jets_pz->at(ijet4_b_e),theBigTree.jets_e->at(ijet4_b_e));


        /*cout << " -*-*-*- CSV Scores -*-*-*-" << endl;
        cout << "bjet " << ijet3_b_b << " " << ijet4_b_b <<": "<< theBigTree.bCSVscore->at(ijet3_b_b) << " " << theBigTree.bCSVscore->at(ijet4_b_b) << endl;
        cout << "Pt   " << ijet3_b_p << " " << ijet4_b_p <<": "<< theBigTree.bCSVscore->at(ijet3_b_p) << " " << theBigTree.bCSVscore->at(ijet4_b_p) << endl;
        cout << "Mjj  " << ijet3_b_m << " " << ijet4_b_m <<": "<< theBigTree.bCSVscore->at(ijet3_b_m) << " " << theBigTree.bCSVscore->at(ijet4_b_m) << endl;
        cout << "dEta " << ijet3_b_e << " " << ijet4_b_e <<": "<< theBigTree.bCSVscore->at(ijet3_b_e) << " " << theBigTree.bCSVscore->at(ijet4_b_e) << endl;*/
        

        // match
        bool dr_csv_b   = (jet1_b.DeltaR(vgenb1H)   <0.5 && jet2_b.DeltaR(vgenb2H)   <0.5) || (jet2_b.DeltaR(vgenb1H)   <0.5 && jet1_b.DeltaR(vgenb2H)   <0.5);
        bool dr_csv_vbf = (jet3_b_b.DeltaR(vgenVBF1)<0.5 && jet4_b_b.DeltaR(vgenVBF2)<0.5) || (jet4_b_b.DeltaR(vgenVBF1)<0.5 && jet3_b_b.DeltaR(vgenVBF2)<0.5);

        bool dr_pt_b    = (jet3_b_p.DeltaR(vgenb1H) <0.5 && jet4_b_p.DeltaR(vgenb2H) <0.5) || (jet4_b_p.DeltaR(vgenb1H) <0.5 && jet3_b_p.DeltaR(vgenb2H) <0.5);
        bool dr_pt_vbf  = (jet1_p.DeltaR(vgenVBF1)  <0.5 && jet2_p.DeltaR(vgenVBF2)  <0.5) || (jet2_p.DeltaR(vgenVBF1)  <0.5 && jet1_p.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_mjj_b   = (jet3_b_m.DeltaR(vgenb1H) <0.5 && jet4_b_m.DeltaR(vgenb2H) <0.5) || (jet4_b_m.DeltaR(vgenb1H) <0.5 && jet3_b_m.DeltaR(vgenb2H) <0.5);
        bool dr_mjj_vbf = (jet1_m.DeltaR(vgenVBF1)  <0.5 && jet2_m.DeltaR(vgenVBF2)  <0.5) || (jet2_m.DeltaR(vgenVBF1)  <0.5 && jet1_m.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_eta_b   = (jet3_b_e.DeltaR(vgenb1H) <0.5 && jet4_b_e.DeltaR(vgenb2H) <0.5) || (jet4_b_e.DeltaR(vgenb1H) <0.5 && jet3_b_e.DeltaR(vgenb2H) <0.5);
        bool dr_eta_vbf = (jet1_e.DeltaR(vgenVBF1)  <0.5 && jet2_e.DeltaR(vgenVBF2)  <0.5) || (jet2_e.DeltaR(vgenVBF1)  <0.5 && jet1_e.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_csv = dr_csv_b && dr_csv_vbf;
        bool dr_pt  = dr_pt_b  && dr_pt_vbf;
        bool dr_mjj = dr_mjj_b && dr_mjj_vbf;
        bool dr_eta = dr_eta_b && dr_eta_vbf;
        
        if (dr_csv) reco5jetsCSV +=1;
        if (dr_pt ) reco5jetsPT  +=1;
        if (dr_mjj) reco5jetsMJJ +=1;
        if (dr_eta) reco5jetsETA +=1;

        if (dr_csv_b)   dr_csv_b_2+=1;
        if (dr_csv_vbf) dr_csv_vbf_2+=1;
        if (dr_pt_b)    dr_pt_b_2+=1;
        if (dr_pt_vbf)  dr_pt_vbf_2+=1;
        if (dr_mjj_b)   dr_mjj_b_2+=1;
        if (dr_mjj_vbf) dr_mjj_vbf_2+=1;
        if (dr_eta_b)   dr_eta_b_2+=1;
        if (dr_eta_vbf) dr_eta_vbf_2+=1;


        // 2 VBF first - by CSV (ijet1_b, ijet2_b), by MJJ (ijet1_m, ijet2_m), by ETA (ijet1_e, ijet2_e) and by PT (ijet1_p, ijet2_p) - then the other two jets by CSV
        vector<pair<float, int> > csv_copy_b = jets_and_btag;
        for (int n=csv_copy_b.size()-1; n>=0; n--)
        {
            if ( csv_copy_b.at(n).second == ijet1_b || csv_copy_b.at(n).second == ijet2_b ) csv_copy_b.erase(csv_copy_b.begin() + n);
        }
        int ijet3_b_b_b = csv_copy_b.at(csv_copy_b.size()-1).second;
        int ijet4_b_b_b = csv_copy_b.at(csv_copy_b.size()-2).second;

        vector<pair<float, int> > csv_copy_m = jets_and_btag;
        for (int n=csv_copy_m.size()-1; n>=0; n--)
        {
            if ( csv_copy_m.at(n).second == ijet1_m || csv_copy_m.at(n).second == ijet2_m ) csv_copy_m.erase(csv_copy_m.begin() + n);
        }
        int ijet3_m_b = csv_copy_m.at(csv_copy_m.size()-1).second;
        int ijet4_m_b = csv_copy_m.at(csv_copy_m.size()-2).second;
        //cout << "eta : " << ijet1_m << ijet2_m <<ijet3_m_b<<ijet4_m_b<<endl;
        
        vector<pair<float, int> > csv_copy_p = jets_and_btag;
        for (int n=csv_copy_p.size()-1; n>=0; n--)
        {
            if ( csv_copy_p.at(n).second == ijet1_p || csv_copy_p.at(n).second == ijet2_p ) csv_copy_p.erase(csv_copy_p.begin() + n);
        }
        int ijet3_p_b = csv_copy_p.at(csv_copy_p.size()-1).second;
        int ijet4_p_b = csv_copy_p.at(csv_copy_p.size()-2).second;

        vector<pair<float, int> > csv_copy_e = jets_and_btag;
        for (int n=csv_copy_e.size()-1; n>=0; n--)
        {
            if ( csv_copy_e.at(n).second == ijet1_e || csv_copy_e.at(n).second == ijet2_e ) csv_copy_e.erase(csv_copy_e.begin() + n);
        }
        int ijet3_e_b = csv_copy_e.at(csv_copy_e.size()-1).second;
        int ijet4_e_b = csv_copy_e.at(csv_copy_e.size()-2).second;

        TLorentzVector jet3_b_b_b(theBigTree.jets_px->at(ijet3_b_b_b),theBigTree.jets_py->at(ijet3_b_b_b),theBigTree.jets_pz->at(ijet3_b_b_b),theBigTree.jets_e->at(ijet3_b_b_b));
        TLorentzVector jet4_b_b_b(theBigTree.jets_px->at(ijet4_b_b_b),theBigTree.jets_py->at(ijet4_b_b_b),theBigTree.jets_pz->at(ijet4_b_b_b),theBigTree.jets_e->at(ijet4_b_b_b));
        TLorentzVector jet3_m_b(theBigTree.jets_px->at(ijet3_m_b),theBigTree.jets_py->at(ijet3_m_b),theBigTree.jets_pz->at(ijet3_m_b),theBigTree.jets_e->at(ijet3_m_b));
        TLorentzVector jet4_m_b(theBigTree.jets_px->at(ijet4_m_b),theBigTree.jets_py->at(ijet4_m_b),theBigTree.jets_pz->at(ijet4_m_b),theBigTree.jets_e->at(ijet4_m_b));
        TLorentzVector jet3_p_b(theBigTree.jets_px->at(ijet3_p_b),theBigTree.jets_py->at(ijet3_p_b),theBigTree.jets_pz->at(ijet3_p_b),theBigTree.jets_e->at(ijet3_p_b));
        TLorentzVector jet4_p_b(theBigTree.jets_px->at(ijet4_p_b),theBigTree.jets_py->at(ijet4_p_b),theBigTree.jets_pz->at(ijet4_p_b),theBigTree.jets_e->at(ijet4_p_b));
        TLorentzVector jet3_e_b(theBigTree.jets_px->at(ijet3_e_b),theBigTree.jets_py->at(ijet3_e_b),theBigTree.jets_pz->at(ijet3_e_b),theBigTree.jets_e->at(ijet3_e_b));
        TLorentzVector jet4_e_b(theBigTree.jets_px->at(ijet4_e_b),theBigTree.jets_py->at(ijet4_e_b),theBigTree.jets_pz->at(ijet4_e_b),theBigTree.jets_e->at(ijet4_e_b));

        //matching
        bool dr_bb_b   = (jet3_b_b_b.DeltaR(vgenb1H) <0.5 && jet4_b_b_b.DeltaR(vgenb2H) <0.5) || (jet4_b_b_b.DeltaR(vgenb1H) <0.5 && jet3_b_b_b.DeltaR(vgenb2H) <0.5);
        bool dr_bb_vbf = (jet1_b.DeltaR(vgenVBF1)  <0.5 && jet2_b.DeltaR(vgenVBF2)  <0.5) || (jet2_b.DeltaR(vgenVBF1)  <0.5 && jet1_b.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_mb_b   = (jet3_m_b.DeltaR(vgenb1H) <0.5 && jet4_m_b.DeltaR(vgenb2H) <0.5) || (jet4_m_b.DeltaR(vgenb1H) <0.5 && jet3_m_b.DeltaR(vgenb2H) <0.5);
        bool dr_mb_vbf = (jet1_m.DeltaR(vgenVBF1)  <0.5 && jet2_m.DeltaR(vgenVBF2)  <0.5) || (jet2_m.DeltaR(vgenVBF1)  <0.5 && jet1_m.DeltaR(vgenVBF2)  <0.5);

        bool dr_pb_b   = (jet3_p_b.DeltaR(vgenb1H) <0.5 && jet4_p_b.DeltaR(vgenb2H) <0.5) || (jet4_p_b.DeltaR(vgenb1H) <0.5 && jet3_p_b.DeltaR(vgenb2H) <0.5);
        bool dr_pb_vbf = (jet1_p.DeltaR(vgenVBF1)  <0.5 && jet2_p.DeltaR(vgenVBF2)  <0.5) || (jet2_p.DeltaR(vgenVBF1)  <0.5 && jet1_p.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_eb_b   = (jet3_e_b.DeltaR(vgenb1H) <0.5 && jet4_e_b.DeltaR(vgenb2H) <0.5) || (jet4_e_b.DeltaR(vgenb1H) <0.5 && jet3_e_b.DeltaR(vgenb2H) <0.5);
        bool dr_eb_vbf = (jet1_e.DeltaR(vgenVBF1)  <0.5 && jet2_e.DeltaR(vgenVBF2)  <0.5) || (jet2_e.DeltaR(vgenVBF1)  <0.5 && jet1_e.DeltaR(vgenVBF2)  <0.5);
        
        bool dr_bb = dr_bb_b && dr_bb_vbf;
        bool dr_mb = dr_mb_b && dr_mb_vbf;
        bool dr_pb = dr_pb_b && dr_pb_vbf;
        bool dr_eb = dr_eb_b && dr_eb_vbf;
        
        if (dr_bb) reco6jetsBB +=1;
        if (dr_mb) reco6jetsMB +=1;
        if (dr_pb) reco6jetsPB +=1;
        if (dr_eb) reco6jetsEB +=1;

        if (dr_bb_b)   dr_csv_b_3+=1;
        if (dr_bb_vbf) dr_csv_vbf_3+=1;
        if (dr_pb_b)    dr_pt_b_3+=1;
        if (dr_pb_vbf)  dr_pt_vbf_3+=1;
        if (dr_mb_b)   dr_mjj_b_3+=1;
        if (dr_mb_vbf) dr_mjj_vbf_3+=1;
        if (dr_eb_b)   dr_eta_b_3+=1;
        if (dr_eb_vbf) dr_eta_vbf_3+=1;

    }
    
    goodEvts +=1;
  }

  TFile* fOut = new TFile("fOut.root", "recreate");
  //dPhiMatched->Write();
  //hMbbMatched->Write();
  //hMbbAll->Write();
  /*hMbbAll->Write();
  hMbbMatched->Write();
  hMbbHMatched->Write();
  dPhiMatched->Write();
  dPhiHMatched->Write();*/
  
  hGenVBFMjj ->Write();
  hGenBMjj   ->Write();
  hGenVBFdEta->Write();
  hGenBdEta->Write();

  /*cout << "DONE" << endl;
  cout << "percentage good selected             : " << 100.*nGoodSelected/nTot << endl;
  cout << "percentage good selected w/ gen cuts : " << 100.*nGoodSelectedGenCuts/nTotGenCuts << endl;
  cout << "there was something good             : " << 100.*nAllJetsMatched/nTot << endl;
  cout << "there was something good w/ gen cuts : " << 100.*nAllJetsMatchedGenCuts/nTotGenCuts << endl;*/

  cout << endl;
  cout << "Total analyzed evts          : " << totEvts <<endl;
  cout << "---" << endl;
  cout << "Less/more than 2 genB, genBH, notSameH, genVBF: " << nBGen <<" ("<< 100.*nBGen/totEvts <<"%) - " << nBGenH <<" ("<< 100.*nBGenH/totEvts <<"%) - "
  << nMothersMatch <<" ("<< 100.*nMothersMatch/totEvts <<"%) - "<< nGenVBF <<" ("<< 100.*nGenVBF/totEvts <<"%)"<< endl;
  cout << "---" << endl;
  cout << " evts with 2 good taus       : " << twoTaus <<" ("<< 100.*twoTaus/totEvts <<"%)"<<endl;
  /*cout << "---" << endl;
  cout << "genB and genBH matching      : " << nbandbH <<" ("<< 100.*nbandbH/totEvts <<"%)"<< endl;
  cout << "genB and genVBF are the same : " << nbandVBF <<" ("<< 100.*nbandVBF/totEvts <<"%)"<< endl;
  cout << "genVBF and genB matched      : " << genBVBFmatch <<" ("<< 100.*genBVBFmatch/totEvts <<"%)"<< endl;*/
  cout << "---" << endl;
  //cout << "Less than 2 reco jets        : " << lessThan2bJets <<" ("<< 100.*lessThan2bJets/totEvts <<"%)"<< endl;
  cout << "  - After cleaning -" << endl;
  cout << "Less than 4 reco jets        : " << less4recoJets <<" ("<<100.*less4recoJets/totEvts <<"%)"<<endl;
  cout << "Exactly   4 reco jets        : " << exactly4recoJets <<" ("<<100.*exactly4recoJets/totEvts <<"%)"<<endl;
  cout << "More than 4 reco jets        : " << more4recoJets <<" ("<<100.*more4recoJets/totEvts <<"%)"<<endl;
  cout << "---" << endl;
  int goodRecoJets = exactly4recoJets + more4recoJets;
  cout << "   - Only 4 reco jets -" << endl;
  cout << " chose 2b   by CSV           : " << reco4jetsCSV << " ("<<100.*reco4jetsCSV/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                     : " << dr_csv_b_1 << " ("<<100.*dr_csv_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_csv_vbf_1 << " ("<<100.*dr_csv_vbf_1/exactly4recoJets <<"%)"<<endl;

  cout << " chose 2vbf by PT            : " << reco4jetsPT << " ("<<100.*reco4jetsPT/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                     : " << dr_pt_b_1 << " ("<<100.*dr_pt_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_pt_vbf_1 << " ("<<100.*dr_pt_vbf_1/exactly4recoJets <<"%)"<<endl;
  
  cout << " chose 2vbf by MJJ           : " << reco4jetsMJJ << " ("<<100.*reco4jetsMJJ/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                     : " << dr_mjj_b_1 << " ("<<100.*dr_mjj_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_mjj_vbf_1 << " ("<<100.*dr_mjj_vbf_1/exactly4recoJets <<"%)"<<endl;
  
  cout << " chose 2vbf by ETA           : " << reco4jetsETA << " ("<<100.*reco4jetsETA/exactly4recoJets <<"%)"<<endl;
  cout << "    b-only                     : " << dr_eta_b_1 << " ("<<100.*dr_eta_b_1/exactly4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_eta_vbf_1 << " ("<<100.*dr_eta_vbf_1/exactly4recoJets <<"%)"<<endl;
  cout << "---"<<endl;
  cout << "   -- More than 4 reco jets --" << endl;
  cout << "Chose 2b first by CSV then 2vbf by:"<< endl;
  cout << "  CSV                        : " << reco5jetsCSV << " ("<<100.*reco5jetsCSV/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_csv_b_2 << " ("<<100.*dr_csv_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_csv_vbf_2 << " ("<<100.*dr_csv_vbf_2/more4recoJets <<"%)"<<endl;
  cout << "  PT                         : " << reco5jetsPT << " ("<<100.*reco5jetsPT/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_pt_b_2 << " ("<<100.*dr_pt_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_pt_vbf_2 << " ("<<100.*dr_pt_vbf_2/more4recoJets <<"%)"<<endl;

  cout << "  MJJ                        : " << reco5jetsMJJ << " ("<<100.*reco5jetsMJJ/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_mjj_b_2 << " ("<<100.*dr_mjj_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_mjj_vbf_2 << " ("<<100.*dr_mjj_vbf_2/more4recoJets <<"%)"<<endl;
  
  cout << "  ETA                        : " << reco5jetsETA << " ("<<100.*reco5jetsETA/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_eta_b_2 << " ("<<100.*dr_eta_b_2/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_eta_vbf_2 << " ("<<100.*dr_eta_vbf_2/more4recoJets <<"%)"<<endl;
  cout << "Chose 2vbf (csv,pt,mjj,eta) first then 2b (csv):"<< endl;
  cout << "  CSV                        : " << reco6jetsBB << " ("<<100.*reco6jetsBB/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_csv_b_3 << " ("<<100.*dr_csv_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_csv_vbf_3 << " ("<<100.*dr_csv_vbf_3/more4recoJets <<"%)"<<endl;
  
  cout << "  PT                         : " << reco6jetsPB << " ("<<100.*reco6jetsPB/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_pt_b_3 << " ("<<100.*dr_pt_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_pt_vbf_3 << " ("<<100.*dr_pt_vbf_3/more4recoJets <<"%)"<<endl;
  
  cout << "  MJJ                        : " << reco6jetsMB << " ("<<100.*reco6jetsMB/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_mjj_b_3 << " ("<<100.*dr_mjj_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_mjj_vbf_3 << " ("<<100.*dr_mjj_vbf_3/more4recoJets <<"%)"<<endl;
  
  cout << "  ETA                        : " << reco6jetsEB << " ("<<100.*reco6jetsEB/more4recoJets <<"%)"<<endl;
    cout << "    b-only                     : " << dr_eta_b_3 << " ("<<100.*dr_eta_b_3/more4recoJets <<"%)"<<endl;
  cout << "    vbf-only                   : " << dr_eta_vbf_3 << " ("<<100.*dr_eta_vbf_3/more4recoJets <<"%)"<<endl;
  
  cout << "---" << endl;
  /*cout << "---" << endl;
  cout << " - eff over all evts with >= 4 jets -" << endl;
  cout << "   - Only 4 reco jets -" << endl;
  cout << " chose 2b   by CSV           : " << reco4jetsCSV << " ("<<100.*reco4jetsCSV/goodRecoJets <<"%)"<<endl;
  cout << " chose 2vbf by PT            : " << reco4jetsPT << " ("<<100.*reco4jetsPT/goodRecoJets <<"%)"<<endl;
  cout << " chose 2vbf by MJJ           : " << reco4jetsMJJ << " ("<<100.*reco4jetsMJJ/goodRecoJets <<"%)"<<endl;
  cout << " chose 2vbf by ETA           : " << reco4jetsETA << " ("<<100.*reco4jetsETA/goodRecoJets <<"%)"<<endl;
  cout << "---"<<endl;
  cout << "   -- More than 4 reco jets --" << endl;
  cout << "Chose 2b first by CSV then 2vbf by:"<< endl;
  cout << "  CSV                        : " << reco5jetsCSV << " ("<<100.*reco5jetsCSV/goodRecoJets <<"%)"<<endl;
  cout << "  PT                         : " << reco5jetsPT << " ("<<100.*reco5jetsPT/goodRecoJets <<"%)"<<endl;
  cout << "  MJJ                        : " << reco5jetsMJJ << " ("<<100.*reco5jetsMJJ/goodRecoJets <<"%)"<<endl;
  cout << "  ETA                        : " << reco5jetsETA << " ("<<100.*reco5jetsETA/goodRecoJets <<"%)"<<endl;
  cout << endl;
  cout << "Chose 2vbf (csv,pt,mjj,eta) first then 2b (csv):"<< endl;
  cout << "  CSV                        : " << reco6jetsBB << " ("<<100.*reco6jetsBB/goodRecoJets <<"%)"<<endl;
  cout << "  PT                         : " << reco6jetsPB << " ("<<100.*reco6jetsPB/goodRecoJets <<"%)"<<endl;
  cout << "  MJJ                        : " << reco6jetsMB << " ("<<100.*reco6jetsMB/goodRecoJets <<"%)"<<endl;
  cout << "  ETA                        : " << reco6jetsEB << " ("<<100.*reco6jetsEB/goodRecoJets <<"%)"<<endl;
  cout << "---" << endl;
  cout << "---" << endl;*/
  cout << "Total good events (>=4 jets) : " << goodRecoJets <<" ("<< 100.*goodRecoJets/totEvts <<"%)"<< endl;
}



