
#ifndef serviceFunctions_H
#define serviceFunctions_H

#include<TSystemDirectory.h>
#include<TList.h>
#include<TSystemFile.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <fstream>
#include <Rtypes.h>
#include <TFile.h>
#include <TKey.h>
#include <dirent.h> 
#include <stdio.h> 
#include <TH1F.h> 
#include <TF1.h> 
#include <TGraphErrors.h> 

/// BOOST libs:
// #include <boost/filesystem.hpp>

using namespace std;

/// print every 2nd, 3rd,.. element (scale define how much to skip)
void printSomeArrayElements(vector<double> inVec, unsigned int scale, string explanationSpring = "");

/// get list of files in directory
vector<string> list_files(const char *dirname="./", const char *ext=".root");

/// preselect items from input list which satisfy specific pattern
vector<string> preselectList(const string& pat, vector<string> inVec);

/// Split string to words. Delimeter - space
vector<string> GetWords(const string inString);

/// split string to words
/// to use if split chrachter is not space
vector<string> GetSplittedWords(const string inputString, const string splitCharachters);

string GetFileExtention(const string fullFileName);

/// example: ./someFolder/fileName.exe -> return fileName
string GetFileName(const string fullFileName);

/// replace charachter in string
string replaceStrChar(string str, const string& replace, char ch);

/// fit hist by input inGausFunc
/// output of GetMean and GetRMS ae used as initial fit parameters
void fitGaus(TH1F* inHist, TF1* inGausFunc);

/// get column of strings from file
// vector<string> GetColumnFromFile(const string path, const unsigned int position);

/// get double column of strings from file
// vector<double> GetDoubleColumnFromFile(const string path, const unsigned int position);

string mergeWords(vector<string> inVec, const string splitCharachters);

/// list all files in folder with spesific extension
/// ask user to chose one files 
/// return file name
string ChooseUserFile(string inFolder, string ext=".root");

/// check if one smaller word is part of bigger one
bool IsInWord(string inWord1, string inWord2);

/// convert double to string
string DoubToStr( double n );

/// check if element in in vector
template<class T>
bool IsInVector(T inElement, vector<T> checkedVector){
  if(std::find(checkedVector.begin(), checkedVector.end(), inElement) != checkedVector.end()) return true;
  else return false;
}

/// check if file exist in specified folder
bool doFileExist(const string inFile, const string inFolder);

/// dump graph to the txt file
/// it will overwrite the output file!
void DumpGraph(string outFileName, TGraphErrors* inGraph, string headerLine = "");

/// restore dumped graph
// TGraphErrors* GetDumpedGraph(string inFileName);

/// get environment variable
string GetEnv( const string & var );
void GetEnv (string & inString, const string & var);

#endif