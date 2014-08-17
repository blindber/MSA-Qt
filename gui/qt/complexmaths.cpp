#include "complexmaths.h"
#include <math.h>

ComplexMaths::ComplexMaths()
{
}
/*
'=====================Start Complex Functions Module====================

sub cxRectToPolarRad R,I, byref m, byref ang  'Convert rectangular coordinates to polar(rad)
    'R+jI to mag m at angle ang (radians, -pi to +pi)
   ang=uATan2(R, I)*0.0174532925199433
   m=sqr(R^2+I^2)   'mag
end sub

sub cxPolarRadToRect  m,ang, byref R, byref I   'Convert polar(rad) coordinates to rectangular
    'mag m at angle ang (radians) to R+jI
    if m<0 then m=0-m : ang=ang+uPi()   'Negative mag; point in opposite direction
    R=m*cos(ang)   'Trig is done in radians
    I=m*sin(ang)
end sub
*/
void ComplexMaths::cxInvert(double R, double I, double &Rres, double &Ires)     //Invert complex number R + jI; put into Rres, Ires
{
  //1/(R+jI)=(R-jI)/(R^2+I^2)
  double D=pow(R,2)+pow(I,2);
  if (D==0)
  {
    Rres=1e12;  //constMaxValue;
    Ires=0;
    return;
  }
  Rres=R/D;
  Ires=0-I/D;
}

void ComplexMaths::cxDivide(double Rnum, double Inum, double Rden, double Iden, double &Rres, double &Ires)     //Divide (Rnum + jInum)/(Rden + jIden); put into Rres, Ires
{
  if (Rnum==0 && Inum==0)    //0 numerator means zero result; we do this even if denominator=0
  {
    Rres=0;
    Ires=0;
    return;
  }
  //on error goto [MathErr]
  try
  {
    //First invert the denominator
    double D=pow(Rden,2)+pow(Iden,2);
    if (D==0)
    {
      Rres= 1e12; //constMaxValue;
      Ires=0;
      return;
    }
    double Rinv=Rden/D;
    double Iinv=0-Iden/D;
    //Now multiply Rnum+jInum times Rinv+jIinv
    Rres=Rnum*Rinv-Inum*Iinv;
    Ires=Rnum*Iinv+Inum*Rinv;
    return;
  }
  catch(...)
  {
    //[MathErr]
    //notice "Division Error"
    Rres=1e12; //constMaxValue;
    Ires=0;
  }
}
/*
'ver115-4e renamed cxMultiply
sub cxMultiply R1, I1, R2, I2, byref Rres, byref Ires     'Multiply (R1 + jI1)*(R2 + jI2); put into Rres, Ires
    on error goto [MathErr]  'ver115-4d
    Rres=R1*R2-I1*I2
    Ires=R1*I2+I1*R2
    exit sub
[MathErr]
    notice "Multiplication Error"
    Rres=constMaxValue : Ires=0
end sub

sub cxSqrt R,I, byref resR, byref resI 'Square root of R+jI is resR+jresI (non-neg real part)
    'Square root of X; branch cut on negative X-axis
    magR = abs(R):magI = abs(I)
    if magR=0 and magI=0 then resR=0 : resI=0 : exit sub  'square root of zero equals zero
    If (magR >= magI) Then
        t = magI / magR
        w = sqr(magR) * sqr(0.5 * (1 + sqr(1 + t * t)))
    Else
        t = magR / magI
        w = sqr(magI) * sqr(0.5 * (t + sqr(1 + t * t)))
    End If

    if w=0 then resR=0 : resI=0 : exit sub

    if (R >= 0) then
        resR=w
        resI=I / (w + w)
    else
        'Note that resR will always be non-negative, since w is non-negative
        resR = magI/ (w + w)
        if I >= 0 then
            resI = w
        else
            resI = 0-w
        end if
    end if
end sub

sub cxNatLog R,I, byref resR, byref resI 'Nat log of R+jI is resR+jresI
    call cxRectToPolarRad R,I, m, ang  'Convert to mag, radian format
        'Take the log of the magnitude and carry over the same angle (radians) as imaginary part
    resR=log(m)
    resI=ang
end sub

sub cxEPower R,I, byref resR, byref resI 'e to powere of R+jI is resR+jresI
    if R=0 and I=0 then resR=1 : resI=0 : exit sub  'e^0 is 1
    ex=exp(R)
    if I=0 then resR=ex : resI=0 : exit sub    'For real X, e^X is simple
    resR=ex*cos(I) : resI=ex*sin(I)      'e^x=e^R*(cos(I)+j*sin(I))
end sub

sub cxCos R,I, byref resR, byref resI 'Cosine of X=R+jI is resR+jresI
    '[e^(X*j)+ e^(-X*j)]/2
    call cxEPower 0-I,R,R1,I1    'e^(X*j)
    call cxEPower I,0-R,R2,I2    'e^(-X*j)
    resR=(R1+R2)/2
    resI=(I1+I2)/2
end sub

sub cxSin R,I, byref resR, byref resI 'Sine of X=R+jI is resR+jresI
    '[e^(X*j)- e^(-X*j)]/(2*j)  Note 1/(2*j)=-j/2
    call cxEPower 0-I,R,R1,I1    'e^(X*j)
    call cxEPower I,0-R,R2,I2    'e^(-X*j)
    resR=(I1-I2)/2      '(a+jb)/(2j)=-j(a+jb)/2=(b-ja)/2
    resI=(R2-R1)/2
end sub

sub cxTan R,I, byref resR, byref resI 'Tan of X=R+jI is resR+jresI
    'sin(X)/cos(X)
    if R=0 and I=0 then resR=0 : resI=0 :  exit sub 'tan(0) is 0
    call cxSin R,I,sR,sI      'sin(X)
    call cxCos R,I,cR,cI      'cos(X)
    call cxDivide sR,sI,cR,cI,resR,resI  'sin(X)/cos(X)
end sub

sub cxCot R,I, byref resR, byref resI 'Cotan of X=R+jI is resR+jresI
    'cot(X)=tan(pi/2-X)
    halfPi=uPi()/2
    call cxTan halfPi-R, 0-I, resR, resI  'Tan(pi/2-X)
end sub

sub cxASin R,I, byref resR, byref resI 'Arcsine of X=R+jI is resR+jresI
    'arcsin(x)=-j*log(jX+sqrt(1-X^2))
    call cxMultiply R,I,0-R,0-I,xsqR, xsqI    '-X^2
    xsqR=xsqR+1         '1-X^2
    call cxSqrt xsqR,xsqI, sqrtR, sqrtI           'sqrt(1-X^2)
    call cxNatLog sqrtR-I, sqrtI+R, logR, logI    'log[jX+sqrt(1-X^2)]
    resR=logI            '-j*log[jX+sqrt(1-X^2)]
    resI=0-logR
 end sub

sub cxACos R,I, byref resR, byref resI 'Arccos of X=R+jI is resR+jresI
    'arccos(x)=-j*log(X+sqrt(X^2-1))=pi/2 - arcsin(X)
    call cxASin R,I,aR,aI                    'Arcsin(X)
    halfPi=uPi()/2
    resR=halfPi-aR           'pi/2 - arcsin(X)
    resI=0-aI
end sub

sub cxCosh R,I, byref resR, byref resI 'Cosh of X=R+jI is resR+jresI
        'hyperbolic cosine of X
        ' Cosh(z) = (e^z + e^(-z))/2 = cos(j*z)
    call cxCos 0-I, R,resR, resI          'cos(j*X)
end sub

sub cxSinh R,I, byref resR, byref resI 'Sinh of X=R+jI is resR+jresI
        'hyperbolic sine of X
        ' Sinh(z) = (e^z - e^(-z))/2 = -j*sin(j*z)
    call cxSin 0-I, R, sR, sI            'sin(j*X)
    resR=sI   '-j*sin(j*x)
    resI=0-sR
end sub

sub cxTanh R,I, byref resR, byref resI 'Tanh of X=R+jI is resR+jresI
 'hyperbolic tangent of X
       ' Tanh(z) = (sinh(z)/cosh(z))
    call cxSinh R,I,sR,sI        'sinh(X)
    call cxCosh R,I,cR,cI        'cosh(X)
    call cxDivide sR, sI, cR, cI, resR, resI    'sinh(X)/cosh(X)
end sub

'=====================End Complex Functions Module====================
*/
