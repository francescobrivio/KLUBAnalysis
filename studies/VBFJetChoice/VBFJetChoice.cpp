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

// c++ -lm -o VBFJetChoice VBFJetChoice.cpp `root-config --glibs --cflags` -I ../../interface/

void appendFromFileList (TChain* chain, TString filename)
{
    //cout << "=== inizio parser ===" << endl;
    std::ifstream infile(filename.Data());
    std::string line;
    while (std::getline(infile, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ")  != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
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

pair<int, int> find_max_dEta( vector<pair<TLorentzVector, int> > jets_and_eta )
{
    float dEtaMax = -999.0;
    int idx1 = -1;
    int idx2 = -1;
    // double loop to find the highest DeltaEta between jet pairs
    for (int i = 0; i < jets_and_eta.size(); i++ )
    {
        float eta1 = jets_and_eta.at(i).first.Eta();
        for (int j = i+1; j < jets_and_eta.size(); j++)
        {
            float eta2 = jets_and_eta.at(j).first.Eta();
            float dEta = TMath::Abs(eta1 - eta2);
            if (dEta > dEtaMax)
            {
                dEtaMax = dEta;
                idx1 = jets_and_eta.at(i).second;
                idx2 = jets_and_eta.at(j).second;
            }
        }
    }
    return make_pair(idx1,idx2);
}

// Get TLorentzVectors of gen particles
vector<TLorentzVector> GetGenTlv(vector<int> vGenParts, bigTree theBigTree)
{
    vector<TLorentzVector> out_vec;
    for (int n = 0; n < vGenParts.size(); n++ )
    {
	if (theBigTree.genpart_px->at(vGenParts.at(n)) == 0. &&  theBigTree.genpart_py->at(vGenParts.at(n)) == 0.) continue;
        // create the TLorentzVector
        TLorentzVector tmp_tlv;
        tmp_tlv.SetPxPyPzE
        (
            theBigTree.genpart_px->at(vGenParts.at(n)),
            theBigTree.genpart_py->at(vGenParts.at(n)),
            theBigTree.genpart_pz->at(vGenParts.at(n)),
            theBigTree.genpart_e ->at(vGenParts.at(n))
        );
        //cout << "   Gen: " << n << Form(" - pt = %.1f , eta = %.2f, phi = %.2f" , tmp_tlv.Pt(), tmp_tlv.Eta(), tmp_tlv.Phi()) << endl;
        //cout << "   Gen: " << n << Form(" - px = %.1f , py = %.2f, pz = %.2f" , theBigTree.genpart_px->at(vGenParts.at(n)), theBigTree.genpart_py->at(vGenParts.at(n)), theBigTree.genpart_pz->at(vGenParts.at(n))) << endl;
        out_vec.push_back(tmp_tlv);
    }
    
    return out_vec;
}

// Match the genParticles to the reco jets
vector<pair<pair<TLorentzVector, int>, int> > MatchToRecoJets(vector<TLorentzVector> vGenParticles, vector<pair<TLorentzVector, int> > recoJets)
{
    vector<pair<pair<TLorentzVector, int>, int> > matched_vec;
    //if (vgenb1.DeltaR(tlv_jet) < 0.5) matchedTo1.push_back(ijet);

    // Loop on reco jets
    for (int ijet = 0; ijet < recoJets.size(); ijet++ )
    {
        bool match = false;
        //cout << " Jet: " << ijet << Form(" - pt = %.1f , eta = %.2f, phi = %.2f" , recoJets.at(ijet).first.Pt(), recoJets.at(ijet).first.Eta(), recoJets.at(ijet).first.Phi()) << endl;
        // Loop on gen particles
        int matched_id = -1;
        for (int igen = 0; igen < vGenParticles.size(); igen++ )
        {
            //cout << "   Gen: " << igen << Form(" - pt = %.1f , eta = %.2f, phi = %.2f" , vGenParticles.at(igen).Pt(), vGenParticles.at(igen).Eta(), vGenParticles.at(igen).Phi()) << endl;
            //if ( vGenParticles.at(igen).Eta() > 5.0 ) continue; // Discard gen particles with (pt=0, phi=0, eta=100000000000.0)
            if ( recoJets.at(ijet).first.DeltaR(vGenParticles.at(igen)) < 0.5 ) {match = true; matched_id = igen; break;}
        }
        if (match) matched_vec.push_back(make_pair(recoJets.at(ijet),matched_id));
    }
    return matched_vec;
}


/// ------------ MAIN -------------
int main (int argc, char** argv)
{
  // input and output setup
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  //TString inputFile = "/home/llr/cms/cadamuro/HH2016/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/Files_7Feb2016/allRadion/61_GluGluToRadionToHHTo2B2Tau_M-300_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1.txt" ;
  TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/VBF_study/1_VBFHHTo2B2Tau_CV_1_C2V_1_C3_1_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt" ;
//  TString inputFile = "/gwpool/users/brivio/Hhh_1718/VBF_studies/CMSSW_7_4_7/src/KLUBAnalysis/inputFiles/JECproduction_Lug2017/81_GluGluToRadionToHHTo2B2Tau_M-250_narrow_13TeV-madgraph__RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1.txt";


  cout << ".. start" << endl;

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
  
  cout << ".. going in the loop" << endl;

  int nGoodSelected = 0;
  int nGoodSelectedGenCuts = 0;
  
  int nAllJetsMatched = 0;
  int nAllJetsMatchedGenCuts = 0;
  
  int nTot = 0;
  int nTotGenCuts = 0;

  TH1F* hMbbLuca = new TH1F("hMbbLuca", "hMbbLuca"   , 200, 0, 200);  
  TH1F* hMbbIo   = new TH1F("hMbbIo"  , "hMbbIo"     , 200, 0, 200);
  TH1F* hMptpt   = new TH1F("hMptpt"  , "hMptpt"  , 200, 0, 200);
  TH1F* hMetaeta = new TH1F("hMetaeta", "hMetaeta", 200, 0, 200);
  TH1F* hMmjj    = new TH1F("hMmjj"   , "hMmjj", 200, 0, 200);
  
  //TH1F* hMbbMatched   = new TH1F("hMbbMatched"  , "hMbbMatched"  , 200, 0, 200);
  //TH1F* dPhibbMatched = new TH1F("dPhibbMatched", "dPhibbMatched", 100, -2.*TMath::Pi(), 2.*TMath::Pi());

  for (uint iEv = 0; true; ++iEv)
  {
    int got = theBigTree.fChain->GetEntry(iEv);
    if (got == 0) break;
    if (iEv > 25) break;

    if (iEv % 1 == 0) cout << "---- Event: " << iEv << endl;
    
    std::vector<int> vGenIsHardProcess;
    //std::vector<int> vGenIsFirst;
    //std::vector<int> vGenIsLast;
    std::vector<int> vHMothInd;
    std::vector<int> vGenVBFParton;
    std::vector<int> vGenBParton;
    
    for (uint igen = 0; igen < theBigTree.genpart_px->size(); ++igen)
    {
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 7 )) continue; // 7; isHardProcess (interesting physics event)
        vGenIsHardProcess.push_back(igen);
        
        //cout << " --> genPart "<< igen << " decay: " << theBigTree.genpart_HZDecayMode->at(igen)<< endl;
        
        //if (!CheckBit(theBigTree.genpart_flags->at(igen), 12)) continue; // 12: first copy
            //vGenIsFirst.push_back(igen);

        //if (!CheckBit(theBigTree.genpart_flags->at(igen), 13)) continue; // 13: last copy
            //vGenIsLast.push_back(igen);
        
        if (theBigTree.genpart_HMothInd->at(igen) > 0) // FIXME - to be understood
            vHMothInd.push_back(igen);
        
        if (CheckBit(theBigTree.genpart_flags->at(igen), 15)) // 15: isVBFParton
            vGenVBFParton.push_back(igen);
        
        if (abs(theBigTree.genpart_pdg->at(igen)) == 5) // 5 = b quark
            vGenBParton.push_back(igen);
    }

    std::vector<TLorentzVector> vGenIsHardProcess_tlv = GetGenTlv(vGenIsHardProcess, theBigTree);
    std::vector<TLorentzVector> vHMothInd_tlv         = GetGenTlv(vHMothInd        , theBigTree);
    std::vector<TLorentzVector> vGenVBFParton_tlv     = GetGenTlv(vGenVBFParton    , theBigTree);
    std::vector<TLorentzVector> vGenBParton_tlv       = GetGenTlv(vGenBParton      , theBigTree);

    /*cout << "Sizes:" << endl;
    cout << " - hard:  " << vGenIsHardProcess.size() << " - " << vGenIsHardProcess_tlv.size() << endl;
    cout << " - HMoth: " << vHMothInd.size() << " - " << vHMothInd_tlv.size() << endl;
    cout << " - VBF:   " << vGenVBFParton.size() << " - " << vGenVBFParton_tlv.size() << endl;
    cout << " - Bpart: " << vGenBParton.size() << " - " << vGenBParton_tlv.size() << endl;
    
    cout << "HardProcess: ";
    for (int n = 0; n < vGenIsHardProcess.size(); n++ ) cout << " - " << vGenIsHardProcess.at(n);
    cout << endl;

    cout << "isFirst:     ";
    for (int n = 0; n < vGenIsFirst.size(); n++ ) cout << " - " << vGenIsFirst.at(n);
    cout << endl;

    cout << "isLast:      ";
    for (int n = 0; n < vGenIsLast.size(); n++ ) cout << " - " << vGenIsLast.at(n);
    cout << endl;

    cout << "HMothInd:    ";
    for (int n = 0; n < vHMothInd.size(); n++ ) cout << " - " << vHMothInd.at(n);
    cout << endl;

    cout << "VBFProcess:  ";
    for (int n = 0; n < vGenVBFParton.size(); n++ ) cout << " - " << vGenVBFParton.at(n);
    cout << endl;

    cout << "Bquarks:     ";
    for (int n = 0; n < vGenBParton.size(); n++ ) cout << " - " << vGenBParton.at(n);
    cout << endl;*/


    std::vector<int> vGenQuarks;
    //cout << " genpart size: " << theBigTree.genpart_px->size() << endl;
    for (uint igen = 0; igen < theBigTree.genpart_px->size(); ++igen)
    {
        //cout << " pdgID: " << abs(theBigTree.genpart_pdg->at(igen)) << endl; //FRA
        if (abs(theBigTree.genpart_pdg->at(igen)) != 5) continue; // 5 = b quark
        //if (!CheckBit(theBigTree.genpart_flags->at(igen), 13)) continue; // 13: last copy
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 12)) continue; // 12: first copy
        if (!CheckBit(theBigTree.genpart_flags->at(igen), 7)) continue; // 7: isHardProcess
        // if (!CheckBit(theBigTree.genpart_flags->at(igen), 8)) continue; // 8: fromHardProcess
        vGenQuarks.push_back(igen);
    }
    
    if (vGenQuarks.size() != 2)
    {
        cout << " Wrong b quark size: " << vGenQuarks.size() << endl;
        continue;
    }
    
    TLorentzVector vgenb1;
    TLorentzVector vgenb2;
    vgenb1.SetPxPyPzE(
        theBigTree.genpart_px->at(vGenQuarks.at(0)),
        theBigTree.genpart_py->at(vGenQuarks.at(0)),
        theBigTree.genpart_pz->at(vGenQuarks.at(0)),
        theBigTree.genpart_e->at(vGenQuarks.at(0))
    );
    vgenb2.SetPxPyPzE(
        theBigTree.genpart_px->at(vGenQuarks.at(1)),
        theBigTree.genpart_py->at(vGenQuarks.at(1)),
        theBigTree.genpart_pz->at(vGenQuarks.at(1)),
        theBigTree.genpart_e->at(vGenQuarks.at(1))
    );

    // select the jets - note: no overlap removal for simplicity, hopefully it is a small effect
    vector<pair<float, int> > jets_and_btag;
    //vector<pair<float, int> > jets_and_pt;
    vector<pair<TLorentzVector, int> > jets_and_eta;

    //vector<int> matchedTo1;
    //vector<int> matchedTo2;

    // Loop on jets
    //int nBtagMed = 0;
    //int nBtagLos = 0;
    //cout << " Jets size: " << theBigTree.jets_px->size() << endl;
    for (uint ijet = 0; ijet < theBigTree.jets_px->size(); ijet++)
    {
        //cout << "   ijet: " << ijet << endl;
        
        // create the jet TLorentzVector
        TLorentzVector tlv_jet 
        (
            theBigTree.jets_px->at (ijet),
            theBigTree.jets_py->at (ijet),
            theBigTree.jets_pz->at (ijet),
            theBigTree.jets_e->at  (ijet)
        );

        // if (theBigTree.jets_PUJetID->at (iJet) < PUjetID_minCut) continue ;
        if (theBigTree.PFjetID->at(ijet) < 1  ) continue;  // PF jet ID --> 0 : don't pass PF Jet ID; 1: loose; 2: tight; 3: tightLepVeto
        if (tlv_jet.Pt()                 < 20.) continue;  // Pt cut     --> 20 GeV
        if (TMath::Abs(tlv_jet.Eta())    > 2.4) continue;  // Eta cut   --> 2.4 for b-tag - 2.4 is the tracker acceptance

        // create vect of pairs (csv score, jetID)
        jets_and_btag.push_back(make_pair(theBigTree.bCSVscore->at(ijet), ijet));
        
        // create vect of pairs (pt, jetID)
        //jets_and_pt.push_back(make_pair(tlv_jet.Pt(), ijet));
        
        // create vect of pairs (tlv_jet, jetID)
        jets_and_eta.push_back(make_pair(tlv_jet, ijet));
        
        // count evts of medium and loose btagged jets
        //if (theBigTree.bCSVscore->at (ijet) > 0.8484) nBtagMed +=1;
        //if (theBigTree.bCSVscore->at (ijet) > 0.5426) nBtagLos +=1;
        
        //if (vgenb1.DeltaR(tlv_jet) < 0.5) matchedTo1.push_back(ijet);
        //if (vgenb2.DeltaR(tlv_jet) < 0.5) matchedTo2.push_back(ijet);
        
        // printing genjet ids:
        //cout << "   - partFlav: " << theBigTree.genjet_partonFlavour->at(ijet) <<  " - hadFlav: " << theBigTree.genjet_hadronFlavour->at(ijet) << endl;  
    }
    
    if (jets_and_btag.size() < 4) 
    {
        cout << "  #### Less than 4 reco jets: " << jets_and_btag.size() << endl;
        cout << "-----------------------------------------------------" << endl;
        continue;
    }
    //cout << "good jets" << endl;
    // Matching between genParticles and reco jets
    vector<pair<pair<TLorentzVector, int>, int> > matchedHardProcess  = MatchToRecoJets(vGenIsHardProcess_tlv, jets_and_eta);
    //cout << "mathced hard" << endl;
    vector<pair<pair<TLorentzVector, int>, int> > matchedHMothInd     = MatchToRecoJets(vHMothInd_tlv        , jets_and_eta);
    //cout << "matched Hmoth" << endl;
    vector<pair<pair<TLorentzVector, int>, int> > matchedGenVBFParton = MatchToRecoJets(vGenVBFParton_tlv    , jets_and_eta);
    //cout << "matched vbf" << endl;
    vector<pair<pair<TLorentzVector, int>, int> > matchedGenBParton   = MatchToRecoJets(vGenBParton_tlv      , jets_and_eta);
    //cout << "matched b" << endl;

    if (matchedGenBParton.size()<2)
    {
       cout << "  #### Wrong MatchedB size: " << matchedGenBParton.size() << endl;
       cout << "-----------------------------------------------------" << endl;
       continue;
    }

    cout << "Reco jets size:    " << jets_and_eta.size()        << " - "; for (int n = 0; n < jets_and_eta.size(); n++ ) cout << " " << jets_and_eta.at(n).second; cout << endl;
    cout << "MatchedHard size:  " << matchedHardProcess.size()  << " - "; for (int n = 0; n < matchedHardProcess.size(); n++ ) cout << " " << matchedHardProcess.at(n).first.second; cout << endl;
    cout << "                   " << matchedHardProcess.size()  << " - "; for (int n = 0; n < matchedHardProcess.size(); n++ ) cout << " " << matchedHardProcess.at(n).second; cout << endl;
    cout << "MatchedHMoth size: " << matchedHMothInd.size()     << " - "; for (int n = 0; n < matchedHMothInd.size(); n++ ) cout << " " << matchedHMothInd.at(n).first.second; cout << endl;
    cout << "                   " << matchedHMothInd.size()     << " - "; for (int n = 0; n < matchedHMothInd.size(); n++ ) cout << " " << matchedHMothInd.at(n).second; cout << endl;
    cout << "MatchedVBF size:   " << matchedGenVBFParton.size() << " - "; for (int n = 0; n < matchedGenVBFParton.size(); n++ ) cout << " " << matchedGenVBFParton.at(n).first.second; cout << endl;
    cout << "                   " << matchedGenVBFParton.size() << " - "; for (int n = 0; n < matchedGenVBFParton.size(); n++ ) cout << " " << matchedGenVBFParton.at(n).second; cout << endl;
    cout << "MatchedB size:     " << matchedGenBParton.size()   << " - "; for (int n = 0; n < matchedGenBParton.size(); n++ ) cout << " " << matchedGenBParton.at(n).first.second; cout << endl;
    cout << "                   " << matchedGenBParton.size()   << " - "; for (int n = 0; n < matchedGenBParton.size(); n++ ) cout << " " << matchedGenBParton.at(n).second; cout << endl;
    

    // 

    // sorting jets by csv, pt, eta
    stable_sort (jets_and_btag.begin(), jets_and_btag.end());
    //stable_sort (jets_and_pt.begin()  , jets_and_pt.end());
    
    /*cout << " csv jets:" << endl;
    for (int n = 0; n < jets_and_btag.size(); n++ )
    {
        cout << "   - jet: " << jets_and_btag.at(n).second << "  - csv: " << jets_and_btag.at(n).first << endl;
    }

    cout << " pt jets:" << endl;
    for (int n = 0; n < jets_and_pt.size(); n++ )
    {
        cout << "   - jet: " << jets_and_pt.at(n).second << "  - pt: " << jets_and_pt.at(n).first << endl;
    }*/

    // 2 higest CSV jets
    int b1 = jets_and_btag.at(jets_and_btag.size()-1).second;
    int b2 = jets_and_btag.at(jets_and_btag.size()-2).second;
    
    TLorentzVector tlv_b1
    (
        theBigTree.jets_px->at (b1),
        theBigTree.jets_py->at (b1),
        theBigTree.jets_pz->at (b1),
        theBigTree.jets_e->at  (b1)
    );

    TLorentzVector tlv_b2
    (
        theBigTree.jets_px->at (b2),
        theBigTree.jets_py->at (b2),
        theBigTree.jets_pz->at (b2),
        theBigTree.jets_e->at  (b2)
    );
    
    /* // 2 highest PT jets
    int pt1 = jets_and_pt.at(jets_and_pt.size()-1).second;
    int pt2 = jets_and_pt.at(jets_and_pt.size()-2).second;
    
    TLorentzVector tlv_pt1
    (
        theBigTree.jets_px->at (pt1),
        theBigTree.jets_py->at (pt1),
        theBigTree.jets_pz->at (pt1),
        theBigTree.jets_e->at  (pt1)
    );

    TLorentzVector tlv_pt2
    (
        theBigTree.jets_px->at (pt2),
        theBigTree.jets_py->at (pt2),
        theBigTree.jets_pz->at (pt2),
        theBigTree.jets_e->at  (pt2)
    );
    
    // 2 highest dETA jets
    pair<int, int> dEta_idxs = find_max_dEta( jets_and_eta );
    
    TLorentzVector tlv_eta1
    (
        theBigTree.jets_px->at (dEta_idxs.first),
        theBigTree.jets_py->at (dEta_idxs.first),
        theBigTree.jets_pz->at (dEta_idxs.first),
        theBigTree.jets_e->at  (dEta_idxs.first)
    );

    TLorentzVector tlv_eta2
    (
        theBigTree.jets_px->at (dEta_idxs.second),
        theBigTree.jets_py->at (dEta_idxs.second),
        theBigTree.jets_pz->at (dEta_idxs.second),
        theBigTree.jets_e->at  (dEta_idxs.second)
    );*/
    
    hMbbLuca ->Fill((tlv_b1  +tlv_b2  ).M());
    hMbbIo   ->Fill(-99.);
    hMptpt   ->Fill(-99.);
    hMetaeta ->Fill(-99.);
    hMmjj    ->Fill(-99.);

    /* // Fill mass histograms
    hMbbAll    ->Fill((tlv_b1  +tlv_b2  ).M());
    hMptptAll  ->Fill((tlv_pt1 +tlv_pt2 ).M());
    hMetaetaAll->Fill((tlv_eta1+tlv_eta2).M());*/

    /*bool drA1 = vgenb1.DeltaR(tlv_b1) < 0.5;
    bool drA2 = vgenb1.DeltaR(tlv_b2) < 0.5;
    bool drB1 = vgenb2.DeltaR(tlv_b1) < 0.5;
    bool drB2 = vgenb2.DeltaR(tlv_b2) < 0.5;

    bool drA1B2 = (drA1 && drB2);
    bool drA2B1 = (drA2 && drB1);*/

    /*nTot += 1;
    
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
        hMbbMatched->Fill((tlv_b1+tlv_b2).M());
        dPhibbMatched->Fill(tlv_b1.DeltaPhi(tlv_b2));
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

    ///// for debuggin and understanding
    if (genCuts && !(drA1B2 || drA2B1) && allMatches > 0 && nBtagMed >= 2 && false) //FRA: there is a FALSE here! remove it to have debug info!!
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
  cout << "-----------------------------------------------------" << endl;
  }

  TFile* fOut = new TFile("fOut.root", "recreate");
  //dPhibbMatched->Write();
  //hMbbMatched->Write();
  //hMbbAll    ->Write();
  //hMptptAll  ->Write();
  //hMetaetaAll->Write();
  hMbbLuca ->Write();
  hMbbIo   ->Write();
  hMptpt   ->Write();
  hMetaeta ->Write();
  hMmjj    ->Write();


  /*cout << "DONE" << endl;
  cout << "percentage good selected             : " << 100.*nGoodSelected/nTot << endl;
  cout << "percentage good selected w/ gen cuts : " << 100.*nGoodSelectedGenCuts/nTotGenCuts << endl;
  cout << "there was something good             : " << 100.*nAllJetsMatched/nTot << endl;
  cout << "there was something good w/ gen cuts : " << 100.*nAllJetsMatchedGenCuts/nTotGenCuts << endl;*/

}
