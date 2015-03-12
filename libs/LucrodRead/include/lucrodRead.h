#ifndef ROOT_LUCRODREAD
#define ROOT_LUCRODREAD

#include <vector>
#include <string>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "lucrodTree.h"

using namespace std;

class lucrodRead {	
	public:
	lucrodRead(string dataFile = "", string outputFile = "");
	virtual ~lucrodRead() {}
	void read(); //Read all events, store them in a TTree, and save them to file
	inline int ch2index(Short_t lucrodId, UInt_t channel); //Translate a channel in a given lucrod to a vector index in the event
	
	string getDataFile() { return dataFile_; }
	void setDataFile(string dataFile) { dataFile_ = dataFile; }
	string getOutputFile() { return outputFile_; }
	void setOutputFile(string outputFile) { outputFile_ = outputFile; }
	
	private:
	void assignBranches();
	bool readLucrodBcidData(ifstream& inFile); //Read a lucrod event that triggered on a specific BCID
	bool readLucrodData(ifstream& inFile); //Read a self-triggered lucrod event
	inline bool isPinDiode(UInt_t channel); //True if this channel corresponds to a PIN diode in the current event
	
	lucrodTree t; //Container class for event variables
	string dataFile_;
	string outputFile_;
	string treeName_;
	Long64_t nBytesRead_;
	UInt_t buffer_; //Data from the input stream is put here.
	TTree* tree_;
	vector<vector<int> > ch2index_; //Used by the ch2index function
	UInt_t modulesRead_; //Used to keep track of the number of modules read so far in a given event
	
	const UInt_t eventHeaderMarker_; //Signifies the start of an event
	const UInt_t rodHeaderMarker_; //Signifies the start of a ROD Header
	const UInt_t rodHeaderSize_;  //Number of dwords in the header, including the header marker
	const UInt_t nSamples_; //Number of samples in an FADC spectrum

	ClassDef(lucrodRead,1)
};


#endif
