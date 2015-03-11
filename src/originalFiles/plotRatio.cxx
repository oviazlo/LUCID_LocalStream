///
/// WARNING usage:
///
///
///
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

/// boost
#include "boost/program_options.hpp"
#include <boost/algorithm/string.hpp>

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
void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle = "");
TGraphErrors* getRatio(po::variables_map &vm, string file1, string file2);
vector<string> GetTabColumnFromFile(const string path, const unsigned int position);
vector<double> GetTabDoubleColumnFromFile(const string path, const unsigned int position);
void GetFitRange(TF1* fitLine, TGraph* inGraph);

Color_t colors[6] = {kBlack, kGreen, kRed, kBlue, kMagenta, kOrange};

///------------------------------------------------------
/// MAIN FUNCTION
///------------------------------------------------------
int main(int argc, char* argv[]){
 
  /// get global input arguments:
  po::variables_map vm; 
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc,argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  string listFileName = vm["list"].as<std::string>();
  vector<string> numFiles = GetTabColumnFromFile(listFileName,0);
  vector<string> denFiles = GetTabColumnFromFile(listFileName,1);
  vector<string> legTitles = GetTabColumnFromFile(listFileName,2);
  
  TF1* func = new TF1("myFunc","[0]");
  TLegend*leg = new TLegend(0.1,0.1,0.4,0.3);
  
  const int nPoints = numFiles.size();
  double fitPar[nPoints];
  double fitParErr[nPoints];
  
  double ratio[nPoints*(nPoints-1)/2];
  double ratioErr[nPoints*(nPoints-1)/2];
  string ratioTitle[nPoints*(nPoints-1)/2];
  
  /// plot graph
  /// TODO make plots!!!
  TCanvas* can = new TCanvas("can","can",800,800);
  
  for (int i=0; i<numFiles.size(); i++){
    
    TGraphErrors* gr = getRatio(vm,numFiles[i],denFiles[i]);
    func->SetLineColor(colors[i]);
    GetFitRange(func, gr);
    SetGraphStyle(gr,colors[i]);
    
//     gr->GetYaxis()->SetRangeUser(14,19);
    
    if (vm.count("fit")) gr->Fit("myFunc");
    
    if (i==0){
//       gr->GetXaxis()->SetRangeUser(0.0,0.11);
      gr->SetTitle("Q_{PMT}/Q_{PIN} vs. signal amplitude;ampl, [V];Q_{PMT}/Q_{PIN}");
      gr->Draw("ALP");
    }
    else{
      gr->Draw("LP");
    }
    
    fitPar[i] = func->GetParameter(0);
    fitParErr[i] = func->GetParError(0);
    
    leg->AddEntry(gr,("HV" + legTitles[i]).c_str(),"lep");
  }
  
  leg->Draw();
  can->SaveAs("plotRatio.png");

  
  /// print information about ration and HV estimation
  if (vm.count("fit")){
  
    double alpha = 8.26;
    double alphaErr = 0.04;
    
    cout << endl;
    cout << "alpha = " << alpha << " +- " << alphaErr << endl;
    cout << endl;
    
    
    int nRatioPoints = 0;
    for (int i=0; i<nPoints; i++){
      for (int j=i+1; j<nPoints; j++){
	ratio[nRatioPoints] = fitPar[j] / fitPar[i];
	ratioErr[nRatioPoints] = ratio[nRatioPoints] * sqrt( pow( fitParErr[i]/fitPar[i] ,2) + pow( fitParErr[j]/fitPar[j] ,2) );
	ratioTitle[nRatioPoints] = legTitles[j] + "/" + legTitles[i];
	cout << ratioTitle[nRatioPoints] << ": (HV1/HV2)^(alpha) = " << pow(atof(legTitles[j].c_str())/atof(legTitles[i].c_str()),alpha) << "; chargeRatio = " << ratio[nRatioPoints] << " +- " << ratioErr[nRatioPoints] << endl;
	nRatioPoints++;
      }
    }
    
    cout << "\nCalculate num. HV, using den. HV + ratio:\n";
    
    cout << "HV_num = HV_den * R^(1/alpha)\n";
    double HV_num, HV_numErr;
    double HV_numTrue;
    
    for (int i=0; i<nRatioPoints; i++){
      vector<string> words;
      boost::split(words, ratioTitle[i], boost::is_any_of("/"));
      double HV_den = atoi(words[1].c_str());
      HV_numTrue = atoi(words[0].c_str());
      HV_num = HV_den * pow(ratio[i],1/alpha);
      HV_numErr = (pow(ratio[i],1./(alpha-1))/alpha) * sqrt( ratioErr[i]*ratioErr[i] + pow(alphaErr/(alpha*alpha) ,2) );
      cout << "HV_den = " << HV_den << "; true HV_num = " << HV_numTrue << "; measured HV_num = " << HV_num << " +- " << HV_numErr << endl;
    }
  
  }
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
      ("list,l", po::value<std::string>()->required(), "list of txt-files which to draw") /// mandatory option
      ("x1", po::value<unsigned int>(), "x1_axis_column")
      ("y1", po::value<unsigned int>(), "y1_axis_column")
      ("x2", po::value<unsigned int>(), "x2_axis_column")
      ("y2", po::value<unsigned int>(), "y2_axis_column")
      ("fit,f","make fit with linear function")
      ("xErrors,e", "read out error for x-axis too, choose bigger error from 2 files; by default errors are asumed to be 0")
      ("removeLine,r", po::value<std::vector<unsigned int> >(), "skip line in file, line-comments are not taken into account, [0..nLines-1]");
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

void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.65);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}


TGraphErrors* getRatio(po::variables_map &vm, string file1, string file2){
 
  unsigned int x1Column = 0;
  unsigned int x2Column = 0;
  
  unsigned int y1Column = 1;
  unsigned int y2Column = 1;
  
  if (vm.count("x1") && vm.count("y1")){
    x1Column = vm["x1"].as<unsigned int>();
    y1Column = vm["y1"].as<unsigned int>();
  }
  if (vm.count("x2") && vm.count("y2")){
    x2Column = vm["x2"].as<unsigned int>();
    y2Column = vm["y2"].as<unsigned int>();
  }
  
  const unsigned int nPoints1 = GetTabDoubleColumnFromFile(file1,x1Column).size();
  const unsigned int nPoints2 = GetTabDoubleColumnFromFile(file2,x2Column).size();
  const unsigned int nPoints = (nPoints1 < nPoints2) ? nPoints1 : nPoints2;
  
  /// convert from vectors to arrays
  double* x1 = new double[nPoints];
  double* x1Err = new double[nPoints];
  double* y1 = new double[nPoints];
  double* y1Err = new double[nPoints];
  
  double* x2 = new double[nPoints];
  double* x2Err = new double[nPoints];
  double* y2 = new double[nPoints];
  double* y2Err = new double[nPoints];
  
  double* zeroArr = new double[nPoints];
  
  for (unsigned int i=0; i<nPoints; i++){
    zeroArr[i] = 0.0;
  }
  
  vector<double> x1Vec = GetTabDoubleColumnFromFile(file1,x1Column);
  vector<double> x1ErrVec = GetTabDoubleColumnFromFile(file1,x1Column+1);
  vector<double> y1Vec = GetTabDoubleColumnFromFile(file1,y1Column);
  vector<double> y1ErrVec = GetTabDoubleColumnFromFile(file1,y1Column+1);

  vector<double> x2Vec = GetTabDoubleColumnFromFile(file2,x2Column);
  vector<double> x2ErrVec = GetTabDoubleColumnFromFile(file2,x2Column+1);
  vector<double> y2Vec = GetTabDoubleColumnFromFile(file2,y2Column);
  vector<double> y2ErrVec = GetTabDoubleColumnFromFile(file2,y2Column+1);
  
//   for (int i=0; i<nPoints; i++){
//     cout << "x1[" << i << "] = " << x1Vec[i] << endl; 
//   } 
  
  x1 = &x1Vec[0];
  if ( vm.count("xErrors") )
    x1Err = &x1ErrVec[0];
  else
    x1Err = &zeroArr[0];
  y1 = &y1Vec[0];
  y1Err = &y1ErrVec[0];
  
  x2 = &x2Vec[0];
  if ( vm.count("xErrors") )
    x2Err = &x2ErrVec[0];
  else
    x2Err = &zeroArr[0];
  y2 = &y2Vec[0];
  y2Err = &y2ErrVec[0];
  
  /// make ratio for y-axis
  
  double* ratio = new double[nPoints];
  double* ratioErr = new double[nPoints];
  
  for (unsigned int i=0; i<nPoints; i++){
    ratio[i] = y1[i] / y2[i];
    ratioErr[i] = ratio[i] * sqrt( pow(y1Err[i]/y1[i],2) + pow(y2Err[i]/y2[i],2) );
    
//     cout << "y1[" << i << "] = " << y1[i] << "\ty2[" << i << "] = " << y2[i] << "\tratio[" << i << "] = " << ratio[i] << endl;
    
  }
    
  /// remove lines which were specified by user
  int nPointsAfterRemoval = nPoints;
  if ( vm.count("removeLine") ){
    int removedLines = 0;    
    double* x_afterRemoval = new double[nPoints];
    double* xErr_afterRemoval = new double[nPoints];
    double* y_afterRemoval = new double[nPoints];
    double* yErr_afterRemoval = new double[nPoints];
    
    vector<unsigned int> linesToRemove = vm["removeLine"].as<vector<unsigned int> >();
    
    for (unsigned int i=0; i<nPoints; i++){
      bool removeLine = false;
      for (unsigned int j=0; j<linesToRemove.size(); j++){
	if (i==linesToRemove[j]) removeLine = true;
      }
      if (!removeLine){
	x_afterRemoval[i - removedLines] = x1[i];
	xErr_afterRemoval[i - removedLines] = x1Err[i];
	y_afterRemoval[i - removedLines] = ratio[i];
	yErr_afterRemoval[i - removedLines] = ratioErr[i];
      }
      else{
	removedLines++;
      }
    }
    
    x1 = x_afterRemoval;
    x1Err = xErr_afterRemoval;
    ratio = y_afterRemoval;
    ratioErr = yErr_afterRemoval;
    nPointsAfterRemoval = nPoints - removedLines;
  }
  
  TGraphErrors* outGraph = new TGraphErrors(nPointsAfterRemoval, x1, ratio, x1Err, ratioErr);
  
  return outGraph;
  
}

vector<string> GetTabColumnFromFile(const string path, const unsigned int position){
 
  vector<string> outVec;

  ifstream myfile (path.c_str());
  string line;
  bool notEmpty = false;
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      if (line=="") continue;
      if (line.at(0) == '#') continue;
      
      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of("\t"));
      
      if (words.size()>position){
	outVec.push_back(words[position]);
	notEmpty = true;
      }
      else{
	outVec.push_back("");
      }
    }
    myfile.close();
  }
  else cout << endl << "WARNING!!! Unable to open file in GetTabColumnFromFile(): file: *" << path << "*" << endl; 
  if (notEmpty){ 
    return outVec;
  }
  else{
    vector<string> emptyVec;
    return emptyVec;
  }

}

vector<double> GetTabDoubleColumnFromFile(const string path, const unsigned int position){
  vector<string> tmpVec =  GetTabColumnFromFile(path,position);
  
  vector<double> outVec;
  for (int i=0; i<tmpVec.size(); i++){
    outVec.push_back( atof(tmpVec[i].c_str()) ); 
  }
  
  return outVec;
  
}

void GetFitRange(TF1* fitLine, TGraph* inGraph){
  /// TODO implement
//   double min = inGraph->GetXaxis()->Get 
}
