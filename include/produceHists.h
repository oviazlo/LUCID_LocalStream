#ifndef produceHists_H
#define produceHists_H

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "FadcEvent.h"
#include "Bi207Event.h"
#include "serviceFunctions.h"
#include "PulseAnalizer.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>

/// boost
#include "boost/program_options.hpp"
#include <boost/filesystem.hpp>


#define PEAK_POSITION_THRESHOLD 0.001
#define DERIVATIVE_THRESHOLD 0.15
#define LEFT_SAFETY_MARGIN 4
#define RIGHT_SAFETY_MARGIN 20
#define YSPREAD_FACTOR_THRESHOLD 2

using namespace std;

string dataFileFolder = "dataFiles";
string outHistFileFolder = "histFiles";
/// range of integration of baseline
double leftBaseLineLimit = 10;
double rightBaseLineLimit = 50;

///----------------------------------------------------------------------------
/// [PMT signal limitation]
/// limitation for signal amplitude: up to 100mV [case w/o amplifier]
/// typical amplitude/charge ratio: A/Q = 0.15..0.2
/// -->
/// limitation for charge: (100 + 20) / 0.15 = 800 -> 0.8 [V*2.5ns]
/// limitation for amplitude: 0.12 [V]
/// 
/// introduce new variable: amplificationFactor
///----------------------------------------------------------------------------


///************************* VARIABLES ****************************************

/// specify this value if data was taken with amplifier
double amplificationFactor = 1.0;

/// range of maxValHistRight and maxValHistLeft histos
double maxAmplRangeLeft;
double maxAmplRangeRight;

/// channel number to read
int iChannel;

/// range for integration peak
double chargeIntegrationRangeLeft;
double chargeIntegrationRangeRight;

/// signal pulse polarity
int polarity = -1; /// by default is negative (PMT signal)

/// globall variables
int nSamplesADC = 80; /// default number of output bins after signal digitization
Long64_t entries;
TBranch *testBranch;

/// "template" variables
FadcEvent *myEvent = NULL; /// associated to testBranch
Bi207Event *bi207Event = NULL; /// associated to testBranch (WARNING for data collected during "PMT equalization study")
vector<vector<double> > fadcVec;

///************************* NAMESPACES ***************************************

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
  const size_t HELP_CALLED = 3;
  
  namespace po = boost::program_options;
 
} /// namespace 

po::variables_map vm;

///************************* FUNCTIONS ****************************************

int produceHists(string fileName);
void adjustHist(TH1F* inHist);
bool checkJitterSilent(string fileName, int iChannel);
TH1F *GetLeftRightThreshold(TH1F* inHist);
int GetIndexOfSmallestElement(std::vector<double> inArr, bool positivePulse = false);
bool CheckForBadEvents(TH1F* peakPosHist);

#endif
