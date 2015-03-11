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
#include<TStyle.h>
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

///------------------------------------------------------
/// MAIN FUNCTION
///------------------------------------------------------
int main(int argc, char* argv[]){
 
  /// get global input arguments:
  po::variables_map vm; 
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc,argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  /// get channel to read
  string fileName = vm["file"].as<std::string>();
  
  unsigned int xColumn = vm["x-axis"].as<unsigned int>();
  unsigned int yColumn = vm["y-axis"].as<unsigned int>();
  
  const unsigned int nPoints = GetDoubleColumnFromFile(fileName,yColumn).size();
  
  double* x = new double[nPoints];
  double* xErr = new double[nPoints];
  double* y = new double[nPoints];
  double* yErr = new double[nPoints];
  double* zeroArr = new double[nPoints];
  
  for (unsigned int i=0; i<nPoints; i++){
    zeroArr[i] = 0.0;
  }
  
  vector<double> xVec, xErrVec;
  
  vector<double> yVec = GetDoubleColumnFromFile(fileName,yColumn);
  vector<double> yErrVec = GetDoubleColumnFromFile(fileName,yColumn+1);

  if ( vm.count("xfile") ){ /// if we want to read x column from another file
    string xFileName = vm["xfile"].as<std::string>();
    xVec = GetDoubleColumnFromFile(xFileName,xColumn);
    xErrVec = GetDoubleColumnFromFile(xFileName,xColumn+1);
  }
  else{
    xVec = GetDoubleColumnFromFile(fileName,xColumn);
    xErrVec = GetDoubleColumnFromFile(fileName,xColumn+1);
  }
  
  /// convert from vectors to arrays
  x = &xVec[0];
  if ( vm.count("xErrors") )
    xErr = &xErrVec[0];
  else
    xErr = &zeroArr[0];
  y = &yVec[0];
  yErr = &yErrVec[0];
  
  /// scale y-axis
  if ( vm.count("scale") ){
    double scale = vm["scale"].as<double>();
    for (unsigned int i=0; i<nPoints; i++){
      y[i] = y[i] / scale;
      yErr[i] = yErr[i] / scale;
    }
  }
  
  /// make ratio for y-axis
  if ( vm.count("denominator") ){
    
    unsigned int denominatorColumn = vm["denominator"].as<unsigned int>();
    vector<double> yDenVec = GetDoubleColumnFromFile(fileName,denominatorColumn);
    vector<double> yDenErrVec = GetDoubleColumnFromFile(fileName,denominatorColumn+1);
    
    double* yDen = new double[nPoints];
    double* yDenErr = new double[nPoints];
    
    yDen = &yDenVec[0];
    yDenErr = &yDenErrVec[0];
    
    for (unsigned int i=0; i<nPoints; i++){
      y[i] = y[i] / yDen[i];
      yErr[i] = y[i] * sqrt( pow(yErr[i]/y[i],2) + pow(yDenErr[i]/yDen[i],2) );
    }
    
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
	x_afterRemoval[i - removedLines] = x[i];
	xErr_afterRemoval[i - removedLines] = xErr[i];
	y_afterRemoval[i - removedLines] = y[i];
	yErr_afterRemoval[i - removedLines] = yErr[i];
      }
      else{
	removedLines++;
      }
    }
    
    x = x_afterRemoval;
    xErr = xErr_afterRemoval;
    y = y_afterRemoval;
    yErr = yErr_afterRemoval;
    nPointsAfterRemoval = nPoints - removedLines;
  }
  
  TGraphErrors* outGraph = new TGraphErrors(nPointsAfterRemoval, x, y, xErr, yErr);
  
  /// get xfile header line:
  string xAxisHeader = "";
  if (vm.count("xfile")){
    string xFileName = vm["xfile"].as<std::string>();
    ifstream myfile (xFileName.c_str());
    if (myfile.is_open()){
      string tmpLine;
      getline (myfile,tmpLine); /// get header line
      char blablaSymbol = tmpLine[0];
      if (blablaSymbol == '#'){
	tmpLine.erase(tmpLine.begin(), tmpLine.begin()+1);
	std::vector<std::string> words;
	boost::split(words, tmpLine, boost::is_any_of("\t"));
	xAxisHeader = words[xColumn];
      }
    }
    myfile.close();
  }
  
  /// get header line:
  ifstream myfile (fileName.c_str());
  string xAxisTitle = "";
  string yAxisTitle = "";
  string histName = "";
  string headerLine = "";
  string tmpLine;
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,tmpLine); /// get header line
      char blablaSymbol = tmpLine[0];
      if (blablaSymbol == '#'){
	headerLine = "#"; /// overwrite header line
	tmpLine.erase(tmpLine.begin(), tmpLine.begin()+1);
	std::vector<std::string> words;
	boost::split(words, tmpLine, boost::is_any_of("\t"));
	if (words[0] == "TITLE" && words.size()==2){
	  histName = words[1];
	  continue;
	}
	else{
	  if (vm.count("xfile")) words[xColumn] = xAxisHeader;
	  headerLine += words[xColumn] + "\txError\t";
	  xAxisTitle = words[xColumn];
	  if ( vm.count("denominator") ){
	    unsigned int denominatorColumn = vm["denominator"].as<unsigned int>();
	    headerLine += words[yColumn] + "/" + words[denominatorColumn] + "\tyError\t";
	    if (histName=="")
	      histName = words[yColumn] + "/" + words[denominatorColumn] + " vs. " + words[xColumn];
	    yAxisTitle = words[yColumn] + "/" + words[denominatorColumn];
	  }
	  else{
	    headerLine += words[yColumn] + "\tyError\t";
	    if (histName=="")
	      histName = words[yColumn] + " vs. " + words[xColumn];
	    yAxisTitle = words[yColumn];
	  }
	}
	
      }
    }
  }
  
  myfile.close();
  
  /// dump graph
  string fileNamePrefix = GetFileName(fileName);
  string postFix = "";
  if (vm.count("xfile")) postFix += "_xfile";
  postFix += "_x" + DoubToStr(xColumn) + "_y" + DoubToStr(yColumn);
  if ( vm.count("denominator") ) postFix += "_d" + DoubToStr(vm["denominator"].as<unsigned int>());
  if ( vm.count("scale") ) postFix += "_s" + DoubToStr(vm["scale"].as<double>());
  DumpGraph(fileNamePrefix + postFix + "_dumpGraph.txt",outGraph,headerLine);
  
  /// plot graph
  /// TODO make plots!!!
  TCanvas* can = new TCanvas("can","can",800,800);
  SetGraphStyle(outGraph,kBlack,(histName+";"+xAxisTitle+";"+yAxisTitle).c_str());
  if (vm.count("y1") && vm.count("y2"))
    outGraph->GetYaxis()->SetRangeUser(vm["y1"].as<double>(),vm["y2"].as<double>());
  
  /// FIXME temprorary implementation! working only if x-axis is uniform!!!
  if (vm.count("xLabels")){
    /// style stuff
    gStyle->SetPadTickY(1); 
    gStyle->SetOptStat(0);

    vector<double> labels = GetDoubleColumnFromFile(fileName,vm["xLabels"].as<unsigned int>());

    TH1F* histToDraw = new TH1F("hist","hist",nPointsAfterRemoval,x[0]-1,x[nPointsAfterRemoval-1]);
    histToDraw->SetTitle((histName+";"+xAxisTitle+";"+yAxisTitle).c_str());
    for (int i=0; i<nPointsAfterRemoval; i++){
      histToDraw->SetBinContent(x[i],y[i]);
      histToDraw->SetBinError(x[i],0.0001);
      histToDraw->SetMarkerStyle(21);
      histToDraw->SetMarkerSize(0.9);
      histToDraw->GetXaxis()->SetBinLabel(x[i],DoubToStr(labels[i]).c_str());
      histToDraw->GetYaxis()->SetTitleOffset(1.25);
      
    }
    
    if (vm.count("y1") && vm.count("y2")){
      cout << "changing visibla part of Y axis to range: " << vm["y1"].as<double>() << ".." << vm["y2"].as<double>() << endl;
      histToDraw->GetYaxis()->SetRangeUser(vm["y1"].as<double>(),vm["y2"].as<double>());
    }
    
    histToDraw->Draw("E");
  }
  else
    outGraph->Draw("AP");
  
  can->SaveAs("plot.png");
  
//   delete[] x;
//   delete[] xErr;
//   delete[] y;
//   delete[] yErr;
  
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
      ("file,f", po::value<std::string>()->required(), "file name") /// mandatory option
      ("xfile", po::value<std::string>(), "additional file from which x column will be extracted") /// mandatory option
      ("x-axis,x", po::value<unsigned int>()->required(), "x_axis_column") /// mandatory option
      ("y-axis,y", po::value<unsigned int>()->required(), "y_axis_column") /// mandatory option
      ("xLabels,l", po::value<unsigned int>(), "x-axis custom labels")
      ("y1", po::value<double>(), "bottom visible Y range")
      ("y2", po::value<double>(), "upper visible Y range") 
      ("xErrors,e", "read out error for x-axis too; by default errors are asumed to be 0") /// mandatory option
      ("denominator,d", po::value<unsigned int>(), "y-axis denominator")
      ("scale,s", po::value<double>(), "scale factor for y-axis")
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

void SetGraphStyle(TGraph* inGr, Color_t inColor, const char* histTitle){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.95);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}