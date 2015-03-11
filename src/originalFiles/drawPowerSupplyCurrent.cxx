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

int parseFileName(string inStr);

string inFolder = "externalInfoFiles";
string curentFile = "curent_stressedPMT.txt";
string intensityFile = "intensVal_stressedPMT.txt";

int main(){

   /// get job name based on used file
  string jobName = GetSplittedWords(curentFile,".").at(0);
  
  /// get columns from file
  vector<string> curentStrVec = GetColumnFromFile(inFolder + "/" + curentFile,0);
  vector<string> intensityStrVec = GetColumnFromFile(inFolder + "/" + intensityFile,0);

  /// check if files contains same number of points
  if (curentStrVec.size()!=(4*intensityStrVec.size())) return -1;

  /// init some vals.
  const int nPoints = intensityStrVec.size();
  double curent[nPoints];
  double intensity[nPoints];

  /// print all points and push them from vectors to arrays:
  cout << nPoints << " point were read!" << endl;
  cout << "Points:" << endl;
  for (unsigned int i=0; i<nPoints; i++){
    intensity[i] = parseFileName(intensityStrVec[i]);
    curent[i] = atoi( curentStrVec[4*i].c_str() ) + atoi( curentStrVec[4*i+1].c_str() ) + atoi( curentStrVec[4*i+2].c_str() ) + atoi( curentStrVec[4*i+3].c_str() )
		- 218;
  }

  /// plot graph
  TGraph *curve = new TGraph(nPoints,intensity,curent);
  curve->SetTitle("baseline = 50.2 mV, threshold = 60 mV, air gap, 200kHz");
  curve->SetMarkerSize(0.7);
  curve->SetMarkerStyle(21);
  curve->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  curve->GetYaxis()->SetTitle("curent, [#muA]");
  curve->GetYaxis()->SetTitleOffset(1.29);
  curve->Draw("AP");

  /// fir it with pol3 function
  TF1* fitFunc = new TF1("fitFunc","pol3");
  curve->Fit("fitFunc");
  double chi2_ndf = fitFunc->GetChisquare()/fitFunc->GetNDF();
  string chi2Str = DoubToStr(chi2_ndf);
  
  /// print chi2/ndf on plot with TLatex
  TLatex Tl;
  Tl.SetTextSize(0.04);
  Tl.SetNDC();
//   Tl.SetTextAlign(12);
  Tl.DrawLatex(0.15, 0.85, "integration method");
  Tl.DrawLatex(0.15, 0.8, ("#chi^{2}/ndf = " + chi2Str).c_str());
  
  gPad->SaveAs((jobName + ".eps").c_str());

return 0;
}

int parseFileName(string inStr){

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
    
  return x10;
  
}