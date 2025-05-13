#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"
#include "G4SystemOfUnits.hh"

#include "G4GeneralParticleSource.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4Proton::ProtonDefinition());
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1, 0, 0.5));
    fParticleGun->SetParticleEnergy(5.0*GeV);
    fParticleGun->SetParticlePosition(G4ThreeVector(-15*cm, 0, -20*cm));
    
    fGPS = new G4GeneralParticleSource();
    
    useGPS = false; //setting it to true only while checking det by shooting photons otherwise I need to set it false
    
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
    
    delete fGPS;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    if (useGPS) {
        fGPS->GeneratePrimaryVertex(anEvent);
    } else {
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    
}
