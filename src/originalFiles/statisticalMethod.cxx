#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>

string inPngFolder = "histPicts";
bool badFile = false;
bool useAmplitudeInsteadDAC = true;

int proceed(string fileName);
void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle = "");

struct gausParStruct{
  
  int size;
  vector<double> dac;
  vector<double> mean;
  vector<double> meanErr;
  vector<double> sigma;
  vector<double> sigmaErr;
  vector<double> amplitude;
  
};

TGraphErrors* getSigma2AmplGraph(gausParStruct inGausParStruct, bool useAmplForXaxis = true);

int main(){

/// get file name choosen by user  
string fileName = ChooseUserFile(inPngFolder,".txt");
/// execute statistical method
proceed(inPngFolder + "/" + fileName);

}

int proceed(string fileName){
  
  vector<double> gausPars[16];

  vector<double> dac_val = GetDoubleColumnFromFile(fileName,0);
  if (dac_val.size()==0) badFile=true;
  for (int i=1; i<=16; i++){
    gausPars[i-1] = GetDoubleColumnFromFile(fileName,i);
    if (gausPars[i-1].size()==0) badFile=true;
  }

  if (badFile == true){
    cout << "ERROR\tBad input file: number of columns less then 17\n";
    return -1;
  }
  
  /// fill in gausParStruct's
  gausParStruct charge_fit;
  gausParStruct charge_get;
  gausParStruct amplitude_fit;
  gausParStruct amplitude_get;
  
  for (unsigned int i=0; i<dac_val.size(); i++){
    charge_fit.size = i+1;
    charge_fit.dac.push_back(dac_val[i]);
    charge_fit.mean.push_back(gausPars[0][i]);
    charge_fit.meanErr.push_back(gausPars[1][i]);
    charge_fit.sigma.push_back(gausPars[2][i]);
    charge_fit.sigmaErr.push_back(gausPars[3][i]);
    charge_fit.amplitude.push_back(gausPars[12][i]); /// if one want to use signal amolitude instead of DAC for X-axis
    
    charge_get.size = i+1;
    charge_get.dac.push_back(dac_val[i]);
    charge_get.mean.push_back(gausPars[4][i]);
    charge_get.meanErr.push_back(gausPars[5][i]);
    charge_get.sigma.push_back(gausPars[6][i]);
    charge_get.sigmaErr.push_back(gausPars[7][i]);
    charge_get.amplitude.push_back(gausPars[12][i]); /// if one want to use signal amolitude instead of DAC for X-axis
    
    amplitude_fit.size = i+1;
    amplitude_fit.dac.push_back(dac_val[i]);
    amplitude_fit.mean.push_back(gausPars[8][i]);
    amplitude_fit.meanErr.push_back(gausPars[9][i]);
    amplitude_fit.sigma.push_back(gausPars[10][i]);
    amplitude_fit.sigmaErr.push_back(gausPars[11][i]);
    amplitude_fit.amplitude.push_back(gausPars[12][i]); /// if one want to use signal amolitude instead of DAC for X-axis
    
    amplitude_get.size = i+1;
    amplitude_get.dac.push_back(dac_val[i]);
    amplitude_get.mean.push_back(gausPars[12][i]);
    amplitude_get.meanErr.push_back(gausPars[13][i]);
    amplitude_get.sigma.push_back(gausPars[14][i]);
    amplitude_get.sigmaErr.push_back(gausPars[15][i]);
    amplitude_get.amplitude.push_back(gausPars[12][i]); /// if one want to use signal amolitude instead of DAC for X-axis
  }
  
  /// get sigma^2/ampl graphs:
  TGraphErrors* gr_charge_fit = getSigma2AmplGraph(charge_fit,useAmplitudeInsteadDAC);
  TGraphErrors* gr_charge_get = getSigma2AmplGraph(charge_get,useAmplitudeInsteadDAC);
  TGraphErrors* gr_amplitude_fit = getSigma2AmplGraph(amplitude_fit,useAmplitudeInsteadDAC);
  TGraphErrors* gr_amplitude_get = getSigma2AmplGraph(amplitude_get,useAmplitudeInsteadDAC);
  
  /// check one of graph if it is not NULL
  if (gr_charge_fit==NULL || gr_charge_get==NULL || gr_amplitude_fit==NULL || gr_amplitude_get==NULL){
    cout << "ERROR\tone of graphs are NULL pointer! TERMINATE!\n";
    cout << "pointer gr_charge_fit = " << gr_charge_fit << endl;
    cout << "pointer gr_charge_get = " << gr_charge_get << endl;
    cout << "pointer gr_amplitude_fit = " << gr_amplitude_fit << endl;
    cout << "pointer gr_amplitude_get = " << gr_amplitude_get << endl;
    return -1;
  }
  
  /// draw graphs
  
  TCanvas* can = new TCanvas("can","can",1200,600);
  can->Divide(2,1);
  
  string histTitle_charge;
  string histTitle_amplitude;
  
  if (useAmplitudeInsteadDAC){
    histTitle_charge = "#sigma^{2}/mean: charge;amplitude, [mV];#sigma^{2}/mean";
    histTitle_amplitude = "#sigma^{2}/mean: max. amplitude;amplitude, [mV];#sigma^{2}/mean";
  }
  else{
    histTitle_charge = "#sigma^{2}/mean: charge;LED DAC;#sigma^{2}/mean";
    histTitle_amplitude = "#sigma^{2}/mean: max. amplitude;LED DAC;#sigma^{2}/mean";
  }
  
  SetGraphStyle(gr_charge_fit,kRed,histTitle_charge.c_str());
  SetGraphStyle(gr_charge_get,kBlack,histTitle_charge.c_str());
  SetGraphStyle(gr_amplitude_fit,kRed,histTitle_amplitude.c_str());
  SetGraphStyle(gr_amplitude_get,kBlack,histTitle_amplitude.c_str());
    
  can->cd(1);
  TLegend* leg = new TLegend(0.4,0.7,0.9,0.9);
  leg->AddEntry(gr_charge_fit,"Charge fitting","lep");
  leg->AddEntry(gr_charge_get,"Charge GetMean/RMS","lep");
  
  
  gr_charge_fit->GetYaxis()->SetRangeUser(0.0,0.01);
  gr_charge_fit->Draw("AP");
  gr_charge_get->Draw("P");
  leg->Draw("same");

  can->cd(2);
  leg = new TLegend(0.4,0.7,0.9,0.9);
  leg->AddEntry(gr_amplitude_fit,"Amplitude fitting","lep");
  leg->AddEntry(gr_amplitude_get,"Amplitude GetMean/RMS","lep");
  
  gr_amplitude_fit->GetYaxis()->SetRangeUser(0.0,0.005);
  gr_amplitude_fit->Draw("AP");
  gr_amplitude_get->Draw("P");
  leg->Draw("same");
  
  /// dump graphs to txt files:
  string headerLine;
  if (useAmplitudeInsteadDAC){
    headerLine = "#ampl, [mV]\tamplErr, [mV]\t#sigma^{2}/mean\t#sigma^{2}/mean error";
  }
  else{
    headerLine = "#LED DAC\tLED DAC err.\t#sigma^{2}/mean\t#sigma^{2}/mean error";
  }
  
  string jobName;
  vector<string> tmpWords = GetSplittedWords(fileName,"/");
  jobName = tmpWords[tmpWords.size() -1];
  tmpWords = GetSplittedWords(jobName,".");
  jobName = tmpWords[0];
  
  DumpGraph("chageFit_" + jobName + ".txt",gr_charge_fit,headerLine);
  DumpGraph("chageGet_" + jobName + ".txt",gr_charge_get,headerLine);
  DumpGraph("amplFit_" + jobName + ".txt",gr_amplitude_fit,headerLine);
  DumpGraph("amplGet_" + jobName + ".txt",gr_amplitude_get,headerLine);
  
  can->SaveAs("statMethod.png");
  
  return 0;
    
}



TGraphErrors* getSigma2AmplGraph(gausParStruct inGausParStruct, bool useAmplForXaxis){
  
  const int nPoints = inGausParStruct.size;
  
  double zeroArr[nPoints];
  double dac[nPoints];
  double amplitude[nPoints];
  double sigma2Ampl[nPoints];
  double sigma2AmplErr[nPoints];
  
  /// find which point correspond to DAC=0x000
  int zeroDACPoint = -1;
  for (int i=0; i<nPoints; i++){
    if (inGausParStruct.dac[i]==0){ 
      zeroDACPoint = i;
      break; 
    }
  }
  
  /// if no DAC zero point -> return NULL
  if (zeroDACPoint==-1){
    cout << "ERROR\tno point DAC=0x000\n";
    return NULL;
  }
  
  int nNonZeroPoints = 0;
  for (int i=0; i<nPoints; i++){
    if (i==zeroDACPoint) continue;
    zeroArr[nNonZeroPoints] = 0.0;
    dac[nNonZeroPoints] = inGausParStruct.dac[i];
    amplitude[nNonZeroPoints] = inGausParStruct.amplitude[i] * 1000;
    
    double mean = inGausParStruct.mean[i];
    double meanErr = inGausParStruct.meanErr[i];
    double sigma = inGausParStruct.sigma[i];
    double sigmaErr = inGausParStruct.sigmaErr[i];
    
    double meanPed = inGausParStruct.mean[zeroDACPoint];
    double meanPedErr = inGausParStruct.meanErr[zeroDACPoint];
    double sigmaPed = inGausParStruct.sigma[zeroDACPoint];
    double sigmaPedErr = inGausParStruct.sigmaErr[zeroDACPoint];
    
    /// (sigma^2 - sigma_pedestal^2)/(mean - mean_pedestal)
    sigma2Ampl[nNonZeroPoints] = (sigma*sigma - sigmaPed*sigmaPed) / (mean - meanPed);

    /// error of above formula:
    sigma2AmplErr[nNonZeroPoints] = sqrt(
      pow( 2*sigma*sigmaErr / (mean - meanPed) ,2) +
      pow( 2*sigmaPed*sigmaPedErr / (mean - meanPed) ,2) +
      pow( meanErr * (sigma*sigma - sigmaPed*sigmaPed) / ((mean - meanPed)*(mean - meanPed)) ,2) +
      pow( meanPedErr * (sigma*sigma - sigmaPed*sigmaPed) / ((mean - meanPed)*(mean - meanPed)) ,2)
    );
    nNonZeroPoints++;
  }
  
  TGraphErrors* outGr;
  if (useAmplForXaxis)
    outGr = new TGraphErrors(nNonZeroPoints,amplitude,sigma2Ampl,zeroArr,sigma2AmplErr);
  else
    outGr = new TGraphErrors(nNonZeroPoints,dac,sigma2Ampl,zeroArr,sigma2AmplErr);
  
  return outGr;
  
}

void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.65);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}