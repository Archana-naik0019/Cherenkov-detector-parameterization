#include "SensitiveDetector.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "RunAction.hh"
#include "G4Track.hh"
#include "RunAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name) {}

SensitiveDetector::~SensitiveDetector() {}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    G4Track* track = aStep->GetTrack();
    if (track->GetDefinition()->GetParticleName() != "opticalphoton") return false;

    G4ThreeVector position = track->GetPosition();
    G4double energy = track->GetTotalEnergy();
    G4double time = track->GetLocalTime();  // LocalTime is more useful for gen timing
    G4double edep = aStep->GetTotalEnergyDeposit();

    G4cout << "[Debug] Checking if photon reached detector at " 
           << position << G4endl;
    
    //G4cout << "[Hit] Photon detected at " << position
      //     << ", Energy: " << energy / eV << " eV, Time: "
        //   << time / ns << " ns" << G4endl;

    //Counting of Detected photons
    //RunAction* runAction = (RunAction*) G4RunManager::GetRunManager()->GetUserRunAction();
    //if (runAction) {
      //  runAction->IncrementDetectedPhotons();
    //} else {
      //  G4cerr << "Error: RunAction is null! Photon count may be incorrect." << G4endl;
    //}
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleDColumn(0, position.x()/mm);
    analysisManager->FillNtupleDColumn(1, position.y()/mm);
    analysisManager->FillNtupleDColumn(2, energy/eV);
    analysisManager->FillNtupleDColumn(3, time/ns);
    analysisManager->FillNtupleDColumn(4, edep/eV);
    
    analysisManager->AddNtupleRow();
    
    //To fix edep
    if (edep > 0) {
    G4cout << "[DEBUG] Energy Deposited: " << edep / eV << " eV at " << position << G4endl;
}


    return true;
}
