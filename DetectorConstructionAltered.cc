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
#include "G4LogicalSkinSurface.hh"



    //defining pressurized nitrogen gas **********************************************
    
    G4Material* CreatePressurizedNitrogenWithOpticalProperties(G4double pressure) {
    auto* nist = G4NistManager::Instance();

    // Base values at STP
    G4double baseRI = 1.000298;      // Refractive index at 1 atm
    G4double baseAbs = 100 * m;      // Absorption length at 1 atm (adjustable)

    // Dynamic scaling
    G4double rIndex = 1.0 + (baseRI - 1.0) * (pressure / atmosphere);
    G4double absLength = baseAbs * (atmosphere / pressure);

    // Photon energy range (visible + UV)
    const G4int nEntries = 2;
    G4double photonEnergy[nEntries] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double rIndexArray[nEntries];
    G4double absLengthArray[nEntries];

    for (int i = 0; i < nEntries; ++i) {
        rIndexArray[i] = rIndex;
        absLengthArray[i] = absLength;
    }

    // molecular nitrogen gas (G4_lN2 = N2)
    G4Material* nitrogen = nist->FindOrBuildMaterial("G4_lN2");
    
    if (!nitrogen) {
    G4Exception("CreatePressurizedNitrogenWithOpticalProperties", "MyCode001", FatalException, "Failed to find G4_lN2 material.");
    return nullptr;
}

    // clone to preserve original
    G4Material* pressurizedNitrogen = new G4Material("PressurizedN2", 
        pressure * nitrogen->GetDensity() / atmosphere,
        nitrogen,
        kStateGas, 
        293.15*kelvin, pressure);

    // Add optical properties
    auto* mpt = new G4MaterialPropertiesTable();
    mpt->AddProperty("RINDEX", photonEnergy, rIndexArray, nEntries);
    mpt->AddProperty("ABSLENGTH", photonEnergy, absLengthArray, nEntries);
    pressurizedNitrogen->SetMaterialPropertiesTable(mpt);

    return pressurizedNitrogen;
}


DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}


    G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Get materials
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    //G4Material* aerogelMat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
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
   G4double photonEnergyAir[numEntriesAir] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; // Visible(+some UV) range
   G4double airRefractiveIndex[numEntriesAir] = {1.00, 1.00}; // Approximate refractive index of air

   airMPT->AddProperty("RINDEX", photonEnergyAir, airRefractiveIndex, numEntriesAir);
   worldMat->SetMaterialPropertiesTable(airMPT);


//############################################################################//



    // -------------------
    // Aerogel Detector (Cylinder)
    // -------------------
///    G4double aerogelRadius = 10 * cm;
///    G4double aerogelHeight = 100 * cm;
///    G4Tubs* aerogelSolid = new G4Tubs("Aerogel", 0, aerogelRadius, aerogelHeight / 2, 0, 360 * deg);
///    G4LogicalVolume* aerogelLV = new G4LogicalVolume(aerogelSolid, aerogelMat, "AerogelLV");
    
///    G4VPhysicalVolume* aerogelPV = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), aerogelLV, "AerogelPV", worldLV, false, 0, true);



     // -------------------
     // Optical Properties for Cherenkov Effect
     // -------------------
///    const G4int numEntries = 2;
///    G4double photonEnergy[numEntries] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; // Visible range
///    G4double refractiveIndex[numEntries] = {1.15, 1.2};
    
    //G4double absorption[numEntries] = {1.0 * m, 1.0 * m}; // Long absorption
    //G4double rayleigh[numEntries] = {10.0 * cm, 10.0 * cm}; // Scattering

///    G4MaterialPropertiesTable* aerogelMPT = new G4MaterialPropertiesTable();
///    aerogelMPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, numEntries);
    //aerogelMPT->AddProperty("ABSLENGTH", photonEnergy, absorption, numEntries);
    //aerogelMPT->AddProperty("RAYLEIGH", photonEnergy, rayleigh, numEntries);
///    aerogelMat->SetMaterialPropertiesTable(aerogelMPT);
    
// ##################################################################################//

    
    // -------------------
    // Barrel Gas Volume (Pressurized Nitrogen)
    // -------------------
    G4double gasRadius = 3.5 * cm;
    G4double gasHeight = 60 * cm;
    G4Material* nitrogenGas = CreatePressurizedNitrogenWithOpticalProperties(4.0 * atmosphere); // Set desired pressure

    auto* gasSolid = new G4Tubs("Gas", 0, gasRadius, gasHeight / 2, 0, 360 * deg);
    auto* gasLV = new G4LogicalVolume(gasSolid, nitrogenGas, "GasLV");
    auto* gasPV = new G4PVPlacement(nullptr, {}, gasLV, "GasPV", worldLV, false, 0, true);
    
    
    // -------------------
    // Quartz Window in front of detector
    // -------------------
    G4Material* quartzMat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

    // Define optical properties
    const G4int quartzNum = 2;
    G4double quartzE[quartzNum] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double quartzRIndex[quartzNum] = {1.46, 1.46}; // typical for fused silica
    G4double quartzAbsLength[quartzNum] = {10*m, 10*m}; // nearly transparent

    G4MaterialPropertiesTable* quartzMPT = new G4MaterialPropertiesTable();
    quartzMPT->AddProperty("RINDEX", quartzE, quartzRIndex, quartzNum);
    quartzMPT->AddProperty("ABSLENGTH", quartzE, quartzAbsLength, quartzNum);
    quartzMat->SetMaterialPropertiesTable(quartzMPT);
    
    //Quartz Placement
    
    G4double quartzThickness = 0.66 * cm;
    G4Tubs* quartzSolid = new G4Tubs("QuartzWindow", 0, gasRadius, quartzThickness / 2, 0, 360 * deg);
    G4LogicalVolume* quartzLV = new G4LogicalVolume(quartzSolid, quartzMat, "QuartzLV");

    // Placing quartz at the end of gas, just before the Ge pixels
    G4ThreeVector quartzPos = G4ThreeVector(0, 0, gasHeight / 2 - quartzThickness / 2);
    G4VPhysicalVolume* quartzPV = new G4PVPlacement(
    nullptr, quartzPos, quartzLV, "QuartzPV", worldLV, false, 0, true
    );



    // -------------------
    // Germanium Detector (Pixelated Surface)
    // -------------------
    G4double geThickness = 3 * mm;
    G4int numPixelsX = 100;
    G4int numPixelsY = 100;
    //G4double pixelSize = 10 * cm / numPixelsX;
    G4double pixelSize = (1.40 * gasRadius) / numPixelsX; //previously aerogelRadius

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
                    
            // Skip pixels outside the barrel radius
            if (std::sqrt(xPos*xPos + yPos*yPos) > gasRadius) continue;


            new G4PVPlacement(
                nullptr, G4ThreeVector(xPos, yPos, gasHeight / 2 + geThickness / 2), //moving pixels inside barrel, previously aerogelHeight
                pixelLV, "PixelPV", worldLV, false, i * numPixelsY + j, true
            );
        }
    }
    
    
//****************************************************************************************
    // -------------------
    // Optical Surface (Highly Reflective Coating)
    // -------------------
///    G4OpticalSurface* opticalSurface = new G4OpticalSurface("AerogelReflector");
///    opticalSurface->SetType(dielectric_metal);
///    opticalSurface->SetFinish(polished);
///    opticalSurface->SetModel(unified);

    // Define Reflectivity Properties
///    const G4int num = 2;
///    G4double ephoton[num] = {1.239841939 * eV / 0.9, 1.239841939 * eV / 0.2};
///    G4double reflectivity[num] = {0.5, 0.5}; // Highly reflective coating
    
    //defining some additional properties
    //G4double specularLobe[num] = {1.0, 1.0}; // Perfect specular reflection
    //G4double specularSpike[num] = {0.0, 0.0}; // No random scattering
    //G4double backscatter[num] = {0.0, 0.0}; // No backward reflection

///    G4MaterialPropertiesTable* optSurfaceTable = new G4MaterialPropertiesTable();
///    optSurfaceTable->AddProperty("REFLECTIVITY", ephoton, reflectivity, num);
    
    //optSurfaceTable->AddProperty("SPECULARLOBECONSTANT", ephoton, specularLobe, num);
    //optSurfaceTable->AddProperty("SPECULARSPIKECONSTANT", ephoton, specularSpike, num);
    //optSurfaceTable->AddProperty("BACKSCATTERCONSTANT", ephoton, backscatter, num);
    
///    opticalSurface->SetMaterialPropertiesTable(optSurfaceTable);

///    new G4LogicalBorderSurface("AerogelReflector", aerogelPV, worldPV, opticalSurface);

///    return worldPV;
///}

//**************************************************************************************


    // -------------------
    // Reflective Inner Barrel Surface
    // -------------------
///    G4OpticalSurface* opticalSurface = new G4OpticalSurface("BarrelReflector");
///    opticalSurface->SetType(dielectric_metal);
///    opticalSurface->SetFinish(polished);
///    opticalSurface->SetModel(unified);

///    G4double ephoton[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
///    G4double reflectivity[2] = {0.5, 0.5};

///    G4MaterialPropertiesTable* optSurfaceTable = new G4MaterialPropertiesTable();
///    optSurfaceTable->AddProperty("REFLECTIVITY", ephoton, reflectivity, 2);
///    opticalSurface->SetMaterialPropertiesTable(optSurfaceTable);

    // check if PVs are non-null before creating border
///    if (!gasLV) {
///    	G4cerr << "[ERROR] gasLV is null. Skin not created." << G4endl;
///    } else {
///    	new G4LogicalSkinSurface("BarrelReflectorSkin", gasLV, opticalSurface); //BorderSurface and 3rd argument worldPV for inc end caps as well
///    }

//********************(the above block is commented out to exclude the end with Ge pixels from being reflective)


	// -------------------
	// Reflective Shell Volume (side walls only)
	// -------------------
	G4double coatingThickness = 1.0 * mm;

	auto* coatingSolid = new G4Tubs("ReflectiveShell", gasRadius, gasRadius + coatingThickness, gasHeight / 2, 0, 360 * deg);
	auto* coatingLV = new G4LogicalVolume(coatingSolid, worldMat, "ReflectiveShellLV");

	auto* coatingPV = new G4PVPlacement(
	    nullptr,
	    G4ThreeVector(),  // same center as gas
	    coatingLV,
	    "ReflectiveShellPV",
	    worldLV,
	    false,
	    0,
	    true
	);

	// Optional visualization
	// coatingLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.3, 0.3))); // grey

	// Reflective Optical Surface only for cylindrical side wall
	G4OpticalSurface* opticalSurface = new G4OpticalSurface("BarrelReflector");
	opticalSurface->SetType(dielectric_metal);
	opticalSurface->SetFinish(polished);
	opticalSurface->SetModel(unified);

	G4double ephoton[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
	G4double reflectivity[2] = {0.85, 0.85};

	G4MaterialPropertiesTable* optSurfaceTable = new G4MaterialPropertiesTable();
	optSurfaceTable->AddProperty("REFLECTIVITY", ephoton, reflectivity, 2);
	opticalSurface->SetMaterialPropertiesTable(optSurfaceTable);

	new G4LogicalSkinSurface("SideWallReflectorSkin", coatingLV, opticalSurface);


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
