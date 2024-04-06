# Z80 Experiment 2
Code and schematics for the PainfulDiodes blog post: https://painfuldiodes.wordpress.com/2024/04/02/z80-experiment-2-arduino-i-o/

The following subfolders within the mega folder all expect the Z80 to be driven by its own clock circuit - i.e. the schematic in experiment 001:
* echo-ext-clk
* echo-ext-clk-wait
* helloworld-ext-clk

The echo folder code expects the Mega to drive the Z80 clock - so the 555 circuit is disconnected from the Z80.