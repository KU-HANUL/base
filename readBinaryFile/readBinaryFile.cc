#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

int main( int argc, char* argv[] )
{
  TString inputFileName;
  TString nItemString;
  int     nItem;

  cout << endl << "=========================================================" << endl;
  cout << "Written by 정우 (phyjics@gmail.com)" << endl << endl;
  cout << "+++ NOTICE" << endl;
  cout << "    This script assume that each item contains" << endl;
  cout << "    12 bit information." << endl;
  if(argc!=3)
  {
    cout << "    You can run this script by giving 2 arguments." << endl;
    cout << "    - 1st : binary file to convert" << endl;
    cout << "    - 2nd : number of channel used" << endl << endl;

    cout << "+++ running manual input mode ..." << endl;
    cout << "    please write the name of the file to read : "; cin  >> inputFileName;
    cout << "    please write the number of item : "; cin >> nItemString;
    nItem = atoi(nItemString);
  }
  else
  {
    inputFileName = argv[1];
    nItem = atoi(argv[2]);
  }
  cout << endl;

  cout << "reading file " << inputFileName << endl;
  cout << "number of item : " << nItem << endl << endl;

  FILE *fp;
  fp=fopen(inputFileName.Data(),"r");

  int nItemEff   = nItem - 1;
  int data[100]  = {0};
  int count      =  0;
  int eventID    = -1;
  int preEventID = -1;

  TString outputFileName = inputFileName;
  outputFileName.ReplaceAll(".dat",".root");
  TString branchName = "data";

  TFile* file = new TFile(outputFileName.Data(),"recreate");
  TTree* tree = new TTree("data","");
  tree -> Branch("eventID",&data[0]);
  for(int iItem=0; iItem<nItemEff; iItem++)
    tree -> Branch(branchName+TString::Itoa(iItem,10),&data[1+iItem]);

  while(fread(data,sizeof(int),nItem,fp))
  {
    eventID=data[0];
    if(eventID <= preEventID)
    {
      cerr << "*** ERROR!" << endl;
      cerr << "    eventID is not increasing!" << endl;
      cerr << "    Last log : eventID / pre eventID >>> " << eventID << " / " << preEventID << endl;
      cout << "=========================================================" << endl;
      return 1;
    }
    preEventID=data[0];

    tree -> Fill();

    count++;
    if(count%10000==0) cout << "reading eventID : " << count << " ..." << endl;
  }
  tree -> Write();
  cout << "total number of event : " << count << endl;
  cout << endl << "output file " << outputFileName << " is written!" << endl;
  cout << "ending program." << endl;
  fclose(fp);
  cout << "=========================================================" << endl << endl;

  file -> Close();

  return 0;
}
