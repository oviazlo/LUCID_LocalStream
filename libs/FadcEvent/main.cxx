#include "FadcRead.h"

int main(int argc, char* argv[]) {

  /// create FadcEvent instance, set address to branch, get nEntries
  FadcRead myRead = FadcRead("test.data","test.root"); 
  myRead.Read();

}
