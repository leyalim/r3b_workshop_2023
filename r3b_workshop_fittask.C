/*const int cMaxId = 4867;
  first iphos crystal 1953 (in iphos)--> first p-crystal (in iphos): 1953+2432=4385 (4384 empty)
  last p-crystal (in iphos):4864, 2432
  last gamma crystal 2432 (2433 is empty)
  const int cMinId = 928; first barrel crystal Messel Ring 3

  bad crsytals: 2024/4456, 2044/4476, 2369/4801, 2030/4462
  disabled crystals: 2023/4455, 2030/4462, 2070/4502, 2182/4614, 2345/4777*/

const int cMinId = 945;
const int cMaxId = 2865;

void run()
{
    TFile *file_in = TFile::Open("pucout_histos_Na22_1to10_245_1keV.root");
    TCanvas *cv = new TCanvas("cv","spectrum",900,900);
    TH1D* hEcrys[cMaxId]{};
    
    for (int i=cMinId; i<cMaxId; i++)
    {
        cv->cd();
        std::cout<<"\n crystal ID: "<<i<<endl;
        hEcrys[i] = (TH1D*)file_in->FindObjectAny(Form("mapEcrys_%04i",i));
        if (!hEcrys[i] || hEcrys[i]->GetEntries()==0)
        {
            std::cout << "histogram for crystal Id " << i << " not found or empty, channel ignored." << endl;
            continue;
        }
        hEcrys[i]->Draw();
        gPad->SetLogy();
        gPad->Update();
 				getchar();
    
	}
}
void r3b_workshop_fittask()
{
run();
}
