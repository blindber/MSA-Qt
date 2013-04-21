MSA-Qt, Version 0.0.1.0, Copyright (c) 2013 Bill Lindbergs

This is a port of the Modular Spectrum Analyzer code from Liberty basic to Qt
The majority of this code has been ported from spectrumanalyzer.bas 116 Rev 0, Release date 12-18-12, written by Scotty Sprowls and modified by Sam Wetterlin and Dave Roberts

The initial aim was, to port the code in such a way that it would be easy to compare the two versions to check that the changes were correct (eg only one source file).
This was quickly became unmanageable and was then broken into the files listed below. As a result of this change of tack, some functions will not be in their final logical location.

For this initial release I have targeted the USB hardware interface, the support for the LPT interface is not currently being ported.

As I only have a USB interface and control board I am not yet able to do much actual testing, on program start-up the latched switches are seen to toggle state, so something is working.

The code has been written for Qt5, but will load and compile under Qt4.7 or greater.

Currently I have it compiling and running under
Windows (win7) and should work on winxp, but no earlier
    -this is my main development environment
    -uses msadll.dll to talk to the USB board,

Linux (ubuntu)
    -needs work with font sizing
    -there is no hardware interface yet (eg no USB / LPT etc)
    -otherwise at the same state as the Windows code

Raspberry Pi
    -currently only runs full screen , all windows including dialogs
    -there is no hardware interface yet (eg no USB / LPT etc)


The state of the port is a long way from complete, only the basic SA functions are being worked on, at the moment

Items that are complete or near to are
- Configuration Gui
- Graph appearance Gui
- Reference line Gui, only tested with "Use Current data" and "Use Fixed Value"
- Copy image function (Edit -> Copy Image)
- Save image function
- Load Data file, has only been tested with a couple of S1P files I found in the yahoo group

The plotting of graphs is far from idea, as most of the code is almost straight from the original Liberty version, with just the drawing routines converted to use Qt's "QGraphicsScene" widget.
The QGraphicsScene can handle floating point positions. (Qt does not use strings for drawing, )

General notes on the code:
    - There maybe problems with the internal data variables being stored in the wrong type (integer / float)
    - The resizable arrays have been changed to vectors of one type or another
    - "for" loops have different end detection in basic / C++, so I might have missed converting some (this could be a problem to debug in the future)
    - Array sizes in general have been increased by 1 under c++, to mimic the sizes in basic
    - there are a lot of functions/variables that are declared as public when they could be private, this is a carry over from splitting the source over multiple files
    - the code is a mess,


mainwindow.cpp
    Main GUI interface
    
uwork.cpp

usbfunctions.cpp
    Interface to the MSADLL.DLL
    
touchstone.cpp

referencedialog.cpp
    Dialog for the setting up the reference traces
    
msautilities.cpp
    Functions from the utilities module
    
interpolation.cpp
    Functions from the interpolation  module
graph.cpp
    Functions for drawing the contents of the graphics window
    
globalvars.cpp
    Storeage area for of all the global variables

dialoggridappearance.cpp
    Dialog for changing the default grpahics colors
    
dialogconfigman.cpp
    Dialog for configuring the MSA

dialogcalmanager.cpp
    Functions from the Calibration Manager Module
    
dialogoslcal.cpp
    Functions to Handle OSL Calibration
    
coaxanalysis.cpp
    Functions from Coax Analysis Module
    
lptfunctions.cpp
    Functions for handling the interface to the LPT
    
dialogvnacal.cpp
    Functions that started with VNA :)