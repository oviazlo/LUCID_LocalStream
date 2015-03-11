#ifndef boostServiceFunctions_H
#define boostServiceFunctions_H

/// basic stuff:
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>      /// std::ifstream

/// boost
#ifndef __CINT__ /// needed for rootcint
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

/// ROOT stuff
#include <TGraphErrors.h>
#endif

using namespace std;

vector<string> GetColumnFromFile(const string path, const unsigned int position, const string delimiter = "\t");
vector<double> GetDoubleColumnFromFile(const string path, const unsigned int position, const string delimiter = "\t");

/// restore dumped graph
TGraphErrors* GetDumpedGraph(string inFileName);

#endif
