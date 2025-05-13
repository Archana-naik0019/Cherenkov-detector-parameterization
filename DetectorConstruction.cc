#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Get materials
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* aerogelMat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* germaniumMat = nist->FindOrBuildMaterial("G4_Ge");

    // -------------------
    // World Volume
    // -------------------
    G4double worldSize = 2 * m;
    G4Box* worldSolid = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
    G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, worldMat, "WorldLV");
    G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "WorldPV", nullptr, false, 0, true);
    //worldLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.5, 0.5))); // Gray
    worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());


   //optical properties for Air
   G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
   const G4int numEntriesAir = 2;
   G4double photonEnergyAir[numEntriesAir] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; // Visible range
   G4double airRefractiveIndex[numEntriesAir] = {1.00, 1.00}; // Approximate refractive index of air

   airMPT->AddProperty("RINDEX", photonEnergyAir, airRefractiveIndex, numEntriesAir);
   worldMat->SetMaterialPropertiesTable(airMPT);



    // -------------------
    // Aerogel Detector (Cylinder)
    // -------------------
    G4double aerogelRadius = 10 * cm;
    G4double aerogelHeight = 100 * cm;
    G4Tubs* aerogelSolid = new G4Tubs("Aerogel", 0, aerogelRadius, aerogelHeight / 2, 0, 360 * deg);
    G4LogicalVolume* aerogelLV = new G4LogicalVolume(aerogelSolid, aerogelMat, "AerogelLV");
    
    G4VPhysicalVolume* aerogelPV = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), aerogelLV, "AerogelPV", worldLV, false, 0, true);

     // -------------------
     // Optical Properties for Cherenkov Effect
     // -------------------
    const G4int numEntries = 2;
    G4double photonEnergy[numEntries] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; // Visible range
    G4double refractiveIndex[numEntries] = {1.2, 1.2};
    
    //G4double absorption[numEntries] = {1.0 * m, 1.0 * m}; // Long absorption
    //G4double rayleigh[numEntries] = {10.0 * cm, 10.0 * cm}; // Scattering

    G4MaterialPropertiesTable* aerogelMPT = new G4MaterialPropertiesTable();
    aerogelMPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, numEntries);
    //aerogelMPT->AddProperty("ABSLENGTH", photonEnergy, absorption, numEntries);
    //aerogelMPT->AddProperty("RAYLEIGH", photonEnergy, rayleigh, numEntries);
    aerogelMat->SetMaterialPropertiesTable(aerogelMPT);

    // -------------------
    // Germanium Detector (Pixelated Surface)
    // -------------------
    G4double geThickness = 3 * mm;
    G4int numPixelsX = 100;
    G4int numPixelsY = 100;
    //G4double pixelSize = 10 * cm / numPixelsX;
    G4double pixelSize = (1.40 * aerogelRadius) / numPixelsX;

    G4Box* pixelSolid = new G4Box("Pixel", pixelSize / 2, pixelSize / 2, geThickness / 2);
    G4LogicalVolume* pixelLV = new G4LogicalVolume(pixelSolid, germaniumMat, "PixelLV");
    
    G4double gridHalfSize = (numPixelsX * pixelSize) / 2.0;
    
    const G4int numEntriesGe = 2;
    G4double photonEnergyGe[numEntriesGe] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double absorptionLength[numEntriesGe] = {10000 * nm, 2000 * nm};
    G4double refractiveIndexGe[numEntriesGe] = {4.0, 4.0};
    
    G4MaterialPropertiesTable* geMPT = new G4MaterialPropertiesTable();
    geMPT->AddProperty("ABSLENGTH", photonEnergyGe,absorptionLength, numEntriesGe);
    geMPT->AddProperty("RINDEX", photonEnergyGe,refractiveIndexGe, numEntriesGe);
    germaniumMat->SetMaterialPropertiesTable(geMPT);


    // Place 100 × 100 pixels
    for (G4int i = 0; i < numPixelsX; i+=2) {
        for (G4int j = 0; j < numPixelsY; j+=2) {

	    G4double xPos = -gridHalfSize + (i + 0.5) * pixelSize;
            G4double yPos = -gridHalfSize + (j + 0.5) * pixelSize;
            //G4double xPos = (-aerogelRadius + pixelSize / 2) + i * pixelSize;
            //G4double yPos = (-aerogelRadius + pixelSize / 2) + j * pixelSize;

            new G4PVPlacement(
                nullptr, G4ThreeVector(xPos, yPos, aerogelHeight / 2 + geThickness / 2), 
                pixelLV, "PixelPV", worldLV, false, i * numPixelsY + j, true
            );
        }
    }

    // -------------------
    // Optical Surface (Highly Reflective Coating)
    // -------------------
    G4OpticalSurface* opticalSurface = new G4OpticalSurface("AerogelReflector");
    opticalSurface->SetType(dielectric_metal);
    opticalSurface->SetFinish(polished);
    opticalSurface->SetModel(unified);

    // Define Reflectivity Properties
    const G4int num = 2;
    G4double ephoton[num] = {1.239841939 * eV / 0.9, 1.239841939 * eV / 0.2};
    G4double reflectivity[num] = {1.0, 1.0}; // Highly reflective coating
    
    //defining some additional properties
    //G4double specularLobe[num] = {1.0, 1.0}; // Perfect specular reflection
    //G4double specularSpike[num] = {0.0, 0.0}; // No random scattering
    //G4double backscatter[num] = {0.0, 0.0}; // No backward reflection

    G4MaterialPropertiesTable* optSurfaceTable = new G4MaterialPropertiesTable();
    optSurfaceTable->AddProperty("REFLECTIVITY", ephoton, reflectivity, num);
    
    //optSurfaceTable->AddProperty("SPECULARLOBECONSTANT", ephoton, specularLobe, num);
    //optSurfaceTable->AddProperty("SPECULARSPIKECONSTANT", ephoton, specularSpike, num);
    //optSurfaceTable->AddProperty("BACKSCATTERCONSTANT", ephoton, backscatter, num);
    
    opticalSurface->SetMaterialPropertiesTable(optSurfaceTable);

    new G4LogicalBorderSurface("AerogelReflector", aerogelPV, worldPV, opticalSurface);

    return worldPV;
}

void DetectorConstruction::ConstructSDandField() {
    SensitiveDetector* geSD = new SensitiveDetector("GermaniumSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(geSD);
    SetSensitiveDetector("PixelLV", geSD);

    // Debugging: Check if the sensitive detector is properly assigned
    //if (!G4SDManager::GetSDMpointer()->FindSensitiveDetector("GermaniumSD")) {
       // G4cerr << "ERROR: Sensitive Detector not properly assigned to PixelLV!" << G4endl;
    //} else {
      //  G4cout << "[DEBUG] Sensitive Detector successfully assigned to PixelLV." << G4endl;

    //}
}
