#include "../include/FadcRead.h"
#include <string>
#include <sstream>
#include <iostream>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <algorithm>

ClassImp(FadcRead)

FadcRead::FadcRead(std::string dataFile, std::string outputFile) 
  : dataFile(dataFile), outputFile(outputFile), nBytesRead(0), event(new FadcEvent()) {
}
	
void FadcRead::Read() {
  if(dataFile.size()<2) {
    cout << "Error: Can't read event data without a data file. Use the SetDataFile function." << endl;
    return;
  }
  ifstream inFile(dataFile.c_str(), ios::binary); //Open data file
  if(!inFile) {
    cout << "Error: could not open input file. Aborting." << endl;
    return;
  }

  //The data files are named after a specific format: extract a tag and the run_number
  string::size_type last_slash = dataFile.rfind("/");
  string::size_type first_dot =  dataFile.find(".");
  string::size_type second_dot = dataFile.find(".", first_dot + 1);
  if(last_slash > first_dot){
    first_dot  =  dataFile.find(".",last_slash + 1);
    second_dot  = dataFile.find(".", first_dot + 1);
  }
  string run_number(dataFile.substr(first_dot + 1, second_dot - first_dot - 1)); //The last -1 is to shave off the dot

  string treeNameRoot;
  cout<<"last slash="<<last_slash<<"; 1st dot="<<first_dot<<"; 2nd dot="<<second_dot<<endl;
  cout<<"run number is "<<run_number<<endl;
  if(outputFile.size()<2) { //If there's no output, generate a default
    treeNameRoot ="run_";
    treeNameRoot.append(run_number);
    treeNameRoot.append(".root");
  }
  else treeNameRoot = outputFile;

  cout<<"root output filename "<<treeNameRoot <<endl;

  TFile FadcReadFile(treeNameRoot.c_str(), "UPDATE");
  string branchName(branchBase);
  branchName.append(run_number);

  TTree *tree = reinterpret_cast<TTree*>(FadcReadFile.Get(treeName)); //Try to get the tree from the file
  if(!tree) tree = new TTree(treeName, treeName); //If it doesn't exist, create it

  TBranch *branch = tree->GetBranch(branchName.c_str()); //Try to get the branch from the tree
  unsigned int events_already_read = 0;

  if(!branch) branch = tree->Branch(branchName.c_str(),"FadcEvent",&event); //If it doesn't exist, create it.
  else { //If it does exist, issue a warning as it might be the same data being processed twice
    cout << "Warning: a branch for this run already exists in this tree. This is OK if you have a data file that is split up into many parts, but if you are reading the same file twice you are only duplicating your data. Reading..." << endl;
    branch->SetAddress(&event);
    events_already_read = static_cast<unsigned int>(branch->GetEntries()); //Start at the correct index
  }

  //Begin reading input
  //First we simply read until the first event header is encountered
  unsigned int nevents = 0;
  while(inFile.read(reinterpret_cast<char*>(&buffer), 4)) {
    nBytesRead += 4;
    if(buffer == event_header_marker) break; 
  }
		
  //Every event may contain any number of ROD Headers in any order
  while(inFile.read(reinterpret_cast<char*>(&buffer), 4)) { 
    nBytesRead += 4;

    if(buffer == event_header_marker) { //If a new event header is encountered, we know that the current one is over
      ++nevents;
      //cout<<"nevents is "<<nevents<<"\n";
      event->index = events_already_read; //So assign the current event its index
      branch->Fill(); //And add it to the branch.
      event->Reset(); //Wipe old event data
      events_already_read++; //And increment event count
    }
    else if(buffer == rod_header_marker) { 
      //If instead a ROD Header is found, we read the data contained in the body
      //Skip the rest of the header, keeping in mind that the header marker has already been read
      inFile.ignore(4*(rod_header_size-1)); 
      nBytesRead += 4*(rod_header_size-1);

      unsigned int modules_already_read = 0;
      while(inFile.read(reinterpret_cast<char*>(&buffer), 4)) { 
        nBytesRead += 4;

        //Read the first dword, which is either a module ID or a status
	unsigned int marker = (buffer & 0x00ff0000) >> 16;
	//cout << hex << marker << endl;
	if(marker == 0x82) { //This tells us that it will be a module ID
	  unsigned int module_id = buffer & 0xf;
	  if(module_id == v1720_id) { 
	    if(!read_fadc_data(inFile)) {
	      event->isCorrupt = true;
              cout<<" Event "<< dec << nevents <<" is corrupted. Position in input file at byte "<<nBytesRead<<"\n";
	      break;
	    }
	    modules_already_read++;
	  }
	}
	else { //We're not reading a module ID, so the event is over. Do a consistency check for each module read.
	  for(unsigned int i = 0; i < modules_already_read; ++i) {
	    if(buffer != 0) {
	      event->isCorrupt = true;
	      cout << "Bad event status! Received " << buffer << " but expected 0. Flagging event as corrupt." << endl;
	    }
	    inFile.read(reinterpret_cast<char*>(&buffer), 4);
	    nBytesRead += 4;
	
	  }
	  if(buffer != modules_already_read) {
	    event->isCorrupt = true;
	    cout << "Bad event status! Read " << modules_already_read << " modules, but the RAW data indicated that there were actually " << buffer << ". Flagging event as corrupted." << endl;
	  }
	  break;
	}
      }
    }
  }

  //If we are here, it means the stream is no longer in a good state (probably because there is nothing left to read)
  event->index = events_already_read; //So assign the last event its index and add it to the branch
  branch->Fill();
  event->Reset(); //Reset the event, because the FadcREad object might be used again and we don't want to start with a full event
  inFile.close(); //Close the ifstream and write the TTree to file
  tree->Write("",TObject::kOverwrite);
  delete tree; //The tree will be deleted automatically once the TFile is closed. I do it here because I selectively don't trust ROOT. Ha!
  tree = 0;
  cout<<"The number of good events found in raw file is "<< dec << nevents <<endl;
  //The TFile is closed automatically
}


bool FadcRead::read_fadc_data(ifstream& inFile) {
  inFile.read(reinterpret_cast<char*>(&buffer), 4); //First FADC Header word must be here
  nBytesRead += 4;

  if(buffer >> 28 != 0xa) { //Flash ADC Header not found
    cout << "Error: encountered a corrupt Flash ADC event. " << hex << (buffer >> 28) << 
      " found where the header should be found" << endl;
    return false;
  }
  unsigned int number_of_dwords = buffer & 0x0fffffff;
	  
  inFile.read(reinterpret_cast<char*>(&buffer), 4);//Second Fadc header word
  nBytesRead += 4;

  bitset<number_of_fadc_channels> mask(static_cast<unsigned long>(buffer & 0xff)); //If a bit is set, the corresponding channel has sent data
	
  inFile.ignore(8); //ignore FADC header words 2, 3 (4+4 = 8 bytes) 
  nBytesRead += 8;


  for(unsigned int i = 0; i < number_of_fadc_channels; i++) { //For each channel in the Flash ADC
    std::vector<double> FADCdata;
    if(mask.test(i)) { //If that channel contains data
      for(unsigned int j = 0; j < (number_of_dwords-4)/mask.count(); j++) { //Read out the samples from the channel (we know how many there are because we know the total number, and each active channel has the same number of samples)
	inFile.read(reinterpret_cast<char*>(&buffer), 4);
        nBytesRead += 4;

        FADCdata.push_back((buffer & 0xfff)*0.000488281-1.);
        FADCdata.push_back((buffer >> 16 & 0xfff)*0.000488281-1.);
      }
      for(unsigned int j = 0; j <FADCdata.size(); ++j){
	event->fadc_data[i].push_back(FADCdata[j]);
      }
    }
  }
  return true;
}



	     
        
