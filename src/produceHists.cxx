/// WARNING Latest Development Logs
/// 
/// I was trying to imlement possibility to use Carla's files from "PMT equalization study". I have implemnented usage of different EventClass - Bi207Event
/// But when I try to run this script I got a following error-message:
/// Error in <TBufferFile::ReadVersion>: Could not find the StreamerInfo with a checksum of 0xf970000 for the class "vector<vector<double> >" in rawFiles/temp/out.root.
///
/// Probably it is because that file has one tree with a FEW leaves inside! While my typical file has only one branch in tree...
///
///


///
/// usage:
/// ./produceAllHits.py -c <channel> [OPTIONAL] -f <file> -p <polarity> -a <amplificationFactor>
/// channel:			integer
/// file:			file name (accept folders)
/// polarity:			pos/neg
/// amplificationFactor:	positive double
///

#include "../include/produceHists.h"
#define BIN_WIDTH 4.0
#define N_FADC_BINS 80

int parseOptionsWithBoost(po::variables_map &vm, int argc, char* argv[]){
  
  try 
  { 
    /** Define and parse the program options 
     */ 
    po::options_description desc("Options"); 
    desc.add_options()
      ("help,h", "Print help messages") 
      ("channel,c", po::value<int>()->required(), "which channel to read - {>=0}") /// mandatory option
      ("file,f", po::value<std::string>(), "file name pattern, support wildcards") 
      ("polarity,p", po::value<std::string>(), "polarity of the pulse - {pos,neg}")
      ("amplification,a", po::value<double>(), "amplification factor of used amplifier - {>0.0}")
      ("trigger,t", po::value<double>(), "software trigger in [mV]")
      ("bi207Cavern,b", "for data collected during 'PMT equalization study'")
      ("upTrigger", po::value<double>(), "software upTrigger in [mV] (defaul - 1V)"); 
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
      
      if ( vm.count("channel")  ) 
	if (vm["channel"].as<int>()<0){
	  
	  std::cout << "Only positive channels are accepted!!!" << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	  
	}
	
      if ( vm.count("amplification")  ) 
	if (vm["amplification"].as<double>()<=0.0){
	  
	  std::cout << "Only positive amplification are accepted!!!" << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	  
	}
      
      if ( vm.count("polarity")  ) 
	if (vm["polarity"].as<std::string>()!="pos" && vm["polarity"].as<std::string>()!="neg"){
	  
	  std::cout << "You specified wrong pulse polarity! Please choose between this 2 options: {pos,neg}" << std::endl;
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

int main(int argc, char* argv[]) {
  
  /// check if there are no environment variables set up
  GetEnv (dataFileFolder, "LUCID_DATA_FILES");
  GetEnv (outHistFileFolder, "LUCID_HIST_FILES");
  
  /// get global input arguments:
  const size_t returnedMessage = parseOptionsWithBoost(vm,argc, argv);
  if (returnedMessage!=SUCCESS) std::exit(returnedMessage);
  
  /// get channel to read
  iChannel = vm["channel"].as<int>();
  
  /// get pulse polarity
  if ( vm.count("polarity")  ){
    if (vm["polarity"].as<std::string>() == "pos")
      polarity = 1;
  }
  
  /// get amplification factor:
  if ( vm.count("amplification")  ){
    amplificationFactor = vm["amplification"].as<double>();
  }
  
  /// WARNING kludge: behaviour when DAC is 0x000:
  /// works only if user specife file with -f option!
  if ( vm.count("file") ){
    string fileName = vm["file"].as<std::string>();
    if (IsInWord(fileName,"0x000")) amplificationFactor = 1.0;
  }
    
  if (polarity==1){ /// PIN-diode
    chargeIntegrationRangeLeft = -0.15 * amplificationFactor;
    chargeIntegrationRangeRight= 0.15 * amplificationFactor;
    maxAmplRangeLeft = -0.2 * amplificationFactor;
    maxAmplRangeRight = 0.2 * amplificationFactor; /// max signal from PIN-diode - 200mV
  }
  else{ /// PMT
    chargeIntegrationRangeLeft = -1.0 * amplificationFactor;
    chargeIntegrationRangeRight= 1.0 * amplificationFactor;
    maxAmplRangeLeft = -0.12 * amplificationFactor;
    maxAmplRangeRight = 0.12 * amplificationFactor; /// max signal from PMT - 10x150mV
  }
  
  /// get file name choosen by user  
  string fileName;
  if ( !vm.count("file")  )  fileName = ChooseUserFile(dataFileFolder); /// ask user to specify file from list
  else{
   
    boost::filesystem::path fullPath(vm["file"].as<std::string>()); 
    
    if (fullPath.parent_path()!="")
      dataFileFolder = fullPath.parent_path().string();
    fileName = fullPath.filename().string();
    
    if (!boost::filesystem::exists( dataFileFolder + "/" + fileName )){
      cout << "No file *" << fileName << "* in *" << dataFileFolder << "* folder" << endl;
      return -1;
    }
  }
  
  cout << "input folder = " << dataFileFolder << endl;
  cout << "input fileName = " << fileName << endl;
  
  /// open the file
  TFile* f;
  if (GetSplittedWords(fileName,"/").size()>=2)
    f = TFile::Open(fileName.c_str());
  else
    f = TFile::Open((dataFileFolder + "/" + fileName).c_str());
  TTree* tree;
  if (!vm.count("bi207Cavern")){
    tree = reinterpret_cast<TTree*>(f->Get("fadc_event"));
  }
  else{
    tree = reinterpret_cast<TTree*>(f->Get("lucrod_event"));
  }
  
  if (tree==NULL){cout << "no tree found!\nTEMINATE!!!\n"; return -1;}

  /// get branch
  TObjArray* allBranches = tree->GetListOfBranches();
  if (allBranches==NULL){cout << "no branches found!\nTEMINATE!!!\n"; return -1;}
  testBranch = reinterpret_cast<TBranch*>(allBranches->At(0));

  /// create FadcEvent instance, set address to branch, get nEntries
  if (!vm.count("bi207Cavern")){
    testBranch->SetAddress(&myEvent);
  }
  else{
    testBranch->SetAddress(&bi207Event);
  }
  entries = testBranch->GetEntries();
  cout << "number of entries in file: " << entries << endl;
  if (entries<=0){cout << "no entries found!\nTEMINATE!!!\n"; return -1;}
  
  cout << endl;
  ///TEST
//   if (checkJitterSilent(fileName,iChannel)){
//     cout << "[INTERVAL_INFO]\tIntegration range to use: " << leftBaseLineLimit << ".." << rightBaseLineLimit << endl;
//   }
//   else{
//     cout << "[INTERVAL_ERROR]\tcheckJitterSilent FAILED to estimate boundaries... using default vaules: " << leftBaseLineLimit << ".." << rightBaseLineLimit << endl;
//   }
  
  return produceHists(fileName);

}



int produceHists(string fileName){
  
  /// create service histos and funcs
  TH1F* loopHist = NULL;
  
  /// get random hists and random numbers:
  const int nRndmHists = 100;
  map<double,bool> rndmNumberMap;
  int rndmHistCounter = 0;
  int tmpCounter = 1;
  
  /// random generator
  TRandom3* rndGen = new TRandom3(0);
  /// get nRndmHists random numbers
  for (int i=0; i<nRndmHists; i++){
    rndmNumberMap[int(rndGen->Rndm()*entries)] = true;
  }
    
  TF1* leftLineFunc = new TF1("leftLineFunc","[0]",0,leftBaseLineLimit);
  TF1* rightLineFunc = new TF1("rightLineFunc","[0]",rightBaseLineLimit,N_FADC_BINS);
  
  int nFadcBits = 4096;
  double amplRange = 1000.0;
  double fadcResolution = 2000.0/(double)nFadcBits;
  int maxIntegralBins = nFadcBits*80;
  int nIntBinsToUse = maxIntegralBins/8;
  double intHistRange = 1000.0*80.0;
  
  /// create histograms to write: amplitude
  double baseLineRangeDown = -10;
  double baseLineRangeUp = 100;
  double binWidthAmpl = int(2*0.5*amplificationFactor)/2.0; /// smallest step: 0.5mV
  TH1F* baseLineLeft = new TH1F("baseLineLeft","baseLineLeft",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineRight = new TH1F("baseLineRight","baseLineRight",nFadcBits,-amplRange,amplRange);
  
  TH1F* baseLineLeft10 = new TH1F("baseLineLeft10","baseLineLeft10",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineRight10 = new TH1F("baseLineRight10","baseLineRight10",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineLeft15 = new TH1F("baseLineLeft15","baseLineLeft15",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineRight15 = new TH1F("baseLineRight15","baseLineRight15",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineLeft20 = new TH1F("baseLineLeft20","baseLineLeft20",nFadcBits,-amplRange,amplRange);
  TH1F* baseLineRight20 = new TH1F("baseLineRight20","baseLineRight20",nFadcBits,-amplRange,amplRange);
  
  
  TH1F* maxValHist = new TH1F("maxValHist","Peak maximum (wo baseline subtraction)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistLeft = new TH1F("maxValHistLeft","Peak maximum (left baseline subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistRight = new TH1F("maxValHistRight","Peak maximum (right baseline subracted)",nFadcBits,-amplRange,amplRange);
  
  TH1F* maxValHistLeft10 = new TH1F("maxValHistLeft10","Peak maximum (left baseline10 subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistRight10 = new TH1F("maxValHistRight10","Peak maximum (right baseline10 subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistLeft15 = new TH1F("maxValHistLeft15","Peak maximum (left baseline15 subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistRight15 = new TH1F("maxValHistRight15","Peak maximum (right baseline15 subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistLeft20 = new TH1F("maxValHistLeft20","Peak maximum (left baseline20 subracted)",nFadcBits,-amplRange,amplRange);
  TH1F* maxValHistRight20 = new TH1F("maxValHistRight20","Peak maximum (right baseline20 subracted)",nFadcBits,-amplRange,amplRange);
  
  
  /// create histograms to write: charge
  TH1F* integralHist = new TH1F("integralHist","Peak integral (wo baseline subtraction)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistLeft = new TH1F("integralHistLeft","Peak integral (left baseline subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistRight = new TH1F("integralHistRight","Peak integral (right baseline subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  
  TH1F* baseLineIntegralLeft = new TH1F("baseLineIntegralLeft","baseLineIntegralLeft",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralRight = new TH1F("baseLineIntegralRight","baseLineIntegralRight",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistBoth = new TH1F("integralHistBoth","Peak integral (with left & right baseline subtraction)",nIntBinsToUse,-intHistRange,intHistRange);
  
  TH1F* integralHistLeft10 = new TH1F("integralHistLeft10","Peak integral (left baseline10 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistRight10 = new TH1F("integralHistRight10","Peak integral (right baseline10 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralLeft10 = new TH1F("baseLineIntegralLeft10","baseLine10IntegralLeft",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralRight10 = new TH1F("baseLineIntegralRight10","baseLine10IntegralRight",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistBoth10 = new TH1F("integralHistBoth10","Peak integral (with left & right baseline10 subtraction)",nIntBinsToUse,-intHistRange,intHistRange);
  
  TH1F* integralHistLeft15 = new TH1F("integralHistLeft15","Peak integral (left baseline15 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistRight15 = new TH1F("integralHistRight15","Peak integral (right baseline15 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralLeft15 = new TH1F("baseLineIntegralLeft15","baseLine15IntegralLeft",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralRight15 = new TH1F("baseLineIntegralRight15","baseLine15IntegralRight",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistBoth15 = new TH1F("integralHistBoth15","Peak integral (with left & right baseline15 subtraction)",nIntBinsToUse,-intHistRange,intHistRange);
  
  TH1F* integralHistLeft20 = new TH1F("integralHistLeft20","Peak integral (left baseline20 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistRight20 = new TH1F("integralHistRight20","Peak integral (right baseline20 subracted)",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralLeft20 = new TH1F("baseLineIntegralLeft20","baseLine20IntegralLeft",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* baseLineIntegralRight20 = new TH1F("baseLineIntegralRight20","baseLine20IntegralRight",nIntBinsToUse,-intHistRange,intHistRange);
  TH1F* integralHistBoth20 = new TH1F("integralHistBoth20","Peak integral (with left & right baseline20 subtraction)",nIntBinsToUse,-intHistRange,intHistRange);
  
  /// this hist contain 2 bins only. value of bins correspond to integration range used to make charge hists.
  TH1F* integrationRangeHist = new TH1F("integrationRangeHist","integrationRangeHist, contains 2 bins which correspond to integration range",2,0,2); 
  integrationRangeHist->SetBinContent(1,leftBaseLineLimit);
  integrationRangeHist->SetBinContent(2,rightBaseLineLimit);
  
  /// get job name based on input file name
  string jobName = GetSplittedWords(fileName,".").at(0);
  
  /// create output file for histograms
  string outFileName;
  outFileName = outHistFileFolder + "/hists_ch" + DoubToStr(iChannel) + "_" + jobName + ".root";
  if (vm.count("trigger")){
    outFileName = outHistFileFolder + "/hists_ch" + DoubToStr(iChannel) + "_" + jobName + "_t" + DoubToStr(vm["trigger"].as<double>()) + ".root"; 
  }
  TFile* outFile = new TFile(outFileName.c_str(),"RECREATE");
  
  
  
  /// loop over all entries
  for(Long64_t i = 0; i < entries; i++) {
        
    /// get entry + if it is corrupted -> take new one
    testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
    bool isCorrupt;
    if (!vm.count("bi207Cavern")){
      isCorrupt = myEvent->isCorrupt;
    }
    else{
      isCorrupt = bi207Event->isCorrupt;
    }
    
    if (isCorrupt == true){
      cout << "EVENT " << i << ":\tcorrupt event is found" << endl;
      continue;
    }
	
    /// get some values
    vector<vector<double> > fadcVec;
    
    if (!vm.count("bi207Cavern")){
      fadcVec = myEvent->fadc_data;
    }
    else{
      fadcVec = bi207Event->lucrod_data;
    }
    
    const int nPoints1 = fadcVec[iChannel].size();
    double* firstChannel = fadcVec[iChannel].data();
  
    if (nPoints1!=N_FADC_BINS){
      cout << "EVENT " << i << ":\tsize of pulse array is wrong!!!" << endl;
      continue;
    }
 
    /// TESTING 
    PulseAnalizer *myPulse = new PulseAnalizer(fadcVec[iChannel],leftBaseLineLimit,80-rightBaseLineLimit);
    if (vm.count("trigger"))
      if (myPulse->Trigger(vm["trigger"].as<double>()) == false) continue;
    if (vm.count("upTrigger")){
      if (myPulse->UpTrigger(vm["upTrigger"].as<double>()) == false) continue;
    }
    else{
      if (myPulse->UpTrigger(1000.0) == false) continue; /// default upTrigger
    }
     
    /// get pulse histogramm:
    loopHist = new TH1F("loopHist","loopHist",nPoints1,0,nPoints1);
    for (unsigned int k=0; k<nPoints1; k++){
      loopHist->SetBinContent(k+1,1000.0*fadcVec[iChannel][k]); 
//       loopHist->Fill(k+0.5,1000.0*fadcVec[iChannel][k]); 
    }
    
    /// check if we want to store randmHist for this entry
    if (rndmNumberMap[i]){
      TH1F* rndmHist = (TH1F*)loopHist->Clone(("randomPulse" + DoubToStr(rndmHistCounter)).c_str());
      rndmHist->SetTitle("Random Pulse");
      rndmHist->Write();
      rndmHistCounter++;
    }
    
    double absMaxVal;
    if (polarity==1)
      absMaxVal = myPulse->absMax;
    else
      absMaxVal = myPulse->absMin;
    
    /// fill max amplitude histogram w/o baseline subtraction
    maxValHist->Fill(polarity*absMaxVal); /// WARNING inverse this histogram if it is negative... Practically it is not amplitude, but min/max value from hist!

    /// fill max amplitude histogram with left/right baseline subtraction
    maxValHistLeft->Fill(polarity*(absMaxVal - myPulse->bsL));
    maxValHistRight->Fill(polarity*(absMaxVal - myPulse->bsR));
    
    maxValHistLeft10->Fill(polarity*(absMaxVal - myPulse->bsL10));
    maxValHistRight10->Fill(polarity*(absMaxVal - myPulse->bsR10));
    maxValHistLeft15->Fill(polarity*(absMaxVal - myPulse->bsL15));
    maxValHistRight15->Fill(polarity*(absMaxVal - myPulse->bsR15));
    maxValHistLeft20->Fill(polarity*(absMaxVal - myPulse->bsL20));
    maxValHistRight20->Fill(polarity*(absMaxVal - myPulse->bsR20));
    
    /// fill left/right baseline
    baseLineRight->Fill(myPulse->bsR);
    baseLineLeft->Fill(myPulse->bsL);
    
    baseLineRight10->Fill(myPulse->bsR10);
    baseLineLeft10->Fill(myPulse->bsL10);
    baseLineRight15->Fill(myPulse->bsR15);
    baseLineLeft15->Fill(myPulse->bsL15);
    baseLineRight20->Fill(myPulse->bsR20);
    baseLineLeft20->Fill(myPulse->bsL20);
    
    /// count integral over peak + subtract baseline
    /// n'th bin has range: (n-1,n] 
    /// leftEdge -> bin number match with baseline fit function: 0..x - range; 1..x - bins
    /// rightEdge ->  bin number doesn't match: x-80 - range; (x+1)-80 bins
    double pulseIntegral = BIN_WIDTH*myPulse->integral;
    double leftBaseLineIntegral = BIN_WIDTH*myPulse->bsL*myPulse->nLeftBSBins;
    double rightBaseLineIntegral = BIN_WIDTH*myPulse->bsR*myPulse->nRightBSBins;
    
    /// fill integral hists
    integralHist->Fill(pulseIntegral);
    baseLineIntegralLeft->Fill(leftBaseLineIntegral);
    baseLineIntegralRight->Fill(rightBaseLineIntegral);
    double leftIntegral = polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsL);
    double rightIntegral = polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsR);
    /// using weighted average of left and right baselines
    double weightedBaseline = (myPulse->bsL*myPulse->nLeftBSBins + myPulse->bsR*myPulse->nRightBSBins)/(myPulse->nRightBSBins+myPulse->nLeftBSBins);
    double twoSideIntegral = polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*weightedBaseline);
    integralHistLeft->Fill((leftIntegral));
    integralHistRight->Fill((rightIntegral));
    integralHistBoth->Fill((twoSideIntegral));
         
    /// fill baseline 10 histograms
    baseLineIntegralLeft10->Fill(BIN_WIDTH*myPulse->bsL10*10);
    baseLineIntegralRight10->Fill(BIN_WIDTH*myPulse->bsR10*10);
    integralHistLeft10->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsL10));
    integralHistRight10->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsR10));
    weightedBaseline = (myPulse->bsL10*10.0 + myPulse->bsR10*10.0)/(20.0);
    integralHistBoth10->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*weightedBaseline));
    
    /// fill baseline 15 histograms
    baseLineIntegralLeft15->Fill(BIN_WIDTH*myPulse->bsL15*15);
    baseLineIntegralRight15->Fill(BIN_WIDTH*myPulse->bsR15*15);
    integralHistLeft15->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsL15));
    integralHistRight15->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsR15));
    weightedBaseline = (myPulse->bsL15*15.0 + myPulse->bsR15*15.0)/(30.0);
    integralHistBoth15->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*weightedBaseline));
    
    /// fill baseline 20 histograms
    baseLineIntegralLeft20->Fill(BIN_WIDTH*myPulse->bsL20*20);
    baseLineIntegralRight20->Fill(BIN_WIDTH*myPulse->bsR20*20);
    integralHistLeft20->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsL20));
    integralHistRight20->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*myPulse->bsR20));
    weightedBaseline = (myPulse->bsL20*20.0 + myPulse->bsR20*20.0)/(40.0);
    integralHistBoth20->Fill(polarity*(pulseIntegral - N_FADC_BINS*BIN_WIDTH*weightedBaseline));
    
    if ((leftIntegral<0.0) && (tmpCounter<20)){
      TH1F* rndmHist = (TH1F*)loopHist->Clone(("negativeIntegralPulse" + DoubToStr(tmpCounter)).c_str());
      rndmHist->SetTitle("Random Pulse");
      rndmHist->Write();
      
      if (tmpCounter==1){
	cout << endl << "***" << endl;
	cout << "leftIntegral = " << leftIntegral << endl;
	cout << "pulseIntegral = " << pulseIntegral << endl;
	cout << "myPulse->bsL = " << myPulse->bsL << endl;
	cout << "N_FADC_BINS*BIN_WIDTH*myPulse->bsL = " << N_FADC_BINS*BIN_WIDTH*myPulse->bsL << endl;
	cout << "polarity = " << polarity << endl;
	cout << endl << "***" << endl;
      }

      tmpCounter++;
    }
    
    if (i<10){
      cout << endl << "absMaxVal = " << absMaxVal << endl;
      cout << "myPulse->bsL = " << myPulse->bsL << endl;
      cout << "myPulse->bsR = " << myPulse->bsR << endl;
      cout << "myPulse->nLeftBSBins = " << myPulse->nLeftBSBins << endl;
      cout << "myPulse->nRightBSBins = " << myPulse->nRightBSBins << endl;
      cout << "pulseIntegral = " << pulseIntegral << endl;
      cout << "leftIntegral = " << leftIntegral << endl;
      cout << "rightIntegral = " << rightIntegral << endl;
      cout << "twoSideIntegral = " << twoSideIntegral << endl;
    }
    
    /// delete current pulse histogramm
    delete loopHist;
  }
  
  /// write maxAmpl histograms to file:
  adjustHist(maxValHist);
  adjustHist(maxValHistLeft);
  adjustHist(maxValHistRight);
  adjustHist(baseLineLeft);
  adjustHist(baseLineRight);
  
  adjustHist(maxValHistLeft10);
  adjustHist(maxValHistRight10);
  adjustHist(baseLineLeft10);
  adjustHist(baseLineRight10);
  adjustHist(maxValHistLeft15);
  adjustHist(maxValHistRight15);
  adjustHist(baseLineLeft15);
  adjustHist(baseLineRight15);
  adjustHist(maxValHistLeft20);
  adjustHist(maxValHistRight20);
  adjustHist(baseLineLeft20);
  adjustHist(baseLineRight20);
  
  maxValHist->Write();
  maxValHistLeft->Write();
  maxValHistRight->Write();
  baseLineLeft->Write();
  baseLineRight->Write();
  
  maxValHistLeft10->Write();
  maxValHistRight10->Write();
  baseLineLeft10->Write();
  baseLineRight10->Write();
  maxValHistLeft15->Write();
  maxValHistRight15->Write();
  baseLineLeft15->Write();
  baseLineRight15->Write();
  maxValHistLeft20->Write();
  maxValHistRight20->Write();
  baseLineLeft20->Write();
  baseLineRight20->Write();
  
  /// write integral histograms to file:
  adjustHist(integralHist);
  adjustHist(baseLineIntegralLeft);
  adjustHist(baseLineIntegralRight);
  adjustHist(integralHistLeft);
  adjustHist(integralHistRight);
  adjustHist(integralHistBoth);
  
  adjustHist(baseLineIntegralLeft10);
  adjustHist(baseLineIntegralRight10);
  adjustHist(integralHistLeft10);
  adjustHist(integralHistRight10);
  adjustHist(integralHistBoth10);
  
  adjustHist(baseLineIntegralLeft15);
  adjustHist(baseLineIntegralRight15);
  adjustHist(integralHistLeft15);
  adjustHist(integralHistRight15);
  adjustHist(integralHistBoth15);
  
  adjustHist(baseLineIntegralLeft20);
  adjustHist(baseLineIntegralRight20);
  adjustHist(integralHistLeft20);
  adjustHist(integralHistRight20);
  adjustHist(integralHistBoth20);
  
  integrationRangeHist->Write();
  integralHist->Write();
  baseLineIntegralLeft->Write();
  baseLineIntegralRight->Write();
  integralHistLeft->Write();
  integralHistRight->Write();
  integralHistBoth->Write();
  
  baseLineIntegralLeft10->Write();
  baseLineIntegralRight10->Write();
  integralHistLeft10->Write();
  integralHistRight10->Write();
  integralHistBoth10->Write();
  
  baseLineIntegralLeft15->Write();
  baseLineIntegralRight15->Write();
  integralHistLeft15->Write();
  integralHistRight15->Write();
  integralHistBoth15->Write();
  
  baseLineIntegralLeft20->Write();
  baseLineIntegralRight20->Write();
  integralHistLeft20->Write();
  integralHistRight20->Write();
  integralHistBoth20->Write();
        
  /// close out file:
  outFile->Close();
  
  return 0;
}


void adjustHist(TH1F* inHist){
  double meanVal = inHist->GetMean();
  double rmsVal = inHist->GetRMS();
  double spread = 10*rmsVal;
  inHist->GetXaxis()->SetRangeUser(meanVal-spread,meanVal+spread);
}


bool checkJitterSilent(string fileName, int iChannel){
  
  /// create min/max pulse shape arrays
  double minVal[const_cast<const int&>(nSamplesADC)], maxVal[const_cast<const int&>(nSamplesADC)];
  int nGoodPulses = 0;
 
  TH1F* peakPositionHist = new TH1F("peakPositionHist","peakPositionHist",nSamplesADC,0,nSamplesADC);
  
  /// loop over all entries
  for(Long64_t i = 0; i < entries; i++) {
    /// get entry + if it is corrupted -> take new one
    testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
    bool isCorrupt;
    if (!vm.count("bi207Cavern")){
      isCorrupt = myEvent->isCorrupt;
    }
    else{
      isCorrupt = bi207Event->isCorrupt;
    }
    if (isCorrupt == true) continue;
    
    /// get some values
    if (!vm.count("bi207Cavern")){
      fadcVec = myEvent->fadc_data;
    }
    else{
      fadcVec = bi207Event->lucrod_data;
    }
    
    PulseAnalizer *myPulse = new PulseAnalizer(fadcVec[iChannel]);
    if (vm.count("trigger"))
      if (myPulse->Trigger(vm["trigger"].as<double>()) == false) continue;
    
    nGoodPulses++;
      
    if (nGoodPulses==1){
      for (unsigned int k=0; k<nSamplesADC; k++){
	minVal[k] = fadcVec[iChannel][k];
	maxVal[k] = fadcVec[iChannel][k];
      }
    }
    else{
      for (unsigned int k=0; k<nSamplesADC; k++){
	double currentVal = fadcVec[iChannel][k];
	if (currentVal > maxVal[k])
	  maxVal[k] = currentVal;
	if (currentVal < minVal[k])
	  minVal[k] = currentVal;
      }
    }
    
    peakPositionHist->Fill( GetIndexOfSmallestElement(fadcVec[iChannel]) + 0.5 );

  }
  
  peakPositionHist->Scale(1.0/peakPositionHist->GetMaximum());
  
  if (CheckForBadEvents(peakPositionHist)==false){
    
    nGoodPulses = 0; /// reset counter
    
    /// loop over all entries
    for(Long64_t i = 0; i < entries; i++) {
      /// get entry + if it is corrupted -> take new one
      testBranch->GetEntry(i); //tree->GetEntry(i) does not work. Why? Who knows!
      bool isCorrupt;
      if (!vm.count("bi207Cavern")){
	isCorrupt = myEvent->isCorrupt;
      }
      else{
	isCorrupt = bi207Event->isCorrupt;
      }
      if (isCorrupt == true) continue;
      nGoodPulses++;
	  
      /// get some values
      if (!vm.count("bi207Cavern")){
	fadcVec = myEvent->fadc_data;
      }
      else{
	fadcVec = bi207Event->lucrod_data;
      }
      
      double peakPos = GetIndexOfSmallestElement(fadcVec[iChannel]) + 0.5;
      if (peakPositionHist->GetBinContent(peakPos+1)<PEAK_POSITION_THRESHOLD){ 
// 	cout << "peakPos = " << peakPos << "; GetBinContent = " << peakPositionHist->GetBinContent(peakPos+1) << endl;
	continue;
      }
      
      if (nGoodPulses==1){
	for (unsigned int k=0; k<nSamplesADC; k++){
	  minVal[k] = fadcVec[iChannel][k];
	  maxVal[k] = fadcVec[iChannel][k];
	}
      }
      else{
	for (unsigned int k=0; k<nSamplesADC; k++){
	  double currentVal = fadcVec[iChannel][k];
	  if (currentVal > maxVal[k])
	    maxVal[k] = currentVal;
	  if (currentVal < minVal[k])
	    minVal[k] = currentVal;
	}
      }
      
    }
    
  }
  
  double pulseMiddle[const_cast<const int&>(nSamplesADC)];
  double pulseSpread[const_cast<const int&>(nSamplesADC)];
  double zeroArr[const_cast<const int&>(nSamplesADC)];
  
  for (unsigned int k=0; k<nSamplesADC; k++){
    pulseMiddle[k] = (maxVal[k] + minVal[k])/2.;
    pulseSpread[k] = maxVal[k] - pulseMiddle[k];
    zeroArr[k] = 0.0;
  }
  
//   can->cd(2);
  TH1F *jitterHist = new TH1F("jitterHist","jitterHist",nSamplesADC,0,nSamplesADC);
  TH1F *jitterHistYSpread = new TH1F("jitterHistYSpread","jitterHistYSpread",nSamplesADC,0,nSamplesADC);
  for (unsigned int k=0; k<nSamplesADC; k++){
    jitterHist->SetBinContent(k+1,pulseMiddle[k]);
    jitterHist->SetBinError(k+1,pulseSpread[k]);
    jitterHistYSpread->SetBinContent(k+1,pulseSpread[k]);
  }
  
  TH1F* derJitterHist = GetLeftRightThreshold(jitterHist);

  double minDer = derJitterHist->GetBinContent(1);
  double maxDer = derJitterHist->GetBinContent(1);
  
  for (unsigned int k=1; k<nSamplesADC-1; k++){
    double iDer = derJitterHist->GetBinContent(k+1);
    if (iDer > maxDer)
      maxDer = iDer;
    if (iDer < minDer)
      minDer = iDer;
  }
  
  derJitterHist->Scale(1.0/(maxDer-minDer));
  jitterHistYSpread->Scale(1.0/jitterHistYSpread->GetMaximum());
  
  int left = -1;
  int right = -1;
  
  for (int i=0;i<nSamplesADC-1;i++){
    double iDer = derJitterHist->GetBinContent(i+1);
    if (iDer<=(-DERIVATIVE_THRESHOLD)){
      if (i!=0)
	left = i;
      break;
    }
  }
  
  for (int i=nSamplesADC-2;i>=0;i--){
    double iDer = derJitterHist->GetBinContent(i+1);
    if (iDer>=DERIVATIVE_THRESHOLD){
      if (i!=nSamplesADC-2)
	right = i+1;
      break;
    }
  }
  
  TF1* spreadLeftFitFunc = new TF1("spreadLeftFitFunc","[0]",0,left-LEFT_SAFETY_MARGIN);
  jitterHistYSpread->Fit("spreadLeftFitFunc","R");
  double fitLeftSpread = spreadLeftFitFunc->GetParameter(0);
  
  int counter = 0;
  int polSwap = 1;
  
  for (unsigned int k=0; k<nSamplesADC; k++){
    double iBinVal = jitterHistYSpread->GetBinContent(k+1);
    if (iBinVal*polSwap>polSwap*fitLeftSpread*YSPREAD_FACTOR_THRESHOLD){
      counter++;
      polSwap *= -1;
    }
    if (counter==2){
      if (right<(k))
	right = k;
      break;
    }
  }
  
  /// sanity check of obtained integration boundaries:
  /// lenght of pulse-integration not less then 20 bins
  /// lenght of left/right baseline no less then 10 bins
  
  bool goodBoundaries = true;
  if 
  (
    (((right+RIGHT_SAFETY_MARGIN) - (left-LEFT_SAFETY_MARGIN))<20) ||
    ((80 - (right+RIGHT_SAFETY_MARGIN)) < 10) ||
    ((left-LEFT_SAFETY_MARGIN) < 10)
  )      
    goodBoundaries = false;
  
  
  if (goodBoundaries){
    leftBaseLineLimit = left-LEFT_SAFETY_MARGIN;
    rightBaseLineLimit = right+RIGHT_SAFETY_MARGIN;
    return true;
  }
  
  return false;
}

TH1F *GetLeftRightThreshold(TH1F* inHist){
  
  TH1F* outHist = new TH1F("jitterHistDerivative","jitterHistDerivative",nSamplesADC,0,nSamplesADC);
  outHist->SetBinContent(1,0.0);
  
  for (int i=0; i<nSamplesADC; i++){
  
    outHist->SetBinContent(i+2, (inHist->GetBinContent(i+2) - inHist->GetBinContent(i+1)) );
    
  }
  
  return outHist;
  
}
      
int GetIndexOfSmallestElement(std::vector<double> inArr, bool positivePulse)
{
    int index = 0;

    if (!positivePulse){
      for(int i = 1; i < inArr.size(); i++)
      {
	  if(inArr[i] < inArr[index])
	      index = i;              
      }
    }
    else{
      for(int i = 1; i < inArr.size(); i++)
      {
	  if(inArr[i] > inArr[index])
	      index = i;              
      }
    }
      
    return index;
}

bool CheckForBadEvents(TH1F* peakPosHist){ /// bad events - when peak position is far from 3 sigma from cente of peak pos. distribution 
  
  /// x-axis vals: 0..79; bins: 1-80    shift for 1
  for (int i=0; i<nSamplesADC; i++){
    double nBinVal = peakPosHist->GetBinContent(i+1);
    if (nBinVal <= PEAK_POSITION_THRESHOLD)
      return false;
  }
  
  return true;
  
}
