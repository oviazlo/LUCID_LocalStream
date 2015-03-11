#ifndef FADCREAD_H
#define FADCREAD_H

#include <vector>
#include <fstream>
#include <cmath>
#include <utility>
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "FadcEvent.h"

using namespace std;

static const char* branchBase = "run_";
static const char* treeName = "fadc_event";

//static unsigned int const event_header_marker = 0xaa1234aa; //Signifies the start of an event
static unsigned int const event_header_marker = 0x1234cccc; //Signifies the start of an event
static unsigned int const rod_header_marker = 0xee1234ee; //Signifies the start of a ROD Header
static unsigned int const rod_header_size = 9;  //Number of dwords in the header, including the header marker
static unsigned int const v1720_id = 0; 

class FadcRead {	
 private:	
  std::string dataFile; 
  std::string outputFile;
  unsigned int nBytesRead;
  FadcEvent *event; //Data from the event is stored here, and the entire event is then written to a TTree
  unsigned int buffer; //Data from the input stream is put here.
  bool read_fadc_data(ifstream& inFile); //Read a FADC event
 public:
  FadcRead(std::string dataFile = "", std::string outputFile = "");
  virtual ~FadcRead() { delete event; }
  void Read(); //Read all events, store them in a TTree, and save them to file
  std::string GetDataFile() { return dataFile; }
  void SetDataFile(std::string dataFile) { this->dataFile = dataFile; }
  std::string GetOutputFile() { return outputFile; }
  void SetOutputFile(std::string outputFile) { this->outputFile = outputFile; }
  
  ClassDef(FadcRead,1)
};


#endif
