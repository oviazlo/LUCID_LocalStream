
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

void makePlot(string histName, vector<string> relDistanceVec, vector<string> chargeVec, vector<string> chargeErrVec, bool makeFitting);
void SetGraphStyle(TGraph* inGr, Color_t inColor);

int main(int argc, char* argv[]){
  
  ///format: 21:05:Jun21
  vector<string> datatakingTime;
  
  vector<string> temperatureTime;
  vector<string> temperatureDate;
  vector<string> temperatureVal;
  vector<string> chargeVec;
  vector<string> chargeErrVec;
  
  vector<string> tempreratureForIntegral;
  
  string integralFile = "histPicts/temperatureScan_June21_Jun22_integral.txt";
  string temperatureFile = "/home/oviazlo/PhD_study/HW_project/LUCID/temperature_test/June21/test2.txt";
  
  vector<string> fileList = GetColumnFromFile(integralFile, 0);
  chargeVec = GetColumnFromFile(integralFile, 1);
  chargeErrVec = GetColumnFromFile(integralFile, 3);
  
  temperatureTime = GetColumnFromFile(temperatureFile, 2);
  temperatureDate = GetColumnFromFile(temperatureFile, 1);
  temperatureVal = GetColumnFromFile(temperatureFile, 5);
    
  cout << "All input files are read" << endl;
  
  /// extract distance from file names:
  for (int i=0; i<fileList.size(); i++){

    vector<string> dummyWords = GetSplittedWords(fileList[i],"_");
    string dummyWord = dummyWords[ dummyWords.size()-1 ];
    vector<string> dummyWords2 = GetSplittedWords(dummyWord,":");
    string dummyWord2 = dummyWords2[0] + ":" + dummyWords2[1];
    string dummyWord3 = dummyWords[ dummyWords.size()-2 ]; /// date in format: Jun21
    
    datatakingTime.push_back(dummyWord2 + ":" + dummyWord3); /// format: 21:05:Jun21
    
  }
  
  cout << endl << "start temperature-integral relation!" << endl;
  
  for (int i=0; i<temperatureTime.size(); i++){
    if (temperatureTime[i] == "") continue;
    
    vector<string> dummyWords = GetSplittedWords(temperatureTime[i],":");
    vector<string> dummyWords01 = GetSplittedWords(temperatureDate[i],".");
   
    string tempTimeStamp = dummyWords[0] + ":" + dummyWords[1] + ":Jun" + dummyWords01[0]; /// format: 21:05:Jun21
    
    for (int j=0; j<datatakingTime.size(); j++){
      
      vector<string> dummyWords2 = GetSplittedWords(datatakingTime[j],":");
      double dataTakingTime_hour = atoi(dummyWords2[0].c_str());
      double dataTakingTime_min = atoi(dummyWords2[1].c_str());
      
      dataTakingTime_min -= 2; /// time correction for data taking
      
      string dataTakingTimeStamp;
      
      if (dataTakingTime_hour<10 && dataTakingTime_min<10){
	dataTakingTimeStamp = "0" + DoubToStr(dataTakingTime_hour) + ":" + "0" + DoubToStr(dataTakingTime_min) + ":" + dummyWords2[2];
      }
      else{
	if (dataTakingTime_hour<10){
	  dataTakingTimeStamp = "0" + DoubToStr(dataTakingTime_hour) + ":" + DoubToStr(dataTakingTime_min) + ":" + dummyWords2[2];  
	}
	else{
	  if (dataTakingTime_min<10){
	    dataTakingTimeStamp = DoubToStr(dataTakingTime_hour) + ":" + "0" + DoubToStr(dataTakingTime_min) + ":" + dummyWords2[2];
	  }
	  else{
	    dataTakingTimeStamp = DoubToStr(dataTakingTime_hour) + ":" + DoubToStr(dataTakingTime_min) + ":" + dummyWords2[2];
	  }
	}
      }
     

      if (dataTakingTimeStamp == tempTimeStamp){
	tempreratureForIntegral.push_back(temperatureVal[i]);
	datatakingTime.erase(datatakingTime.begin()+j);
      }
      
    }
  }
  
  makePlot("temperature test",tempreratureForIntegral,chargeVec,chargeErrVec,false);
}


void makePlot(string histName, vector<string> relDistanceVec, vector<string> chargeVec, vector<string> chargeErrVec, bool makeFitting){

  TCanvas*can = new TCanvas("can","can",0,0,1000,1000);
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
    dist[i] = atof(relDistanceVec[i].c_str());
    charge[i] = atof(chargeVec[i].c_str());
    chargeErr[i] = atof(chargeErrVec[i].c_str());
    
    cout << "temp = " << dist[i] << "; charge = " << charge[i] << endl;
    
  }
  
  TGraphErrors* tmpGr = new TGraphErrors(nPoints,dist,charge,zeroArr,chargeErr);
  SetGraphStyle(tmpGr,kBlack);

  tmpGr->SetTitle(histName.c_str());
  
  tmpGr->GetXaxis()->SetTitle("temperature, [C]");
  tmpGr->GetYaxis()->SetTitle("charge, [V*4ns]");
  
  tmpGr->Draw("AP");
  
  can->SaveAs("tempTest.png");
  
}

void SetGraphStyle(TGraph* inGr, Color_t inColor){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.75);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
}
