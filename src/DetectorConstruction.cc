#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();
    
    // Material Definition
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic"); // Vacuum
    G4Material* denseMat = nist->FindOrBuildMaterial("G4_Pb");       // Lead (Dense)
    G4Material* sparseMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // Scint (Sparse)

    // 1. World Volume
    G4Box* solidWorld = new G4Box("World", 10*cm, 10*cm, 10*cm);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);

    // 2. Dense Volume (Upstream, Z < 0)
    // Center at Z = -1cm, Thickness = 2cm -> Ends at Z = 0
    G4Box* solidDense = new G4Box("DenseSlab", 5*cm, 5*cm, 1*cm);
    G4LogicalVolume* logicDense = new G4LogicalVolume(solidDense, denseMat, "LogicDense");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1*cm), logicDense, "PhysDense", logicWorld, false, 0);

    // 3. Sparse Volume (Downstream, Z > 0)
    // Center at Z = +1cm, Thickness = 2cm -> Starts at Z = 0
    G4Box* solidSparse = new G4Box("SparseSlab", 5*cm, 5*cm, 1*cm);
    G4LogicalVolume* logicSparse = new G4LogicalVolume(solidSparse, sparseMat, "LogicSparse");
    new G4PVPlacement(0, G4ThreeVector(0,0,1*cm), logicSparse, "PhysSparse", logicWorld, false, 0);

    return physWorld;
}
