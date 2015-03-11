#include "../include/boostServiceFunctions.h"
#ifndef __CINT__ /// needed for rootcint

vector<string> GetColumnFromFile(const string path, const unsigned int position, const string delimiter){

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
      boost::split(words, line, boost::is_any_of(delimiter));
      
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
  else cout << endl << "[ERROR]\tUnable to open file in GetColumnFromFile() - file: " << path << endl; 
  if (notEmpty){ 
    return outVec;
  }
  else{
    vector<string> emptyVec;
    return emptyVec;
  }

}

vector<double> GetDoubleColumnFromFile(const string path, const unsigned int position, const string delimiter){
  
  vector<double> outVec;
  vector<string> stringVec = GetColumnFromFile(path, position, delimiter);
  
  for (int i=0; i<stringVec.size(); i++){
    double tmpDouble = atof(stringVec[i].c_str());
    outVec.push_back(tmpDouble);
  }
  
  return outVec;
}

TGraphErrors* GetDumpedGraph(string inFileName){
  
  const int nPoints = GetDoubleColumnFromFile(inFileName,0).size();
  double* x = new double[nPoints];
  double* xErr = new double[nPoints];
  double* y = new double[nPoints];
  double* yErr = new double[nPoints];
  
  vector<double> xVec = GetDoubleColumnFromFile(inFileName,0);
  vector<double> xErrVec = GetDoubleColumnFromFile(inFileName,1);
  vector<double> yVec = GetDoubleColumnFromFile(inFileName,2);
  vector<double> yErrVec = GetDoubleColumnFromFile(inFileName,3);
  
  x = &xVec[0];
  xErr = &xErrVec[0];
  y = &yVec[0];
  yErr = &yErrVec[0];
  
  TGraphErrors* outGraph = new TGraphErrors(nPoints, x, y, xErr, yErr);
  
  return outGraph;
  
}

#endif