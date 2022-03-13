# EM1802
RCA CDP1802 Emulator / Debugger

EM1802 is an emulation package for the CDP1802 series of 8 bit microprocessors, popular in early home computers of the 1970's and current
enthusiast groups. The emulated hardware is based on my Elf-Cube design, providing 64K RAM, CDP1854 Programmable UART and simple 8 bit IDE interface.

Currently the following features are available:

- All 1802 registers are visible, and updated in real time whilst a program is running.
- Processor speed can be controlled via a slider from approximately 1 instruction per second, upto an equivalent clock speed of around 1MHz.
- 1802 Load Mode
- Programs can be loaded directly into memory from the host in Intel Hex format.
- The Console window simulates a dumb terminal attached to the CDP1854 UART
- Support for 1804/5/6 and 1804A/5A/6A selectable

A customised version of Lee Hart's Idiot/4 monitor is included in the distribution for convenience. To use, load the idiot4.hex file into
memory, drag the speed slider to maximum and click "Run". Then click in the console widget, and hit "Enter".

## Load Mode

Entering a hex byte into the text box on the toolbar, and clicking "Load" (or pressing enter, will load the entered byte into 
memory at the address pointed to by R0 and increments R0. Additionally, single line assembly can be entered here.

## Memory

The Memory widget displays a full hex dump of the 64K address space, which scrolls automatically to highlight the current program location.

## Disassembly

The Disassembly widget shows a disassembly of upcoming instructions, and the last instruction executed. Where applicable, if X=P  the next 
byte will display in hex rather than disassembly. (e.g. at reset the first instruction is usually DIS followed by 00).

## CDP1854 UART

The CDP1854 UART is pre-configured in mode 1 (1802 mode) and uses I/O Port 3, with Q controlling RSEL. EM1802 displays the current values 
of the Status, Control, Transmit and Receive Holding registers.

## IDE

The IDE emulator implements an 8 bit PIO Mode IDE interface. Output Port 2 selects the IDE Register, and I/O Port 1 reads and writes data to that port.
Currently only single sector transfers are supported, in either LBA28, or CHS Mode (untested). (Note: this is a very early and as yet incomplete implementation)

# Building EM1802

EM1802 is written in C++ and utilises QT 5.2. It builds on Linux as a QTCreator project.

Windows users can download a pre-built executable package from the Windows-CI action. Thanks to Karl Schultz for providing this workflow, 
and bug fixes.
