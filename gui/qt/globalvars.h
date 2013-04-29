//*******************************************************************************
//
//  -- Modular Spectrum Analyzer , in QT.
//
// Copyright (c) 2013 Bill Lindbergs
//
// The majority of this code is from spectrumanalyzer.bas, written by
// Scotty Sprowls and modified by Sam Wetterlin and Dave Roberts
//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//******************************************************************************
#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <QtCore>
#include <QVector>

template <class T>
class Q2DVector : public QVector< QVector<T> >
{
  public:
    Q2DVector() : QVector< QVector<T> >(){}

    Q2DVector(int rows, int columns) : QVector< QVector<T> >(rows)
    {
      for(int r=0; r<this->size(); r++)
      {
        (*this)[r].resize(columns);
      }
    }
    void mresize(int rows, int columns)
    {
      this->resize(rows);
      for(int r=0; r<this->size(); r++)
      {
        (*this)[r].resize(columns);
      }
    }
    inline Q2DVector &operator<<(const Q2DVector<T> &l)
    { *this += l; return *this; }
    virtual ~Q2DVector() {}
};
typedef Q2DVector<bool> Q2DBoolVector;
typedef Q2DVector<float> Q2DfloatVector;
typedef Q2DVector<int> Q2DintVector;
typedef Q2DVector<QString> Q2DQStringVector;


class globalVars
{
public:
  globalVars();
  int DataTypeIsAngle(int dataType);
  void zeroVars();





  //[EstablishUserVariables]
      //----Start of global variables set from a configuration file; some depend on construction of the
      //spectrum analyzer; others are just convenient defaults that can be changed at runtime.---------
      //[EstablishUserVariables]  //all of the following are "default" values and are dependent on the construction of your Spectrum Analyzer
      //int masterclock;  //Exact frequency of the Master Clock (in MHz).  Example: 64.000056 or 63.999937
                    //You can start with default configuration and change after calibration.
      float centfreq;     //Sweep center frequency, in MHz. For initial set-up use "0"
      float sweepwidth;   //Sweep width in MHz. For initial set-up use 10 times the BW of Final Xtal Filter
      int glitchtime;
      int Y1Top, Y1Bot;     //Top and bottom of Y1 (right) axis
      int Y2Top, Y2Bot;      //Top and bottom of Y2 (right) axis
      int PLL;
      int doingPDMCal;  //=1 when PDM cal in progress to determine invdeg ver114-5L
      float CalInvDeg;    //set to value of invdeg determined by cal ver114-5L
      int cftest;       //=1 when doing cavity filter sweep test (Special Tests window)  //ver116-4b
  //--SEW End of variables initialized from configuration file

  //--SEW2 added the following global declarations to make these available to true subroutines
  //del13-7c    global steps    //whole number of steps per sweep. 1 thru 720 is acceptable.  400 is a good number.
      int steps;

      int thisstep; //keeps track of current step number during a sweep
      int globalSteps;    //SEWgraph; Number of steps set by user. Set in calcWindoInfo. global version of steps.
      //int varwindow,
      int datawindow;    //=1 when indicated window is open   //ver115-1b

      float doSpecialGraph; //=0 for normal operation; for other values see [doSpecialGraph]
      float doSpecialRandom;  //Random number generated at start of each sweep, for doSpecialGraph ver 114-3g
      QString doSpecialRLCSpec;//$ //RLC spec for doSpecialGraph
      QString doSpecialCoaxName;//$ //Name of coax last used in RLC spec ver115-4b

      float LO2;          //actual LO2 frequency ver115-1c
      int suppressPhase;    //=1 to force phase to zero without measuring it ver116-1b

      //int hFileMenu, hOptionsMenu,hDataMenu,hFunctionsMenu,hOperatingCalMenu, hMultiscanMenu;  //Windows handles to some graph window submenus ver115-5d
      //int hTwoPortMenu; //ver116-1b
      int menuOperatingCalShowing;  //=1 when the operating cal menu is shown; otherwise 0
      int menuMultiscanShowing, menuTwoPortShowing; //same for multiscan and two-port menus ver116-1b

          //zero-based index of menu positions in menu bar
      int menuOptionsPosition, menuDataPosition, menuFunctionsPosition, menuOperatingCalPosition, menuMultiscanPosition;
      int menuTwoPortPosition, menuModePosition; //ver116-1b

          //IDs of various listed menu items that may need to be hidden/shown in certain modes
      int menuDataS21ID, menuDataLineCalID, menuDataS11ID, menuDataS11DerivedID;  //ver115-5d
      int menuDataLineCalRefID, menuDataLineCalOSLID;  //ver115-5d
      int menuOptionsSmithID; //ver115-5d
      int menuFunctionsFilterID, menuFunctionsCrystalID, menuFunctionsMeterID, menuFunctionsRLCID;  //ver115-5d
      int menuFunctionsCoaxID,menuFunctionsGenerateS21ID, menuFunctionsGroupDelayID; //ver115-8b

      QString twoPortWinHndl;//$  //handle of open Two-Port window, or blank if not open ver116-1b

      int crystalLastUsedID;    //Last ID of crystal added to crystal list

      QString imageSaveLastFolder;  //Folder in which last graph image was saved, regular or Smith chart ver115-2a


          //ver114-4k variables allowing forward or reverse sweep
      //The following 3 variables are not global, but are used in connection with reverse sweeps
      int sweepDir;     //+1 for left-right sweep; -1 for right-left sweep
      int sweepStartStep, sweepEndStep; //start and end steps for current sweep. Note "startfreq" is the x-axis start (left),
      //but sweepStartStep will be at the x-axis right end if we are going in reverse.
      int alternateSweep;    //=1 to alternate forward and reverse sweeps; =0 if direction is set by sweepDir ver114-5a
      //int componStopAtEnd;  //=1 to stop component Measure and end of measure sweep. ver115-1f
      int RefRLCLastNumPoints;
      QString RefRLCLastConnect;//$   //For continuity calling ReflectionRLC
      int analyzeQLastNumPoints;        //For continuity in AnalyzeQ
      int GDLastNumPoints;  //Number of points last used for group delay analysis



      int doingInitialization;  //Set to 1 during startup initialization of context variables; then to 0 ver114-3f
      int Y1DisplayMode, Y2DisplayMode; //Type of phase and mag graphing. Made global for sub use.
      int isStickMode;      // =1 when Y1DisplayMode or Y2DisplayMode are in a "stick" mode
      int specialOneSweep;  //=1 when [Restart] is called to do one sweep and then return, rather than wait. ver 114-5f
      int returnBeforeFirstStep;    //=1 to initialize on Restart and return to caller before taking any data ver115-1d
      int haltedAfterPartialRestart;    //=1 after halting as a result of returnBeforeFirstStep. Set to 0 when scan continues. ver1166-1b
      QString msaMode;//$      //=SA, ScalarTrans, VectorTrans or Reflection
      QString menuMode;//$    //msaMode$ to which the graph window menus currently conform
      QString restartTimeStamp;//$    //Date/time of last restart
      int primaryAxisNum;   //1 or 2, to indicate primary Y axis (e.g. where mag dBm defaults to) ver115-3b

      int Y1DataType, Y2DataType;    //data component constant to determine Y1 and Y2 graph data


      Q2DfloatVector ReflectArray;//(2020,17) //Actual signal freq (0),GraphS11DB(1), GraphS11Ang(2),linearMag(3),
                              //Impedance Mag(4), Impedance Angle(5),Rs(6), Xs(7), Rp(8),
                              // Xp(9), Cs(10), Ls(11), Cp(12), Lp(13), SWR(14),
                              //intermedDB(), intermedAng(15) (intermed= w/o R0 transform or plane ext) //ver115-2d

      float uWorkReflectData[17]; //Same data as ReflectArray, but for only one entry  //ver115-1b //ver115-2d

      Q2DfloatVector  S21DataArray;//Frequency (actual input freq) (0), mag(1), phase(2) and intermed phase(3) for VectorTrans and ScalarTrans modes. ver116-1b
                              //Intermed phase is phase before plane extension, saved in case of recalculation
          //The following auxGraphDataXXX arrays have info on auxiliary graphs, which are numbered 0 to 5; the
          //graph number is the first index of the array. If the graphs are specified as data types constAux0, etc., then
          //the graph number is the constant for the desired graph minus constAux0.
      Q2DfloatVector  auxGraphData;//Used to hold specially calculated data, such as from Q analysis, which can be retrieved and graphed.
          //auxGraphDataInfo$ Info for UpdateGraphDataFormat for auxGraphData. ver115-4a
          //(,0) is graph Name, (,1) is formatting string, (,2) is axis label, (,3) is marker label
      QString auxGraphDataFormatInfo[6][4];
          //auxGraphDataInfo is numeric info about each item in auxGraphData. (,0) is 1 if the data is an angle
          //(,1) and (,2) are the axis min and max to use as defaults for graphing.
      int auxGraphDataInfo[5][2]; //ver115-4a

      //SEWgraph; The following hold parameters used to perform filter analysis when requested by the user
      int doFilterAnalysis;     //=1 to perform filter analysis; 0 otherwise SEWgraph
      float x1DBDown, x2DBDown;   //positive db values for x1 and x2 points SEWgraph
      QString filterPeakMarkID;//$     //Marker that indicates filter peak




      int maxNumSteps;      //Absolute max number of steps allowed for sweep or in arrays of points (num points=num steps+1) ver114-3e
      int maxPointExtraLines;  //max lines in a file or string with point data, not including the numeric point data itself ver 114-3e
      //      maxPointExtraLines=100  //ver116-4k


      Q2DfloatVector VNAData;//[4000][3];   //For temporary storage of data to be restored to the graph. Resized when needed. ver116-1b

      int VNADataNumSteps; //Number of steps of data in VNAData. Data runs from (0,x) to (VNADataNumSteps, x)  ver116-1b
      int VNADataLinear;    //=1 if the data in VNAData has linear spacing, =0 for log. ver116-4a
      int VNADataZ0;        //Reference impedance of data in VNAData ver116-4a
      QString VNADataTitle[5];    //Title of data in VNAData ver116-1b
      int VNARestoreDoR0AndPlaneExt;    //Tells VNARestoreData whether to perform R0 conversion and plane extension ver116-4j

      int contextTypes[30];    //Used in connection with save and retrieve context files
                              //indicating which contexts are involved ver114-3a

   //Additional variables that need to be available to subroutines
          //Note "startfreq" is the x-axis start (left),
          //but sweepStartStep will be at the x-axis right end if we are going in reverse.
          //baseFrequency  is added when commanding the hardware, but does not affect graph display or file frequencies.
      float startfreq, endfreq, baseFrequency;
      int wate, planeadj;
      float thisfreq;
      int freqBand; //1, 2 or 3, indicating bands 1G, 2G and 3G, or zero to use auto-band mode ver116-4s
      int bandEnd1G, bandEnd2G;     //Final frequencies for 1G and 2G when in auto-band mode. ver116-4s
      int lastSetBand;  //set to 1,2 or 3 each time the band switch is set, so we know its state ver116-4s
      float sgout;   //signal generator output freq when in plain SA mode
      int test;     //SEWgraph  Contents get printed to message box on halt
      int spurcheck;    //=1 to turn spur test on //ver114-4f
      int gentrk;   //=1 when TG is being used (depends on mode); 0 if SG is used or build does not have TG hardware
      int normrev;  //=0 if TG is normal, =1 if TG is in reverse.
      float offset;  //TG offset frequency  moved ver115-5f
      QString path;    //Currently active filter path (1...); a number as a string in form "Path N" ver114-1d
      int FiltA0, FiltA1;   //low and high bits of RBW filter address //ver116-4j
      QString message;         //Message to print in graph window
      //int varwindow;        //=1 if variables window is open ver115-1a
      int suppressPDMInversion; //=1 to suppress inversion in [ReadStep] ver115-1a
      int leftstep;     //Used to hold a marker step number for [preupdatevar] ver115-1a
      int userFreqPref; //0 if user last used Center/Span method; 1 if user last used Start/Stop ver115-1d

           //When loading a path calibration file, we determine lowest ADC value for magdata for which phase is considered valid
      int validPhaseThreshold; //ver116-1b

      //------------Items for auto wait time----------//ver116-1b
           //When loading a path calibration file, we divide the response into three sections and calculate approximate slopes:
          //For ADC<calLowADCofCenterSlope the slope is calLowEndSlope, calculated from a segment near the
          //       low end, but not so low as to have tiny slopes compared to the center slope
          //for calLowADCofCenterSlope < ADC < calHighADCofCenterSlope the slope is calCenterSlope, calculated
          //        from a 20 dB or greater segment somewhere in the center area
          //for ADC>calHighADCofCenterSlope the slope is calHighEndSlope calculated from a segment at top end
          //The slope is delta ADC/delta dB
      int calCanUseAutoWait;    //=1 if cal table is suitable for auto wait time calculations
      int useAutoWait;   //=1 if user specified to use auto wait times
      QString autoWaitPrecision; //"Fast", "Normal" or "Precise"  meaningless if useAutoWait=0

      int calADCofLowFringe;    //ADC below which the ADC/dB slope is tiny
      int calLowADCofCenterSlope, calHighADCofCenterSlope;
      int calLowEndSlope, calCenterSlope, calHighEndSlope;
      int autoWaitTC;    //max time constant for video filters of mag and phase (where applicable) ver116-4j
          //The max db error is converted to a max ADC error for the three regions
          //of the path calibration table.
          //With auto wait time, we do repeated readings after waiting a certain time,
          //and determine when the change in readings falls below a maxChange level
          //The maximum allowed change in ADC values, to keep the error at the allowed limit,
      int autoWaitMaxChangeLowEndADC, autoWaitMaxChangeCenterADC, autoWaitMaxChangeHighEndADC;
      int autoWaitMaxChangePhaseADC;
      //------------End items for auto wait time----------

      //------------Items for transitions between Reflection and VectorTrans modes ver116-1b--------
          //Sweep parameters of last transmission sweep
      int transLastSteps, transLastStartFreq, transLastEndFreq, transLastIsLinear, transLastGraphR0;
          //most recent Y axis settings for transmission
      int transLastY1Type, transLastY1Top, transLastY1Bot, transLastY1AutoScale;  //data type and axis top and bottom
      int transLastY2Type, transLastY2Top, transLastY2Bot, transLastY2AutoScale;
          //Sweep parameters of last reflection sweep
      int refLastSteps, refLastStartFreq, refLastEndFreq, refLastIsLinear, refLastGraphR0;
          //most recent Y axis settings for reflection
      int refLastY1Type, refLastY1Top, refLastY1Bot, refLastY1AutoScale;
      int refLastY2Type, refLastY2Top, refLastY2Bot, refLastY2AutoScale;
          //most recent titles
      QString refLastTitle[5], transLastTitle[5];
      //-------------------------------------------------------------------------------------------

  //Variables for saving/restoring context files via gosub routines, where these are used as parameters
      QString restoreFileName, restoreContext;
      QFile *restoreFileHndl;
      int restoreIsValidation;
      QString restoreErr;
      int restoreLastLineNum; //ver115-8c

          //There can be 1 to four video filter settings, with different capacitor values.   //ver116-1b
          //They each have a name. Mag and phase capacitors can be different, but the names and number
          //of filters are the same. The names must be Wide, Mid, Narrow or XNarrow, but not all those names must be used.
          //Names and capacitor values are set in hardware configuration.

      QString videoFilter;     //Selected video filter: Wide, Mid, Narrow or XNarrow  //ver116-1b
      //float videoFilterCaps[5][3];  //Capacitance(uf) for Wide(1), Mid(2), Narrow(3) and XNarrow(4) video filters   //ver116-1b
                                 //Second index is 0 for magnitude and 1 for phase filters.
      //QString videoFilterNames[5]; //Names of each video filter, or blank if no filter. Index matches videoFilterCaps   //ver116-1b

      int videoFilterAddress;
      float videoMagCap, videoPhaseCap; //current video filter address (0-3) and cap values (uF) for mag and phase ver116-1b
      float videoMagTC, videoPhaseTC; //Time constants (ms) of video mag and phase filters //ver116-1b

      int switchFR, switchTR;  //current or desired state of forward/reverse (0=forward) and transmission/reflection (0=transmission) switches ver116-1b

      //Globals used to remember state info to allow detection of user changes; added by ver114-6e
      //See RememberState and DetectChanges
      QString prevMSAMode;     //msaMode$
      //QString prevPath;        //Filter path. ver115-1a
      int prevFreqMode;     //frequency mode and auto band switch ver116-4s
      float prevStartF, prevEndF, prevBaseF;    //ver116-4k
      int prevXIsLinear, prevY1IsLinear, prevY2IsLinear;
      int prevSteps, prevSweepDir, prevAlternate;
      int prevStartY1, prevEndY1, prevStartY2, prevEndY2;
      int prevHorDiv, prevVertDiv;
      int prevY1Disp,prevY2Disp;
      int prevGenTrk, prevSpurCheck;    //ver114-6k
      QString prevPath;    //ver116-4j
      int prevTGOff, prevSGFreq;    //ver115-1a
      int prevPlaneAdj;     //ver114-7f
      int prevY1DataType, prevY2DataType;   //ver115-1b deleted source constants
      int prevAutoScaleY1, prevAutoScaleY2; //ver114-7a
      int prevDataChanged;      //This must be set to 1 when data is loaded from a context
      QString prevS21JigAttach;  //"Series" or "Shunt" to indicate the Transmission jig used ver114-6k
      int prevS21JigR0;   //Source and load impedances of Transmission
      int prevS21JigShuntDelay;     //Delay of shunt fixture connector
      int prevS11BridgeR0, prevS11GraphR0;   //Bridge reference and graph reference for S11   //ver114-6k
      QString prevS11JigType;      //Jig previously used for reflection mode  ver115-1b
      int prevSwitchFR;     //Previous setting of forward/reverse switch ver116-1b

      //Functions may need to redo the sweep with new parameters. The following are used to save/restore the
      //pre-existing parameters, using SaveAndChangeSweepParameters and [RestoreSweepParameters] ver115-5c
      int functSaveAlternate,functSaveSweepDir,functSavePlaneAdj,functSaveWate;
      QString functSaveVideoFilter;  //ver116-4b

      int functSaveAutoWait;
      QString functSaveAutoWaitPrecision;  //ver116-1b

      //ver114-5e added these cal items. Full Line cal is a calibration of through response with the current sweep settings.
      //Baseline cal is a cal of through response with a generic wideband sweep.
      //desiredCalLevel is the user-specified level. applyCalLevel is the level we are actually applying



      Q2DfloatVector lineCalArray;    //calibration data for each step#: (0)freq (tuning),(1)magpower during cal,(2)phaseofpdm during cal
      Q2DfloatVector bandLineCal; //Bandsweep line calibration data; transferred to lineCalArray when needed ver 114-5f
          //Note baseLineCal is fixed sized
      Q2DfloatVector baseLineCal; //Baseline line calibration data; transferred to lineCalArray when needed ver 114-5f

  //Impedance can be measured via S21 in a test jig, or via S11 in a reflection bridge
          //We need to know the reference impedance (a resistance) of each, and for the jig
          //we need to know if the DUT is in series, or shunted to ground. The S21 jig is actually used only in reflection mode.
          //ver114-6g added these


      int lineCalThroughDelay;  //Delay (ns) of line cal through connection. Used for continuity between lineCal dialog sessions
                                  //and to inform [BandLineCal]. need not be saved as a preference item.


      QString setupList[15];  //List of current test setups; used in test setup dialog only
      //-----OSL cal variables-----
          //ver115-1b added the following OSL items
      int OSLdoneO, OSLdoneS, OSLdoneL; //For communicating with [PerformOSLCal]

          //These items are the currently active data for reflection
      Q2DintVector  OSLa, OSLb, OSLc;    //OSL coefficients a, b, c, at same frequencies as ReflectArray() ver115-1b
      QString OSLRefType;  //Open, Short or Load ver116-4n
      int OSLcalLastUsedFull;   //=1 if cal window last used full OSL; 0 if last used reference.
      //global OSLApplyFull //delver116-4n We now always apply OSL in reflection mode if applyCalLevel=0.

          //These items are used only during the OSL calibration procedure to calculate coefficients.
      Q2DintVector OSLstdOpen;       //Actual refco of open standard at each step, real/imaginary
      Q2DintVector OSLstdLoad;       //Actual refco of load standard at each step, real/imaginary
      Q2DintVector OSLstdShort;       //Actual refco of short standard at each step, real/imaginary
      Q2DintVector OSLcalOpen;       //Measured (during cal) open at each step, db/angle converted to real/imag in ProcessOSLCal
      Q2DintVector OSLcalLoad;       //Measured (during cal) load at each step, db/angle converted to real/imag in ProcessOSLCal
      Q2DintVector OSLcalShort;      //Measured (during cal) short at each step,  db/angle converted to real/imag in ProcessOSLCal

      int OSLLastSelectedCalSet;  //Standard cal set selected last time dialog was open
      QString OSLOpenSpec, OSLShortSpec, OSLLoadSpec;    //RLC specs for currently selected OSL standards ver116-4i
      QString OSLFileOpenSpec, OSLFileShortSpec, OSLFileLoadSpec;    //RLC specs for OSL standards, used to transfer to and from files. ver116-4i
      QString OSLFileCalSetName, OSLFileCalSetDescription;   //ver116-4i

      QString OSLCalSetNames[11];  //List of OSL cal sets; zero entry is used. Entries correspond to OSLCalSetFileNames   ver115-7a
      QString OSLCalSetFileNames[11];  //List of OSL cal set descriptions (long); zero entry is used. redim//d as necessary  ver115-7a
      int OSLCalSetNumber;     //Number of entries in list of cal set names and file names   ver115-7a

          //These items are the result of calibration and allow either base or band cal to be installed
          //The OSLBasex arrays are fixed size; the others are expanded as necessary
      Q2DintVector OSLBaseA, OSLBaseB, OSLBaseC;       //Base coefficients;  used to fill OSLx()
      Q2DintVector OSLBandA, OSLBandB, OSLBandC;       //Band coefficients (matches current frequencies)
      Q2DintVector OSLBaseRef, OSLBandRef;       //Freq(0) and dB(1) and angle(2) update info. Ref is 0 until cal update is run.

      QString OSLBandRefType, OSLBaseRefType; //Open, Short or Load ver116-4n

          //We keep track of the parameters for the last cal. Number of steps is set to -1 to indicate no valid cal
      int OSLError; //=1 if math error occurred in calculating OSL coeff; used to nullify the cal ver115-4j
      int OSLBaseStartFreq, OSLBaseEndFreq, OSLBaseNumSteps, OSLBaseLinear;
      QString OSLBasePath;   //Params for last base OSL calibration
      int OSLBandStartFreq, OSLBandEndFreq, OSLBandNumSteps, OSLBandLinear;
      QString OSLBandPath;   //Params for last band OSL calibration
      QString OSLBandS11JigType, OSLBaseS11JigType;   //Jig type--"Reflect" or "Trans"-- for last cal
      QString OSLBaseS21JigAttach, OSLBandS21JigAttach;   //S21 jig for last OSL cal; relevant only if jig type is "Trans"
      int OSLBaseS11BridgeR0, OSLBandS11BridgeR0;   //Bridge R0 for last OSL cal; relevant only if jig type is "Reflect"
      int OSLBaseS21JigR0, OSLBandS21JigR0;     //S21 jig R0 for last OSL cal; relevant only if jig type is "Trans"
      QString OSLBaseTimeStamp;   //Time stamp for last base open, load, short cal.
      QString OSLBandTimeStamp;   //Time stamp for last band open, load, short cal.

              //Following are sweep params at which base OSL cal coefficients were last installed
              //We don't need these for installed Band cal, because they would all match the sweep params
              //at the time of installation.
      int installedOSLBaseLinear;
      QString installedOSLBaseRefType; //ver116-4n

      //-------End OSL cal variables-------//

       //If doing color cycling, the trace colors are changed at the start of each scan. If not cycling, cycleNumber is stuck at 1.

      int doCycleTraceColors, cycleNumber; //switch to signal cycling, and number (1-3) of current place in cycle ver116-4s


      int calInProgress;  //variable set to 1 before starting a cal sweep, then to 0 when done

          //ver114-6b moved the following dim statements here
          //SEWgraph; the following arrays may be expanded in ResizeArrays to accomodate more steps
          //SEWgraph Pixel values are no longer kept in these arrays, so references to thispointx, thispointmag, thispointphase,
          //oldmagpixwl and oldphapixel should be ignored. Eventually, the arrays could be compacted to eliminate those unused slots.
      long long int constMaxValue;
      //constMaxValue=1e12   //Max value for RLC components and certain calculations. ver115-1b
          //Data is put into datatable() point by point as it is gathered. Its frequency is the 0-1 GHz "equivalent 1G frequency".
          //In VectorTrans and ScalarTrans modes, the data is also saved to S21DataArray() with the actual sweep frequency,
          //and if reflection mode convert to S11 and save to ReflectArray()
      Q2DfloatVector datatable;   //data from most current sweep, (0)thisstep,(1)thisfreq(hardware freq),(2)processed magpower,(3)processed phase, (4)band (1G-3G) ver116-4s
          //Note: magarray(x,1) is no longer used; magarray(x,2) is never actually used to store magnitude ver116-1b
      Q2DintVector magarray;    //magni pixels for each step#: (0)thispointx, (1)oldmagpixel,(2)thispointmag(3)magdata
          //Note: phaarray(x,1) is no longer used; phaarray(x,2) is never actually used to store phase ver116-1b
      Q2DfloatVector phaarray;    //(0)pdmcmd; phase pixels for each step#:(1)oldphapixel,(2)thispointphase,(3)phadata,(4)pdmread ver111-39d

      //Q2DfloatVector PLL1array;  //(0-23)N23thruN0,(24-39)notused,(40)pdf1,(43)LO1freq,(45)ncounter,(46)Fcounter,(47)Acounter,(48)Bcounter. ver111-30a
      //Q2DfloatVector PLL3array;  //(0-23)N23thruN0,(24-39)notused,(40)pdf3,(43)LO3freq,(45)ncounter,(46)Fcounter,(47)Acounter,(48)Bcounter. ver111-30a
      //Q2DfloatVector DDS1array;  //(0-39)sw0-sw39,(40-44)w0-w4,(45)base,(46)actualdds1output
      //Q2DfloatVector DDS3array;  //(0-39)sw0-sw39,(40-44)w0-w4,(45)base,(46)actualdds3output
      QVector<int> freqCorrection; //freq correction factors for frequency of each step in current sweep; added to raw data
          //frontEndCalData is the raw data, freq and dBm, for the current front end. It is interpolated to frontEndCorrection
          //on Restart to match the current scan points. It is resized if necessary when a front end file is loaded
          //first index is 1-based. For second index: 0=freq; 1=dBm ver115-9d
          //The data is the value to be subtracted from the raw power readings. Subtraction is used so a front end file
          //can be created by a transmission measurement of the front end, after calibrating with a through connection.
      Q2DintVector frontEndCalData;
      QVector<int> frontEndCorrection; //correction for front end in use for each step in current sweep; subtracted from raw data

      int frontEndCalNumPoints; //Number of valid points in frontEndCalData
      QString frontEndActiveFilePath;  //Path name for active front end file. Only relevant in SA modes.ver115-9c
      QString frontEndLastFolder;  //path to Last folder from which front end was loaded

              //ver114-2d combined config arrays into one, and deleted configarray
      Q2DintVector cmdallarray;//(2020,40) //(0-15)DDS1+DDS3, (16-39)PLL1+DDS1+PLL3+DDS3

      int suppressHardware; //=1 to suppress hardware operations, otherwise 0 //ver115-6c
      int suppressHardwareInitOnRestart;    //=1 to skip hardware re-initialization to speed up Restart or [PartialRestart].
      //ver116-4d deleted wantHardwareInitOnPartialRestart

      //-------Items for multi-scanning----------- ver115-8c
      //In SA mode only, it is possible to rotate through several different scan settings; this
      //is called multi-scanning. One sweep is completed with one setting, then we move to the next.
      //When one is completed, its graphic is displayed in its own window.
      //The settings are saved, as is the datatable info. The datatable info for a scan can be reloaded
      //into the main window when scanning is stopped.
      //Multiscan entries are numbered from 1 through multiscanMaxNum
      //Entry zero is info for the main graph.
      int multiscanCurrNum;     //Current entry number
      int multiscanMaxNum;      //Maximum entry number
      int multiscanIsOpen;   //=1 when multiscan windows are open, even if scan not in progress.
      int multiscanInProgress;    //=1 when actually scanning in multiscan mode
      int multiscanHaltAtEnd;   //set to non-zero during multiscan to cause halt at end, value depends on window selected
      int  multiscanSaveRefreshEachScan; //Saves for restoration when quitting multiscan


  //---------------END OF VARIABLES DECLARATIONS-------


      float lastdds1output, lastdds3output;
      float dds3output;
      float lastpdmstate;
      float pdmstate;
      float dds1output;
      float dds2output;
      float reference;

      int bUseUsb;
      QString refHeadingColor1,refHeadingColor2;
      int calfigModuleVersion;



};

#endif // GLOBALVARS_H
