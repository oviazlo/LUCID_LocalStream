#ifndef FADCEVENT_H
#define FADCEVENT_H

#include <vector>
#include <fstream>
#include <cmath>
#include <utility>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <iostream>

using namespace std;

static unsigned int const number_of_fadc_channels = 8;  
	
class FadcEvent : public TObject { 	
 public:
  FadcEvent();
  ~FadcEvent(){}	
  void Reset();   
  unsigned int index; 
  bool isCorrupt;
  unsigned int EventNumber;
  vector < vector <double> > fadc_data;   
  ClassDef(FadcEvent,1)
};

#endif
