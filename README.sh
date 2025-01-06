# Charged-current $\nu_\mu$ interaction analysis with two detected protons (CC2p)

#Analysis of charged-current neutrino interactions. This repository is mainly divided into:

#- [`GeneratorAnalysis/`](https://github.com/afropapp13/CC1muAnalysis/tree/main/GeneratorAnalysis): contains scripts analysing events generated by different simulators.
#- [`CAFAnaAnalysis/`](https://github.com/afropapp13/CC1muAnalysis/tree/main/CAFAnaAnalysis): contains scripts analysing experiment data using the CAFAna framework.

#Plots generated through the scripts in both directories are placed under [`Figs/`](https://github.com/afropapp13/CC1muAnalysis/tree/main/Figs). 

me="$(whoami)"
fig_dir="/exp/sbnd/data/users/${me}"

mkdir ${fig_dir}/cc2p
mkdir ${fig_dir}/cc2p/Figs
mkdir ${fig_dir}/cc2p/Figs/Overlay
mkdir ${fig_dir}/cc2p/Figs/Overlay/Serial
mkdir ${fig_dir}/cc2p/Figs/Overlay/MEC
mkdir ${fig_dir}/cc2p/Figs/Overlay/MEC/Serial
mkdir ${fig_dir}/cc2p/Figs/InteBreakDown
mkdir ${fig_dir}/cc2p/Figs/InteBreakDown/PostFSI
mkdir ${fig_dir}/cc2p/Figs/InteBreakDown/PreFSI
mkdir ${fig_dir}/cc2p/Figs/CAFAna
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Cuts
mkdir ${fig_dir}/cc2p/Figs/CAFAna/InteBreakdown
mkdir ${fig_dir}/cc2p/Figs/CAFAna/TopologyBreakdown
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Serial
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Efficiency
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Matrices
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/NTargets
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/POT
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/Reinteraction
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/Detector
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/MCStat
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/Statistical
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/BinUncertainties
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/Unfolded
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/WienerSVDOverlay
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/FakeDataStudies
mkdir ${fig_dir}/cc2p/Figs/CAFAna/Uncertainties/FakeDataStudies/Combined

### Setting up

#If it is the first time working in this repository, you have to run

#```bash
#source setup.sh
#```

#to set up the correct feature branch of `sbnana` used in this codebase. Once this is done, you have to run

#```bash
#source activate.sh
#```

#to activate your local `sbnana` build every time you start a new terminal. This will also setup ROOT, so that you can run scripts in both `GeneratorAnalysis/` and `CAFAnaAnalysis/`. 
