
#include "../include/Bi207Event.h"
#include <string>
#include <sstream>
#include <iostream>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <algorithm>


ClassImp(Bi207Event)

Bi207Event::Bi207Event()
  : index(-1), isCorrupt(false), EventNumber(0), FadcGain(0){
  for(unsigned int i = 0; i < number_of_channels; i++) { 
    lucrod_data.push_back(vector<double>()); 
  }
}

void Bi207Event::Reset() {
  for(unsigned int i = 0; i < number_of_channels; i++) { 
    lucrod_data[i].clear(); 
  }
  index = -1; 
  isCorrupt = false; 
  FadcGain = 0;
  HVCh0 = 0;
  HVCh1 = 0;  
}


