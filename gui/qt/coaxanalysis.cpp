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
#include "coaxanalysis.h"
#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif

coaxAnalysis::coaxAnalysis()
{
  test = 3;
  maxCoaxEntries=100;
}

void coaxAnalysis::CoaxLoadDataFile(QString path)
{
  //Load coax data file into coaxData%. If error, clear coaxData%
  coaxNames.clear();
  numCoaxEntries = 0;

  if (!QFile::exists(path + "/MSA_Info/CoaxData.txt"))
  {
    if (!CoaxCreateFile(path)) //Create file with default entries
    {
      QMessageBox::critical(0,"Error","Unable to open/create coax data file.");// : exit sub
      return;
    }
  }

  QFile textFile(path + "/MSA_Info/CoaxData.txt");
  if (textFile.open(QFile::ReadOnly | QFile::Text))
  {
    //... (open the file for reading, etc.)
    QTextStream textStream(&textFile);
    while (true)
    {
      QString fullLine;
      QString line = textStream.readLine();
      if (line.isNull())
          break;
      else
      {
        fullLine = line;
        if (line.left(1) != "!")
        {
          QString aName = util.uExtractTextItem(line, ",");
          double R0, VF, K1, K2, dum1, dum2;
          if (aName != "")
          {
            int isErr;
              isErr = util.uExtractNumericItems(3,line,",",R0, VF, K1);
              if (isErr==0)
                isErr=util.uExtractNumericItems(1,line,",",K2, dum1, dum2);
              if (isErr) QMessageBox::critical(0,"Error reading coax data file: ",fullLine);
          }
          coaxNames.append(aName);
          coaxData[numCoaxEntries][0]=R0; coaxData[numCoaxEntries][1]=VF;
          coaxData[numCoaxEntries][2]=K1; coaxData[numCoaxEntries][3]=K2;
          numCoaxEntries=numCoaxEntries+1;
        }
      }
    }
    textFile.close();
  }
  else
  {
    QMessageBox::critical(0,"Unable to open/create coax data file.", textFile.errorString());// : exit sub
  }

  //vars->numCoaxEntries = vars->coaxNames.count();
  /*
  while EOF(#fHndl%)=0
      Line Input #fHndl%, tLine%  //get one line
      if Left%(tLine%,1)<>"!" then    //skip comment lines, which start with !  ver115-4b
          //Line contains name, Z0, VF, K1, K2, comma-separated
          fullLine%=tLine%
          aName%=uExtractTextItem%(tLine%, ",")    //get coax name
          if aName%<>"" then
              isErr=uExtractNumericItems(3,tLine%,",",R0, VF, K1)
              if isErr=0 then isErr=uExtractNumericItems(1,tLine%,",",K2, dum1, dum2)
              if isErr then notice "Error reading coax data file: ";fullLine%
          end if
          numCoaxEntries=numCoaxEntries+1
          coaxNames%(numCoaxEntries)=aName% : coaxData(numCoaxEntries, 1)=R0 : coaxData(numCoaxEntries, 2)=VF
          coaxData(numCoaxEntries, 3)=K1 : coaxData(numCoaxEntries, 4)=K2
      end if
  wend
  close #fHndl%
*/
}

bool coaxAnalysis::CoaxCreateFile(QString path)
{
  //Each line contains coax name, Z0, VF, K1, K2, comma-separated
  //VF is velocity of propagation as fraction of speed of light, typically 0.5-0.7 for coax cable
  //K1 and K2 are in db/hundred ft per the equation
  //  Total db loss=(Len(in feet)/100) * ((K1)*Sqrt(freq) +(K2)*freq)  freq is in MHz
  //These are derived from http://www.vk1od.net/calc/tl/tllc.php
  //and the program TLDetails.exe

  QDir().mkpath(path + "/MSA_Info/");
  QFile fOut(path + "/MSA_Info/CoaxData.txt");
  if (fOut.open(QFile::WriteOnly | QFile::Text))
  {
    QTextStream s(&fOut);
    s << "!Transmission line data file\n";
    s << "!Type, Z0, VF, K1, K2; K factors based on 100 ft and MHz.\n";
    s << "Lossless 50 ohms,50,0.66,0,0\n";
    s << "Lossless 75 ohms,75,0.66,0,0\n";
    s << "FR4 50-ohm microstrip,50,0.55,0.30,0.076\n";
    s << "RG6A,75,0.66,0.263,0.00159\n";
    s << "RG8,52,0.66,0.182,0.00309\n";
    s << "RG11,75,0.66,0.186,0.0023\n";
    s << "RG58,51.5,0.66,0.342,0.00901\n";
    s << "RG58C,50,0.66,0.394,0.00901\n";
    s << "RG59,73,0.66,0.321,0.00184\n";
    s << "RG59B,75,0.66,0.318,0.00192\n";
    s << "RG141A,50,0.695,0.26,0.00473\n";
    s << "RG174,50,0.66,0.792,0.00484\n";
    s << "RG188A,50,0.695,0.953,0.000192\n";
    s << "RG213,50,0.66,0.182,0.00309\n";
    s << "RG402,50,0.705,0.401,0.00074\n";
    s << "RG405,50,0.705,0.67,0.00093\n";
    s << "Andrew Heliax LDF4-50A,50,0.891,0.0643,0.000187\n";
    s << "Andrew Heliax LDF5-50A,50,0.891,0.0349,0.000153\n";
    s << "Andrew Heliax LDF6-50,50,0.891,0.0239,0.00014\n";
    s << "ARRL Generic 300,300,0.801,0.102,0.000682\n";
    s << "ARRL Generic 450,450,0.911,0.0271,0.000242\n";
    s << "Wireman 551,400,0.903,0.0496,0.0012\n";
    s << "Wireman 552,370,0.918,0.051,0.001\n";
    s << "Wireman 553,390,0.899,0.0621,0.0009\n";
    s << "Wireman 554,360,0.929,0.0414,0.0017\n";

    fOut.close();
    return true;
  }
  return false;
}

void coaxAnalysis::setFilePath(QString path)
{
  DefaultDir = path;
  CoaxLoadDataFile(path);
}


/*
'=============Start Coax Analysis Module===================================
function CoaxOpenDataFile%(isInput) 'open file for input or output
    'Open coax data file; return its handle
    'If file does not exist, return "".
    fName%=DefaultDir%;"\MSA_Info\CoaxData.txt"
    On Error goto [noFile]
    if isInput then open fName% for input as #coaxFile else open fName% for output as #coaxFile
    CoaxOpenDataFile%="#coaxFile"
    exit function
[noFile]
    CoaxOpenDataFile%=""
end function
        sub CoaxSaveDataFile
            'Save coax data file.
            fHndl%=CoaxOpenDataFile%(0) 'open for output
            if fHndl%="" then notice "Unable to save coax data file." : exit sub
            print #fHndl%, "!Transmission line data file"
            print #fHndl%, "!Type, Z0, VF, K1, K2; K factors based on 100 ft and MHz."
            for i=1 to numCoaxEntries
                    'Print comma-separated items
                cName%=coaxNames%(i)
                if cName%<>" " then print #fHndl%, cName%;",";coaxData(i,1);",";coaxData(i,2);",";coaxData(i,3);",";coaxData(i,4)
            next i
            close #fHndl%
        end sub
        */
void coaxAnalysis::CoaxGetData(QString coaxName, float &R0, float &VF, float &K1, float &K2)
{
  //Return data for named coax, or VF=0 if doesn't exist
  //VF is velocity of propagation as fraction of speed of light
  //K1 and K2 are in db/hundred ft per the equation
  //  Total db loss=(Len(in feet)/100) * ((K1)*Sqrt(freq) +(K2)*freq)  freq is in MHz
  int found = coaxNames.indexOf(coaxName);
  if (found==-1)
  {
    R0=50;
    VF=1;
    K1=0;
    K2=0;
    return;
  }
  R0=coaxData[found][0];
  VF=coaxData[found][1];
  K1=coaxData[found][2];
  K2=coaxData[found][3];
}
/*
    function CoaxDelayNS(VF,lenFeet)    'Return ns of one-way delay for velocity factor and length
        'Speed of light is 983.6 million feet per second
        if VF=0 then CoaxDelayNS=constMaxValue else CoaxDelayNS=1000*lenFeet/(VF*983.6) 'ver116-4i
    end function

    function CoaxWavelengthFt(fMHz,VF)    'Return wavelength for f in MHz and specified veloc. factor
        'Speed of light is 983.6 million feet per second
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        if fMHz=0 then CoaxWavelength=constMaxValue else CoaxWavelength=(983.6*VF)/fMHz
    end function

    function CoaxLossDB(fMHz, K1, K2, lenFeet)    'Return loss in DB for specified coax
        CoaxLossDB=(lenFeet/100) * (K1*Sqr(fMHz)+K2*fMHz)
    end function

    function CoaxLossA0(fMHz, K1, K2)    'Return loss factor in dB per hundred feet
        CoaxLossA0=K1*Sqr(fMHz)+K2*fMHz
    end function

    function CoaxLossAlpha(fMHz, K1, K2)    'Return loss factor in nepers/ft
        CoaxLossAlpha=0.001151 *(K1*Sqr(fMHz)+K2*fMHz)
    end function

    function CoaxBeta(fMHz, VF)    'Return beta, which is the number of radians/ft
        if VF<=0 then CoaxBeta=constMaxValue : exit function 'ver115-4b
        CoaxBeta=2*uPi()*fMHz/(VF*983.6)
    end function

    sub CoaxGetPropagationGamma fMHz, VF, K1, K2, byref Greal, byref Gimag  'Return gamma=alpha +j*beta
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        Greal=0.001151 *(K1*Sqr(fMHz)+K2*fMHz)
        if VF<=0 then Gimag=constMaxValue : exit sub 'ver115-4b
        Gimag=2*uPi()*fMHz/(VF*983.6)
    end sub

    function CoaxDelayDegrees(fMHz,VF, lenFeet)    'Return delay in degrees
        'Speed of light is 983.6 million feet per second
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        if VF<=0 then CoaxDelayDegrees=constMaxValue : exit function
        delayMicroSec=lenFeet/(VF*983.6)
        CoaxDelayDegrees=delayMicroSec*fMHz*360
    end function

    sub CoaxComplexZ0   fMHz, R, G, L, C, byref Z0Real, byref Z0Imag    'Calculate coax impedance from basic parameters
        'R=resistance in ohms/ft  G=conductance in ohms/ft
        'L=inductance in uH per foot
        'C=capacitance in pf per foot
        'Normally, we don't know these parameters directly, and this routine is used in iteration
        'by CoaxComplexZ0Iterate
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        if C<0.001 then C=0.001  'ver115-4b
        twoPiF=2*uPi()*fMHz
        call cxDivide R, twoPiF*L, G, twoPiF*C*1e-6, ZReal, ZImag    '(R+j*2*pi*F*L) / (G+j*2*pi*F*C*1e-6)
        call cxSqrt ZReal, ZImag, Z0Real, Z0Imag     'Take square root for final result
    end sub

    sub CoaxPropagationParams Z0Real, Z0Imag, fMHz, ac, ad, beta, byref R, byref G, byref L, byref C    'Calc R,G,L,C
        'Z0=complex characteristic impedance
        'ac=conductor loss coefficient=0.001151*A0c.  A0c=K1*sqr(fMHz)--see CoaxGetData
        'ad=dielectric loss coefficient=0.001151*A0c.  A0d=K1*fMHz--see CoaxGetData
        'beta=phase coefficient, radians/ft
        'R=resistance in ohms/ft  G=conductance in ohms/ft
        'L=inductance in uH per foot
        'C=capacitance in pf per foot
        'Normally, we don't know these parameters directly, and this routine is used in iteration
        'by CoaxComplexZ0Iterate
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        twoPiF=2*uPi()*fMHz
        gammaReal=ac+ad 'total loss coefficient
        gammaImag=beta
        R=2*ac*Z0Real
        G=2*ad*Z0Real/(Z0Real^2+Z0Imag^2)
        L=(gammaReal*Z0Imag+gammaImag*Z0Real)/twoPiF    'L=Im(gamma*Z0)/twoPiF
        call cxDivide gammaReal, gammaImag,Z0Real, Z0Imag, numR, numI    'gamma/Z0
        C=1e6*numI/twoPiF   'C=1e6*Im(gamma/Z0)/twoPiF
    end sub

    sub CoaxComplexZ0Iterate fMHz, VF, K1, K2, byref Z0Real, byref Z0Imag
        'The complex Z0 is calculated by iteration, starting with Z0Real as the nominal
        'impedance, which must be set on entry.
        'The second iteration causes little change, and after that there is virtually none.
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        if K1=0 and K2=0 then exit sub  'No loss, so no need to do fancy calculations
        twoPiF=2*uPi()*fMHz
        ac=0.0011513*K1*sqr(fMHz) : ad=0.0011513*K2*fMHz    'components of loss coefficients, nepers/ft
        beta=twoPiF/(VF*983.6)  'speed of light is 983.6 million feet/sec
        Z0Imag=0
        if Z0Real<=0 then Z0Real=1  'To avoid divide by zero
            'First iteration
        call CoaxPropagationParams Z0Real, Z0Imag, fMHz, ac, ad, beta, R, G, L, C     'Calcs R,G,L,C
        call CoaxComplexZ0 fMHz, R, G, L, C, Z0Real, Z0Imag     'calcs Z0

            'Second iteration
        call CoaxPropagationParams Z0Real, Z0Imag, fMHz, ac, ad, beta, R, G, L, C     'Calcs R,G,L,C
        call CoaxComplexZ0 fMHz, R, G, L, C, Z0Real, Z0Imag     'calcs Z0

            'Third iteration--needed only for low freq, or high loss at mid frequency
            'The loss thresholds below are just a bit under RG-174
        needThird=(fMHz<1) or ((K1>0.6 or K2>0.03) and fMHz<10) 'ver115-4b
        if needThird then
            call CoaxPropagationParams Z0Real, Z0Imag, fMHz, ac, ad, beta, R, G, L, C     'Calcs R,G,L,C
            call CoaxComplexZ0 fMHz, R, G, L, C, Z0Real, Z0Imag     'calcs Z0
        end if
    end sub

    sub CoaxOpenZ Z0Real, Z0Imag, lenFeet, Greal,Gimag, byref ZReal, byref ZImag  'Calc Z of open coax stub
        'We calculate ZReal+jZimag=Zopen=Z0/tanh(gamma*lenFeet)
        'Z0Real+jZ0Imag is the coax characteristic impedance
        'Greal+jGimag is gamma, the propagation factor obtained from CoaxGetPropagationGamma
        if lenFeet=0 then ZReal=constMaxValue : ZImag=constMaxValue : exit sub  'zero length open has inf imped
        tR=Greal*lenFeet : tI=Gimag*lenFeet
        call cxTanh tR, tI, tanhR, tanhI
        call cxDivide Z0Real, Z0Imag, tanhR, tanhI, ZReal,ZImag  'Zopen=Z0/tanh(gamma*lenFeet)
    end sub

    sub CoaxShortZ Z0Real, Z0Imag, lenFeet, Greal,Gimag, byref ZReal, byref ZImag  'Calc Z of short coax stub
        'We calculate ZReal+jZimag=Zshort=Z0*tanh(gamma*lenFeet)
        'Z0Real+jZ0Imag is the coax characteristic impedance
        'Greal+jGimag is gamma, the propagation factor obtained from CoaxGetPropagationGamma
        if lenFeet=0 then ZReal=0 : ZImag=0 : exit sub  'zero length short has 0 imped
        tR=Greal*lenFeet : tI=Gimag*lenFeet
        call cxTanh tR, tI, tanhR, tanhI
        call cxMultiply Z0Real, Z0Imag, tanhR, tanhI, ZReal,ZImag  'Zshort=Z0*tanh(gamma*lenFeet)
    end sub

    sub CoaxTerminatedZ Z0Real, Z0Imag, ZtReal, ZtImag, lenFeet, Greal,Gimag, byref ZReal, byref ZImag  'Calc Z of terminated coax
        'Z0Real+jZ0Imag is the coax characteristic impedance
        'ZtReal+jZtImag is the terminating impedance
        'Greal+jGimag is gamma, the propagation factor obtained from CoaxGetPropagationGamma
        'We calculate ZReal+jZImag as the input impedance resulting from terminating the coax with Zt
        'lenFeet is the coax length. Note that if we use a negative length, we end up calculating Z as
        'the terminating impedance required to produce an input impedance of Zt.
        'Zin=Z0* [Zt*cosh(A)+Z0*sinh(A)] / [Zt*sinh(A)+Z0*cosh(A)]; A=lenFeet*gamma
        if lenFeet=0 then ZReal=ZtReal : ZImag=ZtImag : exit sub  'zero length has imped of Zt
        tR=Greal*lenFeet : tI=Gimag*lenFeet
        call cxSinh tR, tI, sR, sI
        call cxCosh tR, tI, cR, cI
        call cxMultiply ZtReal, ZtImag, sR, sI, stR, stI  'sinh*Zt
        call cxMultiply ZtReal, ZtImag, cR, cI, ctR, ctI  'cosh*Zt
        call cxMultiply Z0Real, Z0Imag, sR, sI, s0R, s0I  'sinh*Z0
        call cxMultiply Z0Real, Z0Imag, cR, cI, c0R, c0I  'cosh*Z0
        call cxDivide ctR+s0R, ctI+s0I, stR+c0R, stI+c0I, divR, divI
        call cxMultiply Z0Real, Z0Imag, divR, divI, ZReal, ZImag   'Z0 times the result of the divide
    end sub

    sub CoaxTerminatedZFromName coaxName%, fMHz, ZtReal, ZtImag, lenFeet, byref ZReal, byref ZImag 'Calculate Z0 and then terminated Z value
        call CoaxGetData coaxName%, R0, VF, K1, K2
        spec%=CoaxSpecs%(R0, VF, K1, K2,lenFeet)  'put data into a coax spec
        call CoaxTerminatedZFromSpecs spec%, fMHz, ZtReal, ZtImag, ZReal, ZImag
    end sub
*/
void coaxAnalysis::CoaxTerminatedZFromSpecs( QString coaxSpecs, double fMHz, double ZtReal, double ZtImag, double &ZReal, double &ZImag)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //Calculate Z0 and then terminated Z value
  float R0, VF, K1, K2, lenFeet;
  int isErr = CoaxParseSpecs(coaxSpecs, R0, VF, K1, K2, lenFeet);
  if (isErr)
  {
    ZReal=0;
    ZImag=0;
    return;
  }
  float Z0Real=R0;
  float Z0Imag=0;  //starting value
  float Greal,Gimag;
  if (fMHz<=0)
    fMHz=0.000001;   //To avoid numerical problems
  CoaxComplexZ0Iterate(fMHz, VF, K1, K2, Z0Real, Z0Imag);  //Calculate complex Z0
  CoaxGetPropagationGamma(fMHz, VF, K1, K2, Greal, Gimag); //get propagation coefficient alpha +j*beta
  CoaxTerminatedZ(Z0Real, Z0Imag, ZtReal, ZtImag, lenFeet, Greal,Gimag, ZReal, ZImag);
  */
}
/*
    sub CoaxZ0FromTerminationImpedance ZLReal, ZLImag, ZReal, ZImag, GLreal, GLimag, byref Z0Real, byref Z0Imag    'Calc Z0 from Z and ZL
        'ZL is the terminating impedance; Z is the measured input impedance of the line.
        'GLreal and GLimage are the gamma*len components
        'We calculate characteristic impedance Z0.
        'There may be a second solution for very lossy lines if ZL is very small or very large,
        'so it should be within a factor of 5 of the value of Z0, which the caller can check after getting
        'the calculation results. In any case, all measurements will be most accurate if ZL is close to
        'Z0, which will also make Z close to Z0.
        'Z0=(1/2)*[D+sqrt(D^2+4*Z*ZL)], where D=(Z-ZL)coth(gamma*len)

        call cxCosh GLreal, GLimag, cReal, cImag    'cosh of gamma*len
        call cxSinh GLreal, GLimag, sReal, sImag    'sinh of gamma*len
        call cxDivide cReal, cImag, sReal, sImag,cothR, cothI     'coth=cosh/sinh
        difR=ZReal-ZLReal : difI=ZImag-ZLImag   'Z-ZL
        call cxMultiply difR, difI, cothR, cothI, difCothR, difCothI '(Z-ZL)*coth
        call cxMultiply difCothR, difCothI,difCothR, difCothI, difCothSqR, difCothSqI  '((Z-ZL)*coth)^2
        call cxMultiply 4*ZReal, 4*ZImag, ZLReal, ZLImag, fourZZLreal,fourZZLimag    '4*Z*ZL
        call cxSqrt difCothSqR+fourZZLreal,difCothSqI+fourZZLimag, sqrtR, sqrtI   'sqrt(((Z-ZL)*coth)^2-4*Z*ZL)
        Z0Real=(difCothR+sqrtR)/2   'Add (Z-ZL)*coth and sqrt(((Z-ZL)*coth)^2-4*Z*ZL) and divide by 2.
        Z0Imag=(difCothI+sqrtI)/2
    end sub

    function CoaxPhaseDelayAndLossFromSpecs(coaxSpecs%, phase, lossDB)    'Get delay in degrees and loss in dB for specified coax
        'returns 1 if error in spec; otherwise 0
        'Assumes proper termination for loss calculation
        isErr=CoaxParseSpecs(coaxSpecs%, R0, VF, K1, K2, lenFeet)
        if isErr then CoaxDelayAndLossFromSpecs=1 : exit function  'invalid spec
        CoaxDelayAndLossFromSpecs=0
        phase=CoaxDelayDegrees(fMHz,VF,lenFeet)
        lossDB=CoaxLossA0*lenFeet/100  'loss per hundred feet times len in hundreds of feet
    end function

    sub CoaxS21 sysZ0real, sysZ0imag, coaxZ0real, coaxZ0imag, GLreal, GLimag, byRef S21real, byRef S21imag
        'sysZ0 (Z0) is system reference impedance
        'coaxZ0 (ZC) is coax characteristic impedance
        'GL is gamma times length (ft)
        'Formula:
        '   a=e^(gamma*Len) + e^(-gamma*Len)
        '   b=e^(gamma*Len) - e^(-gamma*Len)
        '   S21=2/{a + (b/2)*(Z0/ZC+ZC/Z0)}
        call cxEPower GLreal, GLimag, ePlusR, ePlusI   'e^(gamma*Len)
        call cxEPower 0-GLreal, 0-GLimag, eMinusR, eMinusI   'e^(-gamma*Len)
        aR=ePlusR+eMinusR : aI=ePlusI+eMinusI           'a=e^(gamma*Len) + e^(-gamma*Len)
        bR=(ePlusR-eMinusR)/2 : bI=(ePlusI-eMinusI)/2  'b=[e^(gamma*Len) - e^(-gamma*Len)]/2
        call cxDivide, sysZ0real,sysZ0imag, coaxZ0real, coaxZ0imag, ratioR, ratioI 'ratio=Z0/ZC
        call cxInvert ratioR, ratioI, invR, invI         'inv=ZC/Z0
        cR=ratioR+invR : cI=ratioI+invI         'c=Z0/ZC+ZC/Z0
        call cxMultiply cR, cI, bR, bI, dR, dI   'd=(1/2)[e^(gamma*Len) - e^(-gamma*Len)]*[Z0/ZC+ZC/Z0]
        eR=aR+dR :eI=aI+dI          'e=e^(gamma*Len) + e^(-gamma*Len) + (1/2)[e^(gamma*Len) - e^(-gamma*Len)]*[Z0/ZC+ZC/Z0]
        call cxInvert eR, eI, fR, fI     'f=1/e
        S21real=2*fR : S21imag=2*fI     'S21=2/e, real, imag format
    end sub
*/
void coaxAnalysis::CoaxS21FromSpecs(double sysZ0real, double sysZ0imag, QString coaxSpecs, double fMHz, double &S21dB, double &S21ang)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    sub CoaxS21FromSpecs sysZ0real, sysZ0imag, coaxSpecs%, fMHz, byref S21dB, byref S21ang  'Calc S21 of coax cable
        'sysZ0 is the system reference impedance
        'fMHz is frequency in MHz,
        'coaxSpecs% describes the coax
        'We calculate S21 of the coax cable with source and load of sysZ0. Returned as db, angle (degrees)
        if coaxSpecs%="" then S21dB=0 : S21ang=0 : exit sub 'No specs; treat as zero length coax
        isErr=CoaxParseSpecs(coaxSpecs%, R0, VF, K1, K2, lenFeet)
        if isErr then S21dB=-200 : S21ang=0 : exit sub  'invalid spec
        Z0Real=R0 : Z0Imag=0  'starting value
        if fMHz<=0 then fMHz=0.000001   'To avoid numerical problems
        call CoaxComplexZ0Iterate fMHz, VF, K1, K2, Z0Real, Z0Imag  'Calculate complex Z0
        call CoaxGetPropagationGamma fMHz, VF, K1, K2, Greal, Gimag 'get propagation coefficient alpha +j*beta
        call CoaxS21 sysZ0real, sysZ0imag,Z0Real, Z0Imag, Greal*lenFeet,Gimag*lenFeet, S21real, S21imag
            'Convert real, imag to db, angle
        magSquared=S21real*S21real + S21imag*S21imag
        S21dB=10*uSafeLog10(magSquared)   'use 10 instead of 20 because mag is already squared
        S21ang=uATan2(S21real, S21imag) 'Angle, degrees
    end sub
    */
}

QString coaxAnalysis::CoaxSpecs(float R0, float VF, float K1, float K2, float lenFeet)
{
  //Assemble coax parameters into a spec string
  return QString("Z%1,V%2,K%3,K%4,L%5").arg(R0).arg(VF).arg(K1).arg(K2).arg(lenFeet);
  //CoaxSpecs%="Z";R0;",V";VF;",K";K1;",K";K2;",L";lenFeet
}
int coaxAnalysis::CoaxParseSpecs(QString coaxSpecs, double &R0, double &VF, double &K1, double &K2, double &lenFeet)   //Get coax characteristics from specs
{
  //Returns 0 if no error; 1 if error.
  //coaxSpecs% is in the form Z50, V0.5, K0.05, K0.05, L3
  //Z is R0, V is velocity factor, first K is conductor loss factor (K1),
  //   second K is dielectric loss factor (K2), L is length in feet.
  int retVal = 0;   //Assume no error
  R0=50;
  VF=1;
  K1=0;
  K2=0;
  lenFeet=0;  //default is lossless coax of zero len
  if (coaxSpecs=="")
  {
    return 0;
  }
  int isErr=0;
  int kNum=1;
  QStringList list = coaxSpecs.split(",");
  QStringListIterator i(list);

  while (i.hasNext())
  {
    QString compon = i.next().trimmed();
    if (compon == "")
      break;
    QString firstChar = compon.left(1);   //data tag, single character
    QString data = compon.mid(1);   //From second character to end
    float v;
    if (data!="")
      v = util.uValWithMult(data);   //Value of everything after first char
    if (firstChar == "Z")    //Z0
    {
      R0=v;
      if (R0<=0)
        isErr=1;
    }
    else if (firstChar == "V")    //velocity factor
    {
      VF=v;
      if (VF<=0 || VF>1)
        isErr=1;
    }
    else if (firstChar == "K")    //K1 or K2
    {
      if (v<0 || v>1)
        isErr=1;
      if (kNum==1)
      {
        K1=v;
        kNum=2;
      }
      else
        K2=v;
    }
    else if (firstChar == "L")    //coax len
    {
      lenFeet=v;
    }
    else   //Invalid component spec
    {
      isErr=1;
    }

    if (isErr)
      return 1;
  }

  return retVal;
}
