#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    
    // Methods to update counters
    void IncrementReflectedPhotons();
    void IncrementTIRPhotons();
    void IncrementGeneratedPhotons();
    void IncrementDetectedPhotons();


private:

    G4int reflectedPhotons;
    G4int tirPhotons;
    G4int totalGeneratedPhotons;
    G4int totalDetectedPhotons;
    
    //G4int reflectedPhotons = 0;
    //G4int tirPhotons = 0;

//public:
  //  void IncrementReflectedPhotons() { reflectedPhotons++; }
  //  void IncrementTIRPhotons() { tirPhotons++; }



//Estimating amplification degree    
    //void IncrementGeneratedPhotons() { totalGeneratedPhotons++; }
    //void IncrementDetectedPhotons() { totalDetectedPhotons++;}

//private:
  //  G4int totalGeneratedPhotons = 0;
  //  G4int totalDetectedPhotons = 0;

    
};

#endif
