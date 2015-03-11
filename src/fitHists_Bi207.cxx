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

int fitHists(string fileName, double guess);
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
  if(argc==3){
    fileName = string(argv[1]);
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
  else{
    cout << "We need 2 arguments: <pattern> and <initial guess>" << endl;
    return -1;
  }
  cout << "proceed " << fileName << " file..." << endl;
  return fitHists(fileName,atof(string(argv[2]).c_str()));
  
  
}



int fitHists(string fileName, double guess){
  
  /// get job name based on input file
  string jobName = GetSplittedWords(fileName,".").at(0);

  /// open the file
  TFile* inFile;
  if (GetSplittedWords(fileName,"/").size()>=2)
    inFile = TFile::Open(fileName.c_str());
  else
    inFile = TFile::Open((inHistFileFolder + "/" + fileName).c_str());
  
  TH1F* integralHistBoth = (TH1F*)inFile->Get("integralHistBoth");
  if (integralHistBoth==NULL){cout << "can't find integralHistBoth hist!\nTERMINATE!!!\n"; return -1;}
  integralHistBoth->SetTitle("charge integral;charge, [mV * ns]; counts");
  
  TF1* fitFunc = new TF1("fitFunc","gaus",0.75*guess,10000);
  
  /// make initial fit:
  integralHistBoth->Fit("fitFunc","R");
  
  double mean = fitFunc->GetParameter(1);
  double sigma = fitFunc->GetParameter(2);
  
  double rightIntRange = mean + 3*sigma;
  
  double initLeftIntRange = mean - sigma;
  double finalLeftIntRange = mean;
  
  const int nPoints = 30;
  
  double meanVec[nPoints];
  double leftRangeVec[nPoints];
  double chi2ndfVec[nPoints];
  
  for (int i=nPoints-1; i>=0; i--){
   
    double leftIntRange = initLeftIntRange + i*(finalLeftIntRange - initLeftIntRange)/nPoints;
    fitFunc = new TF1("fitFunc","gaus",leftIntRange,rightIntRange);
    
    integralHistBoth->Fit("fitFunc","R");
  
    leftRangeVec[nPoints-1-i] = (leftIntRange);
    chi2ndfVec[nPoints-1-i] = (fitFunc->GetChisquare() / fitFunc->GetNDF());
    meanVec[nPoints-1-i] = (fitFunc->GetParameter(1));
      
  }
  
  TGraph *meanGraph = new TGraph(nPoints,leftRangeVec,meanVec);
  meanGraph->SetTitle("meanGraph;left edge of integration range, [mV];mean, [mV]");
  
  TGraph *chi2ndfGraph = new TGraph(nPoints,leftRangeVec,chi2ndfVec);
  chi2ndfGraph->SetTitle("chi2ndfGraph;left edge of integration range, [mV];#chi^{2}/ndf");
  
  /// create canvas 3x2
  TCanvas* can = new TCanvas("can","can",50,50,1000,1000);
  can->Divide(2,2);
  
  can->cd(4);
  integralHistBoth->GetXaxis()->SetRangeUser(0,5000);
  integralHistBoth->Draw();
  fitFunc->Draw("same");
  
  can->cd(3);
  chi2ndfGraph->Draw("ALP");

  can->cd(1);
  meanGraph->Draw("ALP");

  int nStablePoint = 9;
  double fittedPlateuVal = -1;
  
  /// attemp to find stable region of mean
  for (int i=0; i<nPoints-nStablePoint; i++){
    meanGraph = new TGraph(nPoints-i,leftRangeVec,meanVec);
    TF1* lineFunc = new TF1("lineFinc","pol0",leftRangeVec[nPoints-i-1],leftRangeVec[nPoints-i-nStablePoint-1]);
    meanGraph->Fit("lineFinc","R");
    double fitPar = lineFunc->GetParameter(0);
    
    /// check deviation
    bool noBifDeviation = true;
    for (int j=nPoints-i-nStablePoint; j<nPoints-i; j++){
      if ( (abs(meanVec[j]-fitPar)/fitPar) > 0.004){
	noBifDeviation = false;
	break;
      }
    }
    if (noBifDeviation==true){
      lineFunc->Draw("same");
      TF1* lineFuncUp = new TF1("lineFincUp","pol0",leftRangeVec[nPoints-i-1],leftRangeVec[nPoints-i-nStablePoint-1]);
      TF1* lineFuncDown = new TF1("lineFincDown","pol0",leftRangeVec[nPoints-i-1],leftRangeVec[nPoints-i-nStablePoint-1]);
      fittedPlateuVal = lineFunc->GetParameter(0);
      lineFuncUp->SetParameter(0,fittedPlateuVal*1.004);
      lineFuncDown->SetParameter(0,fittedPlateuVal*0.996);
      lineFuncUp->Draw("same");
      lineFuncDown->Draw("same");
      break;
    }
  }
  
  can->cd(2);
  fitFunc = new TF1("fitFunc","gaus",initLeftIntRange + (nPoints-1)*(finalLeftIntRange - initLeftIntRange)/nPoints,rightIntRange);
  integralHistBoth->Fit("fitFunc","R");
  integralHistBoth->GetXaxis()->SetRangeUser(0,5000);
  integralHistBoth->Draw();
  
  can->SaveAs(("pngTemp/" + jobName + ".png").c_str());
  
  vector<string> words = GetSplittedWords(jobName,"_");
  string date;
  
  if (words[words.size()-1][0] == 't')
    date = words[words.size()-3] + "_" + words[words.size()-2];
  else
    date = words[words.size()-2] + "_" + words[words.size()-1];
  
  fstream fitInfoIntegral ("bismuthTestFit.txt",ios::out | ios::app); /// create file
  
  fitInfoIntegral << date << "\t" << fittedPlateuVal << "\t" << fittedPlateuVal*0.004 << "\n";
  
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





