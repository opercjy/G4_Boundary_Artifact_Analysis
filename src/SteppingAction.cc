#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Positron.hh"
#include "G4VProcess.hh"

SteppingAction::SteppingAction() : G4UserSteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // Filter: Trace only Positrons (e+)
    if (step->GetTrack()->GetDefinition() != G4Positron::Positron()) return;

    auto analysisManager = G4AnalysisManager::Instance();
    G4StepPoint* postPoint = step->GetPostStepPoint();

    G4double zPost = postPoint->GetPosition().z();
    G4double stepLen = step->GetStepLength();

    // -------------------------------------------------------------------
    // [Data 1] Micro-step logging for V-Shape Truncation
    // Log all steps within +/- 50 um of the interface (Z=0)
    // -------------------------------------------------------------------
    if (std::abs(zPost) < 0.05 * mm) {
        analysisManager->FillNtupleDColumn(0, 0, zPost / um);
        analysisManager->FillNtupleDColumn(0, 1, stepLen / um);
        analysisManager->AddNtupleRow(0);
    }

    // -------------------------------------------------------------------
    // [Data 2] Annihilation Vertex logging for Dip/Peak
    // Check if the process is positron annihilation
    // -------------------------------------------------------------------
    const G4VProcess* process = postPoint->GetProcessDefinedStep();
    if (process && process->GetProcessName() == "annihil") {
        analysisManager->FillNtupleDColumn(1, 0, zPost / um);
        analysisManager->AddNtupleRow(1);
    }
}
