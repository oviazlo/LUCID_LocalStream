#ifndef Bi207Event_H
#define Bi207Event_H

#include <vector>
#include <fstream>
#include <cmath>
#include <utility>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <iostream>

using namespace std;

static unsigned int const number_of_channels = 2;  
	
class Bi207Event : public TObject { 	
 public:
  Bi207Event();
  ~Bi207Event(){}	
  void Reset();   
  unsigned int index; 
  bool isCorrupt;
  unsigned int EventNumber;
  unsigned int FadcGain;
  unsigned int HVCh0;
  unsigned int HVCh1;
  vector < vector <double> > lucrod_data;   
  ClassDef(Bi207Event,1)
};

#endif


