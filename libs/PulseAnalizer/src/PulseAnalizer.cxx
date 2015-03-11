#include "../include/PulseAnalizer.h"

ClassImp(PulseAnalizer)

PulseAnalizer::PulseAnalizer(vector <double>& fadc_data, int _nLeftBSBins, int _nRightBSBins){
  pulseArr = fadc_data;
  nLeftBSBins = _nLeftBSBins;
  nRightBSBins = _nRightBSBins;
  
  for (int i=0; i<80; i++){
    
    pulseArr[i] = 1000.0*pulseArr[i];/// convertion from V to mV
    
    /// fill in left baseline
    if (i<20){
      bsL20 += pulseArr[i]; 
      if (i<15){
	bsL15 += pulseArr[i]; 
	if (i<10){
	  bsL10 += pulseArr[i]; 
	}
      }
    }
    if (i<nLeftBSBins)
      bsL += pulseArr[i];
    
    /// fill in right baseline
    if (i>=80-20){
      bsR20 += pulseArr[i]; 
      if (i>=80-15){
	bsR15 += pulseArr[i]; 
	if (i>=80-10){
	  bsR10 += pulseArr[i]; 
	}
      }
    }
    if (i>=80-nRightBSBins)
      bsR += pulseArr[i];
    
    /// fill in integral and abs max/min
    integral += pulseArr[i];
    if (absMax<pulseArr[i])
      absMax = pulseArr[i];
    if (absMin>pulseArr[i])
      absMin = pulseArr[i];
    
  }
  
  /// get final baseline
  bsL20 = bsL20/20.0;
  bsL15 = bsL15/15.0;
  bsL10 = bsL10/10.0;
  bsL = (nLeftBSBins != 0) ? bsL/nLeftBSBins : 0.0;
  bsR20 = bsR20/20.0;
  bsR15 = bsR15/15.0;
  bsR10 = bsR10/10.0;
  bsR = (nRightBSBins != 0) ? bsR/nRightBSBins : 0.0;
  
  /// WARNING we choose bigger amplitude and don't care about polarity...
  amplitude = ( abs(absMax - bsL) > abs(absMin - bsL) ) ? abs(absMax - bsL) : abs(absMin - bsL);
}

bool PulseAnalizer::Trigger(const double threshold){
 
  if (amplitude > threshold) /// amplitude is in Volts, threshold - in mV
    return true;
  else
    return false;
  
}

bool PulseAnalizer::UpTrigger(const double threshold){
 
  if (amplitude < threshold) /// amplitude is in Volts, threshold - in mV
    return true;
  else
    return false;
  
}