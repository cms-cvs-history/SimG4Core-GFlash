#include "SimG4Core/GFlash/interface/GFlash.h"
#include "SimG4Core/GFlash/interface/ParametrisedPhysics.h"
#include "SimG4Core/GFlash/interface/HadronPhysicsQGSP_WP.h"
#include "SimG4Core/GFlash/interface/HadronPhysicsQGSP_BERT_WP.h"
//#include "SimG4Core/PhysicsLists/interface/CMSEmStandardPhysics.h"
//#include "SimG4Core/PhysicsLists/interface/CMSEmStandardPhysics71.h"
#include "SimG4Core/PhysicsLists/interface/CMSEmStandardPhysicsLPM.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4QStoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh" 
#include "G4NeutronTrackingCut.hh"

#include "G4DataQuestionaire.hh"
#include "SimG4Core/GFlash/interface/GflashHistogram.h"

GFlash::GFlash(G4LogicalVolumeToDDLogicalPartMap& map, const edm::ParameterSet & p) :
  PhysicsList(map, p), thePar(p.getParameter<edm::ParameterSet>("GFlash")) {

  G4DataQuestionaire it(photon);

  int  ver     = p.getUntrackedParameter<int>("Verbosity",0);
  edm::LogInfo("PhysicsList") << "You are using the simulation engine: "
			      << "QGSP_BERT_EML 3.3 + CMS GFLASH\n";

  RegisterPhysics(new ParametrisedPhysics("parametrised",thePar)); 

  // EM Physics
  //  RegisterPhysics( new CMSEmStandardPhysics("standard EM",ver));
  //  RegisterPhysics( new CMSEmStandardPhysics71("standard EM v71",ver));
  RegisterPhysics( new CMSEmStandardPhysicsLPM("standard EM LPM",ver));

  // Synchroton Radiation & GN Physics
  RegisterPhysics(new G4EmExtraPhysics("extra EM"));

  // Decays
  RegisterPhysics(new G4DecayPhysics("decay",ver));

  // Hadron Elastic scattering
  RegisterPhysics(new G4HadronElasticPhysics("elastic",ver,false)); 

  // Hadron Physics
  G4bool quasiElastic=true;
  std::string hadronPhysics = thePar.getParameter<std::string>("GflashHadronPhysics");
  if(hadronPhysics=="QGSP_BERT") {
    RegisterPhysics(new HadronPhysicsQGSP_BERT_WP("hadron",quasiElastic));
  }
  else if (hadronPhysics=="QGSP") {
    RegisterPhysics(new HadronPhysicsQGSP_WP("hadron",quasiElastic));
  }
  else {
    edm::LogInfo("PhysicsList") << hadronPhysics << " is not available for GflashHadronPhysics!"
				<< "... Using QGSP_BERT\n";
    RegisterPhysics(new HadronPhysicsQGSP_BERT_WP("hadron",quasiElastic));
  }

  // Stopping Physics
  RegisterPhysics(new G4QStoppingPhysics("stopping"));

  // Ion Physics
  RegisterPhysics(new G4IonPhysics("ion"));

  // Neutron tracking cut
  RegisterPhysics( new G4NeutronTrackingCut("Neutron tracking cut", ver));


  // singleton histogram object
  theHisto = GflashHistogram::instance();
  if(thePar.getParameter<bool>("GflashHistogram")) {
    theHisto->setStoreFlag(true);
    theHisto->bookHistogram(thePar.getParameter<std::string>("GflashHistogramName"));
  }

}

GFlash::~GFlash() {
  if(theHisto) delete theHisto;
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
#include "SimG4Core/Physics/interface/PhysicsListFactory.h"

DEFINE_SEAL_MODULE();
DEFINE_PHYSICSLIST(GFlash);
