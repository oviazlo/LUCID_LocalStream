///
/// WARNING usage:
///
///
///
///

#include<TGraphErrors.h>
#include<TFile.h>
#include<TStyle.h>
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
void SetGraphStyle(TH1F* inGr, Color_t inColor, const char* histTitle = "");
string GetTitleLine(string fileName, int xColumn = 0, int yColumn = 2);
TH1F* GetHist(string inFileName, int xColumn, int yColumn, int labelColumn = -1);

Color_t colors[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kOrange};

vector<string> GetTabulatedColumnFromFile(const string path, const unsigned int position);

///------------------------------------------------------
/// MAIN FUNCTION
///------------------------------------------------------
int main(int argc, char* argv[]){
 
  /// get global input arguments:
  po::variables_map vm; 
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc,argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  vector<string> files;
  vector<string> legTitles;
  bool drawLegend = false;
  
  /// get channel to read
  if ( vm.count("file")  ){
    files = vm["file"].as<vector<std::string> >();  
  }
  
  if ( vm.count("list")  ){
    vector<string> fileList = GetTabulatedColumnFromFile(vm["list"].as<std::string>(), 0);
    files.insert(files.end(), fileList.begin(), fileList.end());
    legTitles = GetTabulatedColumnFromFile(vm["list"].as<std::string>(), 1);
    if (legTitles.size()==files.size()) drawLegend = true;
    /// TODO second columns in list -> titles for legend
  }
  
  if (files.size()==0){
    cout << "ERROR\tno input files specified\n";
    return -1;
  }
  
  string graphTitles = GetTitleLine(files[0]);
  
  TCanvas *can = new TCanvas("can","can",800,800);
  
  TLegend *leg = new TLegend(0.15,0.15,0.35,0.35);
  
  TF1 *myFunc;
  
  if ( vm.count("fit")){
    string range = vm["fit"].as<std::string>();
    vector<string> rangeVec;
    boost::split(rangeVec, range, boost::is_any_of("-"));
    double low = atof(rangeVec[0].c_str());
    double high = atof(rangeVec[1].c_str());
    myFunc = new TF1("myFunc","[0]",low,high);  
    myFunc->SetLineColor(kBlack);
  }
      
  for (unsigned int i=0; i<files.size(); i++){
    
    if (!vm.count("xLabels")){
    
      TGraphErrors* gr = GetDumpedGraph(files[i]);
      SetGraphStyle(gr,colors[i],graphTitles.c_str());
      
      if (i==0){
	if ( vm.count("yRange")  ){
	  string range = vm["yRange"].as<std::string>();
	  vector<string> rangeVec;
	  boost::split(rangeVec, range, boost::is_any_of("-"));
	  double low = atof(rangeVec[0].c_str());
	  double high = atof(rangeVec[1].c_str());
	  gr->GetYaxis()->SetRangeUser(low,high);
	}
	if ( vm.count("xRange")  ){
	  string range = vm["xRange"].as<std::string>();
	  vector<string> rangeVec;
	  boost::split(rangeVec, range, boost::is_any_of("-"));
	  double low = atof(rangeVec[0].c_str());
	  double high = atof(rangeVec[1].c_str());
	  gr->GetXaxis()->SetRangeUser(low,high);
	}
	if (drawLegend) leg->AddEntry(gr,legTitles[i].c_str(),"lep");
	gr->Draw("AP");
      }
      else{
	if (drawLegend) leg->AddEntry(gr,legTitles[i].c_str(),"lep");
	gr->Draw("P");
      }
      if ( vm.count("fit")) gr->Fit("myFunc","R");
    
    }
    
    /// FIXME buggy implementation
    else{
      
      /// style stuff
      gStyle->SetPadTickY(1); 
      gStyle->SetOptStat(0);
      
      TH1F* hist = GetHist(files[i], 0, 2, 1);
      graphTitles = GetTitleLine(files[0],0,2);
      SetGraphStyle(hist,colors[i],graphTitles.c_str());
      if (i==0){
	
	if ( vm.count("yRange")  ){
	  string range = vm["yRange"].as<std::string>();
	  vector<string> rangeVec;
	  boost::split(rangeVec, range, boost::is_any_of("-"));
	  double low = atof(rangeVec[0].c_str());
	  double high = atof(rangeVec[1].c_str());
	  hist->GetYaxis()->SetRangeUser(low,high);
	}
	
	if (drawLegend) leg->AddEntry(hist,legTitles[i].c_str(),"lep");
	hist->Draw("E");
      }
      else{
	if (drawLegend) leg->AddEntry(hist,legTitles[i].c_str(),"lep");
	hist->Draw("E same");
      }
      
    }
    
  }
  
  if (drawLegend) leg->Draw();
  
  can->SaveAs("plotOnSameCan.png");
  
  return 0;
  
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
      ("file,f", po::value<std::vector<string> >(), "file name")
      ("fit", po::value<string>(), "fit with line; fitting range in format: x1-x2")
      ("yRange,y", po::value<string>(), "yRange in format: y1-y2")
      ("xRange,x", po::value<string>(), "xRange in format: x1-x2")
      ("xLabels", po::value<unsigned int>(), "x-axis custom labels")
      ("list,l", po::value<std::string>(), "list of txt-files which to draw");
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
  inGr->SetMarkerSize(0.99);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}

void SetGraphStyle(TH1F* inGr, Color_t inColor, const char* histTitle){
  inGr->SetMarkerStyle(21);
  inGr->SetMarkerSize(0.99);
  inGr->SetMarkerColor(inColor);
  inGr->SetLineColor(inColor);
  inGr->SetTitle(histTitle);
}

string GetTitleLine(string fileName, int xColumn, int yColumn){

  /// get header line:
  ifstream myfile (fileName.c_str());
  string xAxisTitle = "";
  string yAxisTitle = "";
  string histName = "";
  string tmpLine;
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,tmpLine); /// get header line
      char blablaSymbol = tmpLine[0];
      if (blablaSymbol == '#'){
	tmpLine.erase(tmpLine.begin(), tmpLine.begin()+1);
	std::vector<std::string> words;
	boost::split(words, tmpLine, boost::is_any_of("\t"));
	if (words[0] == "TITLE" && words.size()==2){
	  histName = words[1];
	  continue;
	}
	else{
	  xAxisTitle = words[xColumn];
	  if (histName=="")
	    histName = words[yColumn] + " vs. " + words[xColumn];
	  yAxisTitle = words[yColumn];
	}
      }
    }
  }
  
  /// close file
  myfile.close();
  
  return histName + ";" + xAxisTitle + ";" + yAxisTitle;
  
}


TH1F* GetHist(string inFileName, int xColumn, int yColumn, int labelColumn){
  
  const int nPoints = GetDoubleColumnFromFile(inFileName,xColumn).size();

  vector<double> x = GetDoubleColumnFromFile(inFileName,xColumn);
  vector<double> y = GetDoubleColumnFromFile(inFileName,yColumn);
  vector<double> labels;

  if (labelColumn>=0){
    labels = GetDoubleColumnFromFile(inFileName,labelColumn);
  }
  
  TH1F *histToDraw = new TH1F(inFileName.c_str(),"hist",nPoints,x[0]-1,x[nPoints-1]);
  
  for (int i=0; i<nPoints; i++){
    histToDraw->SetBinContent(x[i],y[i]);
    histToDraw->SetBinError(x[i],0.0001);
    histToDraw->SetMarkerStyle(21);
    histToDraw->SetMarkerSize(0.9);
    if (labelColumn>=0)
      histToDraw->GetXaxis()->SetBinLabel(x[i],DoubToStr(labels[i]).c_str());
    histToDraw->GetYaxis()->SetTitleOffset(1.25);
  }
  
  return histToDraw;
  
}


vector<string> GetTabulatedColumnFromFile(const string path, const unsigned int position){
 
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
  else cout << endl << "WARNING!!! Unable to open file in GetWeights(): file: " << path << endl; 
  if (notEmpty){ 
    return outVec;
  }
  else{
    vector<string> emptyVec;
    return emptyVec;
  }

}


