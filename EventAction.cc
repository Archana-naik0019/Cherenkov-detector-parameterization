#include "EventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"

EventAction::EventAction() {}
EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*) {}

void EventAction::EndOfEventAction(const G4Event*) {
    //G4AnalysisManager::Instance()->AddNtupleRow();
}
