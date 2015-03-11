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

using namespace std;

string inHistFileFolder = "histFiles";
string outPngFolder = "histPicts/picts";
string outTxtFolder = "histPicts";

bool saveAmplitudeInfo = false; /// TODO to implement this flag

int fitHists(string fileName, string prefix = "fit_info");
string getFolder(string fullPathFileName);
pair<double,double> rescaleHist(TH1F* hist, const double _borderInPercentages = 0.1);

bool checkIfFileExist(const string inFile){
  if ( boost::filesystem::exists( inFile ) ){ /// if it exist - do nothing
    return true;
  }
  else{ /// if it not exist - create file and put header line in the beginning
    fstream fitInfoIntegral (inFile.c_str(),ios::out); /// create file
    fitInfoIntegral << "#DAC\tQ_fit_mean\tQ_fit_mean_err\tQ_fit_sigma\tQ_fit_sigma_err\tQ_get_mean\tQ_get_mean_err\tQ_get_sigma\tQ_get_sigma_err\t" 
		    << "A_fit_mean\tA_fit_mean_err\tA_fit_sigma\tA_fit_sigma_err\tA_get_mean\tA_get_mean_err\tA_get_sigma\tA_get_sigma_err\n";
    return false; 
  }
}

int main(int argc, char* argv[]){

  string prefix = "fit_info";
  
  /// get file name choosen by user  
  string fileName;
  if (argc==1) fileName = ChooseUserFile(inHistFileFolder); /// ask user to specify file from list
  if (argc==2 || argc==3){
    fileName = string(argv[1]);
    if (argc==3) prefix = string(argv[2]);
    if (getFolder(fileName)!=""){
      inHistFileFolder = getFolder(fileName);
      vector<string> words = GetSplittedWords(fileName,"/");
      fileName = words[words.size()-1];
      cout << "fileName = " << fileName << endl;
    }
    if (!doFileExist(fileName,inHistFileFolder)){
      cout << "No file *" << fileName << "* in *" << inHistFileFolder << "* folder" << endl;
      fileName = "";
      return -1;
    }
  }
  cout << "proceed " << fileName << " file..." << endl;
  return fitHists(fileName,prefix);
  
  
}

int fitHists(string fileName, string prefix){
  
  /// get job name based on input file
  string jobName = GetSplittedWords(fileName,".").at(0);

  /// open the file
  TFile* inFile;
  if (GetSplittedWords(fileName,"/").size()>=2)
    inFile = TFile::Open(fileName.c_str());
  else
    inFile = TFile::Open((inHistFileFolder + "/" + fileName).c_str());
  
  /// get histograms from file:
  TH1F* baseLineLeft = (TH1F*)inFile->Get("baseLineLeft");
  TH1F* baseLineRight = (TH1F*)inFile->Get("baseLineRight");
  if (baseLineLeft==NULL || baseLineRight==NULL){cout << "can't find baseLine histos!\nTERMINATE!!!\n"; return -1;}
  TH1F* maxValHist = (TH1F*)inFile->Get("maxValHist");
  TH1F* maxValHistLeft = (TH1F*)inFile->Get("maxValHistLeft");
  TH1F* maxValHistRight = (TH1F*)inFile->Get("maxValHistRight");
  if (maxValHist==NULL || maxValHistLeft==NULL || maxValHistRight==NULL){
    cout << "can't find maxValHist histos!\nTERMINATE!!!\n"; 
    return -1;
  }
  TH1F* integralHistBoth = (TH1F*)inFile->Get("integralHistBoth");
  TH1F* baseLineIntegralLeft = (TH1F*)inFile->Get("baseLineIntegralLeft");
  TH1F* baseLineIntegralRight = (TH1F*)inFile->Get("baseLineIntegralRight");
  if (integralHistBoth==NULL){cout << "can't find integralHistBoth hist!\nTERMINATE!!!\n"; return -1;}
  
  /// create canvas 3x2
  TCanvas* can = new TCanvas("can","can",50,50,2000,2000);
  can->Divide(3,3);
  
  /// create output fit info file
  string outFileName = outTxtFolder + "/"+prefix+"_integral.txt";
  checkIfFileExist(outFileName);
  fstream fitInfoIntegral (outFileName.c_str(),ios::out | ios::app);
  
  /// amplitude values
  double meanAmplLeft, meanAmplRight, sigmaAmplLeft, sigmaAmplRight;
  double meanAmplLeft_nonFit, meanAmplRight_nonFit, sigmaAmplLeft_nonFit, sigmaAmplRight_nonFit;
  double meanErrAmplLeft, sigmaErrAmplLeft, meanErrAmplLeft_nonFit, sigmaErrAmplLeft_nonFit;
  
  /// fit maxVal histos
  TF1* gausFunc = new TF1("gausFunc","gaus");
  can->cd(1);
  maxValHist->Draw();
  fitGaus(maxValHist,gausFunc);
//   fitInfo << jobName << "\t" << gausFunc->GetParameter(1) << "\t" << gausFunc->GetParameter(2) << "\t" << gausFunc->GetParError(1) << "\n";
  can->cd(2);
  maxValHistLeft->Draw();
  fitGaus(maxValHistLeft,gausFunc);
  /// get gaus parameters
  meanAmplLeft = gausFunc->GetParameter(1);
  sigmaAmplLeft = gausFunc->GetParameter(2);
  meanAmplLeft_nonFit = maxValHistLeft->GetMean();
  sigmaAmplLeft_nonFit = maxValHistLeft->GetRMS();
  /// errors of parameters:
  meanErrAmplLeft = gausFunc->GetParError(1);
  sigmaErrAmplLeft = gausFunc->GetParError(2);
  meanErrAmplLeft_nonFit = maxValHistLeft->GetMeanError();
  sigmaErrAmplLeft_nonFit = maxValHistLeft->GetRMSError();
//   fitInfoLeft << jobName << "\t" << gausFunc->GetParameter(1) << "\t" << gausFunc->GetParameter(2) << "\t" << gausFunc->GetParError(1) << "\n";
  can->cd(3);
  maxValHistRight->Draw();
  fitGaus(maxValHistRight,gausFunc);
  meanAmplRight = gausFunc->GetParameter(1);
  sigmaAmplRight = gausFunc->GetParameter(2);
  meanAmplRight_nonFit = maxValHistRight->GetMean();
  sigmaAmplRight_nonFit = maxValHistRight->GetRMS();
//   fitInfoRight << jobName << "\t" << gausFunc->GetParameter(1) << "\t" << gausFunc->GetParameter(2) << "\t" << gausFunc->GetParError(1) << "\n";
  
  /// draw randomPulse and baseline histos
  can->cd(4);
  
  const int nRndmHists = 100;
  int rndmStartHist = 0;
  TH1F* rndmPulse = (TH1F*)inFile->Get(("randomPulse" + DoubToStr(rndmStartHist)).c_str());
  rndmPulse->GetYaxis()->SetTitle("amplitude, [V]");
  rndmPulse->GetXaxis()->SetTitle("FADC bin");
  while (rndmPulse==NULL && rndmStartHist<nRndmHists){
    rndmStartHist++;
    rndmPulse = (TH1F*)inFile->Get(("randomPulse" + DoubToStr(rndmStartHist)).c_str());
  }
  
  if(rndmPulse!=NULL){
    
    pair<double,double> yRange = rescaleHist(rndmPulse);
    
    rndmPulse->Draw();
    
    /// draw vertical lines which correspond to integration range
    TH1F* integrationRangeHist = (TH1F*)inFile->Get("integrationRangeHist");
    if (integrationRangeHist!=NULL){
      double leftIntRange = integrationRangeHist->GetBinContent(1);
      double rightIntRange = integrationRangeHist->GetBinContent(2);
      
      TLine* lineLeft = new TLine(leftIntRange,yRange.first,leftIntRange,yRange.second);
      TLine* lineRight = new TLine(rightIntRange,yRange.first,rightIntRange,yRange.second);
      lineLeft->SetLineWidth(2);
      lineRight->SetLineWidth(2);
      
      cout << "[INTERVAL_INFO]\tleft-right integration range: " << leftIntRange << "-" << rightIntRange << endl;

      lineLeft->Draw();
      lineRight->Draw();
    }
    
    for (int i=rndmStartHist+1; i<100; i++){
      TH1F* rndmPulse2 = (TH1F*)inFile->Get(("randomPulse" + DoubToStr(i)).c_str());
      if (rndmPulse2!=NULL)
	rndmPulse2->Draw("same");
    } 

  }
  
  can->cd(5);
  baseLineLeft->Draw();
  can->cd(6);
  baseLineRight->Draw();
  
  /// fit integral hist
  can->cd(7);
  integralHistBoth->Draw();
  fitGaus(integralHistBoth,gausFunc);
  
  can->cd(8);
  baseLineIntegralLeft->Draw();
  
  can->cd(9);
  baseLineIntegralRight->Draw();
  
  /// count sigma^2/ampl ratio with and w/o using fitting
//   double fitSigmaAmpRatio = gausFunc->GetParameter(2)*gausFunc->GetParameter(2) / gausFunc->GetParameter(1);
//   double nonFitSigmaAmpRatio = integralHistBoth->GetRMS()*integralHistBoth->GetRMS() / integralHistBoth->GetMean();
//   double fitSigmaAmpRatio_leftAmpl = sigmaAmplLeft_nonFit*sigmaAmplLeft_nonFit/meanAmplLeft_nonFit;
//   double fitSigmaAmpRatio_rightAmpl = sigmaAmplLeft_nonFit*sigmaAmplRight_nonFit/meanAmplRight_nonFit;
  
  /// save canvas to the file
  vector<string> tmpWords = GetSplittedWords(jobName,"_");
  string DAC_val = tmpWords[tmpWords.size()-2] + "_" + tmpWords[tmpWords.size()-1]; /// FIXME WARNING possible hardcode of position to read dor the first column
  
  boost::filesystem::path dir(outPngFolder + "/" + prefix);
  boost::filesystem::create_directories(dir);
  
  can->SaveAs( (outPngFolder + "/" + prefix + "/" + jobName + ".png").c_str() );
  fitInfoIntegral << DAC_val << "\t" 
		  << gausFunc->GetParameter(1) << "\t" << gausFunc->GetParError(1) << "\t" << gausFunc->GetParameter(2) << "\t" << gausFunc->GetParError(2) << "\t" /// charge fit params
		  << integralHistBoth->GetMean() << "\t" << integralHistBoth->GetMeanError() << "\t" << integralHistBoth->GetRMS() << "\t" << integralHistBoth->GetRMSError() << "\t"  /// charge get params
		  << meanAmplLeft << "\t" << meanErrAmplLeft << "\t" << sigmaAmplLeft << "\t" << sigmaErrAmplLeft << "\t" /// maxAmpl fit params
		  << meanAmplLeft_nonFit << "\t" << meanErrAmplLeft_nonFit << "\t" << sigmaAmplLeft_nonFit << "\t" << sigmaErrAmplLeft_nonFit  /// maxAmpl get params
		  << "\n";
  
  return 0;
}

string getFolder(string fullPathFileName){
  string outStr = "";
  int nSize = GetSplittedWords(fullPathFileName,"/").size(); 
  if (nSize==1)
    return outStr;
  vector<string> iWords = GetSplittedWords(fullPathFileName,"/");
  for (int i=0; i<(nSize-1); i++){
    outStr += iWords[i] + "/";
  }
  return outStr;
}

pair<double,double> rescaleHist(TH1F* hist, const double _borderInPercentages){
  
  double borderInPercentages = _borderInPercentages;
  if (borderInPercentages<0) borderInPercentages = 0;
  
  pair<double,double> outPair;
  
  double min = hist->GetMinimum();
  double max = hist->GetMaximum();
  double range = max - min;
  
  outPair.first = min - range*borderInPercentages;
  outPair.second = max + range*borderInPercentages;
  
  hist->GetYaxis()->SetRangeUser(outPair.first,outPair.second);
  
  return outPair;
  
}





