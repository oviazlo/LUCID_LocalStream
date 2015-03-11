
#include<TGraphErrors.h>
#include<TFile.h>
#include<TAxis.h>
#include<TF1.h>
#include<TMath.h>
#include<TLegend.h>
#include<TLatex.h>
#include<TCanvas.h>
#include<sstream>
#include<iostream>
#include "serviceFunctions.h"

using namespace std;

void SetGraphStyle(TGraph* inGr, Color_t inColor);
int makePlots(string histName, vector<string> relDistanceVec, vector<string> chargeVec, vector<string> chargeErrVec, bool makeFitting);

int main(int argc, char* argv[]){
 
  vector<string> fileList;
  
  vector<string> relDistanceVec;
  vector<string> chargeVec;
  vector<string> chargeErrVec;
  string fileName;
  
  
  if (argc<=2){
    if (argc==1) fileName = "histPicts/" + ChooseUserFile("histPicts","txt"); /// ask user to specify file from list
    else fileName = argv[1];
    fileList = GetColumnFromFile(fileName, 0);
    chargeVec = GetColumnFromFile(fileName, 1);
    chargeErrVec = GetColumnFromFile(fileName, 3);
      
    /// extract distance from file names:
    cout << "Extracted distances:\n";
    for (int i=0; i<fileList.size(); i++){

      vector<string> dummyWords = GetSplittedWords(fileList[i],"_");
      string dummyWord = dummyWords[ dummyWords.size()-1 ];
      relDistanceVec.push_back(dummyWord);
      cout << dummyWord << endl;
      
    }
    cout << endl;
    
    /// take histName from file name:
    vector<string> dummyWords = GetSplittedWords(fileName,"/");
    string dummyWord = dummyWords[dummyWords.size()-1];
    string histName = GetSplittedWords(dummyWord,".")[0];
    
    bool makeFitting = false;
    if ( IsInWord(fileName,"distance")) makeFitting = true; /// WARNING make fitting if in txt file name word "distance" is found!!!
    
    makePlots(histName, relDistanceVec, chargeVec, chargeErrVec, makeFitting);
  }
  else{
    
    cout << "Please specify one input file with extension .txt!!!" << endl;
    cout << "TERMINATE!" << endl;
    
  }


  

  return 0;
}

int makePlots(string histName, vector<string> relDistanceVec, vector<string> chargeVec, vector<string> chargeErrVec, bool makeFitting){
  
  TCanvas*can = new TCanvas("can","can",0,0,1200,800);
  if (makeFitting==true) can->Divide(1,2);
  can->cd(1);
  
  double zeroArr[99];
  for (int i=0; i<99; i++){
    zeroArr[i] = 0.0; 
  }
  
  ///PIN-diode: disctance scan
  const int nPoints = relDistanceVec.size();
  double dist[nPoints];
  double charge[nPoints];
  double chargeErr[nPoints];
  for (int i=0; i<nPoints; i++){
    dist[i] = atof(relDistanceVec[i].c_str())/100.; /// FIXME for distance scan divide by 100
    charge[i] = atof(chargeVec[i].c_str());
    chargeErr[i] = atof(chargeErrVec[i].c_str());
  }
  
  ///PMT stressed@1250V: distance scan
//   const int nPoints = 16;
//   double dist[nPoints] = {0,050,100,150,200,250,300,350,400,450,500,550,600,650,700,750};
//   double charge[nPoints] = {3.622,3.53354,3.44939,3.36684,3.28851,3.2117,3.13772,3.06613,2.99747,2.9313,2.86656,2.8044,2.74435,2.68586,2.62958,2.57363};
//   double chargeErr[nPoints] = {6.19892e-05,6.23921e-05,6.22715e-05,6.25764e-05,6.25686e-05,6.31298e-05,6.30064e-05,6.36333e-05,6.32159e-05,6.32514e-05,6.38832e-05,6.33077e-05,6.39082e-05,6.39555e-05,6.40768e-05,6.40047e-05};
//   for (int i=0; i<nPoints; i++){
//     dist[i] = dist[i]/100.0;
//   }
  
  
  /// temperatureTest1, small1
//   const int nPoints = 13;
//   double zeroArr[nPoints] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1}; /// approximate value on eye!
//   double dist[nPoints] = {21.0071,21.6845,22.0131,22.1527,22.4634,22.7955,23.1369,23.4743,23.8963,24.1597,24.4262,24.4624,24.6513};
//   double charge[nPoints] = {2.52985,2.52446,2.52141,2.51926,2.50832,2.49656,2.48617,2.47658,2.46777,2.46382,2.45869,2.45743,2.45614};
//   double chargeErr[nPoints] = {6.19892e-05,6.23921e-05,6.22715e-05,6.25764e-05,6.25686e-05,6.31298e-05,6.30064e-05,6.36333e-05,6.32159e-05,6.32514e-05,6.38832e-05,6.33077e-05,6.33077e-05};
//   
  TGraphErrors* tmpGr = new TGraphErrors(nPoints,dist,charge,zeroArr,chargeErr);
  SetGraphStyle(tmpGr,kBlack);
  
//   tmpGr->SetTitle("stressedPMT@1250V, DAC=0xa00");
  tmpGr->SetTitle("stressedPMT@1250V, DAC=0xfff");
//   tmpGr->SetTitle(histName.c_str());
  
//   tmpGr->GetXaxis()->SetTitle("temperature, [C]");
  tmpGr->GetXaxis()->SetTitle("distance, [mm]");
  tmpGr->GetYaxis()->SetTitle("charge, [V*4ns]");
  
  tmpGr->Draw("AP");
  if (makeFitting==false){
    tmpGr->GetXaxis()->SetTitle("time, [minutes]");
    can->SaveAs("some_scan.eps");
    can->SaveAs("some_scan.root");
    return 0;
  }
  
  TF1* func = new TF1("func","[0]/((x+[1])*(x+[1]))");
  tmpGr->Fit("func");
  tmpGr->Fit("func");
  tmpGr->Fit("func");
  
  double chi2_ndf = func->GetChisquare()/func->GetNDF();
  string chi2Str = DoubToStr(chi2_ndf);
  
  
  
  TLatex Tl;
  Tl.SetTextSize(0.04);
  Tl.SetNDC();
//   Tl.SetTextAlign(12);
  Tl.DrawLatex(0.55, 0.85, "integration method");
  Tl.DrawLatex(0.55, 0.8, ("#chi^{2}/ndf = " + chi2Str).c_str());
  
  Tl.DrawLatex(0.55, 0.7, "fit function: p_{0}/(x+p_{1})^{2}");
  Tl.DrawLatex(0.55, 0.65, ("p_{0} = " + DoubToStr(func->GetParameter(0))).c_str());
  Tl.DrawLatex(0.55, 0.6,  ("p_{1} = " + DoubToStr(func->GetParameter(1))).c_str());
  
  double initialDistance = func->GetParameter(1);
  double intensity[nPoints];
  for (int i=0; i<nPoints; i++){
    intensity[i] = func->GetParameter(0)/(4*TMath::Pi()*pow((dist[i] + initialDistance),2));
  }
  TGraphErrors* intGraph = new TGraphErrors(nPoints,intensity,charge,zeroArr,chargeErr);
  SetGraphStyle(intGraph,kBlack);
  
  intGraph->GetXaxis()->SetTitle("intensity");
  intGraph->GetYaxis()->SetTitle("charge, [V*4ns]");
  
  intGraph->SetTitle("stressedPMT@1250V, DAC=0xfff");
//   intGraph->SetTitle("PIN-diode, DAC=0xfff");
  
  can->cd(2);
  intGraph->Draw("AP");
  TF1* linFunc = new TF1("linFunc","[0]+[1]*x");
  intGraph->Fit("linFunc");
  
  chi2_ndf = linFunc->GetChisquare()/func->GetNDF();
  chi2Str = DoubToStr(chi2_ndf);
  
  Tl.DrawLatex(0.15, 0.85, "integration method");
  Tl.DrawLatex(0.15, 0.8, ("#chi^{2}/ndf = " + chi2Str).c_str());
  
  Tl.DrawLatex(0.15, 0.7, "fit function: p_{0}+p_{1}*x");
  Tl.DrawLatex(0.15, 0.65, ("p_{0} = " + DoubToStr(linFunc->GetParameter(0))).c_str());
  Tl.DrawLatex(0.15, 0.6,  ("p_{1} = " + DoubToStr(linFunc->GetParameter(1))).c_str());
  
  can->SaveAs("distance_scan.eps");
  can->SaveAs("distance_scan.root");
  
  return 0;
  
}

void SetGraphStyle(TGraph* inGr, Color_t inColor){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(1.15);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
}
