#include "G4OpBoundaryProcess.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"

SteppingAction::SteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    G4Track* track = step->GetTrack();
    G4String volumeName = track->GetVolume()->GetName();
    
    RunAction* runAction = (RunAction*) G4RunManager::GetRunManager()->GetUserRunAction();
    if (!runAction) return;
     
     //getting volume names(for verification)
    G4VPhysicalVolume* preStepVolume  = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    G4VPhysicalVolume* postStepVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
     
     // Only process Cherenkov photons
     //if (track->GetDefinition()->GetParticleName() == "opticalphoton") {
       
     if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
      // if (volumeName == "PixelLV") {  // Ensure this matches your Germanium volume name
        //    G4cout << "Optical photon entered Germanium PixelLV at: "
        //           << "X=" << track->GetPosition().x() << " mm, "
        //           << "Y=" << track->GetPosition().y() << " mm, "
        //           << "Z=" << track->GetPosition().z() << " mm, "
        //           << "Energy=" << track->GetKineticEnergy() << " MeV"
        //           << G4endl;
        //}
        
        //to get Counting of generated photons
       //RunAction* runAction = (RunAction*) G4RunManager::GetRunManager()->GetUserRunAction();
       runAction->IncrementGeneratedPhotons();
       
       //if (postStepVolume && postStepVolume->GetName() == "PixelLV") {
       if (track->GetVolume()->GetName() == "PixelPV") {
            runAction->IncrementDetectedPhotons();
            G4cout << "[Detected] Optical Photon hit the Germanium Detector!" << G4endl;
        }
        
        //
        //G4cout << "volume name" << track->GetVolume()->GetName() << G4endl;
        //
        
        
        //*********************************Ref vs TIR*************************//
        
        //method_1//
        //gives all things right but number of TIR needs fixing//
        
        
        // Boundary Process Tracking
//        G4OpBoundaryProcessStatus boundaryStatus = Undefined;
//        static G4OpBoundaryProcess* boundary = nullptr;

//        if (!boundary) {
//            G4ProcessManager* pm = track->GetDefinition()->GetProcessManager();
//            G4int nProcesses = pm->GetProcessListLength();
//            for (G4int i = 0; i < nProcesses; i++) {
//                G4VProcess* process = (*pm->GetProcessList())(i);
//                if (process->GetProcessName() == "OpBoundary") {
//                    boundary = static_cast<G4OpBoundaryProcess*>(process);
//                    break;
//                }
//            }
//        }

//        if (boundary) {
//            boundaryStatus = boundary->GetStatus();
        }

 	//method_2
 	
 	G4StepPoint* postStepPoint = step->GetPostStepPoint();
 	
 	if (postStepPoint->GetStepStatus() == fGeomBoundary) {
 	 
		const G4VProcess* process = postStepPoint->GetProcessDefinedStep();

		if (process && process->GetProcessName() == "OpBoundary") {
        		const G4OpBoundaryProcess* boundary = dynamic_cast<const G4OpBoundaryProcess*>(process);
        		if (boundary) {
        			G4OpBoundaryProcessStatus boundaryStatus = boundary->GetStatus();
 	
 
 
        // Identify Reflection or TIR
        			if (boundaryStatus == FresnelReflection) {
            			G4cout << "[Reflection] Photon was reflected from the coating at: "
                   			<< track->GetPosition() << G4endl;
            			runAction->IncrementReflectedPhotons(); // Track reflection
        			}

        			if (boundaryStatus == TotalInternalReflection) {
            			G4cout << "[TIR] Photon underwent Total Internal Reflection at: "
                   			<< track->GetPosition() << G4endl;
            			runAction->IncrementTIRPhotons(); // Track TIR
        			}	




        // Store Photon Data in ROOT
       // G4ThreeVector position = track->GetPosition();
        //G4double energy = track->GetTotalEnergy();
        //G4double time = track->GetGlobalTime();

        //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        //analysisManager->FillNtupleDColumn(0, position.x() / mm);
        //analysisManager->FillNtupleDColumn(1, position.y() / mm);
        //analysisManager->FillNtupleDColumn(2, energy / eV);
        //analysisManager->FillNtupleDColumn(3, time / ns);
        
        // Add a column for reflection type: (0 = None, 1 = Coating Reflection, 2 = TIR)
        //if (boundaryStatus == FresnelReflection) {
          //  analysisManager->FillNtupleIColumn(4, 1);
        //} else if (boundaryStatus == TotalInternalReflection) {
          //  analysisManager->FillNtupleIColumn(4, 2);
        //} else {
          //  analysisManager->FillNtupleIColumn(4, 0);
        //}

        //analysisManager->AddNtupleRow();
    }
}
        
 //       G4ThreeVector position = track->GetPosition();
 //       G4double energy = track->GetTotalEnergy();
 //       G4double time = track->GetGlobalTime();

        //G4cout << "[Step] Optical Photon at " << position
          //     << ", Energy: " << energy / eV << " eV, Time: "
            //   << time / ns << " ns" << G4endl;
               
        // Check if photon entered the Germanium detector
        //if (postStepVolume && postStepVolume->GetName() == "GermaniumDetector") {
          //  runAction->IncrementDetectedPhotons();
           // G4cout << "[Detected] Optical Photon hit the Germanium Detector!" << G4endl;
	//}

        //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        //analysisManager->FillNtupleDColumn(0, position.x()/mm);
        //analysisManager->FillNtupleDColumn(1, position.y()/mm);
        //analysisManager->FillNtupleDColumn(2, energy/eV);
        //analysisManager->FillNtupleDColumn(3, time/ns);
        
        //analysisManager->AddNtupleRow();



   }
}
