#include "RunAction.hh"
#include "G4AnalysisManager.hh"

RunAction::RunAction() : G4UserRunAction() {
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetNtupleMerging(true); // Enable merging for MT mode

    // Ntuple ID 0: StepData (For V-Shape Artifact check)
    analysisManager->CreateNtuple("StepData", "Micro-step Info near boundary");
    analysisManager->CreateNtupleDColumn("Z_um");      // Position Z (micrometer)
    analysisManager->CreateNtupleDColumn("StepLen_um"); // Step Length (micrometer)
    analysisManager->FinishNtuple(0);

    // Ntuple ID 1: VertexData (For Dip/Peak check)
    analysisManager->CreateNtuple("VertexData", "Annihilation Vertices");
    analysisManager->CreateNtupleDColumn("Z_Vtx_um");   // Vertex Position Z (micrometer)
    analysisManager->FinishNtuple(1);
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("Boundary_Result.root");
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}
