#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>
#include <TLine.h>

/// BOOST libs:
#include <boost/filesystem.hpp>
#include "boost/program_options.hpp"


///************************* NAMESPACES ***************************************

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
  const size_t HELP_CALLED = 3;
  
  namespace po = boost::program_options;
 
} /// namespace 

using namespace std;

///************************* GLOBAL VARIABLES *********************************

po::variables_map vm;

string inHistFileFolder = "histFiles";
string outPngFolder = "histPicts/picts";
string outTxtFolder = "histPicts";
int rangeBinSize = 10; /// size in bins of fiting plateu to find out where to cut distribution in order to calculate truncated mean
int peakFindingRegion = 10; /// first we need to find two peak. we will make scan fitting pol0. We will draw distbiution of fitted value.
			    /// Then we will analize this distribution. When we find a maximum, we need to check is it really a LOCAL maximum.
			    /// For this puropse we chaeck "peakFindingRegion" values before the peak and after. If all of these values are 
			    /// smaller then value of peak - then we clasify this peak as real LOCAL maximum.

int rebinFactor = 1;
			    
int stopScan = 4000; /// arbitarary value which we use as stop for our scan. Scan starts from 0 to this value.
   
bool saveAmplitudeInfo = false; /// TODO to implement this flag

double binSize; /// size of bin in input hist
boost::filesystem::path* fullPath; /// input file

///************************* FUNCTIONS ****************************************
int fitHists(string fileName);
pair<int,int> GetTwoLocalMaxPeaks(TH1F* inHist);
int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]);

///************************* MAIN *********************************************
int main(int argc, char* argv[]){

  parseOptionsWithBoost(vm,argc, argv);
  
  string prefix = "fit_info";
  
  /// get file name choosen by user  
  string fileName;
  
  string inputArgument = vm["inputFile"].as<string>();

  cout << "input file = " << inputArgument << endl;
  
  fullPath = new boost::filesystem::path(inputArgument);  
  
  if (fullPath->parent_path().string()!=""){
    fileName = fullPath->filename().string();
    cout << "fileName = " << fileName << endl;
  }
  if (!boost::filesystem::exists( fullPath->parent_path().string() + "/" + fileName )){
    cout << "No file *" << fileName << "* in *" << fullPath->parent_path().string() << "* folder" << endl;
    fileName = "";
    return -1;
  }

  cout << "proceed " << fileName << " file..." << endl;
  return fitHists(fileName);
  
}
///************************* END OF MAIN **************************************

///************************* FUNCTION IMPLEMENTATIONS *************************
int fitHists(string fileName){
  
  cout << endl << endl;
  
  /// get job name based on input file
  string jobName = GetSplittedWords(fileName,".").at(0);

  /// open the file
  TFile* inFile;
  if (GetSplittedWords(fileName,"/").size()>=2)
    inFile = TFile::Open(fileName.c_str());
  else
    inFile = TFile::Open((inHistFileFolder + "/" + fileName).c_str());
  
  string histNameToUse = "integralHistRight";
  if (vm.count("useAmplitudeDistribution")){
    histNameToUse = "maxValHistRight";
  }
  
  cout << "[fitHists]\tused hist = " << histNameToUse << endl;
  
  
  TH1F* integralHistBoth_original = (TH1F*)inFile->Get(histNameToUse.c_str());
  if (integralHistBoth_original==NULL){cout << "can't find " + histNameToUse + " hist!\nTERMINATE!!!\n"; return -1;}
  integralHistBoth_original->SetTitle("charge integral;charge, [mV * ns]; counts");
  
  TH1F *integralHistBoth = (TH1F *)integralHistBoth_original->Rebin(rebinFactor,(histNameToUse + "_rebinned").c_str());
  
  binSize = integralHistBoth->GetBinWidth(2);
  double plateuFitRangeSize = binSize*rangeBinSize;
  int nFitIteration = int(stopScan / binSize);
  
  TH1F *scanHist = new TH1F("scanHist","scanHist",nFitIteration,0,nFitIteration);
  
  double leftPeak = -666;
  double rightPeak = -666;
  if ( vm.count("fixedCut")  ){
    leftPeak = vm["fixedCut"].as<double>();
    cout << "fixed cut at: " << leftPeak << endl;
  }
  else{
    
    for (int i=0; i<nFitIteration; i++){
      TF1* linFunc = new TF1("linFunc","pol0",i*binSize,i*binSize+plateuFitRangeSize);
      integralHistBoth->Fit("linFunc","Rq");
      scanHist->SetBinContent(i+1,linFunc->GetParameter(0));
    }
    
    pair<int,int> tempPair = GetTwoLocalMaxPeaks(scanHist);
    leftPeak = ((tempPair.first-1)*binSize+(plateuFitRangeSize/2.0));
    rightPeak = ((tempPair.second-1)*binSize+(plateuFitRangeSize/2.0));
  }
  
  cout << "leftPeak = " << leftPeak << "; rightPeak = " << rightPeak << endl;

  TCanvas* can = new TCanvas("can","can",50,50,1000,1000);
  can->Divide(1,1);
  
  can->cd(1);
    integralHistBoth->GetXaxis()->SetRangeUser(0,stopScan);
    integralHistBoth->DrawCopy();
    
    TLine *leftPeakLine = new TLine(leftPeak,99999,leftPeak,-99999);
    leftPeakLine->SetLineColor(kRed);
    leftPeakLine->SetLineWidth(2);
    leftPeakLine->Draw();   
    if ( !vm.count("fixedCut")  ){
      TLine *rightPeakLine = new TLine(rightPeak,99999,rightPeak,-99999);
      rightPeakLine->SetLineColor(kRed);
      rightPeakLine->SetLineWidth(2);
      rightPeakLine->Draw();   
    }
  
  can->cd(2);
//     scanHist->Draw();
  
  /// count truncated mean
  int nomBin_valeyCut = 0;
  int nomBin_peakCut = 0;
  
  nomBin_valeyCut = integralHistBoth->FindBin(leftPeak);
  if ( !vm.count("fixedCut")  ){
    nomBin_peakCut = integralHistBoth->FindBin(rightPeak);
  }
  
  double meanVal[3];
  double meanErrVal[3];
  
  for (int i=1; i<(nomBin_valeyCut-1); i++){
    integralHistBoth->SetBinContent(i,0.0);
  }
  meanVal[0] = integralHistBoth->GetMean();
  meanErrVal[0] = integralHistBoth->GetMeanError();
  
  integralHistBoth->SetBinContent(nomBin_valeyCut-1,0.0);
  meanVal[1] = integralHistBoth->GetMean();
  meanErrVal[1] = integralHistBoth->GetMeanError();
  
  integralHistBoth->SetBinContent(nomBin_valeyCut,0.0);
  meanVal[2] = integralHistBoth->GetMean();
  meanErrVal[2] = integralHistBoth->GetMeanError();  
  
  double systErr = (abs(meanVal[0] - meanVal[1]) > abs(meanVal[2] - meanVal[1])) ? abs(meanVal[0] - meanVal[1]) : abs(meanVal[2] - meanVal[1]);
  
  cout << "left: " << meanVal[0] << " +- " << meanErrVal[0] << endl;
  cout << "center: " << meanVal[1] << " +- " << meanErrVal[1] << endl;
  cout << "right: " << meanVal[2] << " +- " << meanErrVal[2] << endl;
  
  can->cd(3);
    integralHistBoth->GetXaxis()->SetRangeUser(0,2*stopScan);
//     integralHistBoth->DrawCopy();
  
  boost::filesystem::create_directories("truncatedMeanTest");
  can->SaveAs(("truncatedMeanTest/" + fullPath->stem().string() + ".png").c_str());
  
  cout << "binSize = " <<  binSize << "; nFitIteration = " << nFitIteration << "; plateuFitRangeSize = " << plateuFitRangeSize << endl;
  
  vector<string> words = GetSplittedWords(jobName,"_");
  string date;
  
  if (words[words.size()-1][0] == 't')
    date = words[words.size()-3] + "_" + words[words.size()-2];
  else
    date = words[words.size()-2] + "_" + words[words.size()-1];
  
  string outFileName = "bismuthTestFit_truncatedMean";
  if (vm.count("useAmplitudeDistribution"))
    outFileName += "_amp";
  else
    outFileName += "_charge";
  if ( vm.count("fixedCut")  ){
    outFileName += "_c" + DoubToStr(leftPeak);
  }
  outFileName += ".txt";
  
  fstream fitInfoIntegral (outFileName.c_str(),ios::out | ios::app); /// create file
  fitInfoIntegral << date << "\t" << meanVal[1] << "\t" << meanErrVal[1] << "\t" << systErr << "\n";
  
  return 0;
}


pair<int,int> GetTwoLocalMaxPeaks(TH1F* inHist){
  
  int peakPos[2];
  peakPos[0] = 0.0;
  peakPos[1] = 0.0;
  
  int startingPoint = peakFindingRegion+1;
  int stopPoint = int(stopScan / binSize)-10;

  /// find local maximum
  for (int i=startingPoint; i<stopPoint; i++){
   
    bool leftMaxOK = true;
    bool rightMaxOK = true;
    bool leftMinOK = true;
    bool rightMinOK = true;
    double centralVal = inHist->GetBinContent(i);
    
    for (int j=1; j<=peakFindingRegion; j++){
      if ((inHist->GetBinContent(i-j)-centralVal) > 0.0)
	leftMaxOK = false;
      if ((inHist->GetBinContent(i+j)-centralVal) > 0.0)
	rightMaxOK = false;
      if ((inHist->GetBinContent(i-j)-centralVal) < 0.0)
	leftMinOK = false;
      if ((inHist->GetBinContent(i+j)-centralVal) < 0.0)
	rightMinOK = false;
    }
    
    if (leftMinOK && rightMinOK){
      peakPos[0] = i;
    }
    
    if (leftMaxOK && rightMaxOK){
      peakPos[1] = i;
    }
    
//     if (peakPos[0]!=0 && peakPos[1]!=0)
//       break;
    
  }
  
  pair<int,int> returnValue(peakPos[0],peakPos[1]);
  
  return returnValue;
}


int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]){
  
  try 
  { 
    /** Define and parse the program options 
     */ 
    po::options_description desc("Options"); 
    desc.add_options()
      ("help,h", "Print help messages") 
      ("inputFile,f", po::value<std::string>()->required(), "input hist file name") /// mandatory option
      ("fixedCut,c", po::value<double>(), "if true - use fixed value for cut for truncated mean")
      ("useAmplitudeDistribution,a", "if true - use maxValHistLeft distribution instead of integralHistBoth")
      ;
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
      
      if ( vm.count("fixedCut")  ) 
	if (vm["fixedCut"].as<double>()<0){
	  
	  std::cout << "Only positive cut values are accepted!!!" << std::endl;
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
