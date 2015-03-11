///
/// usage:
/// .checkJitter.exe -f <file> [OPTIONAL] -c <channel>
/// channel:			integer
/// file:			file name (accept folders)
///

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TLine.h"
#include "FadcEvent.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <fstream>


/// boost
#include "boost/program_options.hpp"
#include <boost/filesystem.hpp>

#define PEAK_POSITION_THRESHOLD 0.001
#define DERIVATIVE_THRESHOLD 0.15
#define LEFT_SAFETY_MARGIN 4
#define RIGHT_SAFETY_MARGIN 11
#define YSPREAD_FACTOR_THRESHOLD 2
// #define 

using namespace std;


/// globall variables
int nSamplesADC;
Long64_t entries;
TBranch *testBranch;

/// "template" variables
FadcEvent *myEvent; /// associated to testBranch
vector<vector<double> > fadcVec;

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
  const size_t HELP_CALLED = 3;
  
  namespace po = boost::program_options;
 
} /// namespace 

int checkJitter(string fileName, int iChannel);
const int GetNumberOfADCSamples(int iChannel);
int ReadFileAndInitGlobalVars(string fileName, int iChannel);
int GetIndexOfSmallestElement(std::vector<double> inArr, bool positivePulse = false);
bool CheckForBadEvents(TH1F* peakPosHist);
TH1F *GetLeftRightThreshold(TH1F* inHist);

int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]){
  
  try 
  { 
    /** Define and parse the program options 
     */ 
    po::options_description desc("Options"); 
    desc.add_options()
      ("help,h", "Print help messages") 
      ("channel,c", po::value<int>(), "which channel to read - {>=0}")
      ("file,f", po::value<std::string>()->required(), "file name pattern, support wildcards"); 
    try 
    { 
      po::store(po::parse_command_line(argc, argv, desc),  
                vm); /// can throw 
 
      /** --help option 
       */ 
      if ( vm.count("help")  ) 
      { 
        std::cout << "Basic Command Line Parameter App" << std::endl 
                  << desc << std::endl; 
        return HELP_CALLED; 
      } 
      
      if ( vm.count("channel")  ) 
	if (vm["channel"].as<int>()<0){
	  std::cout << "Only positive channels are accepted!!!" << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	  
	}
	if ( !vm.count("file")  ){
	  std::cout << "Specify input file!" << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	}
 
      po::notify(vm); /// throws on error, so do after help in case 
                      /// there are any problems 
    } 
    catch(po::error& e) 
    { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cerr << desc << std::endl; 
      return ERROR_IN_COMMAND_LINE; 
    } 
 
    /// application code here // 
    return SUCCESS;
 
  } 
  catch(std::exception& e) 
  { 
    std::cerr << "Unhandled Exception reached the top of main: " 
              << e.what() << ", application will now exit" << std::endl; 
    return ERROR_UNHANDLED_EXCEPTION; 
 
  } 
  
}

int main(int argc, char* argv[]) {
  
  /// get global input arguments:
  po::variables_map vm; 
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc, argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  int iChannel;
  
  /// get channel to read
  if (vm.count("channel"))
    iChannel = vm["channel"].as<int>();
  else
    iChannel = 0;
   
  /// get file name choosen by user  
  string fileName;
  fileName = vm["file"].as<std::string>();

  if (!boost::filesystem::exists( (fileName).c_str() )){
    cout << "No file *" << fileName << " found" << endl;
    fileName = "";
    return -1;
  }

  cout << "proceed " << fileName << " file..." << endl;
  return checkJitter(fileName,iChannel);

}





int checkJitter(string fileName, int iChannel){
  
  /// read file and init global vars
  ReadFileAndInitGlobalVars(fileName,iChannel);
  
  /// check if something was read
  cout << "number of entries in file: " << entries << endl;
  if (entries<=0){cout << "no entries found!\nTEMINATE!!!\n"; return -1;}
  
  /// create min/max pulse shape arrays
  double minVal[const_cast<const int&>(nSamplesADC)], maxVal[const_cast<const int&>(nSamplesADC)];
  int nGoodPulses = 0;
  
  TCanvas *can = new TCanvas("can","can",800,800);
  can->Divide(1,4);
  can->cd(1);
  
  TH1F* peakPositionHist = new TH1F("peakPositionHist","peakPositionHist",nSamplesADC,0,nSamplesADC);
  
  /// loop over all entries
  for(Long64_t i = 0; i < entries; i++) {
    /// get entry + if it is corrupted -> take new one
    testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
    if (myEvent->isCorrupt == true) continue;
    nGoodPulses++;
	
    /// get some values
    fadcVec = myEvent->fadc_data;
    
    if (nGoodPulses==1){
      for (unsigned int k=0; k<nSamplesADC; k++){
	minVal[k] = fadcVec[iChannel][k];
	maxVal[k] = fadcVec[iChannel][k];
      }
    }
    else{
      for (unsigned int k=0; k<nSamplesADC; k++){
	double currentVal = fadcVec[iChannel][k];
	if (currentVal > maxVal[k])
	  maxVal[k] = currentVal;
	if (currentVal < minVal[k])
	  minVal[k] = currentVal;
      }
    }
    
    peakPositionHist->Fill( GetIndexOfSmallestElement(fadcVec[iChannel]) + 0.5 );

  }
  
  peakPositionHist->Scale(1.0/peakPositionHist->GetMaximum());
  
  if (CheckForBadEvents(peakPositionHist)==false){
    
    nGoodPulses = 0; /// reset counter
    
    /// loop over all entries
    for(Long64_t i = 0; i < entries; i++) {
      /// get entry + if it is corrupted -> take new one
      testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
      if (myEvent->isCorrupt == true) continue;
      nGoodPulses++;
	  
      /// get some values
      fadcVec = myEvent->fadc_data;
      
      double peakPos = GetIndexOfSmallestElement(fadcVec[iChannel]) + 0.5;
      if (peakPositionHist->GetBinContent(peakPos+1)<PEAK_POSITION_THRESHOLD){ 
// 	cout << "peakPos = " << peakPos << "; GetBinContent = " << peakPositionHist->GetBinContent(peakPos+1) << endl;
	continue;
      }
      
      if (nGoodPulses==1){
	for (unsigned int k=0; k<nSamplesADC; k++){
	  minVal[k] = fadcVec[iChannel][k];
	  maxVal[k] = fadcVec[iChannel][k];
	}
      }
      else{
	for (unsigned int k=0; k<nSamplesADC; k++){
	  double currentVal = fadcVec[iChannel][k];
	  if (currentVal > maxVal[k])
	    maxVal[k] = currentVal;
	  if (currentVal < minVal[k])
	    minVal[k] = currentVal;
	}
      }
      
    }
    
  }
  
  double pulseMiddle[const_cast<const int&>(nSamplesADC)];
  double pulseSpread[const_cast<const int&>(nSamplesADC)];
  double zeroArr[const_cast<const int&>(nSamplesADC)];
  
  for (unsigned int k=0; k<nSamplesADC; k++){
    pulseMiddle[k] = (maxVal[k] + minVal[k])/2.;
    pulseSpread[k] = maxVal[k] - pulseMiddle[k];
    zeroArr[k] = 0.0;
  }
  
//   can->cd(2);
  TH1F *jitterHist = new TH1F("jitterHist","jitterHist",nSamplesADC,0,nSamplesADC);
  TH1F *jitterHistYSpread = new TH1F("jitterHistYSpread","jitterHistYSpread",nSamplesADC,0,nSamplesADC);
  for (unsigned int k=0; k<nSamplesADC; k++){
    jitterHist->SetBinContent(k+1,pulseMiddle[k]);
    jitterHist->SetBinError(k+1,pulseSpread[k]);
    jitterHistYSpread->SetBinContent(k+1,pulseSpread[k]);
  }
  jitterHist->Draw();
  
  
  
  can->cd(2);
//   peakPositionHist->DrawCopy();
  
//   can->cd(4);
  peakPositionHist->DrawCopy();
//   TF1 *thresholdLine = new TF1("thresholdLine","[0]",0,80);
//   thresholdLine->SetParameter(0,PEAK_POSITION_THRESHOLD);
//   thresholdLine->SetTitle("");
//   thresholdLine->SetLineColor(kBlack);
//   thresholdLine->Draw("same");
    
  
  gPad->SetLogy();
  
  TLine *thresholdLine = new TLine(0,PEAK_POSITION_THRESHOLD,80,PEAK_POSITION_THRESHOLD);
  thresholdLine->Draw();
  
  can->cd(3);
  TH1F* derJitterHist = GetLeftRightThreshold(jitterHist);

  double minDer = derJitterHist->GetBinContent(1);
  double maxDer = derJitterHist->GetBinContent(1);
  
  for (unsigned int k=1; k<nSamplesADC-1; k++){
    double iDer = derJitterHist->GetBinContent(k+1);
    if (iDer > maxDer)
      maxDer = iDer;
    if (iDer < minDer)
      minDer = iDer;
  }
  
  derJitterHist->Scale(1.0/(maxDer-minDer));
  jitterHistYSpread->Scale(1.0/jitterHistYSpread->GetMaximum());
  
  int left = -1;
  int right = -1;
  
  for (int i=0;i<nSamplesADC-1;i++){
    double iDer = derJitterHist->GetBinContent(i+1);
    if (iDer<=(-DERIVATIVE_THRESHOLD)){
      if (i!=0)
	left = i;
      break;
    }
  }
  
  for (int i=nSamplesADC-2;i>=0;i--){
    double iDer = derJitterHist->GetBinContent(i+1);
    if (iDer>=DERIVATIVE_THRESHOLD){
      if (i!=nSamplesADC-2)
	right = i+1;
      break;
    }
  }
  
  cout << "right before using spread hist: " << right << endl;
  
  TF1* spreadLeftFitFunc = new TF1("spreadLeftFitFunc","[0]",0,left-LEFT_SAFETY_MARGIN);
  jitterHistYSpread->Fit("spreadLeftFitFunc","R");
  double fitLeftSpread = spreadLeftFitFunc->GetParameter(0);
//   cout << "fitLeftSpread = " << fitLeftSpread << endl;
  
  int counter = 0;
  int polSwap = 1;
  
  for (unsigned int k=0; k<nSamplesADC; k++){
    double iBinVal = jitterHistYSpread->GetBinContent(k+1);
//     cout << "iBinVal*polSwap = " << iBinVal*polSwap << "; polSwap*fitLeftSpread*YSPREAD_FACTOR_THRESHOLD = " << polSwap*fitLeftSpread*YSPREAD_FACTOR_THRESHOLD << endl;
    if (iBinVal*polSwap>polSwap*fitLeftSpread*YSPREAD_FACTOR_THRESHOLD){
      counter++;
      polSwap *= -1;
    }
    if (counter==2){
//       cout << "right = " << right << "; k = " << k << endl;
      if (right<(k))
	right = k;
      break;
    }
  }
  
  cout << "right after using spread hist: " << right << endl;
  
  cout << "left = " << left << "; right = " << right << endl;
  cout << "left = " << left-LEFT_SAFETY_MARGIN << "; right = " << right+RIGHT_SAFETY_MARGIN << endl;

  derJitterHist->Draw();
  TF1* zeroLine = new TF1("zeroLine","[0]",0,80);
  zeroLine->SetLineWidth(1);
  zeroLine->SetTitle("");
  zeroLine->SetParameter(0,0.0);
  zeroLine->Draw("same");
  
  TLine *thrLineTop = new TLine(0,DERIVATIVE_THRESHOLD,80,DERIVATIVE_THRESHOLD);
  thrLineTop->Draw("same");

  TLine *thrLineBottom = new TLine(0,-DERIVATIVE_THRESHOLD,80,-DERIVATIVE_THRESHOLD);
  thrLineBottom->Draw("same");  
  
  can->cd(4);
  jitterHistYSpread->Draw();
  
  TLine *leftLineOriginal = new TLine(left,-99999,left,99999);
  TLine *rightLineOriginal = new TLine(right,-99999,right,99999);
  
  TLine *leftLine = new TLine(left-LEFT_SAFETY_MARGIN,-99999,left-LEFT_SAFETY_MARGIN,99999);
  TLine *rightLine = new TLine(right+RIGHT_SAFETY_MARGIN,-99999,right+RIGHT_SAFETY_MARGIN,99999);
  leftLine->SetLineColor(kRed);
  rightLine->SetLineColor(kRed);
  
  can->cd(1);
  leftLineOriginal->Draw();
  rightLineOriginal->Draw();
  leftLine->Draw();
  rightLine->Draw();
  
  can->cd(2);
  leftLineOriginal->Draw();
  rightLineOriginal->Draw();
  leftLine->Draw();
  rightLine->Draw();
  
  can->cd(3);
  leftLineOriginal->Draw();
  rightLineOriginal->Draw();
  leftLine->Draw();
  rightLine->Draw();
  
  can->cd(4);
  leftLineOriginal->Draw();
  rightLineOriginal->Draw();
  leftLine->Draw();
  rightLine->Draw();
  
  TLine *thrSprLine = new TLine(0,fitLeftSpread*YSPREAD_FACTOR_THRESHOLD,80,fitLeftSpread*YSPREAD_FACTOR_THRESHOLD);
  thrSprLine->Draw("same");
  
  TLine *thrSprLine2 = new TLine(right,fitLeftSpread,80,fitLeftSpread);
  thrSprLine2->Draw("same");
  
  
  
  can->SaveAs("jitterCheck.cxx");
  
  return 0;
}

int ReadFileAndInitGlobalVars(string fileName, int iChannel){
 
  /// open the file
  TFile* f;
  if (GetSplittedWords(fileName,"/").size()>=2)
    f = TFile::Open(fileName.c_str());
  else
    f = TFile::Open((fileName).c_str());
  TTree* tree = reinterpret_cast<TTree*>(f->Get("fadc_event"));
  if (tree==NULL){cout << "no tree found!\nTEMINATE!!!\n"; return -1;}

  /// get branch
  TObjArray* allBranches = tree->GetListOfBranches();
  if (allBranches==NULL){cout << "no branches found!\nTEMINATE!!!\n"; return -1;}
  testBranch = reinterpret_cast<TBranch*>(allBranches->At(0));
  cout << "branch name = " << testBranch->GetName() << endl;

  /// create FadcEvent instance, set address to branch, get nEntries
  myEvent = 0;
  testBranch->SetAddress(&myEvent);
  entries = testBranch->GetEntries();
  
  nSamplesADC = GetNumberOfADCSamples(iChannel);
 
  return 0;
}

const int GetNumberOfADCSamples(int iChannel){
/// get number of samples (bins) which return ADC after digitization of signal
  
  for(Long64_t i = 0; i < entries; i++) {
    testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
    if (myEvent->isCorrupt == true) continue;
    
    fadcVec = myEvent->fadc_data;
    const int nSamples = fadcVec[iChannel].size();
    return nSamples;
  }
  
  return 0;
  
}


int GetIndexOfSmallestElement(std::vector<double> inArr, bool positivePulse)
{
    int index = 0;

    if (!positivePulse){
      for(int i = 1; i < inArr.size(); i++)
      {
	  if(inArr[i] < inArr[index])
	      index = i;              
      }
    }
    else{
      for(int i = 1; i < inArr.size(); i++)
      {
	  if(inArr[i] > inArr[index])
	      index = i;              
      }
    }
      
    return index;
}

bool CheckForBadEvents(TH1F* peakPosHist){ /// bad events - when peak position is far from 3 sigma from cente of peak pos. distribution 
  
  /// x-axis vals: 0..79; bins: 1-80    shift for 1
  for (int i=0; i<nSamplesADC; i++){
    double nBinVal = peakPosHist->GetBinContent(i+1);
    if (nBinVal <= PEAK_POSITION_THRESHOLD)
      return false;
  }
  
  return true;
  
}

TH1F *GetLeftRightThreshold(TH1F* inHist){
  
  TH1F* outHist = new TH1F("jitterHistDerivative","jitterHistDerivative",nSamplesADC,0,nSamplesADC);
  outHist->SetBinContent(1,0.0);
  
  for (int i=0; i<nSamplesADC; i++){
  
    outHist->SetBinContent(i+2, (inHist->GetBinContent(i+2) - inHist->GetBinContent(i+1)) );
    
  }
  
  return outHist;
  
}


