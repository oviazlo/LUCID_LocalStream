#include "../include/lucrodRead.h"
#include <iostream>
#include <cstdlib>

using namespace std;

ClassImp(lucrodRead)

lucrodRead::lucrodRead(string dataFile, string outputFile) : dataFile_(dataFile), outputFile_(outputFile), treeName_("lucrodTree"),
nBytesRead_(0), tree_(0), ch2index_(4, vector<int>()), modulesRead_(0), eventHeaderMarker_(0x1234cccc), rodHeaderMarker_(0xee1234ee),
rodHeaderSize_(9), nSamples_(64) {
	//Map each lucrod channel to an index in the ntuple. The mapping should be such that less frequently used channels have higher index
	int ch2indexA1[16] = {0, -1, 4, 8, 1, -1, 5, 9, 2, -1, 6, 10, 3, -1, 7, 11};
	int ch2indexA2[16] = {0, 8, 1, 9, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1, 7, -1};
	int ch2indexC1[16] = {0, -1, 4, 8, 1, -1, 5, 9, 2, -1, 6, 10, 3, -1, 7, 11};
	int ch2indexC2[16] = {0, 8, 1, 9, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1, 7, -1};
	
	for(UInt_t i = 0; i < 16; ++i) { //Put the numbers in a vector
		ch2index_.at(0).push_back(ch2indexA1[i]);
		ch2index_.at(1).push_back(ch2indexA2[i]);
		ch2index_.at(2).push_back(ch2indexC1[i]);
		ch2index_.at(3).push_back(ch2indexC2[i]);
	}
}

void lucrodRead::read() {
	if(dataFile_.empty()) {
		cout << "lucrodRead::read: Can't read event data without a data file. Use the SetDataFile function." << endl;
		return;
	}
	ifstream inFile(dataFile_.c_str(), ios::binary); //Open data file
	if(!inFile) {
		cout << "lucrodRead::read: Could not open input file " << dataFile_ << ". Aborting." << endl;
		return;
	}
	
	string outputFile;
	if(outputFile_.empty()) { //If there's no output, generate a default
		string::size_type dot1 =	dataFile_.find("."); //Parse the file name to get the run number, description and type
		string::size_type dot2 = dataFile_.find(".", dot1 + 1);
		string::size_type underscore = dataFile_.find("_", dot2);
		string::size_type dot3 = dataFile_.find(".", underscore);
		string::size_type descriptionStart = dataFile_.find("LUCID-Side", dot3) + 11;
		string::size_type dot4 = dataFile_.find(".", descriptionStart);
		
		string runNumber(dataFile_.substr(dot1 + 1, dot2 - dot1 - 1)); //The last -1 is to shave off the dot itself
		string runDescription(dataFile_.substr(underscore + 1, dot3 - underscore - 1));
		string runType(dataFile_.substr(descriptionStart, dot4 - descriptionStart));
		
		if(runDescription.size()) runDescription = "_" + runDescription; //If a description is defined, add a leading underscore
		if(runType.size()) runType = "_" + runType.substr(1); //If a run type is defined, replace the leading '-' with an underscore
		outputFile = "run_" + runNumber + runDescription + runType + ".root"; 
	}
	else outputFile = outputFile_;
	cout << "Reading " << dataFile_ << " into " << outputFile << endl;

	TFile lucrodReadFile(outputFile.c_str(), "UPDATE");
	tree_ = dynamic_cast<TTree*>(lucrodReadFile.Get(treeName_.c_str())); //Try to get the tree from the file
	if(!tree_) tree_ = new TTree(treeName_.c_str(), treeName_.c_str()); //If it doesn't exist, create it
	t.init(tree_);
	t.reset();
	
	//Begin reading input
	nBytesRead_ = 0;
	while(inFile.read(reinterpret_cast<char*>(&buffer_), 4)) { //First we simply read until the first event header is encountered
		nBytesRead_ += 4;
		if(buffer_ == eventHeaderMarker_) break;
	}
	
	while(inFile.read(reinterpret_cast<char*>(&buffer_), 4)) {
		nBytesRead_ += 4;

		if(buffer_ == rodHeaderMarker_) { //There should be exactly one ROD header per event
			inFile.ignore(4*(rodHeaderSize_ - 1)); //Skip the rest of the header
			nBytesRead_ += 4*(rodHeaderSize_ - 1);
			modulesRead_ = 0;
			
			while(inFile.read(reinterpret_cast<char*>(&buffer_), 4)) { //This could be either a new module to read out or a trailer
				nBytesRead_ += 4;
				if(((buffer_ & 0x00ff0000) >> 16) == 0x82) { //If it's a new module, the format is 0x0082XXXX where XXXX is the module ID
					t.reset(); //Wipe old event data
					t.lucrodId = (buffer_ & 0x0000ffff) - 100; //A1, A2, C1, C2 are 100, 102, 101, 103
					
					inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Data channel header
					nBytesRead_ += 4;
					
					if(buffer_ == 0xabcd || buffer_ == 0xabcf) { //The data is self-triggered
						if(!readLucrodData(inFile)) break; //If the event was corrput, don't save it and move directly to the next one
					}
					if(buffer_ == 0xabce) { //The data triggered on a specific BCID
						if(!readLucrodBcidData(inFile)) break;
					}
					inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Lucrod trailer, 0xff000000
					nBytesRead_ += 4;
					if(((buffer_ & 0xff000000) >> 24) != 0xff) {
						cout << "lucrodRead::read: Found corrupt event ending at byte " << nBytesRead_ << endl;
						break;
					}
					++modulesRead_;
					tree_->Fill(); //Everything in this event has been read, so fill the tree
				}
				else { //We're reading a trailer. It has one field per lucrod that is zero if the data was OK. Then it has one final field that contains the number of lucrods
					for(UInt_t i = 0; i < modulesRead_; ++i) { //Read the trailer, keeping in mind that the first field has aleady been read
						if(buffer_ != 0) cout << "lucrodRead::read: Bad event trailer at byte " << nBytesRead_ <<". Received " << buffer_ << " but expected 0. This should never happen as the problem should already have been detected by a missing Lucrod trailer. Investigate!" << endl;
						inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Nonzero if data was missing
						nBytesRead_ += 4;
					}
					if(buffer_ != modulesRead_) cout << "lucrodRead::read: Bad event trailer at byte " << nBytesRead_ <<". Read out " << modulesRead_ << " modules, but the RAW data indicated that there were actually " << buffer_ << ". This should never happen as the problem should already have been detected by a missing Lucrod trailer. Investigate!" << endl;
					break; //We've reached the end of the event, so move to the next one
				}
			}
		}
	}

	//If we are here, it means the stream is no longer in a good state (probably because there is nothing left to read)
	inFile.close(); //Close the ifstream and write the TTree to file
	tree_->Write("", TObject::kOverwrite);
	delete tree_;
	tree_ = 0;
	lucrodReadFile.Close();
	cout << "Read " << nBytesRead_ << " bytes" << endl;
}


bool lucrodRead::readLucrodData(ifstream& inFile) {
	inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Number of enabled channels
	nBytesRead_ += 4;
	UInt_t nCh = buffer_;
	
	for(UInt_t i = 0; i < nCh; ++i) { //There are 65 words per channel. The first is the channel index (from 0 to 15)
		inFile.read(reinterpret_cast<char*>(&buffer_), 4); //0xaa000000 & channel
		nBytesRead_ += 4;
		if((buffer_ >> 24) != 0xaa){
			cout << "lucrodRead::readLucrodData: Bad channel at byte " << nBytesRead_ << ". Found 0x" << buffer_ << " when expecting 0xaa.." << endl;
			return false;
		}
		int index = ch2index(t.lucrodId, buffer_ & 0x00ffffff); //Map the lucrodId and channel to an index in a vector
		if(index < 0) { //Skip this channel
			inFile.ignore(4*nSamples_);
			nBytesRead_ += 4*nSamples_;
		}
		else { //Read all the samples into the appropriate vector
			while(index >= static_cast<int>(t.fadcData->size())) t.fadcData->push_back(vector<Float_t>()); //Pad with empty vectors if the containers aren't yet big enough
			
			int sign = 1; //Pulses from PMTs grow in the positive direction
			if(isPinDiode(buffer_ & 0x00ffffff)) sign = -1; //Pulses from PIN diodes grow in the negative direction. Flip them for consistency.
			for(UInt_t j = 0; j < nSamples_; j++) {
				inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Sample
				nBytesRead_ += 4;
				t.fadcData->at(index).push_back((buffer_&0xfff)*0.3663*sign);
			}
		}
	}
	return true;
}

bool lucrodRead::readLucrodBcidData(ifstream& inFile) {
	inFile.read(reinterpret_cast<char*>(&buffer_), 4); //Triggering BCID
	nBytesRead_ += 4;
	t.trigBCID = buffer_;
	return readLucrodData(inFile);
}

/*
Encapsulating this mapping in a function gives us some flexibility in
case we ever decide to change it in the future. For now, just return the
index as given by the vector.
*/
inline int lucrodRead::ch2index(Short_t lucrodId, UInt_t channel) {
	return ch2index_.at(lucrodId).at(channel);
}

inline bool lucrodRead::isPinDiode(UInt_t channel) {
	return ((t.lucrodId == 1 || t.lucrodId == 3) && (channel == 1 || channel == 3));
}
