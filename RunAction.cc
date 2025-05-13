#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

RunAction::RunAction() 
    :reflectedPhotons(0), tirPhotons(0), totalGeneratedPhotons(0), totalDetectedPhotons(0) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->OpenFile("output.root");

    // Define ntuple structure for photon hits
    analysisManager->CreateNtuple("Hits", "Photon Hits");
    analysisManager->CreateNtupleDColumn("x");      // x position
    analysisManager->CreateNtupleDColumn("y");      // y position
    analysisManager->CreateNtupleDColumn("energy"); // Energy in eV
    analysisManager->CreateNtupleDColumn("time");   // LocalTime in ns
    analysisManager->CreateNtupleDColumn("edep");   // eV
    analysisManager->FinishNtuple();
    
    //for counter(line7)
    
}

RunAction::~RunAction() {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

void RunAction::BeginOfRunAction(const G4Run*) {
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    G4AnalysisManager::Instance()->Reset();
    //G4AnalysisManager::Instance()->OpenFile("output.root");;
    
    //Resetting counters
    reflectedPhotons = 0;
    tirPhotons = 0;
    totalGeneratedPhotons = 0;
    totalDetectedPhotons = 0;
}

//void RunAction::EndOfRunAction(const G4Run*) {
    // Compute amplification factor
  //  G4double amplificationFactor = 0.0; // Initialize with default value

   // if (totalGeneratedPhotons > 0) {
     //   amplificationFactor = static_cast<G4double>(totalDetectedPhotons) / totalGeneratedPhotons;
    //}
    
   // G4cout << "\n==== Cherenkov Photon Statistics ====" << G4endl;
   // G4cout << "Total Generated Photons: " << totalGeneratedPhotons << G4endl;
   // G4cout << "Total Detected Photons: " << totalDetectedPhotons << G4endl;
   // G4cout << "Amplification Factor: " << amplificationFactor << G4endl;
   // G4cout << "=====================================" << G4endl;
   // G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   // analysisManager->Write();
    
void RunAction::EndOfRunAction(const G4Run*) {
    G4double amplificationFactor = (totalGeneratedPhotons > 0) 
                                   ? static_cast<G4double>(totalDetectedPhotons) / totalGeneratedPhotons 
                                   : 0.0;

    // Print only the numbers (no extra text)
    G4cout << totalGeneratedPhotons << " " 
           << totalDetectedPhotons << " " 
           << reflectedPhotons << " "
           << tirPhotons << " "
           << amplificationFactor << G4endl;
}

// Functions to count various photon interactions
void RunAction::IncrementReflectedPhotons() { reflectedPhotons++; }
void RunAction::IncrementTIRPhotons() { tirPhotons++; }
void RunAction::IncrementGeneratedPhotons() { totalGeneratedPhotons++; }
void RunAction::IncrementDetectedPhotons() { totalDetectedPhotons++; }
    //G4AnalysisManager::Instance()->CloseFile();
//}

//counting photons generated
//void RunAction::IncrementGeneratedPhotons() {
    //totalGeneratedPhotons++;
//}

//counting photons detected
//void RunAction::IncrementDetectedPhotons() {
 //   totalDetectedPhotons++;
//}
