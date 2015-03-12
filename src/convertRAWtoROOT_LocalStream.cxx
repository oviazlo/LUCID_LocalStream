#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TKey.h"
#include "TCanvas.h"
#include "serviceFunctions.h"
#include "TRandom3.h"
#include <iostream>
#include <algorithm>    // std::sort
#include <fstream>
#include <omp.h>
#include <lucrodRead.h>

using namespace std;

string rawFileFolder = "rawFiles";
string dataFileFolder = "dataFiles";

int main() {
  
  /// check if there are no environment variables set up
  GetEnv (rawFileFolder, "LUCID_RAW_FILES");
  GetEnv (dataFileFolder, "LUCID_DATA_FILES");
  
  double time = -omp_get_wtime();
  
  /// get ROOT and RAW file lists:
  vector<string> rootFileListPrim = list_files((dataFileFolder + "/").c_str(),".root");
  vector<string> rawFileListPrim =  list_files((rawFileFolder  + "/").c_str(),".data");
   
  cout << rootFileListPrim.size() << " files found in *" << dataFileFolder << "* folder" << endl;
  cout << rawFileListPrim.size() << " files found in *" << rawFileFolder << "* folder" << endl;
  
  vector<string> rootFileList;
  vector<string> rawFileList;
  
  /// remove extension from file names:
  for (unsigned int i=0; i<rootFileListPrim.size(); i++){
    vector<string> tmpVec = GetSplittedWords(rootFileListPrim[i],".");
    string nameToPush = "";
    for (int iWord=0; iWord<tmpVec.size()-1; iWord++){
      nameToPush += tmpVec[iWord];
      if (iWord!=(tmpVec.size()-2)) nameToPush += ".";
    }
    rootFileList.push_back(nameToPush);
  }
  for (unsigned int i=0; i<rawFileListPrim.size(); i++){
    vector<string> tmpVec = GetSplittedWords(rawFileListPrim[i],".");
    string nameToPush = "";
    for (int iWord=0; iWord<tmpVec.size()-1; iWord++){
      nameToPush += tmpVec[iWord];
      if (iWord!=(tmpVec.size()-2)) nameToPush += ".";
    }
    rawFileList.push_back(nameToPush);
  }
  
  
  /// check if RAW file was already converted
//   #pragma omp parallel for
  for (unsigned int i=0; i<rawFileList.size(); i++){
    cout << "\n\nLooking for file: ***" << rawFileList[i] << "***\n\n";
    if(!(std::find(rootFileList.begin(), rootFileList.end(), rawFileList[i]) != rootFileList.end())) {
      /// if it was not converted --> convert it!
      lucrodRead myRead = lucrodRead(rawFileFolder + "/" + rawFileList[i] + ".data",dataFileFolder + "/" + rawFileList[i] + ".root");
      myRead.read();
      cout << (rawFileFolder + "/" + rawFileList[i] + ".data") << "file was proceced\n";
    } 
  }
 
  time += omp_get_wtime();
  std::cout << "time = " << time << " seconds" <<std::endl;
 
  return 0;
 
}