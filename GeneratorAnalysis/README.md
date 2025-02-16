# Generator analysis

Events simulated with different generators (GENIE, GiBUU, etc.) are analyzed and different single and double differential plots are created. 

### Setup

To setup your workspace to run the scripts in this directory, you have to run `source setup.sh/activate.sh` as indicated [here](https://github.com/afropapp13/sbn_analyzer/blob/main/README.md).

### Running scripts

The file that processes all the `.flat.root` files is ran by 

```bash
root -b -q Scripts/script_LoopGenerators.C 
```

This will create `.root` files with the analyzed data ready to create plots. The plots are generated by calling 

```bash
root -b -q Scripts/${filename}
```

where `${filename}` can be one of the following:

- `GeneratorOverlay.cpp`: creates overlaid plots for all the available variables and generators.
- `GeneratorInteBreakDown.cpp`: creates a plot with event interaction type breakdown for each plot and generator.
- `SerialGeneratorOverlay.cpp`: creates sliced plots for the double differential variables and normal generators.
- `MECGeneratorOverlay.cpp`: same as `GeneratorOverlay.cpp` but for purely MEC generators.
- `MECSerialGeneratorOverlay.cpp`: same as `SerialGeneratorOverlay.cpp` but for purely MEC generators.
