#include "../include/lucrodTree.h"

ClassImp(lucrodTree)

lucrodTree::lucrodTree(TTree *t) : tree(0) {
	fadcData = new vector<vector<Float_t> >();
	if(t) init(t);
}

Int_t lucrodTree::getEntry(Long64_t entry) {
	if(!tree) return 0;
	return tree->GetEntry(entry);
}

void lucrodTree::init(TTree *t) {
	if(!t) return;
	tree = t;
	
	//isCorruptb = tree->GetBranch("isCorrupt"); //Try to get the branch from the tree
	//if(!isCorruptb) isCorruptb = tree->Branch("isCorrupt", &isCorrupt); //If it doesn't exist, create it.
	//else isCorruptb->SetAddress(&isCorrupt);
	
	lucrodIdb = tree->GetBranch("lucrodId");
	if(!lucrodIdb) lucrodIdb = tree->Branch("lucrodId", &lucrodId);
	else lucrodIdb->SetAddress(&lucrodId);
	
	trigBCIDb = tree->GetBranch("trigBCID");
	if(!trigBCIDb) trigBCIDb = tree->Branch("trigBCID", &trigBCID);
	else trigBCIDb->SetAddress(&trigBCID);
	
	fadcDatab = tree->GetBranch("fadcData");
	if(!fadcDatab) fadcDatab = tree->Branch("fadcData", &fadcData);
	else fadcDatab->SetAddress(&fadcData);
}

void lucrodTree::reset() {
	lucrodId = -1;
	trigBCID = -1;
	fadcData->clear();
}
