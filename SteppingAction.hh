#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4Step;

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction();
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);
    
    void IncrementReflectedPhotons() { reflectedPhotons++; }
    void IncrementTIRPhotons() { tirPhotons++; }
    
    G4int GetReflectedPhotons() const { return reflectedPhotons; }
    G4int GetTIRPhotons() const { return tirPhotons; }
    
private:
    G4int reflectedPhotons = 0; // Counter of photons reflected by the coating
    G4int tirPhotons = 0;       // Counter of photons undergoing Total Internal Reflection
};

#endif
