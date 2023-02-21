
typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_CALIFA_t califa;
} EXT_STR_h101;

void califa_calParFinder_Gabriel_14Mai2022() {
  TStopwatch timer;
  timer.Start();
  
  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  const Int_t fRunId = 1;
  Bool_t NOTstoremappeddata = false; // if true, don't store mapped data in the root file
  
  /* Create source using ucesb for input ------------------ */
  
  TString filename = "/lustre/r3b/califa/PuC_Aug2022_stitched/main0220_007*_stitched.lmd";
  TString outputFileName = "/lustre/r3b/califa/rootfiles/PuC_Aug2022/testcalib70.root";

   // Calibration files ------------------------------------
    TString dir = gSystem->Getenv("VMCWORKDIR");
   // Parameters for CALIFA mapping
    TString califamapfilename = "/u/land/r3broot/202205_s509/R3BParams_S509/califa/califamappar_v1.par";
    califamapfilename.ReplaceAll("//", "/");

  /* Create source using ucesb for input ------------------ */
  //UCESB paths
  TString ntuple_options = "RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  TString ucesb_path = ucesb_dir + "/../upexps/202205_s509/202205_s509  --allow-errors --input-buffer=180Mi";
  ucesb_path.ReplaceAll("//","/");

  EXT_STR_h101 ucesb_struct;
  
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack)));
					
    R3BCalifaFebexReader* unpackcalifa = new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa,
					     offsetof(EXT_STR_h101, califa));					
  unpackcalifa->SetOnline(NOTstoremappeddata);					     
  source->AddReader(unpackcalifa);

  
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(fRunId);
  run->SetSink(new FairRootFileSink(outputFileName));
  
  /* Runtime data base ------------------------------------ */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo(); // Ascii 
  parIo1->open(califamapfilename, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
  
  
  /* Add analysis task ------------------------------------ */
  //R3BCalifaMapped2CrystalCalPar ----
  TArrayF* EnergythePeaks = new TArrayF();
  //Float_t e1 = 1274.5;// 22Na
  //Float_t e2 = 511.0;// 22Na
  //Float_t e1=1332.5;//60Co
  //Float_t e2=1173.2;//60Co
  Float_t e1 = 6130.;// PuC
  Float_t e2 = 511.;// PuC
  EnergythePeaks->Set(2);
  EnergythePeaks->AddAt(e1,0);
  EnergythePeaks->AddAt(e2,1);
  
  R3BCalifaMapped2CrystalCalPar* CalPar = new R3BCalifaMapped2CrystalCalPar();
  CalPar->SetMinStadistics(1000);
  CalPar->SetNumParameterFit(2);//OPTIONAL by default 2
  //Gamma range
  CalPar->SetCalRange_left(200);  
  CalPar->SetCalRange_right(8200);
  CalPar->SetCalRange_bins(400);
  //particle range  
  CalPar->SetCalRangeP_left(20);//60  
  CalPar->SetCalRangeP_right(820);//130
  CalPar->SetCalRangeP_bins(400);//70   
  CalPar->SetSigma(3.0);
  CalPar->SetThreshold(0.0001);      
  CalPar->SetEnergyPeaks(EnergythePeaks);
  CalPar->SetDebugMode(1);
  run->AddTask(CalPar);


  /* Initialize ------------------------------------------- */
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");

  
  //Choose Root or Ascii file	
  //1-Root file with the Calibartion Parameters
  /*Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open("califa_CalibParamFeb2019.root");
  rtdb->setOutput(parOut);
  */
  //2-Ascii file with the Calibration Parameters
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open("Califa_CalPar_PuC_test.par","out");
  rtdb->setOutput(parIo2);


  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


  /* Save parameters (if needed) -------------------------- */
  rtdb->saveOutput();


  /* Finish ----------------------------------------------- */
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outputFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
       << endl << endl;
  gApplication->Terminate();
}
