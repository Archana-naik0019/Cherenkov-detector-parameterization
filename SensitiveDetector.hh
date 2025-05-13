#ifndef SENSITIVE_DETECTOR_HH
#define SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();

    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
};

#endif
