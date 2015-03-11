#ifndef PulseAnalizer_H
#define PulseAnalizer_H

#include <algorithm>
#include <vector>
#include <fstream>
#include <cmath>
#include <utility>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <iostream>

using namespace std;
	
class PulseAnalizer : public TObject { 	
 public:
   PulseAnalizer(vector <double>& fadc_data, int _nLeftBSBins = 15, int _nRightBSBins = 0);
   ~PulseAnalizer(){}
   
   vector<double> pulseArr;
   double amplitude = 0;
   
   /// baseline left/right 10/15/20 bins
   double bsL10 = 0;
   double bsL15 = 0;
   double bsL20 = 0;
   double bsR10 = 0;
   double bsR15 = 0;
   double bsR20 = 0;
   
   /// custom baseline
   double bsL = 0;
   double bsR = 0;
   
   /// number of bins for custom baseline
   int nLeftBSBins;
   int nRightBSBins;
   
   /// absolute max/min
   double absMax = -9999.9;
   double absMin = 9999.9;
   
   /// integral
   double integral = 0;
   
   bool Trigger(const double threshold);
   bool UpTrigger(const double threshold);
   
   ClassDef(PulseAnalizer,1)
};





#endif