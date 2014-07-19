#ifndef TWOPORT_H
#define TWOPORT_H

#include <QtCore>

class twoPortModule
{
public:
  twoPortModule();

  void TwoPortInitVariables();
  void TwoPortResize(int maxSteps);

  int TwoPortGetY1Type();
  int TwoPortGetY2Type();
  void TwoPortDetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString yLabel, int &yIsPhase, QString &yForm);




private:

  //==========================Two-Port Module  ver116-1b============================
  /*
  //Individual scans can be assembled into two-port S-Params by adding them to the appropriate
  //entries in TwoPortArray, whose first index specifies the step number, and second index indicates
  //Frequency(0), S11(1=db, 2=angle), S21(3=db, 4=angle), S12(5=db, 6=angle) and S22(7=db, 8=angle)
  //Frequency is the true frequency in MHz, not the equivalent 1G frequency.
  //This array is resized when necessary, not in ResizeArrays
  dim TwoPortArray(100,8)

  //TwoPortABCD contains the parameters converted to ABCD format, stored in the order
  //A(1=Real, 2=Imag), B(3=Real, 4=Imag), C(5=Real, 6=Imag), D(7=Real, 8=Imag).
  //They are used to speed calculations of TwoPortMatchedSParam when impedance matching is changed.
  //TwoPortMatchParamValid=1 when these arrays have valid values, and =0 when they need recalculation.
  dim TwoPortABCD(100,8)  //Used to convert TwoPortArray to ABCD params ver116-2a
  //TwoPortMatchedSParam is derived from TwoPortABCD and the impedance match specifications,
  //and holds the S-Params of the matched DUT.
  dim TwoPortMatchedSParam(100,8) //S params in same order as TwoPortArray
  */
  int  TwoPortMatchParamValid;   //=1 when TwoPortABCD and TwoPortMatchedSParam have valid values, and =0 when they need recalculation.
  /*
  //Whenever one of the 8 parameters is drawn, its drawing commands are saved in TwoPortParamTraces$ for future draws.
  //TwoPortParamTraces$(p,1) holds the axis 1 commands; TwoPortParamTraces$(p,2) holds axis 2 commands.
  dim TwoPortParamTraces$(8,2)    //Traces for drawing; zero not used for second index
                        //zero for first index means individual graphs; not a param pair

  //A data type may be described as an S-parameter, but we may be graphing reflection parameters as
  //something else, such as impedance. The form in which we are graphing is specified in TwoPortS11S22Form$,
  //which is: "S"(S-param), "Z"(Z mag/angle), "SerResReact"(series resistance/reactance),
  // "ParResReact"(parallel resistance/reactance),"RhoTheta"(rho/theta), "RLSWR"(return loss/SWR),
  // "ParRL" (parallel RL), "ParRC"(parallel RC), "SerRL"(series RL) or "SerRC"(series RC)
  */
  QString TwoPortS11S22Form;   //indicates whether S11 and S22 should be graphed as S-param or in another form

  int TwoPortReflectTypeDB;  //type that replaces reflection DB when TwoPortS11S22Form$<>"S"
  int TwoPortReflectTypeAng; //type that replaces reflection Ang when TwoPortS11S22Form$<>"S"

  int TwoPortTermWindowOpen;    //=1 when #twoPortTermWin is open for altering impedance match values

  int TwoPortValidEntries[5];   //Items 1-4 are 1 if corresponding S parameters have been entered
                        //1=S11, 2=S21, 3=S12, 4=S22
  int TwoPortParamShowing[9];  //1 if parameter is showing from click of parameter button 1-4 are S params 5-8 are terminated gain and RL
  int TwoPortMaxSteps;   //max number of steps for current size of TwoPortArray
  /*global TwoPortZ0        //Reference impedance of two port parameters
  global TwoPortGraphBoxWidth, TwoPortGraphBoxHeight   //width and height of our graph box; adusted when resizing occurs

  //Impedance matching can be specified in several ways. The simplest (and at the moment the only) is to
  //specify fixed impedances to which the source and load are matched--i.e. they are the impedances seen by
  //the DUT. The impedance seen by the actual source and load (and the actual impedances of source and load) are TwoPortZ0.
  //The reactance is specified by a capacitor or inductor, in series or shunt. For narrow band matching, this is
  //a fairly fixed reactance.
  */
  int TwoPortLoadIsShunt, TwoPortSourceIsShunt, TwoPortSourceIsCap, TwoPortLoadIsCap; //config of reactive components, and whether they are cap or inductor
  int TwoPortSourceComponValue, TwoPortLoadComponValue; //value of source and load capacitors (in pF) or inductors (in uH)
  int TwoPortSourceZR, TwoPortLoadZR;   //In and out termination resistances //ver116-2a
  //TwoPortTermCalcFreq is the freq(MHz) for calculating auto match and converting negative caps and inductors
  //into positive inductors and caps before doing match graphs. TwoPortTermCalcStep is the step number of that freq
  float TwoPortTermCalcFreq;
  int TwoPortTermCalcStep;
  /*
  dim TwoPortTitle$(4)       //Title of two port params. 4 lines; zero entry not used
  dim TwoPortSaveTitle$(4)    //Saved title from main graph when two-port window was opened
  //A specific parameter is sometimes selected for some action. The parmeter being graphed is always selected.
  global TwoPortSelectedParam    //1(S11), 2(S21), 3(S12), 4(S22), 5(matched S11), 6(matched S21), 7(matched S12) or 8(matched S22)
  //The following x and Y axis values are preserved when two port is closed
  global TwoPortStartFreq, TwoPortEndFreq, TwoPortNumSteps, TwoPortLinear //Info on sweep settings of current data
  */
  int TwoPortY1Bot, TwoPortY1Top, TwoPortY1Type, TwoPortY2Bot, TwoPortY2Top, TwoPortY2Type;
  /*
  //The following are used to save/restore the main graph settings
  global TwoPortSaveY1Top, TwoPortSaveY1Bot, TwoPortSaveY1Auto, TwoPortSaveY1Type
  global TwoPortSaveY2Top, TwoPortSaveY2Bot, TwoPortSaveY2Auto, TwoPortSaveY2Type
  global TwoPortSaveIsLinear
  global TwoPortSaveHorDiv, TwoPortSaveVertDiv    //ver116-4a
  */
  int TwoPortZ0Left, TwoPortZ0Top;  //top left coordinate of "Z0=..." text. Used to check for clicks.

  int TwoPortTransDBTop, TwoPortTransDBBot; //Most recent axis top and bottom for transmission params DB
  int TwoPortRefDBTop, TwoPortRefDBBot; //Most recent axis top and bottom for reflection params DB
  int TwoPortPhaseTop, TwoPortPhaseBot;     //Most recent phase top and bottom for any parameters
  int TwoPortStabilityTop, TwoPortStabilityBot;      //Most recent top and bottom for stability factors.ver116-2a
  int TwoPortZMagTop, TwoPortZMagBot;       //Most recent top and bottom for S11 or S22 as impedance  mag  
  int TwoPortReactTop, TwoPortReactBot;     //Most recent top and bottom for S11 or S22 as reactance  //ver116-4e
  int TwoPortRTop, TwoPortRBot;    //Most recent top and bottom for S11 or S22 as resistance  //ver116-4e
  float TwoPortCTop, TwoPortCBot;    //Most recent top and bottom for S11 or S22 as capacitance  //ver116-4e
  float TwoPortLTop, TwoPortLBot;    //Most recent top and bottom for S11 or S22 as inductance  //ver116-4e
  int TwoPortReturnLossTop, TwoPortReturnLossBot;    //Most recent top and bottom for S11 or S22 as return loss  //ver116-4e

  QString twoPortSaveMarkers, twoPortOurMarkers;      //saved main graph marker context when opening, and two port markers saved when closing

  int TwoPortWindH, TwoPortMenuBarH, TwoPortOptionsH;    //Windows handles to #twoPortWin, its menu bar and options submenu  

  QString TwoPortFileDataForm, TwoPortFileFreqForm; //Output file form for frequency (HZ, KHZ, MHZ, GHZ) and data (RA, MI, DB) ver116-4m




//==========================End Two-Port Module=================================


};

#endif // TWOPORT_H
