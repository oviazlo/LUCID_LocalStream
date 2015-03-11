///
/// time shift:
/// data-taking was done on 10kHz rate
/// average run time - 12 seconds
/// average middle run time after start script: 26 +- 2 seconds
/// files were renamed 1 minute and 1 or 2 second after starting of script
/// time shift = 35 +- 3 second
///


#include<TGraphErrors.h>
#include<TFile.h>
#include<TAxis.h>
#include<TGaxis.h>
#include<TF1.h>
#include<TH1F.h>
#include<TMath.h>
#include<TLegend.h>
#include<TLatex.h>
#include<TLine.h>
#include<TCanvas.h>
#include<TFrame.h>
#include<sstream>
#include<iostream>
#include <fstream>      /// std::ifstream
#include <TGraph.h>
#include <TGraphErrors.h>
#include <time.h>       /// time_t, struct tm, difftime, time, mktime

#include "serviceFunctions.h"
#include "boostServiceFunctions.h"

/// boost
#include "boost/program_options.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
  const size_t HELP_CALLED = 3;
  
  namespace po = boost::program_options;
 
} /// namespace 

///------------------------------------------------------ 
/// user functions
///------------------------------------------------------

/// parse input arguments
int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]);
// vector<string> GetTabulatedColumnFromFile(const string path, const unsigned int position);
// vector<double> GetTabulatedDoubleColumnFromFile(const string path, const unsigned int position);
void ScaleDoubleVec(vector<double> &inVec, double scaleFactor);
TGraph* GetGraph(vector<double> xVec, vector<double> yVec);
TGraphErrors* GetGraphErrors(vector<double> xVec, vector<double> yVec, vector<double> xVecErr, vector<double> yVecErr);
vector<double> ParseTimeVec(vector<string> inVec, const string initialTime, const bool timeFix = false);
void ParseTime(struct tm & timeStruct, const string timeString);
void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle);
int applyWindowCut(vector<double> &vecMain, vector<double> &vecAdditional, double val1, double val2);
int applyWindowCut(vector<double> &vecMain, vector<double> &vecAdditional, vector<double> &vecAdditional2, double val1, double val2);
void applyShiftToVector(vector<double> &shiftVec, double shiftVal);

void drawTwoPlots(TGraph *grLeft, TGraph *grRight, vector<double> lineToDraw);

po::variables_map vm; 

///------------------------------------------------------
/// MAIN FUNCTION
///------------------------------------------------------
int main(int argc, char* argv[]){
 
  /// get global input arguments:
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc,argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  /// get files
  string temperatureFileName = vm["temperatureFile"].as<std::string>();
  string integralFileName = vm["integralFile"].as<std::string>();
  
  /// get time-temp data
  vector<double> timeIndex = GetDoubleColumnFromFile(temperatureFileName,0);
  vector<double> tempIndex = GetDoubleColumnFromFile(temperatureFileName,1);
  if (vm.count("timeScale"))
    ScaleDoubleVec(timeIndex,vm["timeScale"].as<double>());
  else
    ScaleDoubleVec(timeIndex,15.0);
    
  if (vm.count("debug")) printSomeArrayElements(tempIndex,100,"\nPRINT EVERY 100 ELEMENT OF COLUMN 1 (TEMPERATURE) in TEMPERATURE FILE:");
  if (vm.count("debug")) printSomeArrayElements(timeIndex,100,"\nPRINT EVERY 100 ELEMENT OF COLUMN 0 (TIME_INDEX) in TEMPERATURE FILE:");
  
  /// apply window cut if necessary
  if (vm.count("timeWindow")){
    std::vector<std::string> words;
    boost::split(words, vm["timeWindow"].as<std::string>(), boost::is_any_of("-"));
    if (words.size()==2)
      applyWindowCut(timeIndex,tempIndex,atof(words[0].c_str()),atof(words[1].c_str()));
    else
      cout << "[main]\tERROR: not valid argument for timeWindow flag. It should be in format: t1-t2" << endl;
  }
  
  cout << timeIndex.size() << " time point were read\n";
  cout << tempIndex.size() << " temp point were read\n";
  
  if (vm.count("debug")) printSomeArrayElements(tempIndex,100,"\nPRINT EVERY 100 ELEMENT OF COLUMN 1 (TEMPERATURE) in TEMPERATURE FILE:");
  if (vm.count("debug")) printSomeArrayElements(timeIndex,100,"\nPRINT EVERY 100 ELEMENT OF COLUMN 0 (TIME_INDEX) in TEMPERATURE FILE:");
  
  /// get time-temp graph
  TGraph* timeGraph = GetGraph(timeIndex,tempIndex);
  
  /// fit time-temp dependence
  TF1 *myFunc = new TF1("timeTempFunc","pol19",timeIndex[0],timeIndex[timeIndex.size()-1]);
  TGraph* timeGraph2 = (TGraph*)timeGraph->Clone("timeGraph2");
  if (!vm.count("overview")) timeGraph->Fit("timeTempFunc","R");
  
  /// get start time of temperature data logging
  string tempStartDataLoggingTime = vm["startingTime"].as<std::string>();

  /// get integral-time data
  vector<string> tdaqTime = GetColumnFromFile(integralFileName,0);
  vector<double> integral = GetDoubleColumnFromFile(integralFileName,1);
  vector<double> integral_err = GetDoubleColumnFromFile(integralFileName,2);
  
  /// parse time to the time difference in seconds
  vector<double> doubTdaqTime = ParseTimeVec(tdaqTime,tempStartDataLoggingTime, vm.count("timeFix"));

  /// apply time shift
  if (vm.count("timeShift")){
  
    double timeShift = vm["timeShift"].as<double>();
    if (vm.count("debug")) cout << "applying time shift to the time-integral dependence: " << timeShift << " s" << endl;
    if (vm.count("debug")) cout << "sanity check of time shift - before: " << doubTdaqTime[0]; 
    applyShiftToVector(doubTdaqTime,timeShift);
    if (vm.count("debug")) cout << "; after - " << doubTdaqTime[0] << endl;
    
  }
  
  /// apply window cut if necessary
  if (vm.count("timeWindow")){
    std::vector<std::string> words;
    boost::split(words, vm["timeWindow"].as<std::string>(), boost::is_any_of("-"));
    if (words.size()==2)
      applyWindowCut(doubTdaqTime,integral,integral_err,atof(words[0].c_str()),atof(words[1].c_str()));
    else
      cout << "[main]\tERROR: not valid argument for timeWindow flag. It should be in format: t1-t2" << endl;
  }
  
  /// get time-integral dependence
  TGraph* integralGraph = GetGraph(doubTdaqTime,integral);
  
  /// get temperature at datataking nPoints
  vector<double> tempVsIntegral;
  vector<double> tempErr;
  vector<double> zeroArr;
  vector<double> testVec;
  
  for (int i=0; i<doubTdaqTime.size(); i++){
    tempErr.push_back(0.1); /// WARNING emperical value from 1st plot
    zeroArr.push_back(0.0);
  }

  /// get time-integral dependence with errors
  TGraphErrors* integralGraphError = GetGraphErrors(doubTdaqTime,integral, zeroArr, integral_err);
  
  for (int i=0; i<doubTdaqTime.size(); i++){
    tempVsIntegral.push_back(myFunc->Eval(doubTdaqTime[i]));
  }
  
  for (int i=0; i<timeIndex.size(); i++){

    if ((doubTdaqTime[0] <= timeIndex[i]) && (doubTdaqTime[doubTdaqTime.size()-1] >= timeIndex[i])) 
      testVec.push_back( abs(myFunc->Eval(timeIndex[i]) - tempIndex[i]) );
  }
  
  std::sort(testVec.begin(),testVec.end());
//   cout << "testVec[n-2] = " << testVec[testVec.size()-2] << "; testVec[n-1] = " << testVec[testVec.size()-1] << endl;
  
  /// get temp-integral dependence
  TGraph* integralTempGraph = GetGraph(tempVsIntegral,integral);
  
  /// get temp-integral dependence with errors
  TGraphErrors* integralTempGraph_Final = GetGraphErrors(tempVsIntegral,integral,tempErr,integral_err);
  TGraphErrors* integralTempGraph_Final_inverted = GetGraphErrors(integral,tempVsIntegral,integral_err,tempErr);
  
  /// make pictures to save
  TCanvas *can;
  if (vm.count("overview")){
    can = new TCanvas("can","can",1200,400);
    can->Divide(2,1);
  }
  else{
    can = new TCanvas("can","can",1200,800);
    if (vm.count("pinDiodeFile"))
      can->Divide(3,2);
    else
      can->Divide(2,2);
  }
  
  can->cd(1);
  SetGraphStyle(timeGraph,kBlack,"Temperature vs. time;time [s];temperature [C]");
  SetGraphStyle(timeGraph2,kBlack,"Temperature vs. time;time [s];temperature [C]");
  timeGraph->Draw("ALP");
  
  if (vm.count("verticalLine")){
   
    vector<double> linesToDraw = vm["verticalLine"].as<vector<double> >();
    
    for (int i=0; i<linesToDraw.size(); i++){
      TLine *someLine = new TLine(linesToDraw[i],-99999,linesToDraw[i],99999);
      someLine->Draw();   
    }
    
  }
   
  
  
  can->cd(2);
//   SetGraphStyle(integralGraph,kBlack,"Charge vs time;time [s];Charge [mV * ns]");
  SetGraphStyle(integralGraph,kBlack,"Amplitude vs time;time [s];Amplitude [mV]");
  SetGraphStyle(integralGraphError,kBlack,"Charge vs, time;time [s];Charge [mV * ns]");
  
//   integralGraph->Draw("ALP");
  integralGraphError->Draw("ALP");
  
  TF1* testFunc = new TF1("testFunc","pol1");
  integralGraphError->Fit("testFunc");
  
  cout << "Offset = " << testFunc->GetParameter(0) << endl;
  cout << "Slope = " << testFunc->GetParameter(1) << endl;
  
  if (vm.count("verticalLine")){
   
    vector<double> linesToDraw = vm["verticalLine"].as<vector<double> >();
    
    for (int i=0; i<linesToDraw.size(); i++){
      TLine *someLine = new TLine(linesToDraw[i],-99999,linesToDraw[i],99999);
      someLine->Draw();   
    }
    
    drawTwoPlots(integralGraph,timeGraph2,vm["verticalLine"].as<vector<double> >());
    
  }
  else{
    vector<double> emptyVec;
    drawTwoPlots(integralGraph,timeGraph2,emptyVec);
  }
  
  if (vm.count("overview")){
    can->SaveAs("timeTempGraph.png");
    can->SaveAs("timeTempGraph.eps");
    can->SaveAs("timeTempGraph.C");
    return 0;
  }
  
  can->cd(3);
  SetGraphStyle(integralTempGraph,kBlack,"Charge vs. Temperature;temperature, [C];Charge, [mV * 4ns]");
  integralTempGraph->Draw("ALP");
  
  TF1* linFunc = new TF1("linFunc","pol1",integral[0],integral[integral.size()-1]);
  SetGraphStyle(integralTempGraph_Final_inverted,kBlack,"Temperature vs. Charge;Charge, [mV * 4ns];temperature, [C]");
  integralTempGraph_Final_inverted->Draw("AP");
  integralTempGraph_Final_inverted->Fit("linFunc","R");
  
  cout << "Chi2/ndf = " << linFunc->GetChisquare() / linFunc->GetNDF() << endl;

  double qMax = (integral[integral.size()-1] > integral[0]) ? integral[integral.size()-1] : integral[0];
  double qMin = (integral[integral.size()-1] < integral[0]) ? integral[integral.size()-1] : integral[0];
  double qAve = 2.0 * abs(integral[integral.size()-1] - integral[0]) / (integral[integral.size()-1] + integral[0]);
  double temp1 = linFunc->Eval(integral[0]);
  double temp2 = linFunc->Eval(integral[integral.size()-1]);
  
  double slope = ((qMax - qMin) / qAve) / abs(temp1 - temp2);
  cout << "slope: " << slope << "; t1 = " << temp1 << "; t2 = " << temp2 << "; qMax = " << qMax << "; qMin = " << qMin << endl;

  can->cd(4);
  
  TF1* linFunc2 = new TF1("linFunc2","pol1",22.0,25.6);
  SetGraphStyle(integralTempGraph_Final,kBlack,"Charge vs. Temperature;temperature, [C];Charge, [mV * 4ns]");
  integralTempGraph_Final->Draw("AP");
//   integralTempGraph_Final->Fit("linFunc2");
//   linFunc2->Draw();
//   cout << "Chi2/ndf = " << linFunc2->GetChisquare() / linFunc2->GetNDF() << endl;
  
  if (vm.count("pinDiodeFile")){
    
    /// get start time of temperature data logging
    string pinDiodeFileName = vm["pinDiodeFile"].as<std::string>();

    /// get integral-time data
    vector<string> tdaqTime_pin = GetColumnFromFile(pinDiodeFileName,0);
    vector<double> integral_pin = GetDoubleColumnFromFile(pinDiodeFileName,1);
//     vector<double> integral_pin_err = GetDoubleColumnFromFile(pinDiodeFileName,2);
    
    /// parse time to the time difference in seconds
    vector<double> doubTdaqTime_pin = ParseTimeVec(tdaqTime_pin,tempStartDataLoggingTime, vm.count("timeFix"));
    
    /// get time-integral dependence
    TGraph* integralGraph_pin = GetGraph(doubTdaqTime_pin,integral_pin);
    
    /// draw PIN-diode plot
    can->cd(5);
    SetGraphStyle(integralGraph_pin,kBlack,"PIN-diode: Charge vs, time;time, [s];Charge, [mV * 4ns]");
    integralGraph_pin->Draw("ALP");
    
  }
  
  can->SaveAs("timeTempGraph.png");
  can->SaveAs("timeTempGraph.eps");
  can->SaveAs("timeTempGraph.C");
  
}

///------------------------------------------------------ 
/// user functions implementations
///------------------------------------------------------

/// parse input arguments
int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]){
  
  try 
  { 
    /** Define and parse the program options 
     */ 
    po::options_description desc("Options"); 
    desc.add_options()
      ("help,h", "Print help messages") 
      ("temperatureFile,t", po::value<std::string>()->required(), "temperature-time file name") /// mandatory option
      ("integralFile,i", po::value<std::string>()->required(), "integral-time file name") /// mandatory option
      ("startingTime,s", po::value<std::string>()->required(), "starting time of temp. data logging in format: Sep4_23:11:34") /// mandatory options
      ("pinDiodeFile,p",po::value<std::string>(),"file with PIN-diode results")
      ("verticalLine,v",po::value<std::vector<double> >(),"draw vertical line at")
      ("timeWindow,w",po::value<std::string>(),"use data which lays within specific time window, fomrat t1-t2")
      ("timeFix","crude time fix, add 3600.0 second to all")
      ("overview,o","don't make charge-temperature dependance")
      ("timeShift",po::value<double>(),"time shift in second for time-charg dependence")
      ("timeScale",po::value<double>(),"specify frequency of time loggind (default: 15 secons)")
      ("debug,d","pring DEBUG information")
      ;
    try 
    { 
      po::store(po::parse_command_line(argc, argv, desc),  
                vm); /// can throw 
 
      /** --help option 
       */ 
      if ( vm.count("help")  ) 
      { 
        std::cout << "Basic Command Line Parameter App" << std::endl 
                  << desc << std::endl; 
        return HELP_CALLED; 
      } 
      
      if ( vm.count("scale")  ) 
	if (vm["scale"].as<double>()==0){
	  
	  std::cout << "Scale factor can't be equal to 0" << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	  
	}

 
      po::notify(vm); /// throws on error, so do after help in case 
                      /// there are any problems 
    } 
    catch(po::error& e) 
    { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cerr << desc << std::endl; 
      return ERROR_IN_COMMAND_LINE; 
    } 
 
    /// application code here // 
    return SUCCESS;
 
  } 
  catch(std::exception& e) 
  { 
    std::cerr << "Unhandled Exception reached the top of main: " 
              << e.what() << ", application will now exit" << std::endl; 
    return ERROR_UNHANDLED_EXCEPTION; 
 
  } 
  
}






// vector<string> GetTabulatedColumnFromFile(const string path, const unsigned int position){
//  
//   vector<string> outVec;
// 
//   ifstream myfile (path.c_str());
//   string line;
//   bool notEmpty = false;
//   if (myfile.is_open())
//   {
//     while ( myfile.good() )
//     {
//       getline (myfile,line);
//       if (line=="") continue;
//       if (line.at(0) == '#') continue;
//       
//       std::vector<std::string> words;
//       boost::split(words, line, boost::is_any_of("\t"));
//       
//       if (words.size()>position){
// 	outVec.push_back(words[position]);
// 	notEmpty = true;
//       }
//       else{
// 	outVec.push_back("");
//       }
//     }
//     myfile.close();
//   }
//   else cout << endl << "WARNING!!! Unable to open file in GetWeights(): file: " << path << endl; 
//   if (notEmpty){ 
//     return outVec;
//   }
//   else{
//     vector<string> emptyVec;
//     return emptyVec;
//   }
// 
// }


// vector<double> GetTabulatedDoubleColumnFromFile(const string path, const unsigned int position){
//   
//   vector<double> outVec;
//   vector<string> stringVec = GetTabulatedColumnFromFile(path, position);
//   
//   for (int i=0; i<stringVec.size(); i++){
//     double tmpDouble = atof(stringVec[i].c_str());
//     outVec.push_back(tmpDouble);
//   }
//   
//   return outVec;
// }

void ScaleDoubleVec(vector<double> &inVec, double scaleFactor){

  std::transform(inVec.begin(), inVec.end(), inVec.begin(),
                std::bind2nd(std::multiplies<double>(), scaleFactor));

}

TGraph* GetGraph(vector<double> xVec, vector<double> yVec){
  const int nPoints = (xVec.size()>yVec.size()) ? yVec.size() : xVec.size();
  
  double* x = &xVec[0];
  double* y = &yVec[0];
  
  TGraph* outGr = new TGraph(nPoints,x,y);
  
  return outGr;
}

TGraphErrors* GetGraphErrors(vector<double> xVec, vector<double> yVec, vector<double> xVecErr, vector<double> yVecErr){
  const int nPoints = (xVec.size()>yVec.size()) ? yVec.size() : xVec.size();
  
  double* x = &xVec[0];
  double* y = &yVec[0];
  double* xErr = &xVecErr[0];
  double* yErr = &yVecErr[0];
  
  TGraphErrors* outGr = new TGraphErrors(nPoints,x,y,xErr,yErr);
  
  return outGr;
}

vector<double> ParseTimeVec(vector<string> inVec, const string initialTime, const bool timeFix){

  vector<double> vecOut;
  
  struct tm startTimeStruct;
  ParseTime(startTimeStruct,initialTime);
  struct tm iTimeStruct;
  
  double timeFixDouble = 0;
  if (timeFix)
    timeFixDouble = 3600.0;
  
  for (int i=0; i<inVec.size(); i++){
    ParseTime(iTimeStruct,inVec[i]);
    if (vm.count("debug")) cout << inVec[i] << " - " << initialTime << " --> ";
    vecOut.push_back(difftime(mktime(&iTimeStruct),mktime(&startTimeStruct)) + timeFixDouble);
    if (vm.count("debug")) cout << vecOut[i] << endl;
  }
  
  return vecOut;
  
}

void ParseTime(struct tm & timeStruct, const string timeString){
  
///  struct tm {
///   int tm_sec;         /* seconds,  range 0 to 59          */
///   int tm_min;         /* minutes, range 0 to 59           */
///   int tm_hour;        /* hours, range 0 to 23             */
///   int tm_mday;        /* day of the month, range 1 to 31  */
///   int tm_mon;         /* month, range 0 to 11             */
///   int tm_year;        /* The number of years since 1900   */
///   int tm_wday;        /* day of the week, range 0 to 6    */
///   int tm_yday;        /* day in the year, range 0 to 365  */
///   int tm_isdst;       /* daylight saving time             */	
///  };
  
  
  /// timeString format: Sep2_23:45:12
  std::vector<std::string> words;
  boost::split(words, timeString, boost::is_any_of("_"));
  
  string months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  int iMonth = -1;
  
  std::string inMonth (words[0], 0, 3);
  
  for (int i=0; i<12; i++){
    if  (inMonth == months[i]){
      iMonth = i;
      break; 
    }
  }
  
  words[0].erase(words[0].begin(), words[0].begin()+3);
  int iDay = atoi(words[0].c_str());
  
  string timeLine = words[words.size()-1];
  
  boost::split(words, timeLine, boost::is_any_of(":"));
  
  int iHour = atoi(words[0].c_str());
  int iMin  = atoi(words[1].c_str());
  int iSec  = atoi(words[2].c_str());
  
  if (vm.count("debug")) cout << timeString << " --> " << iMonth << "_" << iDay << "_" << iHour << ":" << iMin << ":" << iSec << endl;
  
  timeStruct.tm_hour = iHour;   timeStruct.tm_min = iMin; timeStruct.tm_sec = iSec;
  timeStruct.tm_year = 114; timeStruct.tm_mon = iMonth; timeStruct.tm_mday = iDay;
  timeStruct.tm_isdst = -1;
  
}

void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.8);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}



void drawTwoPlots(TGraph *tempGr, TGraph *intGr, vector<double> lineToDraw){

  double *xTemp = new double[tempGr->GetN()];
  double *xInt = new double[intGr->GetN()];
  xTemp = tempGr->GetX();
  xInt = intGr->GetX();
  
  double *yTemp = new double[tempGr->GetN()];
  double *yInt = new double[intGr->GetN()];
  yTemp = tempGr->GetY();
  yInt = intGr->GetY();
  
  double xmin =  (xTemp[0] < xInt[0]) ? xTemp[0] : xInt[0];
  double xmax =  (xTemp[tempGr->GetN()-1] < xInt[intGr->GetN()-1]) ? xTemp[tempGr->GetN()-1] : xInt[intGr->GetN()-1];
  
  double yTempMin = yTemp[TMath::LocMin(tempGr->GetN(),yTemp)];
  double yTempMax = yTemp[TMath::LocMax(tempGr->GetN(),yTemp)];
  double yTempRange = yTempMax - yTempMin;
  yTempMin = yTempMin - yTempRange*0.1;
  yTempMax = yTempMax + yTempRange*0.1;
  
  cout << "yTempMin = " << yTempMin << endl;
  cout << "yTempMax = " << yTempMax << endl;
  
  double yIntMin = yInt[TMath::LocMin(intGr->GetN(),yInt)];
  double yIntMax = yInt[TMath::LocMax(intGr->GetN(),yInt)];
  double yIntRange = yIntMax - yIntMin;
  yIntMin = yIntMin - yIntRange*0.1;
  yIntMax = yIntMax + yIntRange*0.1;
  
  cout << "yIntMin = " << yIntMin << endl;
  cout << "yIntMax = " << yIntMax << endl;
  
  cout << "yTempMax - yTempMin = " << yTempMax - yTempMin << endl;
  cout << "yIntMax - yIntMin = " << yIntMax - yIntMin << endl;
  cout << "charge [%] per degree = " << (100.0 * 2.0 * (yTempMax - yTempMin) / (yTempMax + yTempMin)) / (yIntMax - yIntMin) << endl;
  
  tempGr->SetMarkerColor(kRed);
  tempGr->SetMarkerStyle(21);
  intGr->SetMarkerColor(kBlack);
  intGr->SetMarkerStyle(21);
    
  /// draw canvas  
  TCanvas* c1 = new TCanvas("c1","gerrors2",200,10,700,500);
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetGrid();
  pad->Draw();
  pad->cd();

  /// draw a frame to define the range
  TH1F *hr = pad->DrawFrame(xmin,yIntMin,xmax,yIntMax);
  hr->SetXTitle(intGr->GetXaxis()->GetTitle());
  hr->SetYTitle(intGr->GetYaxis()->GetTitle());
  pad->GetFrame()->SetBorderSize(12);
  
  intGr->Draw("LP");
  
  

  ///create a transparent pad drawn on top of the main pad
  c1->cd();
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  overlay->Draw();
  overlay->cd();
  
  xmin = pad->GetUxmin();
  xmax = pad->GetUxmax();
  
  TH1F *hframe = overlay->DrawFrame(xmin,yTempMin,xmax,yTempMax);
  hframe->GetXaxis()->SetLabelOffset(99);
  hframe->GetYaxis()->SetLabelOffset(99);
  
  tempGr->Draw("LP");
  
  ///Draw an axis on the right side
  TGaxis *axis = new TGaxis(xmax,yTempMin,xmax, yTempMax,yTempMin,yTempMax,510,"+L");
  axis->SetTitle(tempGr->GetYaxis()->GetTitle());
  axis->SetTitleColor(kRed);
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->Draw();
  
  for (int i=0; i<lineToDraw.size(); i++){
    TLine *someLine = new TLine(lineToDraw[i],yTempMin,lineToDraw[i],yTempMax);
    someLine->SetLineColor(kRed);
    someLine->SetLineWidth(2);
    someLine->Draw();   
  }
  
  string outFileName = "2plots";
  
  if (vm.count("timeWindow")){
    outFileName += "_" + vm["timeWindow"].as<std::string>();
  }
    c1->SaveAs((outFileName + ".png").c_str());
    c1->SaveAs((outFileName + ".eps").c_str());
    c1->SaveAs((outFileName + ".cxx").c_str());
  
}

int applyWindowCut(vector<double> &vecMain, vector<double> &vecAdditional, double val1, double val2){
 
  /// two input vector should have the same number of elements
  /// and each pair of elements (one from each vector) with the same index correspond to one phase-space point
  /// 
  /// each element of vecMain vector are compared with window cut. If it is not in the window then erase this element and
  /// correponding element from vecAdditional
    
  if (vecMain.size()!=vecAdditional.size()){
    cout << "[applyWindowCut]\tERROR: sizes of two input vectors are not the same! Terminate execution of function" << endl;
    return -1;
  }
  
  double lowerLimit = (val1 > val2) ? val2 : val1;
  double upperLimit = (val1 > val2) ? val1 : val2;
  
//   cout << "lowerLimit = " << lowerLimit << endl;
//   cout << "upperLimit = " << upperLimit << endl;
  
  for (int i=vecMain.size()-1; i>=0; i--){
    if (vecMain[i]<lowerLimit || vecMain[i]>upperLimit){
//        cout << "erase point: [" << vecMain[i] << "," << vecAdditional[i] << "]" << endl;
       vecMain.erase(vecMain.begin()+i);
       vecAdditional.erase(vecAdditional.begin()+i);
    }
  }
  
  return 0;
  
}

int applyWindowCut(vector<double> &vecMain, vector<double> &vecAdditional, vector<double> &vecAdditional2, double val1, double val2){
 
  /// 3 input vector should have the same number of elements
  /// and each pair of elements (one from each vector) with the same index correspond to one phase-space point
  /// 
  /// each element of vecMain vector are compared with window cut. If it is not in the window then erase this element and
  /// correponding element from vecAdditional
    
  if (vecMain.size()!=vecAdditional.size() || vecMain.size()!=vecAdditional2.size()){
    cout << "[applyWindowCut]\tERROR: sizes of three input vectors are not the same! Terminate execution of function" << endl;
    return -1;
  }
  
  double lowerLimit = (val1 > val2) ? val2 : val1;
  double upperLimit = (val1 > val2) ? val1 : val2;
  
//   cout << "lowerLimit = " << lowerLimit << endl;
//   cout << "upperLimit = " << upperLimit << endl;
  
  for (int i=vecMain.size()-1; i>=0; i--){
    if (vecMain[i]<lowerLimit || vecMain[i]>upperLimit){
//        cout << "erase point: [" << vecMain[i] << "," << vecAdditional[i] << "]" << endl;
       vecMain.erase(vecMain.begin()+i);
       vecAdditional.erase(vecAdditional.begin()+i);
       vecAdditional2.erase(vecAdditional2.begin()+i);
    }
  }
  
  return 0;
  
}

void applyShiftToVector(vector<double> &shiftVec, double shiftVal){
  
  for (int i=0; i<shiftVec.size(); i++){
    shiftVec[i] = shiftVec[i] + shiftVal;
  }
  
}

