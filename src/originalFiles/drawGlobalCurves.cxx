#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>

double parseFileName(string inStr);
string inPngFolder = "histPicts";

int drawGlobalCurves(string infoFile);

int main(){

  /// get file name choosen by user  
  string infoFile = ChooseUserFile(inPngFolder,".txt");
  return drawGlobalCurves(infoFile);
}

int drawGlobalCurves(string infoFile){
  
  /// get job name based on used file
  string jobName = GetSplittedWords(infoFile,".").at(0);
  
  /// get columns from file
  /// original:
  vector<string> ledVoltage = GetColumnFromFile(inPngFolder + "/" + infoFile,0);
  vector<string> peakPos = GetColumnFromFile(inPngFolder + "/" + infoFile,1);
  vector<string> peakPosErr = GetColumnFromFile(inPngFolder + "/" + infoFile,3);

  /// custom:
//   vector<string> ledVoltage = GetColumnFromFile(inPngFolder + "/" + infoFile,0);
//   vector<string> peakPos = GetColumnFromFile(inPngFolder + "/" + infoFile,1);
//   vector<string> peakPosErr = GetColumnFromFile(inPngFolder + "/" + infoFile,2);

  
  /// check if we read same number of elements in each column
  if (ledVoltage.size()!=peakPos.size()||ledVoltage.size()!=peakPosErr.size()) return -1;

  /// init some vals.
  const int nPoints = peakPos.size();
  double ledVolVal[nPoints];
  double peakPosVal[nPoints];
  double peakPosErrVal[nPoints];
  double zeroArr[nPoints];
  double sigmaDiff[nPoints];
  double diffAbs[nPoints];

  /// print all points and push them from vectors to arrays:
  cout << nPoints << " point were read!" << endl;
  cout << "Points:" << endl;
  for (unsigned int i=0; i<nPoints; i++){
    zeroArr[i] = 0;
    ledVolVal[i] = parseFileName(ledVoltage[i]);
    cout << "ledVoltage[" << i << "] = " << ledVoltage[i] << endl;
    cout << "ledVolVal[" << i << "] = " << ledVolVal[i] << endl;
    peakPosVal[i] = atof(peakPos[i].c_str());
    peakPosErrVal[i] = atof(peakPosErr[i].c_str());
    cout << i << ": " << ledVolVal[i] << "\t" <<  peakPosVal[i] << endl;
  }

  /// plot graph
  TGraphErrors *curve = new TGraphErrors(nPoints,ledVolVal,peakPosVal,zeroArr,peakPosErrVal);
//   curve->SetTitle("baseline = 50.2 mV, threshold = 53 mV, air gap between Pin-diode and LED");
  curve->SetTitle("calibrationGraph");
  curve->SetTitle("baseline = 50.2 mV, threshold = 60 mV, air gap, 200kHz");
  curve->SetMarkerSize(0.6);
  curve->SetMarkerStyle(21);
//   curve->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  curve->GetXaxis()->SetTitle("(mV@750V)");
  curve->GetYaxis()->SetTitle("collected charge, [V*ns]");
  curve->GetYaxis()->SetTitleOffset(1.29);
  curve->Draw("APE");

  /// fir it with pol3 function
  TString fitFunction = "pol1";
  TF1* fitFunc = new TF1("fitFunc",fitFunction);
  curve->Fit("fitFunc");
  double chi2_ndf = fitFunc->GetChisquare()/fitFunc->GetNDF();
  string chi2Str = DoubToStr(chi2_ndf);
  
  /// count sigmaDiff
  for (unsigned int i=0; i<nPoints; i++){
    sigmaDiff[i] = 100.0*(peakPosVal[i] - fitFunc->Eval(ledVolVal[i]))/fitFunc->Eval(ledVolVal[i]);
    diffAbs[i] = (peakPosVal[i] - fitFunc->Eval(ledVolVal[i]));
    diffAbs[i] = diffAbs[i];
  }
  TGraph *diffSigmaGraph = new TGraph(nPoints,ledVolVal,sigmaDiff);
  diffSigmaGraph->SetTitle(fitFunction);
//   diffSigmaGraph->SetTitle("diffSigmaGraph");
  diffSigmaGraph->SetMarkerStyle(21);
//   diffSigmaGraph->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  diffSigmaGraph->GetXaxis()->SetTitle("(mV@750V)");
  diffSigmaGraph->GetYaxis()->SetTitle("relative deviation, [%]");
  
  TGraph *diffSigmaGraph2 = new TGraph(nPoints,ledVolVal,diffAbs);
  diffSigmaGraph2->SetTitle(fitFunction);
//   diffSigmaGraph2->SetTitle("diffSigmaGraph2");
  diffSigmaGraph2->SetMarkerStyle(21);
//   diffSigmaGraph2->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  diffSigmaGraph2->GetXaxis()->SetTitle("(mV@750V)");
  diffSigmaGraph2->GetYaxis()->SetTitle("absolute deviation");
  
  /// print chi2/ndf on plot with TLatex
  TLatex Tl;
  Tl.SetTextSize(0.04);
  Tl.SetNDC();
//   Tl.SetTextAlign(12);
  Tl.DrawLatex(0.15, 0.85, "integration method");
  Tl.DrawLatex(0.15, 0.8, ("#chi^{2}/ndf = " + chi2Str).c_str());
  
  /// WARNING temprorary comment this line
//   gPad->SaveAs((jobName + ".eps").c_str());
  
  TFile rootFile(("./" + jobName + ".root").c_str(),"RECREATE");
  curve->SetName("calibCurve");
  diffSigmaGraph->SetName("absDeviation");
  diffSigmaGraph2->SetName("relDeviation");
  curve->Write();
//   fitFunc->Write(); /// WARNING don't save fitting function
  diffSigmaGraph->Write();
  diffSigmaGraph2->Write();
  
  /// try to count one point from fit:
  double randomCharge = 0.0911281;
  int realPoint = 3164;
  if (fitFunction=="pol1"){
    double slope = fitFunc->GetParameter(1);
    double offset = fitFunc->GetParameter(0);
    double slopeErr = fitFunc->GetParError(1);
    double offsetErr = fitFunc->GetParError(0);
    
    cout << "slope = " << slope << "+-" << slopeErr << endl;
    cout << "offset = " << offset << "+-" << offsetErr << endl;

    double dacValFromFit = (randomCharge - offset)/slope;
    cout << "dacValFromFit = " << dacValFromFit << "; fitFunc->GetX = " << fitFunc->GetX(randomCharge,3000,4000) << endl;
    double dacValErrFromFit = sqrt( pow(offsetErr/slope,2) + pow(dacValFromFit*slopeErr/slope,2) );
    cout << "dacValErrFromFit = " << dacValErrFromFit << endl;
    cout << "realPoint = " << realPoint << endl;
  }
  if (fitFunction=="pol2"){
    double a = fitFunc->GetParameter(2);
    double aErr = fitFunc->GetParError(2);
    double b = fitFunc->GetParameter(1);
    double bErr = fitFunc->GetParError(1);
    double c = fitFunc->GetParameter(0);
    double cErr = fitFunc->GetParError(0);    

    c = c -randomCharge;
    
    double discriminant = b*b-4*a*c;
    double dSqrt = sqrt(discriminant);
    
    /// possitive sign before discriminant:
    double dxda = (b-dSqrt)/(2*a*a) - c/(a*dSqrt);
    double dxdb = (-1+b/dSqrt)/(2*a);
    double dxdc = -(1/dSqrt);
    
    double error = sqrt(pow(dxda*aErr,2) + pow(dxdb*bErr,2) + pow(dxdc*cErr,2));
    double centralVal = (-b + dSqrt)/(2*a);
    
    cout << "dxda*aErr = " << dxda*aErr << "; dxdb*bErr = " << dxdb*bErr << "; dxdc*cErr = " << dxdc*cErr << endl;
    
    cout << "centralVal = " << centralVal << "+-" << error << endl;
    cout << "realPoint = " << realPoint << endl;
    
  }
  
return 0;
}

double parseFileName(string inStr){

  /// split name vs. "_" symbol
  vector<string> words = GetSplittedWords(inStr,"_");
  /// x16 - int in hexidecimal; 
  /// x10 - int in decimal
  string x16;
  int x10 = -1;
  
  /// loop over all words to find amplitude intensity
  for (unsigned int k=0; k<words.size(); k++){
    if (IsInWord("0x",words[k])){
      x16 = words[k];
     
      /// hexidecimal -> decimal
      std::stringstream stream;
      stream << x16;
      stream >> std::hex >> x10;
      break;
    }
  }
  
  if (x10==-1) x10 = atof(words[words.size()-1].c_str());
    
  return x10;
  
}