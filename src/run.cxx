#include <iostream>
#include <fstream>
#include <omp.h>

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "FadcEvent.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>

using namespace std;

#define CHUNKSIZE 1





string dataFileFolder = "dataFiles";
string outHistFileFolder = "histFiles";
/// range of integration of baseline
double leftBaseLineLimit = 20;
double rightBaseLineLimit = 70;

/// range of maxValHistRight and maxValHistLeft histos
double leftSize = 0.003;
double rightSize =  0.03;

/// range for integration peak
double intRangeLeft = 0.0;
double intRangeRight= 5.0;
// double intRangeLeft = 0.01;
// double intRangeRight= 0.15;

string inFolder1 = "dataFiles";

int main(int argc,char *argv[]){

  if (argc!=3){
    cout << "Wrong number of input arguments: " << argc-1 << endl;
    cout << "Specify <2> arguments:" << endl;
    cout << "./run.exe <pattern> <run_name>" << endl;
    return -1;
  }
 
  vector<string> allDataFiles = list_files(inFolder1.c_str());
  vector<string> fileList1 = preselectList(argv[1],allDataFiles);
  
  const int nFiles = fileList1.size();
  int i;
  
  cout << "used pattern: *" << argv[1] << "*" << endl;
  cout << "preselected files:" << endl;
  for (int i=0; i<nFiles; i++){
    cout << i << ": " << fileList1[i] << endl;
  }
    
  int chunk = CHUNKSIZE;
  
  string fileName;
  TFile* f;
  TTree* tree;
  TObjArray* allBranches;
  TBranch *testBranch;
  FadcEvent *myEvent;
  Long64_t entries;

  TH1F* loopHist;
  TH1F* rndmHist;
  TF1* leftLineFunc;
  TF1* rightLineFunc;
  
  /// create histograms to write
  TH1F* baseLineLeft;
  TH1F* baseLineRight;
  TH1F* maxValHist;
  TH1F* maxValHistLeft;
  TH1F* maxValHistRight;
  TH1F* integralHist;
  TH1F* integralHistLeft;
  TH1F* integralHistRight;
  TH1F* baseLineIntegralLeft;
  TH1F* baseLineIntegralRight;
  TH1F* integralHistBoth;
  
  TFile* outFile;
  string jobName;
  TRandom3* rndGen;
    
  #pragma omp parallel for shared(fileList1) private(i,fileName,f,tree,allBranches,testBranch,myEvent,entries,loopHist,rndmHist,leftLineFunc,rightLineFunc,baseLineLeft,baseLineRight,maxValHist, maxValHistLeft,   maxValHistRight,   integralHist,   integralHistLeft,   integralHistRight,   baseLineIntegralLeft,   baseLineIntegralRight,   integralHistBoth,outFile,jobName,rndGen)
  
    for (i=0; i < fileList1.size(); i++){
      
      fileName = fileList1[i];
      
      /// open the file
      f = TFile::Open((dataFileFolder + "/" + fileName).c_str());
//       tree = reinterpret_cast<TTree*>(f->Get("fadc_event"));
//       if (tree==NULL){cout << "no tree found!\nTEMINATE!!!\n";}
/*
      /// get branch
      allBranches = tree->GetListOfBranches();
      if (allBranches==NULL){cout << "no branches found!\nTEMINATE!!!\n";}
      testBranch = reinterpret_cast<TBranch*>(allBranches->At(0));
      cout << "branch name = " << testBranch->GetName() << endl;

      /// create FadcEvent instance, set address to branch, get nEntries
      myEvent = 0;
      testBranch->SetAddress(&myEvent);
      entries = testBranch->GetEntries();
      cout << "number of entries in file: " << entries << endl;
      if (entries<=0){cout << "no entries found!\nTEMINATE!!!\n";}
      
      /// create service histos and funcs
      loopHist = NULL;
      rndmHist = NULL;
      leftLineFunc = new TF1("leftLineFunc","[0]",0,leftBaseLineLimit);
      rightLineFunc = new TF1("rightLineFunc","[0]",rightBaseLineLimit,80);
      
      /// create histograms to write
      baseLineLeft = new TH1F("baseLineLeft","baseLineLeft",100,0.045,0.055);
      baseLineRight = new TH1F("baseLineRight","baseLineRight",100,0.045,0.055);
      maxValHist = new TH1F("maxValHist","Peak maximum (wo baseline subtraction)",(int)(0.5/0.0005),0.00,0.5);
      maxValHistLeft = new TH1F("maxValHistLeft","Peak maximum (left baseline subracted)",(int)((rightSize-leftSize)/0.0001),leftSize,rightSize);
      maxValHistRight = new TH1F("maxValHistRight","Peak maximum (right baseline subracted)",(int)((rightSize-leftSize)/0.0001),leftSize,rightSize);
      integralHist = new TH1F("integralHist","Peak integral (wo baseline subtraction)",(int)(10.0/0.005),0.0,10.0);
      integralHistLeft = new TH1F("integralHistLeft","Peak integral (left baseline subracted)",(int)(10.0/0.005),0.0,10.0);
      integralHistRight = new TH1F("integralHistRight","Peak integral (right baseline subracted)",(int)(10.0/0.005),0.0,10.0);
      baseLineIntegralLeft = new TH1F("baseLineIntegralLeft","baseLineIntegralLeft",(int)(10.0/0.005),0.0,10.0);
      baseLineIntegralRight = new TH1F("baseLineIntegralRight","baseLineIntegralRight",(int)(10.0/0.005),0.0,10.0);
      integralHistBoth = new TH1F("integralHistBoth","Peak integral (with left & right baseline subtraction)",(int)((intRangeRight-intRangeLeft)/0.0025),intRangeLeft,intRangeRight);
      
      /// get job name based on input file name
      jobName = GetSplittedWords(fileName,".").at(0);
      
      /// create output file for histograms
      outFile = new TFile((outHistFileFolder + "/hists_" + jobName+ ".root").c_str(),"RECREATE");
      
      /// random generator
      rndGen = new TRandom3(0);
      
      cout << "rndGen->Rndm() = " << rndGen->Rndm() << endl;*/
      
//       /// FIXME hardcoded channel number
//       int iChannel = 1;
//       
//       /// loop over all entries
//       for(Long64_t i = 0; i < entries; i++) {
// 	/// get entry + if it is corrupted -> take new one
// 	testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
// 	if (myEvent->isCorrupt == true) continue;
// 	    
// 	/// get some values
// 	vector<vector<double> > fadcVec = myEvent->fadc_data;
// 	const int nPoints1 = fadcVec[iChannel].size();
// 	const int nPoints2 = fadcVec[1].size();
// 	double* firstChannel = fadcVec[iChannel].data();
// 	double* secondChannel = fadcVec[1].data();
// 	
// 	/// get pulse histogramm:
// 	loopHist = new TH1F("loopHist","loopHist",nPoints1,0,nPoints1);
// 	for (unsigned int k=0; k<nPoints1; k++){
// 	  loopHist->Fill(k+0.5,fadcVec[iChannel][k]); 
// 	}
// 	
// 	/// sort fadcVec vector to make last element the biggest
// 	std::sort(fadcVec[iChannel].begin(),fadcVec[iChannel].end());
// 	
// 	/// store random pulse
// 	if (rndmHist==NULL){
// 	  double rndNumber = rndGen->Rndm();
// 	  if (entries*rndNumber>entries*0.995){
// 	    rndmHist = (TH1F*)loopHist->Clone("randomPulse");
// 	    rndmHist->SetTitle("Random Pulse");
// 	    rndmHist->Write();
// 	  }
// 	}
// 	
// 	/// get ampitude
// 	double maxVal = fadcVec[iChannel][nPoints1-1];
// 	if (iChannel==1) maxVal = fadcVec[iChannel][0]; /// FIXME hardcoded for iChannel usage
// 	
// 	/// fill max amplitude histogram w/o baseline subtraction
// 	maxValHist->Fill(maxVal);
// 	
// 	/// fit baseline left/right from peak
// 	loopHist->Fit("leftLineFunc","QR");
// 	loopHist->Fit("rightLineFunc","QR");
// 	double leftBaseLine = leftLineFunc->GetParameter(0);
// 	double rightBaseLine = rightLineFunc->GetParameter(0);
// 	
// 	/// fill max amplitude histogram with left/right baseline subtraction
// 	maxValHistLeft->Fill(maxVal - leftBaseLine);
// 	maxValHistRight->Fill(maxVal - rightBaseLine);
// 	
// 	/// fill left/right baseline
// 	baseLineRight->Fill(rightBaseLine);
// 	baseLineLeft->Fill(leftBaseLine);
// 	
// 	/// count integral over peak + subtract baseline
// 	/// n'th bin has range: (n-1,n] 
// 	/// leftEdge -> bin number match with baseline fit function: 0..x - range; 1..x - bins
// 	/// rightEdge ->  bin number doesn't match: x-80 - range; (x+1)-80 bins
// 	double pulseIntegral = loopHist->Integral(1,80);
// 	double leftBaseLineIntegral = loopHist->Integral(1,leftBaseLineLimit);
// 	double rightBaseLineIntegral = loopHist->Integral(rightBaseLineLimit+1,80);
// 	
// 	/// fill integral hists
// 	integralHist->Fill(pulseIntegral);
// 	baseLineIntegralLeft->Fill(leftBaseLineIntegral);
// 	baseLineIntegralRight->Fill(rightBaseLineIntegral);
// 	double leftIntegral = pulseIntegral - leftBaseLineIntegral*(80./(leftBaseLineLimit-0.));
// 	double rightIntegral = pulseIntegral - rightBaseLineIntegral*(80./(80.-rightBaseLineLimit));
// 	double twoSideIntegral = pulseIntegral - (leftBaseLineIntegral+rightBaseLineIntegral)*(80./((leftBaseLineLimit-0.) + (80.-rightBaseLineLimit)));
// 	if (iChannel==1) twoSideIntegral = -twoSideIntegral; /// FIXME hardcodded to usage iChannel
// 	integralHistLeft->Fill(leftIntegral);
// 	integralHistRight->Fill(rightIntegral);
// 	integralHistBoth->Fill(twoSideIntegral);
// 	
// 	/// delete current pulse histogramm
// 	delete loopHist;
      
      
    }  
  
 
  
  return 0;

}
