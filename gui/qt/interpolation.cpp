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
#include "interpolation.h"

interpolation::interpolation()
{
  intSrc.mresize(802,3);
  intDest.mresize(802,3);
  intSrcCoeff.mresize(802,8);
}
void interpolation::setGlobalVars(globalVars *newVars)
{
  vars = newVars;
}
//ver114-5p added Interpolation Module; ver114-5q moved it to preced config module
//====================START INTERPOLATION ROUTINES==========================
//The interpolation module handles linear and cubic interpolation, and deals with the fact
//that LB does not allow arrays as arguments. Three arrays are created: intSrc() contains the
//original data, which we assume for the moment is freq, mag and phase, in ascending order of freq.
//intDest() contains only frequency, and needs its mag and phase determined by interpolation. If
//cubic interpolation is used, the user calls intCreateCubicCoeffTable which fills the third array,
//intSrcCoeff(), with the eight coefficients (4 for mag, 4 for phase) that are needed to apply cubic
//interpolation.
//The user fills intSrc and intDest, calls intCreateCubicCoeffTable if necessary, and then calls
//intSrcToDest, which performs the interpolation. The user then copies the data (rounding as desired)
//from intDest() to the array where the data really belongs. The data in these arrays should not be
//relied on long-term, because another routine may make use of them.
//Angles must be in the range -180 to 180 degrees to interpolate properly. Returned angles may be
//outside that range and must be normalized as desired.

//'------------------Data access routines--------------------
//Even though our data is global, the user should access data only through these routines

void interpolation::intSetMaxNumPoints(int maxPoints)
{
  if (maxPoints+5<=intMaxPoints) return; //ver115-2d  'so we never shrink or waste time
  intMaxPoints=maxPoints+5; //ver115-2d
  intSrc.mresize(intMaxPoints,3); intDest.mresize(intMaxPoints,3);
  intSrcCoeff.mresize(intMaxPoints,8);
}
void interpolation::intReset()
{
  //Reset arrays and variables
  for (int i=0; i < intMaxPoints; i++) //we don't use zero for first index, but clear it anyway
  {
    intSrc[i][0]=0; intSrc[i][1]=0; intSrc[i][2]=0;
    intDest[i][0]=0; intDest[i][1]=0; intDest[i][2]=0;
  }
  intSrcPoints=0;
  intDestPoints=0;
}
void interpolation::intClearSrc()
{
  //Set source table to zero entries
   intSrcPoints=0;
}
void interpolation::intClearDest()
{
  //Set destination table to zero entries
  intDestPoints=0;
}
void interpolation::intAddSrcEntry(int f, int r, int im)
{
  //Add entry to end of source table
  //f=frequency, r=real part, im=imaginary part
  intSrcPoints=intSrcPoints+1;
  intSrc[intSrcPoints][0]=f;
  intSrc[intSrcPoints][1]=r;
  intSrc[intSrcPoints][2]=im;
}
void interpolation::intAddDestFreq(float f)
{
  //'Add new frequency to end of destination table
  intDestPoints=intDestPoints+1;
  intDest[intDestPoints][0]=f;
}
void interpolation::intGetSrc(int num, int &f, int &r, int &im)
{
  //Get values for source entry number num (1...)
  //f=frequency, r=real part, im=imaginary part
  f=intSrc[num][0];
  r=intSrc[num][1];
  im=intSrc[num][2];
}
int interpolation::intSrcFreq(int num)
{
  //Get frequency for source entry number num (1...)
  return intSrc[num][0];
}
void interpolation::intGetDest(int num, int &f, int &r, int &im)
{
  //Get values for dest entry number num (1...)
  //f=frequency, r=real part, im=imaginary part
  f=intDest[num][0];
  r=intDest[num][1];
  im=intDest[num][2];
}
int interpolation::intDestFreq(int num)
{
  //Get frequency for dest entry number num (1...)
  return intDest[num][0];
}
int interpolation::intMaxEntries()
{
 //Get maximum number of entries
 return intMaxPoints;
}
int interpolation::intSrcEntries()
{
  //Get number of steps in source table
  return intSrcPoints;
}
int interpolation::intDestEntries()
{
  //'Get number of steps in destination table
  return intDestPoints;
}
//------------------End Data access routines--------------------
float interpolation::intLinearInterpolateDegrees(float fract, float v1, float v2, float angleMin, float angleMax)
{
  //linearly interpolate between phase v1 and v2 based on fract
  //fract is a proportion (0...1) representing how far from v1 to v2 we want to go.
  //angleMin and angleMax are the min and max allowable values for angles; if the total range is at
  //least 360 degrees, angleMin is actually not allowed.
  //The complication with phase is that it wraps around. If the distance from v1 to v2 is shorter via wrap-around,
  //we will assume that wrap-around occurred.
  //This works when the real phase shift from point to point is normally small.
  float dif=v2-v1;
  float absDif;
  float range;
  if (dif<0) absDif=0-dif; else absDif=dif;
  if (absDif>180)  //change must exceed 180 degrees before we consider wrap-around
  {
    //If allowed range is 360 or less, we have wrap, which changes the angle difference by 360
    range=angleMax-angleMin;
    if (range<=360)
    {
        if (dif>0) dif=dif-360; else dif=dif+360;
    }
    else    //Unusual case with allowed range over 360 degrees
    {
        range=360*(int)((angleMax-angleMin)/360);  //make range a multiple of 360
        if (absDif>range/2)   //Assume wrap if change exceeds half the range
        {
            if (dif>0) dif=dif-range; else dif=dif+range;  //change difference by range, which makes dif magnitude smaller
        }
    }
  }
  float res=v1+fract*dif;
    //At this point, interp may be outside the bounds of angleMin and angleMax
  if (res>angleMin && res<=angleMax) {return res;}
  if (res==angleMin)    //disallow angleMin value if range is at least 360 degrees
  {
    if (angleMax-angleMin>=360) res=res+360;
    return res;
  }
  //Here res is below angleMin or above angleMax, so it should wrap around
  float delta;
  if (range<=360) delta=360; else delta=range;
  float wrappedRes;
  if (res<angleMin) wrappedRes=res+delta; else wrappedRes=res-delta;   //do wrap
  if (wrappedRes>=angleMin && wrappedRes<=angleMax)
    return wrappedRes;
  else
    return res;    //Use wrapped value if wrap put it in allowed range (which it will if range>=360)
}
void interpolation::intLinearInterpolation(int freq, int isPolar, int f1, int R1, int I1, int f2, int R2, int I2, int &p1, int &p2)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
' linearly interpolate between points 1 and 2 (each a frequency with complex value)
' freq=frequency for which value is to be determined
' This function will return the interpolated value for the real and imaginary parts
' in p1 and p2, respectively
'Interpolated angles may end up outside the original bounds of the angles, if wrap-around occurred,
'so the user should put them back in bounds. They will be at most 360 degress out-out-bounds.

    fSpan=f2-f1
            'If we are interpolating between identical entries, just use data from the first
    if fSpan=0 then p1=R1:p2=I1:exit sub
    ratio=(freq-f1)/fSpan  'Interpolation ratio
    p1=R1+ratio*(R2-R1)
    dif=I2-I1
    if isPolar=1 then
        'We are interpolating polar data, so what we are labeling as the imaginary part
        'is really the angle in degrees.
        if dif<0 then absDif=0-dif else absDif=dif
        if absDif<=180 then
            p2=I1+ratio*dif
        else
            'Apparent shift is more than 180 degrees, which means wrap occurred between v1 and v2.
            'We shift v1 360 degrees towards v2 which reduces the magnitude of dif by 360.
            if dif>0 then dif=dif-360 else dif=dif+360
            p2=I1+ratio*dif
            'At this point, p2 may be outside the bounds of the scaling of v1 and v2. The user must
            'adjust the result
        end if
    else
        p2=I1+ratio*dif
    end if

*/
}
void interpolation::intCreateCubicCoeffTable(int doPart1, int doPart2, int isAngle, int favorFlat, int doingPhaseCorrection)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'Create table of cubic coefficients intSrc() in intCubicCoeff ver116-1b
    'We separately calculate for the specified parts of srcInt. If isAngle=1 then part2 is an angle.
    'We pass favorFlat on to intCalcCubicCoeff, except if part2 is an angle we pass on favorFlat=1 for that part.
    'Each entry of the cubic coefficient table will have 4 numbers. 0-3 are the A,B,C,D
    'coefficients for interpolating the frequency power correction, which is a scalar.
    'Angles must be in the range -180<=angle<=180
    'If doingPhaseCorrection is 1, we are interpolating the phase correction factor,
    'and want to force the correction to 180 if this point phase correction is >179 or <-179. Extreme
    'correction values are used to indicate that phase at that level is unreliable, so the table of
    'phase corrections may have values of 180 for all ADC readings at and below a certain level. ver116-1b
    for i=1 to intSrcPoints
        if doPart1=0 then
            A=0 : B=0 : C=0 : D=0
        else
            partNum=1 : partIsAngle=0
            call intCalcCubicCoeff i,partNum,partIsAngle,favorFlat,A,B,C,D
        end if
        intSrcCoeff(i,0)=A :intSrcCoeff(i,1)=B
        intSrcCoeff(i,2)=C :intSrcCoeff(i,3)=D
        forcedTo180=0
        if doingPhaseCorrection=1 then  'ver116-1b
            'If this point is 180, force calculation to 180
            checkPhase=intSrc(i,2)
            if checkPhase>179 or checkPhase<-179 then
                'This point is 180; set coefficients so phase correction will calculate to 180
                A=180 : B=0 : C=0 : D=0
                forcedTo180=1   'Flag that we need no more calculation at this point
            end if
            if forcedTo180=0 then
                'this point is not 180 but one of prior two points is, then treat the 180 value
                'as being the same as this point. Note we may alter intSrc, but that is just a temporary
                'array used only for these interpolations.
                if i>1 then
                    checkPhase=intSrc(i-1,2)
                    if checkPhase>179 or checkPhase<-179 then intSrc(i-1,2)=intSrc(i,2)
                end if
                if i>2 then
                    checkPhase=intSrc(i-2,2)
                    if checkPhase>179 or checkPhase<-179 then intSrc(i-2,2)=intSrc(i,2)
                end if
            end if
        end if

        if doPart2=0 then
            A=0 : B=0 : C=0 : D=0
        else
            if forcedTo180=0 then    'ver116-1b
                partNum=2 : partIsAngle=isAngle
                'For an angle, specify to "favor flat", because
                'we expect the phase not to approach vertical
                if isAngle then doFlat=1 else doFlat=favorFlat
                call intCalcCubicCoeff i,partNum,partIsAngle,doFlat,A,B,C,D
            end if
        end if
        intSrcCoeff(i,4)=A :intSrcCoeff(i,5)=B
        intSrcCoeff(i,6)=C :intSrcCoeff(i,7)=D
    next i
    */
}
void interpolation::intCalcCubicCoeff(int pointNum, int partNum, int isAngle, int favorFlat, int &A, int &B, int &C, int &D)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
   'Calculate the cubic interpolation coefficients to apply to a point
    'lying between (possibly including) points pointNum-1 and pointNum of intSrc().
    'partNum=1 to process real part and =2 to process imag part.
    'If isAngle=1 then we are interpolating an angle, otherwise not.

'The coefficients will approximate y values in the interval from pointNum-1 to pointNum
'as a cubic equation, such that it passes through the endpoints with the desired slope.
'To determine the desired slope, we use the points to the left and right of the interval.
'This gives us four points, 0-3, of which pointNum is number 2. The interval in which data
'will be interpolated with these coefficients is from point 1 to point 2. We determine what
' slopes we want at the endpoints and then fit a cubic equation to the interval. In
' general, at each point we want the curve on each side to have a common slope equal
' to some sort of average of the interval slopes on each side. We do a straight
' arithmetic average, except that if favorFlat=1 then we average the inverses of the
' slopes and invert that average. The latter tends to make the averaged slope flatter,
' which is useful to avoid overshoot/undershoot.
' Assume the cubic function
'       y=A + B(x-x2) + C(x-x2)^2 + D(x-x2)^3
' passes through points (x1, y1) and (x2, y2), with f1<f2, and with slopes of
' m1 and m2 respectively. Then
'   Let K=x1-x2
'   A=y2
'   B=m2
'   C=(m1-m2)/2 - (3/2)*D*K
'   D=[K(m1+m2)+2(y2-y1)] / K^3

    '(0)If data is less than point 1, we are going to use the point 1 value
    if pointNum=1 then
        A=intSrc(1,partNum)
        B=0 : C=0 :D=0
        exit sub
    end if

    '(1) Get data for four points, 0-3, to be used in interpolation. pointNum is
    'point 2 of these four, so the data to be interpolated will lie between points
    '1 and 2.If we are near one end, point 0 or point 3 will be missing.
    x1=intSrc(pointNum-1,0) : y1=intSrc(pointNum-1,partNum)
    x2=intSrc(pointNum,0) : y2=intSrc(pointNum,partNum)
    if pointNum>2 then x0=intSrc(pointNum-2,0) : y0=intSrc(pointNum-2,partNum)
    if pointNum<intSrcPoints then x3=intSrc(pointNum+1,0) : y3=intSrc(pointNum+1,partNum)
    dif01=y1-y0     'y change from point 0 to point 1 (not used if there is no point 1)
    dif12=y2-y1     'y change from point 1 to point 2
    dif23=y3-y2     'y change from point 2 to point 3 (not used if there is no point 3)

'(2) if points 1 and 2 are the same, then use Y1 value
    if x1=x2 then A=y1 : B=0 : C=0 :D=0 : exit sub

'(3) Deal with angles wrapping at +/-180
    if isAngle=1 then    'deal with angles wrapping at +/-180
        'For any pair of adjacent points, the absolute phase difference exceeds 180 degrees,
        'we assume wrap-around occurred so that the real difference is no more than 180 degrees.
        'If this occurs we reduce the magnitude of the difference by 360 degrees.
        if dif12<0 then absDif12=0-dif12 else absDif12=dif12
        if pointNum<intSrcPoints then   'true if we have point 3
            if dif23<0 then absDif23=0-dif23 else absDif23=dif23
            if absDif23>180 then   'large difference between points 2 and 3
                if dif23>0 then dif23=dif23-360 else dif23=dif23+360  'reduce magnitude of difference by 360
            end if
        end if
        if absDif12>180 then   'large difference between points 1 and 2
            if dif12>0 then dif12=dif12-360 else dif12=dif12+360  'reduce magnitude of difference by 360
        end if
        if pointNum>2 then  'true if we have point 0
            if dif01<0 then absDif01=0-dif01 else absDif01=dif01
            if absDif01>180 then   'large difference between points 0 and 1
                if dif01>0 then dif01=dif01-360 else dif01=dif01+360  'reduce magnitude of difference by 360
            end if
        end if
    end if
'(4) Find m1, the desired slope of the cubic at point 1
    if pointNum>2 then
        if x0=x1 then
            inSlope=0      'should not happen
        else
            inSlope=dif01/(x1-x0)    'slope from point 0 to point 1
        end if
    else
        inSlope=dif12/(x2-x1)    'slope from point 1 to point 2
    end if
    outSlope=dif12/(x2-x1)  'slope from point 1 to point 2
    prod=inSlope*outSlope
    if prod <=0 then
            'if slope on either side is zero, or is positive on one side
            'and negative on the other, we want a zero slope
            m1= 0
    else
        'Calculate an average slope for point 1 based on connecting lines
        'We average the inverses of the slopes, then invert
        m1= 2*prod/(inSlope+outSlope)
    end if

'(5) Find m2, the desired slope of the cubic at point 2
    inSlope=outSlope    'slope from point 1 to point 2
    if pointNum<intSrcPoints then
        if x2=x3 then
            outSlope=0      'should not happen
        else
            outSlope=dif23/(x3-x2)   'slope from point 2 to point 3
        end if
    end if
    'if pointNum is the final point, outSlope will remain the slope from point 1 to point 2
    prod=inSlope*outSlope
    if prod <=0 then
            'if slope on either side is zero, or is positive on one side
            'and negative on the other, we want a zero slope
            m2= 0
    else
        'Calculate an average slope for point 1 based on connecting lines
        if favorFlat then
            m2= 2*prod/(inSlope+outSlope) 'average the inverses of the slopes, then invert
        else
            m2=(inSlope+outSlope)/2
        end if
    end if

'(6) Calc constants for cubic. We are returning A,B,C and D.
    K=x1-x2
    A=y2
    B=m2
    D=(K*(m1+m2)+2*(dif12))/K^3
    C=(m1-m2)/(2*K)-1.5*D*K
end sub

sub intCubicInterpolation targData, ceil, wantV2, byref v1, byref v2
    'This function returns the interpolated values v1 and v2 (but v2 only if wantV2=1),
    'based on the value targData and its position in intSrc(). ceil specifies the ceiling
    'entry for targData in intSrc()--i.e. the first entry whose x value is  >= targData.
    'if ceil=-1, we will look up that position with binary search. Arrays must be in
    'ascending order of x values (usually frequency).
    'We use cubic interpolation using the cubic coefficients which must have been
    'precalculated in intSrcCoeff()

    if ceil=-1 then ceil=intBinarySearch(targData)   'search intSrc() to get ceil
    'ceil now is the first entry >= magdata, except that if no entry meets that test,
    'ceil will be one past the end.
    v1=0 :v2=0
    if ceil>intSrcPoints then
        'Off top end;use values for final intSrc() entry
        v1=intSrc(intSrcPoints,1)
        v2=intSrc(intSrcPoints,2)
        exit sub
    end if
    if ceil=1 then
        'Off bottom end;use mag and phase correction for smallest ADC entry
        v1=intSrc(1,1)
        v2=intSrc(1,2)
        exit sub
    end if

    'Evaluate cubic at x=targData
    dif=targData-intSrc(ceil,0)
    A=intSrcCoeff(ceil,0) : B=intSrcCoeff(ceil,1)
    C=intSrcCoeff(ceil,2) : D=intSrcCoeff(ceil,3)
    v1 = A+dif*(B+dif*(C+dif*D))

    if wantV2=1 then
        A=intSrcCoeff(ceil,4) : B=intSrcCoeff(ceil,5)
        C=intSrcCoeff(ceil,6) : D=intSrcCoeff(ceil,7)
        v2 = A+dif*(B+dif*(C+dif*D))
        'TO DO--caller must put phase in proper range
    end if

*/
}
int interpolation::intBinarySearch(int searchVal)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Perform search of intSrc() to find the lowest entry whose lookup value is >=searchVal
'If dataType=0 then the lookup is for ADC value in calMagTable; otherwise it is for freq in in calFreqTable
'If searchVal is beyond the highest entry, we will return intSrcPoints+1
    top=intSrcPoints
    bot=1
    span=top-bot+1
    while span>4  'Do preliminary search to narrow the search area
        halfSpan=int(span/2)
        mid=bot+halfSpan
        thisVal=intSrc(mid,0)
        if thisVal=searchVal then intBinarySearch=mid : exit function   'exact hit
        if thisVal<searchVal then bot=mid+1 else top=mid  'Narrow to whichever half thisVal is in
        span=top-bot+1  'Repeat with either bot or top endpoints changed
    wend
    'Here thisVal is >= entry bot-1 but <= entry top, and we have a span of less than 4
    'Start with bot entry and find first entry >= searchVal
    ceil=bot
    thisVal=intSrc(ceil,0)
    if thisVal>=searchVal then intBinarySearch=ceil : exit function
    ceil=ceil+1 : if ceil>intSrcPoints then intBinarySearch=ceil : exit function
    thisVal=intSrc(ceil,0)
    if thisVal>=searchVal then intBinarySearch=ceil : exit function
    ceil=ceil+1 : if ceil>intSrcPoints then intBinarySearch=ceil : exit function
    thisVal=intSrc(ceil,0)
    if thisVal>=searchVal then intBinarySearch=ceil : exit function
    intBinarySearch=ceil+1     'searchVal is off the top of the table

*/
  return 1;
}
void interpolation::intSrcToDest(int isPolar, int interpMode, int params)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Creates a table of frequency vs. complex numbers by interpolating from another table.
'isPolar=1 if data is in polar form; otherwise 0
'interpMode=0 for linear interpolation; interpMode=1 for cubic interpolation
'params=1 means process real part only; params=2 means imag only; params=3 means both parts
'This is used to create the tables used for calibration. We may have measured data for the
'calibration standards, which may not be at the specific frequencies needed. We may also have
'already calculated the OSL coefficients, but not at the current frequency points. This is
'used to convert such data into data at the exact frequencies of the current scan.
'Since LB does not allow arrays as arguments, this has to be done with two fixed arrays.
'The original array is intSrc(point, data); the final is intDest(point, data),
'where point is the index for the entry number (0 is not used) and data is the index for
'the table data, which consists of frequency (0), real (1) and imaginary (2). (The actual
'labels "point" and "data" are not used.)
'intDest must be pre-filled with the desired frequencies, and intDestSteps must contain
'the number of frequency entries in intDest.
'intSrcSteps must contain the number of entries in intSrc (it's actually the number of points, not steps).
'Everything is written in terms of the data being real and imaginary, but it could instead
'be magnitude and angle. Different results are obtained using the different formats; the
'difference is small if the changes in the data from step to step are small, as they normally
'should be. The complication of interpolating in polar format is that the angle wraps around
'at +/- 180 degrees. If interpolating half way between -178 and +178, we don't want to get 0.
'Therefore, if isPolar=1, if the angle difference between two successive points exceeds 180, we
'assume a wrap-around occurred.
'For cubic interpolation, the cubic coefficient table must be up to date.
'Any angles we actually interpolate will be put into in the range -180<angle<=180
    if intSrcPoints<2 then notice "Interpolation error: insufficient points"
    currDestPoint=0 'Keeps track of last intDest point successfully processed
    srcF1=intSrc(1,0) 'First frequency in intSrc()
    'Use the cal data for the first intSrc() point so long as our target
    'frequency is less than or equal to the intSrc() frequency.
    for i=1 to intDestPoints
        destF=intDest(i,0) 'current step frequency
        if destF>srcF1 then exit for
        currDestPoint=i
        intDest(i,1)=intSrc(1,1)    'Directly transfer value from first entry
        intDest(i,2)=intSrc(1,2)
    next i
    if currDestPoint=intDestPoints then exit sub 'We have processed all points in intDest() ver115-4i
    currDestPoint=currDestPoint+1 'The next intDest() point to process
    'As long as we are within the bounds of intSrc,
    'interpolate to get the cal data for each step frequency.
    ceil=1 'This will become the point number of the first intSrc() freq
                    'greater than or equal to the then current intDest() frequency.
    for i=currDestPoint to intDestPoints
        destF=intDest(i,0)  'current point frequency
        'Move through intSrc() until we hit a frequency >= this intDest() frequency.
        while destF>intSrc(ceil,0)
            ceil=ceil+1   'move to next point in intSrc()
            if ceil>intSrcPoints then exit while   'ran off end of source table
        wend
        if ceil>intSrcPoints then exit for 'ran off end
        'Here ceil is the first baseLine step with a frequency>= our target step freq.
        'So we interpolate between points ceil-1 and ceil.
        srcF1=intSrc(ceil-1,0)   'low entry in src table : freq, mag and phase
        srcMag1=intSrc(ceil-1,1)
        srcPhase1=intSrc(ceil-1,2)  'Note this may actually be imaginary part, not phase
        srcF2=intSrc(ceil,0)   'high entry in src table : freq, mag and phase
        srcMag2=intSrc(ceil,1)
        srcPhase2=intSrc(ceil,2)
        if srcF2=destF then 'If exact frequency match, just copy source data ver115-2d
            v1=srcMag2 : v2=srcPhase2
        else
                'Do the interpolation
            if interpMode=0 then
                call intLinearInterpolation destF, isPolar, srcF1, srcMag1, srcPhase1, srcF2, srcMag2, srcPhase2,v1, v2
            else
                wantV2=1
                call intCubicInterpolation destF, ceil, wantV2, v1, v2 'find v1, v2 by cubic interpolation
            end if
        end if

        currDestPoint=i 'Record that we have this step taken care of
        intDest(i,1)=v1  'Enter results in intDest; x value is already there
        if isPolar then
            if v2>180 then v2=v2-360 else if v2<=-180 then v2=v2+360   'put into range -180<v2<=180
        end if
        intDest(i,2)=v2
    next i
    if currDestPoint=intDestPoints then exit sub 'We have processed all dest points
    currDestPoint=currDestPoint+1 'First unprocessed step
    'For any remaining points, use the values for the final entry
    'in intSrc
    for i=currDestPoint to intDestPoints
        intDest(i,1)=intSrc(intSrcPoints,1) 'Enter results in intDest; x value is already there
        intDest(i,2)=intSrc(intSrcPoints,2)
    next i
    'TO DO--caller must round results if desired

*/
}
int interpolation::LinearInterpolateDataType(int dataType, float fract, float v1, float v2)
{
  //Interpolate between values v1 and v2 based on fract
  //If the dataType is an angle, deal with wraparound using angle range of -180 to +180
  if (vars->DataTypeIsAngle(dataType))
  {
    return intLinearInterpolateDegrees(fract, v1, v2, -180, 180);
  }
  else //non-angles
  {
    return v1 + fract*(v2-v1);
  }
}

//====================END INTERPOLATION MODULE==========================

