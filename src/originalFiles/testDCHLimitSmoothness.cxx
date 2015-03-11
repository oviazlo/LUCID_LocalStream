
#include<TGraph.h>
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

int main(int argc, char* argv[]){
  
  if (argc==1){
    cout << "Please indicate at least one input file!" << endl;
    return -1;
  }
  
  vector<TGraph*> grVec;
  
  for (int i=1; i<argc; i++){
    vector<string> FirstColumn = GetColumnFromFile(argv[i], 0);
    vector<string> SecondColumn = GetColumnFromFile(argv[i], 1);
    vector<string> FifthColumn = GetColumnFromFile(argv[i], 4);
    
    double mass[999];
    double totalMC[999];
    int nPoints = 0;
    
    for (int i=0; i<FifthColumn.size(); i++){
      if (SecondColumn[i] == "massBin:"){
	mass[nPoints] = atof(FifthColumn[i].c_str());
      }
      if (FirstColumn[i] == "totalMC"){
	totalMC[nPoints] = atof(SecondColumn[i].c_str());
	nPoints++;
      }
    }
    
    TGraph*gr = new TGraph(nPoints, mass, totalMC);
    gr->GetXaxis()->SetTitle("Mass, [GeV]");
    gr->GetYaxis()->SetTitle("total prediction, [pairs]");
    gr->SetLineColor(i);
    gr->SetMarkerColor(i);
    gr->SetMarkerStyle(21);
    grVec.push_back(gr);
    
  }
  
  TLegend* leg = new TLegend(0.6,0.6,0.9,0.9);
  
  TCanvas* can = new TCanvas("can","can",0,0,1920,1200);
  
  for (int i=0; i<grVec.size(); i++){
   
    if (i==0){ 
      grVec[i]->Draw("ALP");
      grVec[i]->GetXaxis()->SetRangeUser(10,630);
      grVec[i]->GetYaxis()->SetRangeUser(0,250);
    }
    grVec[i]->Draw("LP");
    
    leg->AddEntry(grVec[i],( GetSplittedWords(argv[i+1],".")[0]).c_str(),"lep");
    
  }
  
  leg->Draw();
  
  
  
  can->SaveAs("dch_smoothness.png");
  
  return 0;
}