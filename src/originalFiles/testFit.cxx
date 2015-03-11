///
/// WARNING usage:
/// ./testFit.exe chargeRatio_July22.txt
///
/// file structure:
///
/// first column - root-file
/// second column - legend title
/// third column [optional] - which ratio to make  
///
/// example:
/// LEDTrig_HV700.root PMT@700V
/// LEDTrig_HV750.root PMT@750V
/// LEDTrig_HV800.root PMT@800V
/// LEDTrig_PIN_HV700.root PIN PMT@700V
/// LEDTrig_PIN_HV750.root PIN PMT@750V
/// LEDTrig_PIN_HV800.root PIN PMT@800V
///
///
/// WARNING Alternative usage:
/// ./testFit.exe chargeRatio_July22.txt
///
/// first column - txt-file
/// second column - legend title
/// third column [optional] - which ratio to make
/// TODO implement possibility to make ratio over constant value (simple scaling)
///
/// first line of file should have header line!
/// it will be used to set title for x- and y-axises
///

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
int testFit(vector<string> fileNames, vector<string>, vector<string>);
TGraph* GetRatioGr(string grName, TGraphErrors* inGr1, TGraphErrors* inGr2, double &accuracy);
string DoubToStr( double n );


int main(int argc, char* argv[]){
  
  vector<string> fileList;
  vector<string> legendTitles;
  vector<string> ratioInfo;
  
  if (argc>=2){
    string firstArgExtention = GetFileExtention(argv[1]);
    cout << "firstArgExtention = " << firstArgExtention << endl;
    if (argc==2 && firstArgExtention=="txt"){
      cout << "Read files from " << argv[1] << " text file" << endl;
      fileList = GetColumnFromFile(argv[1], 0);
      legendTitles = GetColumnFromFile(argv[1], 1);
      ratioInfo = GetColumnFromFile(argv[1], 2);
      for (int i=0; i<ratioInfo.size(); i++){
	cout << i << ": " << ratioInfo[i] << endl;
      } 
    }
    else{
      for (int i=1; i<argc; i++){
	fileList.push_back(argv[i]);
	legendTitles.push_back(argv[i]);
      }
    }
    
  }
  
  testFit(fileList,legendTitles,ratioInfo);
  return 0;
}

int testFit(vector<string> fileNames, vector<string> legendTitles, vector<string> ratioInfo){

if (fileNames.size() == 0){
  cout << "WARNING: no input files... Terminate" << endl;
  return -1;
}
  
TCanvas* can = new TCanvas("can","can",0,0,1000,1000);
can->Divide(1,1);
can->cd(1);  

Color_t colors[5] = {kBlack, kRed, kBlue, kGreen, kOrange};

vector<TGraphErrors*> graphs;
for (int i=0; i<fileNames.size(); i++){
  TGraphErrors *gr;
  string fileExtention = GetFileExtention(fileNames[i]);
  
  if (fileExtention=="root"){
    TFile* file = new TFile(fileNames[i].c_str(),"READ");
    gr = (TGraphErrors*)file->Get("calibCurve");
    file->Close();
  }
  if (fileExtention=="txt"){
     gr = GetDumpedGraph(fileNames[i]);
  }

  SetGraphStyle(gr,colors[i]);
//   gr->GetYaxis()->SetRangeUser(0,0.7);

  graphs.push_back(gr);
  
}

// can->cd(2);
TLegend* leg = new TLegend(0.1,0.7,0.68,0.9);

int dummyCounter = 0;

bool alreadyDrawn = false;

for (int i=0; i<graphs.size(); i++){
  
  double loopIndex = i+1;
  if (ratioInfo.size()!=0) loopIndex = 0;
  
  for (int j=loopIndex; j<graphs.size(); j++){
    if (ratioInfo.size()!=0){
      if (ratioInfo[j]!=legendTitles[i]) continue; 
      cout << "make ratio for: " << legendTitles[j] << "/" << legendTitles[i] << endl;
    }
    double accuracy;
    TGraph* ratioGr = GetRatioGr(("ratio_" + DoubToStr(i) + "_" + DoubToStr(j)).c_str(),graphs[i],graphs[j], accuracy);
    ratioGr->GetYaxis()->SetRangeUser(4.0,6.0);
    SetGraphStyle(ratioGr,colors[dummyCounter]);
    ratioGr->SetTitle("Charge ratio at different HV vs. DAC");
    ratioGr->GetXaxis()->SetTitle("LED DAC");
//     ratioGr->GetXaxis()->SetTitle("(mv@750V)");
    ratioGr->GetYaxis()->SetTitle("Q_{1}/Q_{2}");
    if (alreadyDrawn==false) {
      ratioGr->Draw("ALP");
      alreadyDrawn = true;
    }
    else{ 
      ratioGr->Draw("LP");
    }
    //cout << "j = " << j << endl;

    leg->AddEntry(ratioGr,(legendTitles[j] + " / " + legendTitles[i] + "; accuracy = " + DoubToStr(int(1000.0*accuracy)/10.0) + "%").c_str(),"lep");
    dummyCounter++;
  }
}

leg->Draw();

can->SaveAs("chargeRatio.png");
can->SaveAs("chargeRatio.eps");
can->SaveAs("chargeRatio.root");

}

void SetGraphStyle(TGraph* inGr, Color_t inColor){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.75);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
}

TGraph* GetRatioGr(string grName, TGraphErrors* inGr1, TGraphErrors* inGr2, double &accuracy){
  int nBins1 = inGr1->GetN();
  int nBins2 = inGr2->GetN();
  const int nBins = (nBins1 > nBins2) ? nBins1 : nBins2; 

  double *x = new double[nBins];
  double *x1 = inGr1->GetX();
  double *x2 = inGr2->GetX();
  double *y1 = inGr1->GetY();
  double *y2 = inGr2->GetY();
  double ratio[nBins];
  
  int nPointsToDraw = 0;
  
  
  for (int i=0; i<nBins1; i++){
    for (int j=0; j<nBins2; j++){
      if (x1[i] == x2[j]){
	x[nPointsToDraw] = x1[i];
	ratio[nPointsToDraw] = (double)(y2[j]/y1[i]);
        cout << nPointsToDraw+1 << ": x = " << x[i] << "; y1 = " << y1[i] << "; y2 = " << y2[j] << "; ratio = " << ratio[i] << endl;
	nPointsToDraw++;
      }
    }
  }
  
  double minRatio = ratio[0];
  double maxRatio = ratio[0];
  double averageRatio = ratio[0];  

  for (int i=1; i<nPointsToDraw; i++){
    averageRatio += ratio[i];
    if (ratio[i] < minRatio) minRatio = ratio[i];
    if (ratio[i] > maxRatio) maxRatio = ratio[i];
  }

  averageRatio = averageRatio/nPointsToDraw;
  
  accuracy = (maxRatio - minRatio) / ((maxRatio + minRatio) / 2.0);
  
  cout << "maxRatio = " << maxRatio << "; minRatio = " << minRatio << "; accuracy = " << accuracy*100.0 << "%" << endl;
  cout << "averageRatio = " << averageRatio << endl;
  
  TGraph* outGraph = new TGraph(nPointsToDraw,x,ratio);
  outGraph->SetName(grName.c_str());
  
  return outGraph;
}

string DoubToStr( double n ){
  std::ostringstream result;
  result << n;
  return result.str();
}
