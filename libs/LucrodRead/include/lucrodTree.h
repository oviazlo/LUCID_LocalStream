#ifndef LUCRODTREE_H
#define LUCRODTREE_H

#include <TTree.h>
#include <vector>

using namespace std;

class lucrodTree {
	public:
	lucrodTree(TTree* t = 0);
	~lucrodTree() {};
	Int_t getEntry(Long64_t entry);
	void init(TTree* t);
	void reset();
	
	TTree* tree;
	
	//Bool_t isCorrupt;
	Short_t lucrodId;
	Short_t trigBCID;
	vector<vector<Float_t> >* fadcData; //This has to be a pointer for ROOT-related reasons

	//TBranch* isCorruptb;
	TBranch* lucrodIdb;
	TBranch* trigBCIDb;
	TBranch* fadcDatab;

	ClassDef(lucrodTree,1)
};

#endif
