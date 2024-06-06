#include <TFile.h>
#include <TTree.h>
#include <TString.h>

using namespace std;

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>

void GeneratorOverlay() {

    //------------------------------//

    TH1D::SetDefaultSumw2();
    TH2D::SetDefaultSumw2();
    gStyle->SetOptStat(0);

    int FontStyle = 132;
    double TextSize = 0.06;			

    TString OutFilePath = "/pnfs/sbnd/persistent/users/epelaez/HighSamples/FlatTree/";

    //------------------------------//

    // Event generators

    std::vector<TString> Names; std::vector<TString> Labels; std::vector<int> Colors;
    
    Names.push_back(OutFilePath+"FlatTreeAnalyzerOutput_GENIE_AR23.root"); 
    Labels.push_back("GENIE G18");
    Colors.push_back(kBlue+8);

    Names.push_back(OutFilePath+"FlatTreeAnalyzerOutput_GENIE_G18.root"); 
    Labels.push_back("GENIE AR23");
    Colors.push_back(kBlue+2);

    Names.push_back(OutFilePath+"FlatTreeAnalyzerOutput_NuWro.root"); 
    Labels.push_back("NuWro");
    Colors.push_back(kRed+1);

    Names.push_back(OutFilePath+"FlatTreeAnalyzerOutput_NEUT.root"); 
    Labels.push_back("NEUT");
    Colors.push_back(kOrange+7);

    Names.push_back(OutFilePath+"FlatTreeAnalyzerOutput_GiBUU.root"); 
    Labels.push_back("GiBUU");
    Colors.push_back(kGreen+1);			

    const int NSamples = Names.size();
    std::vector<TFile*> Files; Files.resize(NSamples);

    //------------------------------//

    // Plots to overlay

    std::vector<TString> PlotNames;

    PlotNames.push_back("TrueMuonCosThetaPlot");
    PlotNames.push_back("TrueLeadingProtonCosThetaPlot");
    PlotNames.push_back("TrueRecoilProtonCosThetaPlot");
    PlotNames.push_back("TrueLeadingProtonMomentumPlot");
    PlotNames.push_back("TrueRecoilProtonMomentumPlot");
    PlotNames.push_back("TrueMuonMomentumPlot");
    PlotNames.push_back("TrueCosOpeningAngleProtonsPlot");
    PlotNames.push_back("TrueCosOpeningAngleMuonTotalProtonPlot");
    PlotNames.push_back("TrueTransverseMomentumPlot");

    const int NPlots = PlotNames.size();

    //------------------------------//	

    // Loop over the samples to open the files and the TTree

    for (int iSample = 0; iSample < NSamples; iSample++) {

        Files[iSample] = new TFile(Names[iSample],"readonly");

    } // End of the loop over the samples

    //------------------------------//

    // Loop over the plots to be compared

    for (int iPlot = 0; iPlot < NPlots; iPlot++) {

        TString CanvasName = "Canvas_" + PlotNames[iPlot];
        TCanvas* PlotCanvas = new TCanvas(CanvasName,CanvasName,205,34,1024,768);
        PlotCanvas->cd();
        PlotCanvas->SetTopMargin(0.12);
        PlotCanvas->SetLeftMargin(0.15);
        PlotCanvas->SetBottomMargin(0.15);		
        PlotCanvas->Draw();	

        TLegend* leg = new TLegend(0.2,0.7,0.75,0.83);
        leg->SetBorderSize(0);
        leg->SetNColumns(2);
        leg->SetTextSize(TextSize*0.8);	
        leg->SetTextFont(FontStyle);						

        // Loop over the samples to open the files and to get the corresponding plot

        std::vector<TH1D*> Histos; Histos.resize(NSamples);

        for (int iSample = 0; iSample < NSamples; iSample++) {	

            Histos[iSample] = (TH1D*)(Files[iSample]->Get(PlotNames[iPlot]));

            Histos[iSample]->SetLineWidth(4);
            Histos[iSample]->SetLineColor( Colors.at(iSample) );	

            Histos[iSample]->GetXaxis()->SetTitleFont(FontStyle);
            Histos[iSample]->GetXaxis()->SetLabelFont(FontStyle);
            Histos[iSample]->GetXaxis()->SetNdivisions(8);
            Histos[iSample]->GetXaxis()->SetLabelSize(TextSize);
            Histos[iSample]->GetXaxis()->SetTitleSize(TextSize);	
            Histos[iSample]->GetXaxis()->SetTitleOffset(1.1);					
            Histos[iSample]->GetXaxis()->CenterTitle();						

            Histos[iSample]->GetYaxis()->SetTitleFont(FontStyle);
            Histos[iSample]->GetYaxis()->SetLabelFont(FontStyle);
            Histos[iSample]->GetYaxis()->SetNdivisions(6);
            Histos[iSample]->GetYaxis()->SetLabelSize(TextSize);
            Histos[iSample]->GetYaxis()->SetTitle("Cross Section [10^{-38} cm^{2}/Ar]");
            Histos[iSample]->GetYaxis()->SetTitleSize(TextSize);
            Histos[iSample]->GetYaxis()->SetTitleOffset(1.3);
            Histos[iSample]->GetYaxis()->SetTickSize(0);
            Histos[iSample]->GetYaxis()->CenterTitle();	

            double imax = TMath::Max(Histos[iSample]->GetMaximum(),Histos[0]->GetMaximum());

            double YAxisRange = 1.15*imax;
            if (PlotNames[iPlot] == "TrueLeadingProtonMomentumPlot") { YAxisRange *= 1.05; };
            Histos[iSample]->GetYaxis()->SetRangeUser(0.,YAxisRange);
            Histos[0]->GetYaxis()->SetRangeUser(0.,YAxisRange);			

            PlotCanvas->cd();
            Histos[iSample]->Draw("hist same");
            Histos[0]->Draw("hist same");	

            leg->AddEntry(Histos[iSample],Labels[iSample],"l");
            
            //----------------------------------------//					

        } // End of the loop over the samples grabing the plots	

        PlotCanvas->cd();
        leg->Draw();
        PlotCanvas->SaveAs("./Figs/Overlay/Overlay_"+PlotNames[iPlot]+".png");
        delete PlotCanvas;

    } // End of the loop over the plots

    //------------------------------//

} // End of the program
