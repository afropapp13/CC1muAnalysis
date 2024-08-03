// ROOT includes.
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TCanvas.h>
#include <TVectorD.h>
#include <TMatrixD.h>

// std includes.
#include <vector>

// Generator analysis includes.
#include "../../GeneratorAnalysis/Scripts/Constants.h"

// Utils includes.
#include "../../Utils/Tools.cxx"
#include "../../Utils/Util.C"
#include "../../Utils/WienerSVD.C"
#include "Helpers.cpp"

using namespace std;
using namespace Constants;

void ReweightXSec(TH1D* h, double SF = 1.) {
	int NBins = h->GetXaxis()->GetNbins();
	for (int i = 0; i < NBins; i++) {
		double CurrentEntry = h->GetBinContent(i+1);
		double NewEntry = CurrentEntry * SF / h->GetBinWidth(i+1);

		double CurrentError = h->GetBinError(i+1);
		double NewError = CurrentError * SF / h->GetBinWidth(i+1);

		h->SetBinContent(i+1,NewEntry); 
		h->SetBinError(i+1,NewError); 
	}
}

void Unfold() {
    // Set defaults and load tools
    TH1D::SetDefaultSumw2();
    TH2D::SetDefaultSumw2();
    gStyle->SetOptStat(0);

    int FontStyle = 132;
    double TextSize = 0.06;	

    Tools tools;

    // Root file to store objects in
    TString RootFilePath = "/exp/sbnd/data/users/epelaez/CAFAnaOutput/Unfolded.root";
    TFile* SaveFile = new TFile(RootFilePath, "UPDATE");

    // Load root file with histograms
    TString SelectionRootFilePath = "/exp/sbnd/data/users/epelaez/CAFAnaOutput/Selection.root";
    std::unique_ptr<TFile> SelectionFile(TFile::Open(SelectionRootFilePath));

    // Load root file with response matrices
    TString MatrixRootFilePath = "/exp/sbnd/data/users/epelaez/CAFAnaOutput/Matrix.root";
    std::unique_ptr<TFile> MatrixFile(TFile::Open(MatrixRootFilePath));

    // Load root file with total covariance matricex
    TString CovRootFilePath = "/exp/sbnd/data/users/epelaez/CAFAnaOutput/TotalCovMatrices.root";
    std::unique_ptr<TFile> CovFile(TFile::Open(CovRootFilePath));

    // Vector with all systematic files
    std::vector<std::unique_ptr<TFile>> CovFiles;

    // Add xsec systematics
    for (int iSyst = 0; iSyst < (int) XSecSystsVector.size(); iSyst++) {
        std::string SystName = std::get<0>(XSecSystsVector.at(iSyst));
        TString FilePath =  "/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematics"+TString(SystName)+".root";
        std::unique_ptr<TFile> File(TFile::Open(FilePath));
        CovFiles.push_back(std::move(File));
    }

    // Add flux systematics
    for (int iSyst = 0; iSyst < (int) FluxSystsVector.size(); iSyst++) {
        std::string SystName = std::get<0>(FluxSystsVector.at(iSyst));
        TString FilePath =  "/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematics"+TString(SystName)+".root";
        std::unique_ptr<TFile> File(TFile::Open(FilePath));
        CovFiles.push_back(std::move(File));
    }

    // Add stat systematics
    std::unique_ptr<TFile> StatsFile(TFile::Open("/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematicsStats.root"));
    CovFiles.push_back(std::move(StatsFile));

    // Add POT systematics
    std::unique_ptr<TFile> POTFile(TFile::Open("/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematicsPOT.root"));
    CovFiles.push_back(std::move(POTFile));

    // Add NTargets systematics
    std::unique_ptr<TFile> NTargetsFile(TFile::Open("/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematicsNTargets.root"));
    CovFiles.push_back(std::move(NTargetsFile));

    // Add Detector systematics
    std::unique_ptr<TFile> DetectorFile(TFile::Open("/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematicsDetector.root"));
    CovFiles.push_back(std::move(DetectorFile));

    // Add Reinteraction systematics
    std::unique_ptr<TFile> ReinteractionFile(TFile::Open("/exp/sbnd/data/users/epelaez/CAFAnaOutput/SelectionSystematicsReinteraction.root"));
    CovFiles.push_back(std::move(ReinteractionFile));

    // Dir to save plots
    TString dir = "/exp/sbnd/app/users/epelaez/CC1muAnalysis";

    // Flux file
    TFile* FluxFile = TFile::Open("MCC9_FluxHist_volTPCActive.root"); // make sure file is in path
	TH1D* HistoFlux = (TH1D*)(FluxFile->Get("hEnumu_cv"));

    // Integrated flux
    double IntegratedFlux = (HistoFlux->Integral() * TargetPOT / POTPerSpill / Nominal_UB_XY_Surface);

    // Plots to unfold
    std::vector<TString> PlotNames; std::vector<TString> XLabels; std::vector<TString> YLabels;

    ////////////////////////////////
    // Single differential variables
    ////////////////////////////////

    // Event count
    PlotNames.push_back("EventCount");
    XLabels.push_back("");
    YLabels.push_back("# events #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Muon angle
    PlotNames.push_back("MuonCosTheta");
    XLabels.push_back("cos(#theta_{#vec{p}_{#mu}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{#mu}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Leading proton angle
    PlotNames.push_back("LeadingProtonCosTheta");
    XLabels.push_back("cos(#theta_{#vec{p}_{L}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{L}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Recoil proton angle
    PlotNames.push_back("RecoilProtonCosTheta");
    XLabels.push_back("cos(#theta_{#vec{p}_{R}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{R}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Opening angle between protons
    PlotNames.push_back("CosOpeningAngleProtons"); 
    XLabels.push_back("cos(#theta_{#vec{p}_{L},#vec{p}_{R}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{L},#vec{p}_{R}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Opening angle between muon and total proton
    PlotNames.push_back("CosOpeningAngleMuonTotalProton"); 
    XLabels.push_back("cos(#theta_{#vec{p}_{#mu},#vec{p}_{sum}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{#mu},#vec{p}_{sum}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Delta alpha transverse
    PlotNames.push_back("DeltaAlphaT"); 
    XLabels.push_back("#delta #alpha_{T}");
    YLabels.push_back("#frac{d#delta #alpha_{T}}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Transverse momentum
    PlotNames.push_back("TransverseMomentum"); 
    XLabels.push_back("#delta P_{T}");
    YLabels.push_back("#frac{d#delta P_{T}}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Muon momentum    
    PlotNames.push_back("MuonMomentum"); 
    XLabels.push_back("|#vec{p}_{#mu}|");
    YLabels.push_back("#frac{d|#vec{p}_{#mu}|}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Leading proton momentum 
    PlotNames.push_back("LeadingProtonMomentum"); 
    XLabels.push_back("|#vec{p}_{L}|");
    YLabels.push_back("#frac{d|#vec{p}_{L}|}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Recoil proton momentum 
    PlotNames.push_back("RecoilProtonMomentum"); 
    XLabels.push_back("|#vec{p}_{R}|");
    YLabels.push_back("#frac{d|#vec{p}_{R}|}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    ////////////////////////////////
    // Double differential variables
    ////////////////////////////////

    // Serial transverse momentum in muon cos theta
    PlotNames.push_back("SerialTransverseMomentum_InMuonCosTheta"); 
    XLabels.push_back("#delta P_{T}");
    YLabels.push_back("#frac{d#delta P_{T}}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Delta alpha transverse in muon cos theta
    PlotNames.push_back("SerialDeltaAlphaT_InMuonCosTheta"); 
    XLabels.push_back("#delta #alpha_{T}");
    YLabels.push_back("#frac{d#delta #alpha_{T}}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    // Opening angle between protons in muon cos theta
    PlotNames.push_back("SerialCosOpeningAngleProtons_InMuonCosTheta"); 
    XLabels.push_back("cos(#theta_{#vec{p}_{L},#vec{p}_{R}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{L},#vec{p}_{R}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");
    
    // Opening angle between muon and protons in muon cos theta
    PlotNames.push_back("SerialCosOpeningAngleMuonTotalProton_InMuonCosTheta"); 
    XLabels.push_back("cos(#theta_{#vec{p}_{#mu},#vec{p}_{sum}})");
    YLabels.push_back("#frac{dcos(#theta_{#vec{p}_{#mu},#vec{p}_{sum}})}{d#delta P_{T}} #left[10^{-38} #frac{cm^{2}}{Ar}#right]");

    const int NPlots = PlotNames.size();

    for (int iPlot = 0; iPlot < NPlots; iPlot++) {
        // Load necessary plots
        TH2D* ResponseHist = (TH2D*)(MatrixFile->Get<TH2D>(PlotNames[iPlot]+"_response")); // response matrix
        TH2D* TotalCovHist = (TH2D*)(CovFile->Get<TH2D>(PlotNames[iPlot])); // total cov matrix
        TH1D* TruePlot = (TH1D*)(MatrixFile->Get<TH1D>(PlotNames[iPlot]+"_true")); // all true generated events
        TH1D* RecoPlot = (TH1D*)(SelectionFile->Get<TH1D>(PlotNames[iPlot]+"_reco")); // reco events
        TH1D* BkgPlot = (TH1D*)(SelectionFile->Get<TH1D>(PlotNames[iPlot]+"_bkg")); // bkg events
        RecoPlot->Add(BkgPlot, -1); // subtract background from reco events

        // Get dimension for cov matrices
        int n = TruePlot->GetXaxis()->GetNbins();
        double max = TruePlot->GetXaxis()->GetXmax();
        double min = TruePlot->GetXaxis()->GetXmin();

        // Get dimensions for matrices
        // int n = TruePlot->GetNbinsX();
        // int m = RecoPlot->GetNbinsX();
        // double edges[n+1];
        // for (int i = 0; i < n+1; i++) { edges[i] = TruePlot->GetBinLowEdge(i+1); }

        // Create objects to store matrices/vectors from Wiener SVD
        TMatrixD AddSmear(n,n);
        TVectorD WF(n);
        TMatrixD UnfoldCov(n,n);
        TMatrixD CovRotation(n,n);

        // Scale histograms
        TruePlot->Scale(Units / (IntegratedFlux * NTargets));
        RecoPlot->Scale(Units / (IntegratedFlux * NTargets));

        // Convert histograms to matrices/vectors
        TVectorD SignalVector(n); H2V(TruePlot, SignalVector);
        TVectorD MeasureVector(n); H2V(RecoPlot, MeasureVector);
        TMatrixD ResponseMatrix(n, n); H2M(ResponseHist, ResponseMatrix, kFALSE);
        TMatrixD CovarianceMatrix(n, n); H2M(TotalCovHist, CovarianceMatrix, kTRUE);

        TVectorD unfold = WienerSVD(
            ResponseMatrix,
            SignalVector,
            MeasureVector,
            CovarianceMatrix,
            2,
            0.5,
            AddSmear,
            WF,
            UnfoldCov,
            CovRotation
        );
        // Get transpose cov rotation matrix
        TMatrixD CovRotationT (TMatrixD::kTransposed, CovRotation);

        // Add smear to signal
        TH1D* UnfoldedSpectrum = new TH1D("Unfolded"+PlotNames[iPlot],";"+XLabels[iPlot]+";"+YLabels[iPlot],n,min,max);
        V2H(unfold, UnfoldedSpectrum); ReweightXSec(UnfoldedSpectrum);

        TH1D* SmearedSignal = new TH1D("SmearedTrue"+PlotNames[iPlot],";"+XLabels[iPlot]+";"+YLabels[iPlot],n,min,max);
        TVectorD SmearedVector = AddSmear * SignalVector;
        V2H(SmearedVector, SmearedSignal); ReweightXSec(SmearedSignal);

        // Declare canvas
        TCanvas* PlotCanvas = new TCanvas(PlotNames[iPlot],PlotNames[iPlot],205,34,1124,768);

        // Plot smearing matrix
        TH2D* SmearMatrixHisto = new TH2D("Smearing"+PlotNames[iPlot], "Smearing"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(AddSmear, SmearMatrixHisto);

        // Margins for matrices
        PlotCanvas->SetTopMargin(0.13);
        PlotCanvas->SetLeftMargin(0.15);
        PlotCanvas->SetRightMargin(0.15);
        PlotCanvas->SetBottomMargin(0.16);

        double SmearMin = SmearMatrixHisto->GetMinimum();
        double SmearMax = SmearMatrixHisto->GetMaximum();
        SmearMatrixHisto->GetZaxis()->SetRangeUser(SmearMin,SmearMax);
        TotalCovHist->GetZaxis()->CenterTitle();
        SmearMatrixHisto->GetZaxis()->SetTitleFont(FontStyle);
        SmearMatrixHisto->GetZaxis()->SetTitleSize(TextSize);
        SmearMatrixHisto->GetZaxis()->SetLabelFont(FontStyle);
        SmearMatrixHisto->GetZaxis()->SetLabelSize(TextSize);
        SmearMatrixHisto->GetZaxis()->SetNdivisions(6);
        SmearMatrixHisto->GetXaxis()->SetTitle(SmearMatrixHisto->GetXaxis()->GetTitle());
        SmearMatrixHisto->GetYaxis()->SetTitle(SmearMatrixHisto->GetYaxis()->GetTitle());

        PlotCanvas->cd();
        SmearMatrixHisto->Draw("colz");
        PlotCanvas->SaveAs(dir+"/Figs/CAFAna/Smear/"+PlotNames[iPlot]+".png");

        // Margins for unfolded xsecs
        PlotCanvas->SetTopMargin(0.13);
        PlotCanvas->SetLeftMargin(0.17);
        PlotCanvas->SetRightMargin(0.05);
        PlotCanvas->SetBottomMargin(0.16);

        // Deserialize double differential plots
        if (PlotNames[iPlot].Contains("Serial")) {
            auto [SliceDiscriminators, SliceBinning] = PlotNameToDiscriminator["True"+PlotNames[iPlot]+"Plot"];
            auto [NSlices, SerialVectorRanges, SerialVectorBins, SerialVectorLowBin, SerialVectorHighBin] = tools.FlattenNDBins(SliceDiscriminators, SliceBinning);
            int StartIndex = 0;

            // Loop over slices
            for (int iSlice = 0; iSlice < NSlices; iSlice++) {
                // Slice name
                TString SlicePlotName = PlotNames[iPlot] + "_" + TString(std::to_string(iSlice));

                // Get slice width
                double SliceWidth = SliceDiscriminators[iSlice + 1] - SliceDiscriminators[iSlice]; 

                // Get number of bins
                int SliceNBins = SerialVectorBins.at(iSlice);
                std::vector<double> SerialSliceBinning;

                for (int iBin = 0; iBin < SliceNBins + 1; iBin++) {
                    double value = SerialVectorRanges.at(StartIndex + iBin);
                    SerialSliceBinning.push_back(value);
                } // End of the number of bins and the bin ranges declaration

                // Slice true and reco true histos
                TH1D* SlicedSmearedSignal = tools.GetHistoBins(
                    SmearedSignal,
                    SerialVectorLowBin.at(iSlice),
                    SerialVectorHighBin.at(iSlice),
                    SliceWidth,
                    SerialSliceBinning,
                    "SmearedSignal"
                );
                TH1D* SlicedUnfoldedSpectrum = tools.GetHistoBins(
                    UnfoldedSpectrum,
                    SerialVectorLowBin.at(iSlice),
                    SerialVectorHighBin.at(iSlice),
                    SliceWidth,
                    SerialSliceBinning,
                    "UnfoldedSpectrum"
                );

                // Create legend object
                TLegend* leg = new TLegend(0.2,0.73,0.55,0.83);
                leg->SetBorderSize(0);
                leg->SetNColumns(3);
                leg->SetTextSize(TextSize*0.8);
                leg->SetTextFont(FontStyle);

                TLegendEntry* legRecoTrue = leg->AddEntry(SlicedSmearedSignal,"True","l");
                SlicedSmearedSignal->SetLineColor(kRed+1);
                SlicedSmearedSignal->SetLineWidth(4);

                TLegendEntry* legRecoBkg = leg->AddEntry(SlicedUnfoldedSpectrum,"Unfolded","l");
                SlicedUnfoldedSpectrum->SetLineColor(kOrange+7);
                SlicedUnfoldedSpectrum->SetLineWidth(4);
                SlicedUnfoldedSpectrum->SetMarkerColor(kOrange+7);
                SlicedUnfoldedSpectrum->SetMarkerStyle(20);
                SlicedUnfoldedSpectrum->SetMarkerSize(1.);

                double imax = TMath::Max(SlicedUnfoldedSpectrum->GetMaximum(),SlicedSmearedSignal->GetMaximum());
                double YAxisRange = 1.35*imax;
                SlicedUnfoldedSpectrum->GetYaxis()->SetRangeUser(0.,YAxisRange);
                SlicedSmearedSignal->GetYaxis()->SetRangeUser(0.,YAxisRange);

                PlotCanvas->cd();
                SlicedSmearedSignal->Draw("hist");
                SlicedUnfoldedSpectrum->Draw("p0 hist same");
                leg->Draw();

                // Slice label
                TLatex *textSlice = new TLatex();
                TString SliceLabel = tools.to_string_with_precision(SliceDiscriminators[iSlice], 1) + " < " + PlotNameToSliceLabel["True"+PlotNames[iPlot]+"Plot"] + " < " + tools.to_string_with_precision(SliceDiscriminators[iSlice + 1], 1);
                textSlice->DrawLatexNDC(0.4,0.92,SliceLabel);

                // Save histogram
                PlotCanvas->SaveAs(dir+"/Figs/CAFAna/Unfolded/"+SlicePlotName+".png");	
            }
        } else {
            TLegend* leg = new TLegend(0.2,0.73,0.55,0.83);
            leg->SetBorderSize(0);
            leg->SetNColumns(3);
            leg->SetTextSize(TextSize*0.8);
            leg->SetTextFont(FontStyle);

            TLegendEntry* legRecoTrue = leg->AddEntry(SmearedSignal,"True","l");
            SmearedSignal->SetLineColor(kRed+1);
            SmearedSignal->SetLineWidth(4);

            TLegendEntry* legRecoBkg = leg->AddEntry(UnfoldedSpectrum,"Unfolded","l");
            UnfoldedSpectrum->SetLineColor(kOrange+7);
            UnfoldedSpectrum->SetLineWidth(4);
            UnfoldedSpectrum->SetMarkerColor(kOrange+7);
			UnfoldedSpectrum->SetMarkerStyle(20);
			UnfoldedSpectrum->SetMarkerSize(1.);

            double imax = TMath::Max(UnfoldedSpectrum->GetMaximum(),SmearedSignal->GetMaximum());
            double YAxisRange = 1.35*imax;
            UnfoldedSpectrum->GetYaxis()->SetRangeUser(0.,YAxisRange);
            SmearedSignal->GetYaxis()->SetRangeUser(0.,YAxisRange);	

            PlotCanvas->cd();
            UnfoldedSpectrum->Draw("p0 hist");
            SmearedSignal->Draw("hist same");
            leg->Draw();

            // Save histogram
            PlotCanvas->SaveAs(dir+"/Figs/CAFAna/Unfolded/"+PlotNames[iPlot]+".png");
        }

        ///////////////////////////
        // Bin by bin uncertainties
        ///////////////////////////

        // Get xsec cov matrix
        TMatrixD XSecCov(n, n);
        for (int iSyst = 0; iSyst < (int) XSecSystsVector.size(); iSyst++) {
            TH2D* CovHist = (TH2D*)(CovFiles[iSyst]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
            TMatrixD CovMatrix(n, n); H2M(CovHist, CovMatrix, kTRUE);
            XSecCov += CovMatrix;
        }
        TMatrixD UnfXSecCov = CovRotation * XSecCov * CovRotationT;
        TH2D* UnfXSecCovHist = new TH2D("UnfCovXSec"+PlotNames[iPlot], "UnfCovXSec"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfXSecFracCovHist = new TH2D("UnfFracCovXSec"+PlotNames[iPlot], "UnfFracCovXSec"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfXSecCorrHist = new TH2D("UnfCorrXSec"+PlotNames[iPlot], "UnfCorrXSec"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfXSecCov, UnfXSecCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfXSecCovHist, UnfXSecFracCovHist, UnfXSecCorrHist, n);
        TMatrixD UnfXSecFracCov(n, n); H2M(UnfXSecFracCovHist, UnfXSecFracCov, kTRUE);

        // Get flux cov matrix
        TMatrixD FluxCov(n, n);
        for (int iSyst = 0; iSyst < (int) FluxSystsVector.size(); iSyst++) {
            TH2D* CovHist = (TH2D*)(CovFiles[iSyst + (int)XSecSystsVector.size()]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
            TMatrixD CovMatrix(n, n); H2M(CovHist, CovMatrix, kTRUE);
            FluxCov += CovMatrix;
        }
        TMatrixD UnfFluxCov = CovRotation * FluxCov * CovRotationT;
        TH2D* UnfFluxCovHist = new TH2D("UnfCovFlux"+PlotNames[iPlot], "UnfCovFlux"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfFluxFracCovHist = new TH2D("UnfFracCovFlux"+PlotNames[iPlot], "UnfFracCovFlux"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfFluxCorrHist = new TH2D("UnfCorrFlux"+PlotNames[iPlot], "UnfCorrFlux"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfFluxCov, UnfFluxCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfFluxCovHist, UnfFluxFracCovHist, UnfFluxCorrHist, n);
        TMatrixD UnfFluxFracCov(n, n); H2M(UnfFluxFracCovHist, UnfFluxFracCov, kTRUE);

        int offset = XSecSystsVector.size() + FluxSystsVector.size();

        // Get stat cov matrix
        TH2D* StatCovHist = (TH2D*)(CovFiles[offset]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
        TMatrixD StatCov(n, n); H2M(StatCovHist, StatCov, kTRUE); TMatrixD UnfStatCov = CovRotation * StatCov * CovRotationT;
        TH2D* UnfStatCovHist = new TH2D("UnfCovStat"+PlotNames[iPlot], "UnfCovStat"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfStatFracCovHist = new TH2D("UnfFracCovStat"+PlotNames[iPlot], "UnfFracCovStat"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfStatCorrHist = new TH2D("UnfCorrStat"+PlotNames[iPlot], "UnfCorrStat"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfStatCov, UnfStatCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfStatCovHist, UnfStatFracCovHist, UnfStatCorrHist, n);
        TMatrixD UnfStatFracCov(n, n); H2M(UnfStatFracCovHist, UnfStatFracCov, kTRUE);

        // Get POT cov matrix
        TH2D* POTCovHist = (TH2D*)(CovFiles[offset + 1]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
        TMatrixD POTCov(n, n); H2M(POTCovHist, POTCov, kTRUE); TMatrixD UnfPOTCov = CovRotation * POTCov * CovRotationT;
        TH2D* UnfPOTCovHist = new TH2D("UnfCovPOT"+PlotNames[iPlot], "UnfCovPOT"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfPOTFracCovHist = new TH2D("UnfFracCovPOT"+PlotNames[iPlot], "UnfFracCovPOT"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfPOTCorrHist = new TH2D("UnfCorrPOT"+PlotNames[iPlot], "UnfCorrPOT"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfPOTCov, UnfPOTCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfPOTCovHist, UnfPOTFracCovHist, UnfPOTCorrHist, n);
        TMatrixD UnfPOTFracCov(n, n); H2M(UnfPOTFracCovHist, UnfPOTFracCov, kTRUE);

        // Get NTargets cov matrix
        TH2D* NTargetsCovHist = (TH2D*)(CovFiles[offset + 2]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
        TMatrixD NTargetsCov(n, n); H2M(NTargetsCovHist, NTargetsCov, kTRUE); TMatrixD UnfNTargetsCov = CovRotation * NTargetsCov * CovRotationT;
        TH2D* UnfNTargetsCovHist = new TH2D("UnfCovNTargets"+PlotNames[iPlot], "UnfCovNTargets"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfNTargetsFracCovHist = new TH2D("UnfFracCovNTargets"+PlotNames[iPlot], "UnfFracCovNTargets"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfNTargetsCorrHist = new TH2D("UnfCorrNTargets"+PlotNames[iPlot], "UnfCorrNTargets"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfNTargetsCov, UnfNTargetsCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfNTargetsCovHist, UnfNTargetsFracCovHist,UnfNTargetsCorrHist, n);
        TMatrixD UnfNTargetsFracCov(n, n); H2M(UnfNTargetsFracCovHist, UnfNTargetsFracCov, kTRUE);

        // Get Detector cov matrix
        TH2D* DetectorCovHist = (TH2D*)(CovFiles[offset + 3]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
        TMatrixD DetectorCov(n, n); H2M(DetectorCovHist, DetectorCov, kTRUE); TMatrixD UnfDetectorCov = CovRotation * DetectorCov * CovRotationT;
        TH2D* UnfDetectorCovHist = new TH2D("UnfCovDetector"+PlotNames[iPlot], "UnfCovDetector"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfDetectorFracCovHist = new TH2D("UnfFracCovDetector"+PlotNames[iPlot], "UnfFracCovDetector"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfDetectorCorrHist = new TH2D("UnfCorrDetector"+PlotNames[iPlot], "UnfCorrDetector"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfDetectorCov, UnfDetectorCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfDetectorCovHist, UnfDetectorFracCovHist, UnfDetectorCorrHist, n);
        TMatrixD UnfDetectorFracCov(n, n); H2M(UnfDetectorFracCovHist, UnfDetectorFracCov, kTRUE);

        // Get Reinteraction cov matrix
        TH2D* ReinteractionCovHist = (TH2D*)(CovFiles[offset + 4]->Get<TH2D>(PlotNames[iPlot]+"_cov"));
        TMatrixD ReinteractionCov(n, n); H2M(ReinteractionCovHist, ReinteractionCov, kTRUE); TMatrixD UnfReinteractionCov = CovRotation * ReinteractionCov * CovRotationT;
        TH2D* UnfReinteractionCovHist = new TH2D("UnfCovReinteraction"+PlotNames[iPlot], "UnfCovReinteraction"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfReinteractionFracCovHist = new TH2D("UnfFracCovReinteraction"+PlotNames[iPlot], "UnfFracCovReinteraction"+PlotNames[iPlot], n, min, max, n, min, max);
        TH2D* UnfReinteractionCorrHist = new TH2D("UnfCorrReinteraction"+PlotNames[iPlot], "UnfCorrReinteraction"+PlotNames[iPlot], n, min, max, n, min, max);
        M2H(UnfReinteractionCov, UnfReinteractionCovHist);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfReinteractionCovHist, UnfReinteractionFracCovHist, UnfReinteractionCorrHist, n);
        TMatrixD UnfReinteractionFracCov(n, n); H2M(UnfReinteractionFracCovHist, UnfReinteractionFracCov, kTRUE);

        // Histograms for uncertainties
        TH1D* XSecHisto = new TH1D("XSec"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* FluxHisto = new TH1D("Flux"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* StatsHisto = new TH1D("Stats"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* POTHisto = new TH1D("POT"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* NTargetsHisto = new TH1D("NTargets"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* DetectorHisto = new TH1D("Detector"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* ReinteractionHisto = new TH1D("Reinteraction"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);
        TH1D* TotalHisto = new TH1D("Total"+PlotNames[iPlot], ";;Uncertainty [%]", n, min, max);

        // Histograms to store total frac cov and corr
        TH2D* UnfTotalCovHisto = new TH2D("UnfTotalCov"+PlotNames[iPlot],"UnfTotalCov" + PlotNames[iPlot],n, min, max, n, min, max);
        TH2D* UnfTotalFracCovHisto = new TH2D("UnfTotalFracCov"+PlotNames[iPlot],"UnfTotalFracCov" + PlotNames[iPlot],n, min, max, n, min, max);
        TH2D* UnfTotalCorrHisto = new TH2D("UnfTotalCorr"+PlotNames[iPlot],"UnfTotalCorr" + PlotNames[iPlot],n, min, max, n, min, max);
        M2H(UnfoldCov, UnfTotalCovHisto);
        SelectionHelpers::GetFracCovAndCorrMatrix(UnfoldedSpectrum, UnfTotalCovHisto, UnfTotalFracCovHisto, UnfTotalCorrHisto, n);
        TMatrixD UnfTotalFracCov(n, n); H2M(UnfTotalFracCovHisto, UnfTotalFracCov, kTRUE);

        // Loop over each bin
        for (int iBin = 0; iBin < n; iBin++) {
            double Total = 0.;

            // Add xsec uncertainties
            XSecHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfXSecFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfXSecFracCov(iBin, iBin)) * 100, 2);

            // Add flux uncertainties
            FluxHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfFluxFracCov(iBin, iBin)) * 100);
            Total +=  TMath::Power(TMath::Sqrt(UnfFluxFracCov(iBin, iBin)) * 100, 2);

            // Add Stat systematics
            StatsHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfStatFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfStatFracCov(iBin, iBin)) * 100, 2);

            // Add POT systematics
            POTHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfPOTFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfPOTFracCov(iBin, iBin)) * 100, 2);

            // Add NTargets systematics
            NTargetsHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfNTargetsFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfNTargetsFracCov(iBin, iBin)) * 100, 2);

            // Add Detector systematics
            DetectorHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfDetectorFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfDetectorFracCov(iBin, iBin)) * 100, 2);

            // Add Reinteraction systematics
            ReinteractionHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfReinteractionFracCov(iBin, iBin)) * 100);
            Total += TMath::Power(TMath::Sqrt(UnfReinteractionFracCov(iBin, iBin)) * 100, 2);

            // Total 
            TotalHisto->SetBinContent(iBin + 1, TMath::Sqrt(UnfTotalFracCov(iBin, iBin)) * 100);

            // These should be the same
            std::cout << TMath::Sqrt(UnfTotalFracCov(iBin, iBin)) * 100 << " ==? ";
            std::cout << TMath::Sqrt(Total) << std::endl;;
        }
        // Margins for bin by bin uncertainties plot
        PlotCanvas->SetTopMargin(0.13);
        PlotCanvas->SetLeftMargin(0.17);
        PlotCanvas->SetRightMargin(0.05);
        PlotCanvas->SetBottomMargin(0.16);

        TLegend* leg = new TLegend(0.2,0.73,0.75,0.83);
        leg->SetBorderSize(0);
        leg->SetNColumns(3);
        leg->SetTextSize(TextSize*0.8);
        leg->SetTextFont(FontStyle);

        TLegendEntry* legXSec = leg->AddEntry(XSecHisto,"XSec","l");
        XSecHisto->SetLineColor(kBlue+2);
        XSecHisto->SetLineWidth(4);
        XSecHisto->SetMarkerSize(1.5);
        XSecHisto->SetMarkerColor(kBlue+2);

        // Style 
        XSecHisto->GetXaxis()->SetNdivisions(5);
        if (PlotNames[iPlot] == "EventCount") {
            XSecHisto->GetXaxis()->SetLabelSize(0);
            XSecHisto->GetXaxis()->SetTitleSize(0);
        } else {
            XSecHisto->GetXaxis()->SetTitleFont(FontStyle);
            XSecHisto->GetXaxis()->SetLabelFont(FontStyle);
            XSecHisto->GetXaxis()->SetLabelSize(TextSize);
            XSecHisto->GetXaxis()->SetTitleSize(TextSize);
            XSecHisto->GetXaxis()->SetTitleOffset(1.);
            XSecHisto->GetXaxis()->CenterTitle();
            XSecHisto->GetXaxis()->SetTitle(UnfoldedSpectrum->GetXaxis()->GetTitle());
        }

        XSecHisto->GetYaxis()->SetTitleFont(FontStyle);
        XSecHisto->GetYaxis()->SetLabelFont(FontStyle);
        XSecHisto->GetYaxis()->SetLabelSize(TextSize);
        XSecHisto->GetYaxis()->SetTitleSize(TextSize);
        XSecHisto->GetYaxis()->SetNdivisions(6);
        XSecHisto->GetYaxis()->SetTitleOffset(1.);
        XSecHisto->GetYaxis()->SetTickSize(0);
        XSecHisto->GetYaxis()->CenterTitle();
        XSecHisto->GetYaxis()->SetRangeUser(0.,TotalHisto->GetMaximum()*1.35);
        XSecHisto->Draw("hist text0");

        TLegendEntry* legFlux = leg->AddEntry(FluxHisto,"Flux","l");
        FluxHisto->SetLineColor(kRed+1);
        FluxHisto->SetLineWidth(4);
        FluxHisto->SetMarkerSize(1.5);
        FluxHisto->SetMarkerColor(kRed+1);
        FluxHisto->Draw("hist text0 same");

        TLegendEntry* legStats= leg->AddEntry(StatsHisto,"Stat","l");
        StatsHisto->SetLineColor(kMagenta+1);
        StatsHisto->SetLineWidth(4);
        StatsHisto->SetMarkerSize(1.5);
        StatsHisto->SetMarkerColor(kMagenta+1);
        StatsHisto->Draw("hist text0 same");

        TLegendEntry* legPOT= leg->AddEntry(POTHisto,"POT","l");
        POTHisto->SetLineColor(kGreen);
        POTHisto->SetLineWidth(4);
        POTHisto->SetMarkerSize(1.5);
        POTHisto->SetMarkerColor(kGreen);
        POTHisto->Draw("hist text0 same");

        TLegendEntry* legNTargets= leg->AddEntry(NTargetsHisto,"NTargets","l");
        NTargetsHisto->SetLineColor(kYellow+1);
        NTargetsHisto->SetLineWidth(4);
        NTargetsHisto->SetMarkerSize(1.5);
        NTargetsHisto->SetMarkerColor(kYellow+1);
        NTargetsHisto->Draw("hist text0 same");

        TLegendEntry* legDetector= leg->AddEntry(DetectorHisto,"Detector","l");
        DetectorHisto->SetLineColor(kCyan-3);
        DetectorHisto->SetLineWidth(4);
        DetectorHisto->SetMarkerSize(1.5);
        DetectorHisto->SetMarkerColor(kCyan-3);
        DetectorHisto->Draw("hist text0 same");

        TLegendEntry* legReinteraction= leg->AddEntry(ReinteractionHisto,"Reinteraction","l");
        ReinteractionHisto->SetLineColor(kTeal + 3);
        ReinteractionHisto->SetLineWidth(4);
        ReinteractionHisto->SetMarkerSize(1.5);
        ReinteractionHisto->SetMarkerColor(kTeal+3);
        ReinteractionHisto->Draw("hist text0 same");

        TLegendEntry* legTotal= leg->AddEntry(TotalHisto,"Total","l");
        TotalHisto->SetLineColor(kBlack);
        TotalHisto->SetLineWidth(4);
        TotalHisto->SetMarkerSize(1.5);
        TotalHisto->SetMarkerColor(kBlack);
        TotalHisto->Draw("hist text0 same");

        // Save plot
        leg->Draw();
        PlotCanvas->SaveAs(dir+"/Figs/CAFAna/UnfBinUncertainties/"+PlotNames[iPlot]+".png");

        delete PlotCanvas;
    }
}