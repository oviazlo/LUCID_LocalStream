#include "../include/serviceFunctions.h"

vector<string> preselectList(const string& pat, vector<string> inVec){
  vector<string> outVec;
  for (int i=0; i<inVec.size(); i++){
    TString tmpString(inVec[i]);
    if (tmpString.Contains(pat)) outVec.push_back(inVec[i]);
  }
  return outVec;
}


vector<string> list_files(const char *dirname, const char *ext)
{
  vector<string> outVec;
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(ext)) {
	outVec.push_back(fname.Data());
      }
    }
  }
  sort(outVec.begin(), outVec.end());
  
  return outVec;
}

vector<string> GetSplittedWords(const string inputString, const string splitCharachters){
  
  string serviceString = replaceStrChar(inputString,splitCharachters,' ');
  return GetWords(serviceString);
}

vector<string> GetWords(const string inString){
  istringstream iss(inString);
  vector<string> tokens;
  copy(istream_iterator<string>(iss),
	  istream_iterator<string>(),
	  back_inserter<vector<string> >(tokens));
  
  return tokens;
}

string replaceStrChar(string str, const string& replace, char ch){

  // set our locator equal to the first appearance of any character in replace
  size_t found = str.find_first_of(replace);

  while (found != string::npos) { // While our position in the sting is in range.
    str[found] = ch; // Change the character at position.
    found = str.find_first_of(replace, found+1); // Relocate again.
  }

  return str; // return our new string.
}


void fitGaus(TH1F* inHist, TF1* inGausFunc){
  /// setup initial fit parameters
  double meanVal = inHist->GetMean();
  double rmsVal  = inHist->GetRMS();
  inGausFunc->SetParameter(1,meanVal);
  inGausFunc->SetParameter(2,rmsVal);
  
  /// fit by input gausFunc
  string funcName = inGausFunc->GetName();
  inHist->Fit(funcName.c_str());
}
/*
vector<string> GetColumnFromFile(const string path, const unsigned int position){
 
  vector<string> outVec;

  std::ifstream myfile (path.c_str(), std::ifstream::in);
  string line;
  bool notEmpty = false;
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      if (line=="") continue;
      if (line.at(0) == '#') continue;
      if (GetWords(line).size()>position){
	outVec.push_back(GetWords(line)[position]);
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

vector<double> GetDoubleColumnFromFile(const string path, const unsigned int position){
  
  vector<double> outVec;
  vector<string> stringVec = GetColumnFromFile(path, position);
  
  for (int i=0; i<stringVec.size(); i++){
    double tmpDouble = atof(stringVec[i].c_str());
    outVec.push_back(tmpDouble);
  }
  
  return outVec;
}*/

string ChooseUserFile(string inFolder, string ext){
  
  /// list all root files from inHistFileFolder folder
  /// ask user which file to use
  cout << "Available files from <" << inFolder << "> folder:" << endl;
  int iFile = -1;
  vector<string> availFileList = list_files(("./" + inFolder + "/").c_str(),ext.c_str());
  for (unsigned int i=0; i<availFileList.size(); i++){
    cout << i << ": " << availFileList[i] << endl;
  }
  cout << endl << "Choose file from above list:" << endl;
  cin >> iFile;
  if ((iFile==-1) || (iFile>=availFileList.size())){
    cout << "wrong file number!" << endl << "TERMINATE" << endl;
    return "";
  }
  string fileName = availFileList[iFile];
  cout << "Process file: " << fileName << endl;
  
  return fileName;
  
}

bool doFileExist(const string inFile, string inFold){
  string inFolder = inFold;

  char symbolToCompare = '/';
  char blablaSymbol = inFolder[inFolder.length()-1];

  if (blablaSymbol==symbolToCompare){
    string blablaFolder(inFolder,0,inFolder.length()-1);
    inFolder = blablaFolder;
  }
  
  blablaSymbol = inFolder[0];
  if (blablaSymbol==symbolToCompare){
    string blablaFolder(inFolder,1);
    inFolder = blablaFolder;
  }
 
  vector<string> words = GetSplittedWords(inFile,".");
  vector<string> availFileList = list_files(("./" + inFolder + "/").c_str(),words[1].c_str());
  if (!IsInVector<string>(inFile,availFileList)) return false;
  return true;
}

string GetFileExtention(const string fullFileName){
 
  vector<string> words = GetSplittedWords(fullFileName,".");
  int nSize = words.size();
  return words[nSize-1];
  
}

string GetFileName(const string fullFileName){
  
  vector<string> words = GetSplittedWords(fullFileName,"/");
  vector<string> words2 = GetSplittedWords(words[words.size()-1],".");
  
  return words2[0];
  
}

string mergeWords(vector<string> inVec, const string splitCharachters){
  
  if (inVec.size()==1)
    return inVec[0];
  
  string outStr = "";
  for (int i=0; i<inVec.size(); i++){
    outStr += inVec[i];
    if (i!=inVec.size()-1) outStr += splitCharachters;
  }
  return outStr;
  
}

bool IsInWord(string inWord1, string inWord2){
 
  string smallWord;
  string longWord;
  
  if (inWord1.size()<=inWord2.size()){
    smallWord = inWord1;
    longWord = inWord2;
  }
  else{
    smallWord = inWord2;
    longWord = inWord1;
  }
  
  if (strstr(longWord.c_str(), smallWord.c_str()) == NULL){
    return false; 
  }
  else{
    return true;
  }
  
}

string DoubToStr( double n ){
  std::ostringstream result;
  result << n;
  return result.str();
}

void DumpGraph(string outFileName, TGraphErrors* inGraph, string headerLine){
  
  fstream fitInfoIntegral (outFileName.c_str(),ios::out); /// create file (if exist - overwrite)
  if (headerLine!="")
    fitInfoIntegral << headerLine << endl;
  
  const int nPoints = inGraph->GetN();
  double* x = new double[nPoints];
  double* xErr = new double[nPoints];
  double* y = new double[nPoints];
  double* yErr = new double[nPoints];
  
  x = inGraph->GetX();
  xErr = inGraph->GetEX();
  y = inGraph->GetY();
  yErr = inGraph->GetEY();
  
  for (int i=0; i<nPoints; i++){
    fitInfoIntegral << x[i] << "\t" << xErr[i] << "\t" << y[i] << "\t" << yErr[i] << "\n"; 
  }
  
}

// TGraphErrors* GetDumpedGraph(string inFileName){
//   
//   const int nPoints = GetDoubleColumnFromFile(inFileName,0).size();
//   double* x = new double[nPoints];
//   double* xErr = new double[nPoints];
//   double* y = new double[nPoints];
//   double* yErr = new double[nPoints];
//   
//   vector<double> xVec = GetDoubleColumnFromFile(inFileName,0);
//   vector<double> xErrVec = GetDoubleColumnFromFile(inFileName,1);
//   vector<double> yVec = GetDoubleColumnFromFile(inFileName,2);
//   vector<double> yErrVec = GetDoubleColumnFromFile(inFileName,3);
//   
//   x = &xVec[0];
//   xErr = &xErrVec[0];
//   y = &yVec[0];
//   yErr = &yErrVec[0];
//   
//   TGraphErrors* outGraph = new TGraphErrors(nPoints, x, y, xErr, yErr);
//   
//   return outGraph;
//   
// }

void printSomeArrayElements(vector<double> inVec, unsigned int scale, string explanationSpring){
  
  /// foolproof check
  if (scale==0) scale = 1;
  
  if (explanationSpring!="")
    cout << explanationSpring << endl;
  
  for (int i=0; i<inVec.size(); i++){
    if ((i % scale) == scale-1) cout << i << ":\t" << inVec[i] << '\n';
  }

}

string GetEnv( const string & var ) {
     const char * val = ::getenv( var.c_str() );
     if ( val == 0 ) {
         return "";
     }
     else {
         return val;
     }
}

void GetEnv (string & inString, const string & var){
 
  string envString = GetEnv(var);
  if (envString!=""){
    cout << "[GetEnv]\tEnv. var. for " << var << " is found!" << endl;
    cout << "[GetEnv]\tChange string *" << inString << "* to *" << envString << "*" << endl;
    inString = envString;
  }
  
}

std::vector<std::string> glob(const std::string& pat){
    using namespace std;
    glob_t glob_result;
    glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
}


TGraphErrors* GetTGraphErrors(int nPoints, vector<double> x, vector<double> y, vector<double> ex, vector<double> ey){

	double* xA = &x[0];
	double* yA = &y[0];
	double* exA = &ex[0];
	double* eyA = &ey[0];
	
	return new TGraphErrors(nPoints,xA,yA,exA,eyA);
	
}


vector<TH1*> GetAllHistInFile (string inFile){

	vector<TH1*> outVec;
	
	TFile *f1 = TFile::Open(inFile.c_str());
	TIter next(f1->GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)next())) { 
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TH1")) continue;
		TH1 *h = (TH1*)key->ReadObj();
		outVec.push_back(h);
	}
	
	return outVec;
}
