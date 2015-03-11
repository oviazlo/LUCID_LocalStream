#include "../include/FadcEvent.h"

ClassImp(FadcEvent)

FadcEvent::FadcEvent()
  : index(-1), isCorrupt(false),EventNumber(0){
  for(unsigned int i = 0; i < number_of_fadc_channels; i++) { 
    fadc_data.push_back(vector<double>()); 
  }
}

void FadcEvent::Reset() {
  for(unsigned int i = 0; i < number_of_fadc_channels; i++) { 
    fadc_data[i].clear(); 
  }
  index = -1; 
  isCorrupt = false; 
}
