#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>

using namespace std;

void printInstruction();
TGraph* getRatioGraph(TGraph* refGraph, TGraph* inGraph);
TGraph* getNormalizedGraph(char* fileName, Color_t grColor, string graphName = "calibCurve", bool normalize = true);
vector<TF1*> getFitFuncs(vector<TGraph*> inGraphs, TString prefixName, TString fitFunction, double leftFitEdge, double rightFitEdge, bool make1stFitParameterZero);
int getBiggestFunction(vector<TF1*> inVec, double referencePoint);
TF1* getRatioPolFunc(TString outFuncName, TF1* inFunc_den, TF1* inFunc_nom);

/// global values
Color_t colorList[9] = {kBlack,kRed,kBlue,kGreen,kOrange,kYellow,kYellow,kYellow,kYellow};

bool subtractOffset = false;

int main(int argc, char* argv[]) {
  
  /// check number of input arguments
  if (argc<3){
    cout << "Wrong number of arguments!" << endl;
    cout << "It should be at least 2 arguments!!!" << endl;
    printInstruction();
    return -1;
  }
  
  ///define some constants
  const int nGr = argc-2;
  vector<TGraph*> graphs;
  vector<TGraph*> ratioGraphs;
  
  /// get referance graph (correspond to first input file)
  TGraph* refGr = getNormalizedGraph(argv[1],kBlack);
  cout << "Get reference graph" << endl;
  
  /// get other graphs and ratioGraphs
  for (int i=0; i<nGr; i++){
    TGraph* tmpGr = getNormalizedGraph(argv[i+2],colorList[i+1]);
    graphs.push_back(tmpGr);
    ratioGraphs.push_back(getRatioGraph(refGr,tmpGr));
  }
  
  /// create canvas
  TCanvas *can = new TCanvas("can","can",0,0,1200,750);
  can->Divide(2,2);
  
  ///draw normalized graphs
  can->cd(1);
  refGr->Draw("ALP");
  for (int i=0; i<nGr; i++){
    graphs[i]->Draw("LP");
  }
  
  /// create legend
  TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
  leg->SetHeader("Legend");
  leg->AddEntry(refGr, GetSplittedWords(argv[1],".")[0].c_str() ,"lep");
  for (int i=0; i<nGr; i++){
    leg->AddEntry(graphs[i], GetSplittedWords(argv[i+2],".")[0].c_str() ,"lep");
  }
  leg->Draw();
  
  /// draw normalized ratio plots
  can->cd(2);
  ratioGraphs[0]->SetTitle(("Ratio to " + GetSplittedWords(argv[1],".")[0] + " graph").c_str());
  ratioGraphs[0]->Draw("ALP");
  for (int i=1; i<nGr; i++){
    ratioGraphs[i]->Draw("LP");
  }

  /// NOT NORMALIZED PLOTS
  /// 
  /// get NOT normalized graphs:
  vector<TGraph*> notNormalizedGraphs;
  for (int i=1; i<argc; i++){ /// start with 1 because first input argument is name of executed file
    cout << "Get non-normalized graph from file: " << argv[i] << endl;
    TGraph* tmpGr = getNormalizedGraph(argv[i],colorList[i-1],"calibCurve",false);
    notNormalizedGraphs.push_back(tmpGr);
  }
  
  /// create linear funcs and fit all graphs
  /// set offset to all functions to 0!!!
  vector<TF1*> fitFuncs = getFitFuncs(notNormalizedGraphs,"fitFunc_","pol1",2940,3230,subtractOffset);
  int biggestFunctionIndex = getBiggestFunction(fitFuncs,3250); 
  
  /// draw fitted non-normalized functions
  /// with offset equal 0
  can->cd(3);
//   TCanvas* canTmp = new TCanvas("canTmp","canTmp",0,0,1200,750);
  notNormalizedGraphs[biggestFunctionIndex]->GetYaxis()->SetRangeUser(3.8,4.3);
  notNormalizedGraphs[biggestFunctionIndex]->GetXaxis()->SetRangeUser(2900, 3300);
  notNormalizedGraphs[biggestFunctionIndex]->SetTitle("Non-normalized graphs: collected charge");
  notNormalizedGraphs[biggestFunctionIndex]->Draw("ALP");
  for (int i=0; i<fitFuncs.size(); i++){
    fitFuncs[i]->Draw("same");
    if (i!=biggestFunctionIndex) notNormalizedGraphs[i]->Draw("LP");
  }
  leg->Draw();
//   canTmp->SaveAs("canTmp.eps");
  
  can->cd(4);
  
  vector<TF1*> ratioFuncs;
  for (int i=1; i<fitFuncs.size(); i++){
    TF1* ratioFunc = getRatioPolFunc(("ratioFunc" + DoubToStr(i)).c_str(),fitFuncs[0],fitFuncs[i]);
    ratioFuncs.push_back(ratioFunc);  
  }
  
  int bIndex = getBiggestFunction(ratioFuncs,3250);
    
  ratioFuncs[bIndex]->GetYaxis()->SetRangeUser(0,ratioFuncs[bIndex]->Eval(2900)*1.1);
  ratioFuncs[bIndex]->SetTitle("Non-normalized Ratio of fitted lines");
  ratioFuncs[bIndex]->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  ratioFuncs[bIndex]->Draw();
  
  for (int i=0; i<ratioFuncs.size(); i++){
    if (i!=bIndex) ratioFuncs[i]->Draw("same");
  }

  can->SaveAs("ratio_comparioson.png");
  can->SaveAs("ratio_comparioson.eps");
  
}

TGraph* getRatioGraph(TGraph* refGraph, TGraph* inGraph){
  
  if (refGraph->GetN()!=inGraph->GetN()){
    cout << "Different number of bins in graphs:" << endl;
    cout << "referenceGraph = " << refGraph->GetName();
    cout << "inGraph = " << inGraph->GetName();
    return NULL;
  }
  
  const int nPoints = refGraph->GetN();
  
  double *dacVal = refGraph->GetX();
  double *yRef = refGraph->GetY();
  double *yIn = inGraph->GetY();
  
  double ratio[nPoints];
  
  for (int i=0; i<nPoints; i++){
    ratio[i] = yIn[i]/yRef[i];
  }
  
  TGraph* ratioGr= new TGraph(nPoints,dacVal,ratio);
  Color_t colorLine = inGraph->GetLineColor();
  ratioGr->SetMarkerColor(colorLine);
  ratioGr->SetLineColor(colorLine);
  ratioGr->SetMarkerSize(0.7);
  ratioGr->SetMarkerStyle(21);
  ratioGr->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  ratioGr->GetYaxis()->SetTitle("ratio");
  ratioGr->SetTitle("");
  
  return ratioGr;
  
}


TGraph* getNormalizedGraph(char* fileName, Color_t grColor, string graphName, bool normalize){

  /// get initial stuff  
  TFile *file = new TFile(fileName,"READ");
  TGraphErrors* gr = (TGraphErrors*)file->Get(graphName.c_str());
  const int nPoints = gr->GetN();
  double *dacVal = gr->GetX();
  double *y = gr->GetY();
  double integral = 0;

  /// count integrals over graphs
  /// and normalize the graph
  if (normalize){
    for (int i=0; i<nPoints; i++){
      integral += y[i];
    }
    for (int i=0; i<nPoints; i++){
      y[i] = y[i]/integral;
    }
  }
  
  /// get the output graph:
  TGraph* grOut = new TGraph(nPoints,dacVal,y);
  
  /// make it looks nice
  grOut->SetMarkerColor(grColor);
  grOut->SetLineColor(grColor);
  grOut->SetMarkerSize(0.7);
  grOut->SetMarkerStyle(21);
  grOut->GetXaxis()->SetTitle("LED intensity, [DAC value]");
  grOut->GetYaxis()->SetTitle("collected charge");
  grOut->SetTitle("Collected charge normalized to 1");
  
  delete gr;
  file->Close();
  
  return grOut;
  
}

void printInstruction(){
  cout << endl;
  cout << "Specify two root files!" << endl;
}

vector<TF1*> getFitFuncs(vector<TGraph*> inGraphs, TString prefixName, TString fitFunction, double leftFitEdge, double rightFitEdge, bool make1stFitParameterZero){
  
  vector<TF1*> fitFuncs;
  
  for (int i=0; i<inGraphs.size(); i++){
    TF1* tmpFunc = new TF1(prefixName + (TString)(DoubToStr(i+1)) ,fitFunction,leftFitEdge,rightFitEdge);
    tmpFunc->SetLineColor(colorList[i]);
    inGraphs[i]->Fit(prefixName + (TString)(DoubToStr(i+1)),"R0");
    if (make1stFitParameterZero) tmpFunc->SetParameter(0,0);
    fitFuncs.push_back(tmpFunc);
  }
  
  return fitFuncs;
  
}

int getBiggestFunction(vector<TF1*> inVec, double referencePoint){
  int index = 0;
  for (int i=1; i<inVec.size(); i++){
    if (inVec[i]->Eval(referencePoint)>inVec[i-1]->Eval(referencePoint)) index=i;
  }
  return index;
}

TF1* getRatioPolFunc(TString outFuncName, TF1* inFunc_den, TF1* inFunc_num){
  TString dPar0 = DoubToStr( inFunc_den->GetParameter(0) );
  TString dPar1 = DoubToStr( inFunc_den->GetParameter(1) );
  TString nPar0 = DoubToStr( inFunc_num->GetParameter(0) );
  TString nPar1 = DoubToStr( inFunc_num->GetParameter(1) );
  TString expression = "(" + nPar1 + "*x + " + nPar0 + ")/(" + dPar1 + "*x + " + dPar0 + ")";
  double left, right;
  inFunc_num->GetRange(left, right);
  TF1 *outFunc = new TF1(outFuncName, expression,left, right);
  outFunc->SetLineColor(inFunc_num->GetLineColor());
  cout << "Fit Ratio Function: " << expression << endl;
  return outFunc;
}

