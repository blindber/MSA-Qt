#include "twoport.h"

twoPortModule::twoPortModule()
{
  //The following can be used directly to index TwoPortArray to retrieve the data
  //Note that the constant for an angle is always one more than that for its corresponding DB
  /*constTwoPortS11DB=1;
  constTwoPortS21DB=3;
  constTwoPortS12DB=5;
  constTwoPortS22DB=7;
  constTwoPortS11Ang=2;
  constTwoPortS21Ang=4;
  constTwoPortS12Ang=6;
  constTwoPortS22Ang=8;

  //The following can be used directly (after subtracting constTwoPortMatchedS11DB-1) to index TwoPortMatchedSParam to retrieve the data
  //Note that the constant for an angle is always one more than that for its corresponding DB
  constTwoPortMatchedS11DB=9; //ver116-2a
  constTwoPortMatchedS11Ang=10; //ver116-2a
  constTwoPortMatchedS21DB=11; //ver116-2a
  constTwoPortMatchedS21Ang=12; //ver116-2a
  constTwoPortMatchedS12DB =13; //ver116-2a
  constTwoPortMatchedS12Ang =14; //ver116-2a
  constTwoPortMatchedS22DB=15; //ver116-2a
  constTwoPortMatchedS22Ang=16; //ver116-2a

  //The following do not directly index an array; they need to be calculated
  constTwoPortKStability=17;    //ver116-2a
  constTwoPortMuStability=18;  //ver116-2a
*/
  TwoPortInitVariables(); //initialize variables including default Y axis ranges for Two-Port ver116-1b

}
//==========================Two-Port Module  ver116-1b============================

//Individual scans can be assembled into two-port S-Params by adding them to the appropriate
//entries in TwoPortArray, whose first index specifies the step number, and second index indicates
//Frequency(0), S11(1=db, 2=angle), S21(3=db, 4=angle), S12(5=db, 6=angle) and S22(7=db, 8=angle)
//Frequency is the true frequency in MHz, not the equivalent 1G frequency.
//This array is resized when necessary, not in ResizeArrays

void twoPortModule::TwoPortInitVariables()
{
  //Initialize variables including Y axis default ranges for each category
  TwoPortRefDBTop = 0;
  TwoPortRefDBBot = -75;
  TwoPortTransDBTop = 0 ;
  TwoPortTransDBBot = -100;
  TwoPortPhaseTop = 180;
  TwoPortPhaseBot = -180;
  TwoPortStabilityTop = 5;
  TwoPortStabilityBot = 0;
  TwoPortZMagTop = 250;
  TwoPortZMagBot = 0;
  TwoPortReactTop = 250;
  TwoPortReactBot = -250;
  TwoPortRTop = 250;
  TwoPortRBot = -250;
  TwoPortCTop = 1e-8;
  TwoPortCBot = 0;
  TwoPortLTop = 1e-6;
  TwoPortLBot = 0;
  TwoPortReturnLossTop = 75;
  TwoPortReturnLossBot = 0;
  TwoPortFileDataForm = "DB" ;
  TwoPortFileFreqForm = "MHz";

  TwoPortMatchParamValid = 0;
  TwoPortS11S22Form = "S";
  TwoPortSourceIsCap = 1;
  TwoPortLoadIsCap = 1;
  TwoPortSourceComponValue = 0;
  TwoPortLoadComponValue = 0;
  TwoPortSourceIsShunt = 1;
  TwoPortLoadIsShunt = 1;
  TwoPortSourceZR = 50;
  TwoPortLoadZR = 50;
}

void twoPortModule::TwoPortResize(int maxSteps)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub TwoPortResize maxSteps 'Resize TwoPortArray to hold maxSteps steps and clear sweep description
    'To save time, we never make the array smaller, unless it is huge
    if maxSteps>TwoPortMaxSteps or (maxSteps<TwoPortMaxSteps and TwoPortMaxSteps>1000) then
        redim TwoPortArray(maxSteps,8)
        redim TwoPortABCD(maxSteps,8)
        redim TwoPortMatchedSParam(maxSteps,8)
        TwoPortMaxSteps=maxSteps    'Caller must deal with TwoPortNumSteps
    end if
    'Enter default values for virtually no reflection or transmission. These will persist until
    'actual data is entered. This allows a valid two-port to be created with only actual S11 data.
    for i=0 to TwoPortMaxSteps 'Set to default values ver116-1b
        TwoPortArray(i,0)=0
        TwoPortArray(i,1)=-199 : TwoPortArray(i,2)=0
        TwoPortArray(i,3)=-199 : TwoPortArray(i,4)=0
        TwoPortArray(i,5)=-199 : TwoPortArray(i,6)=0
        TwoPortArray(i,7)=-199 : TwoPortArray(i,8)=0
    next i
    for i=0 to 4 : TwoPortValidEntries(i)=0 : next i  'We have no valid data
    for i=0 to 8 : TwoPortParamShowing(i)=0 : next i
    TwoPortMatchParamValid=0
    call mClearMarkers  'we assume the existing markers will make no sense
end sub
*/

}

int twoPortModule::TwoPortGetY1Type()
{
  return TwoPortY1Type;
}

int twoPortModule::TwoPortGetY2Type()
{
  return TwoPortY2Type;
}

void twoPortModule::TwoPortDetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString yLabel, int &yIsPhase, QString &yForm)
{
  //Return format info
  //graph.componConst indicates the data type. We return
  //yAxisLabel$  The label to use at the top of the Y axis
  //yLabel$   A typically shorter label for the marker info table
  //yIsPhase$ =1 if the value represents phase. This indicates whether we have wraparound issues.
  //yForm$    A formatting string to send to uFormatted$() to format the data
  //
  /* fix me
  QString SRef;
  yIsPhase=0;  //Default, since most are not phase
  if (componConst == vars->constTwoPortS11DB
      || componConst == vars->constTwoPortS22DB
      || componConst == vars->constTwoPortMatchedS11DB
      || componConst == vars->constTwoPortMatchedS22DB) 
  {
    if (componConst==vars->constTwoPortS11DB) SRef="S11";
    if (componConst==vars->constTwoPortS22DB) SRef="S22";
    if (componConst==vars->constTwoPortMatchedS11DB) SRef="*S11";   //Add * for matched param
    if (componConst==vars->constTwoPortMatchedS22DB) SRef="*S22";
    if (TwoPortS11S22Form=="S")
    {
      yAxisLabel=SRef+" dB";
      yLabel=SRef+"dB";
      yForm="####.###";
    }
    else //will graph reflection in another form, using the same data type constants as the regular graph
    {
      DetermineGraphDataFormat(TwoPortReflectTypeDB, yAxisLabel, yLabel,yIsPhase, yForm);     
      yAxisLabel=SRef+" "+yAxisLabel;
      yLabel=SRef+" "+yLabel;
    }
  }
  else if (componConst == vars->constTwoPortS21DB
           || componConst == vars->constTwoPortS12DB
           || componConst == vars->constTwoPortMatchedS21DB
           || componConst == vars->constTwoPortMatchedS12DB) 
  {
    if (componConst==vars->constTwoPortS21DB) SRef="S21";
    if (componConst==vars->constTwoPortS12DB) SRef="S12";
    if (componConst==vars->constTwoPortMatchedS21DB) SRef="*S21";
    if (componConst==vars->constTwoPortMatchedS12DB) SRef="*S12";
    yAxisLabel=SRef+" dB";
    yLabel=SRef+"dB";
    yForm="####.###";
  }
  else if (componConst == vars->constTwoPortS11Ang
           || componConst == vars->constTwoPortS22Ang
           || componConst == vars->constTwoPortMatchedS11Ang
           || componConst == vars->constTwoPortMatchedS22Ang)
  {
    if (componConst==vars->constTwoPortS11Ang) SRef="S11";
    if (componConst==vars->constTwoPortS22Ang) SRef="S22";
    if (componConst==vars->constTwoPortMatchedS11Ang) SRef="*S11";
    if (componConst==vars->constTwoPortMatchedS22Ang) SRef="*S22";
    if (TwoPortS11S22Form=="S")
    {
      yAxisLabel=SRef+" Deg";
      yLabel=SRef+"Deg";
      yForm="#####.##";
      yIsPhase=1;
    }
    else //will graph reflection in another form, using the same data type constants as the regular graph
    {
      DetermineGraphDataFormat(TwoPortReflectTypeAng, yAxisLabel, yLabel,yIsPhase, yForm);                 end if
      yAxisLabel=SRef+" "+yAxisLabel; yLabel=SRef+" "+yLabel;   
    }
  }
  else if (componConst == vars->constTwoPortS12Ang || componConst == vars->constTwoPortS21Ang)
  {
    if (componConst==vars->constTwoPortS12Ang) SRef="S12"; else SRef="S21";
    yAxisLabel=SRef+" Deg";
    yLabel=SRef+"Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (componConst == vars->constTwoPortMatchedS21Ang || componConst == vars->constTwoPortMatchedS12Ang) //ver116-2a
  {
    if (componConst==vars->constTwoPortMatchedS21Ang) SRef="*S21"; else SRef="*S12";
    yAxisLabel=SRef+" Deg";
    yLabel=SRef+"Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (componConst == vars->constTwoPortKStability) //ver116-2a
  {
    yAxisLabel="K Stability";
    yLabel="K";
    yIsPhase=0;
    yForm="###.###";
  }
  else if (componConst == vars->constTwoPortMuStability) //ver116-2a
  {
    yAxisLabel="Mu Stability";
    yLabel="Mu";
    yIsPhase=0;
    yForm="###.###";
  }
  else if (componConst == vars->constNoGraph)
  {
    yAxisLabel="None"; yLabel="None";
    yForm="####.##";    //Something valid, in case it gets mistakenly used

  }
  else
  {
    yForm="###.##";
    yAxisLabel="Invalid";
    yLabel="Invalid";
  }
  */
}



//==========================End Two-Port Module=================================

