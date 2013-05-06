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
#include "msautilities.h"
#include <QMessageBox>
#include <QFileDialog>
#include <math.h>
//==========================UTILITIES MODULE===========================================

msaUtilities::msaUtilities()
{
  uKRadsPerDegree=0.0174532925199433;
  uKDegreesPerRad=57.2957795130823;
  uKPi=3.14159265358979;
  uKNatLog10=log(double(10));
  uKE=exp(double(1));
}

QString msaUtilities::uGetLine(QString inStr, int &startPos)
{
  ////Get line and increment startPos
  //startPos is the starting position of the data
  //We return data to but not including the next chr$(13) or chr$(10) or end of string, and increment
  //startPos to the first position after that chr$(13) or end of string.
  //QString s = inStr.mid(startPos, inStr.length());
  QString s = inStr;
  int lineStart=startPos;
  int lineEnd=s.indexOf("\r",startPos);   //Position of chr$(13)
  if (lineEnd==-1)
  {
    lineEnd=s.indexOf("\n",startPos);
  }
  if (lineEnd==-1)
  {
     lineEnd=s.length();
     startPos=lineEnd;
  }
  else
  {
     startPos=lineEnd+1;  //skip over the chr$(13) or chr$(10)
  }
  return s.mid(lineStart, lineEnd-lineStart);  //Return from startPos to but not including end position
}

QString msaUtilities::Word(QString strExp, int n)
{
  strExp = strExp.trimmed();
  //strExp = "12        13";
  QStringList list;
  list = strExp.split(QRegExp("\\s+"));

  if (n < 1 || n > list.count())
  {
    return "";
  }
  else
  {
    return list.at(n-1);
  }
}

QString msaUtilities::uExtractTextItem(QString &data, QString delim)
{
  QString retVal;
  //function uExtractTextItem$(byref data$,delim$)     //Remove and return first delimited item from data$
          //Locates first delimited item in data$, returns it, and removes the item and the
          //delimiter from data$. If delim$ is more than one character, the entire sequence is
          //considered to be the required delimiter. Leading/trailing blanks are not removed.
  int dataLen = data.length();
  //int pos=1;
  int delimLen;
  //QString delimChar = delim.left(1);
  delimLen=delim.length();

  if (dataLen<delimLen)
  {
    data="";
    //uExtractTextItem$="": exit function
    return "";
  }
  else
  {
    if (data.indexOf(delim) == -1)
    {
      retVal = data;
      data = "";
      return retVal;
    }
    else
    {
      retVal = data.left(data.indexOf(delim));
      data = data.right(data.length() - data.indexOf(delim) - delimLen);
      return retVal;
    }
    /*
    for i=1 to dataLen  //will continue until a delimiter is found
          if delimChar$=Mid$(data$,pos,1) then exit for   //ver115-1a
          pos=pos+1
      next i
          //pos is now the position of the first instance of delimChar$, or dataLen+1 if none found
      if pos>dataLen then  uExtractTextItem$=data$ : data$="": exit function
          //We now need to see if the full delimiter string is present
      delimFound=1
      if delimLen>1 then
          if Mid$(data$,pos, delimLen)<>delim$ then delimFound=0  //full delimiter must be present
      end if

      item$=Left$(data$,pos-1)   //Our desired item is everything preceding the delimiter
       if delimFound=1 then
          data$=Mid$(data$, pos+delimLen)      //Remaining data is everything following the delimiter
          uExtractTextItem$=item$
          exit function
      end if

          //We get here if we found the first delimiter character but not the whole thing.
          //We use recursion to keep searching
      data$=Mid$(data$, pos+1)     //chop off what we already have in item$, plus first delimiter character modver115-1a
      uExtractTextItem$=item$ + delimChar$+ uExtractTextItem$(data$, delim$)
      */
  }
}

int msaUtilities::uExtractNumericItems(int nItems, QString &data, QString delims, float &val1, float &val2, float &val3)
{
  //function uExtractNumericItems(nItems, byref data$, delims$, byRef val1, byref val2, byref val3)
  //Removes first nItems (1-3) delimited items per uExtractDataItem$
  //Returns their values in item1$, item2$. Missing items are returned as 0
  //Returns 1 if any item is blank or non-numeric

  val1=0; val2=0; val3=0;
  if (nItems<1 || nItems>3)
  {
    return 1; // exit function
  }

  int retVal=0;  //No error yet
  QString item1 = uExtractDataItem(data, delims);
  if (item1=="" ||  uIsNumeric(item1)==0)
  {
    return 1;
  }
  val1=item1.toFloat();
  if (nItems==1)
  {
    return retVal;
  }
  QString item2 = uExtractDataItem(data, delims);
  if (item2=="" || uIsNumeric(item2)==0)
  {
    return 1;
  }
  val2=item2.toFloat();
  if (nItems==2)
  {
    return retVal;
  }
  QString item3=uExtractDataItem(data, delims);
  if (item3=="" || uIsNumeric(item3)==0)
  {
    retVal=1;
  }
  val3=item3.toFloat();
  return retVal;

}

QString msaUtilities::uExtractDataItem(QString &data, QString delims)
{
  //Remove and return first delimited item
  //Locates first item in data$, returns it, and removes from data$ the
  //item and the first delimiter character.
  //Data is separated by any character in delims$. data$ may begin with spaces.
  //two delimiters in a row indicate a blank data item, except when the multiple delimiters are spaces.
  //ver116-2a changed this so multiple delimiters in a row were not all deleted.
  data=data.trimmed(); //QString item="";
  int dataLen=data.length(); int pos=0;
  for (int i=0; i < dataLen; i++)  //will continue until a delimiter is found
  {
    QString thisChar = data.mid(pos,1);   //next character of data$
    if (delims.indexOf(thisChar) >= 0 ) break;
    pos=pos+1;
  }
    //pos is now the position of the first delimiter found, or dataLen+1 if none found

  QString retVal = data.left(pos);   //Our desired item is everything preceding the delimiter
  if (pos>=dataLen) { data=""; return retVal;}   //Nothing except maybe delimiter left for further processing

  //pos is now the position of the next data item, or possibly one past the end
  data=data.mid(pos+1);    //deletes everything to and including the first delimiter item    //ver116-2a
  return retVal;
}

int msaUtilities::uIsNumeric(QString data)
{
  //Returns 1 if data seems valid numeric
  //We don't check precisely. We check to be sure there is at least one digit, and
  //no non-digits other than "-", "+", "." "E" or "e"
  //An empty string will return 0
  int dataLen=data.length(); int foundDig=0;
  for (int i=0; i < dataLen; i++)
  {
    QString thisChar = data.mid(i,1);
    if (QString("0123456789").indexOf(thisChar) != -1)
    {
      foundDig=1;
    }
    else
    {
      if (QString("+-.eE").indexOf(thisChar)==-1) return 0;   //Bad character
    }
  }
  return foundDig;     //True if we found a digit, otherwise false
}

QString msaUtilities::ToHex(int value)
{
  QString retVal;
  if (value <= 15)
    retVal = "0" + QString::number(value, 16);
  else
  {
    retVal = QString::number(value & 0xff, 16);
  }
  return retVal;
}

void msaUtilities::beep()
{
  QApplication::beep();
}

QString msaUtilities::time(QString t)
{
  QTime tim;
  QString retStr = "";

  int itime;
  tim = QTime::currentTime();
  if (t == "")
  {
    return tim.toString("hh:mm:ss");
  }
  else if (t == "ms")
  {
    itime = QTime(0,0).msecsTo(tim);
    return QString::number(itime);
  }
  return retStr;
}

QString msaUtilities::usingF(QString form, float v)
{
  // fix me, this may need more work to be more like the BASIC version
  QString str;

  int decPos = form.indexOf(".");
  int decCount = form.length() - decPos - 1;

  str = QString::number(v,'f', decCount);
  str = QString("%1").arg(str,form.length(),' ');

  return str;
}

QString msaUtilities::readLine(QFile *fHndl, char delm)
{
  QString retVal = "";
  while(!fHndl->atEnd())
  {
    char c;
    if (fHndl->getChar(&c))
    {
      if (c == delm)
        break;
      retVal = retVal + c;
    }
  }
  retVal = retVal.remove("\n");
  return retVal;
}

QString msaUtilities::Space(int N)
{
  QString retVal;
  return retVal.fill(' ', N);
}
float msaUtilities::uSinh(float x)
{ //sinh of real number
    return 0.5*(exp(x)-exp(0-x));
}

float msaUtilities::uCosh(float x)       //sinh of real number
{
  return 0.5*(exp(x)+exp(0-x));
}

float msaUtilities::uTanh(float x)       //sinh of real number
{
  return uSinh(x)/uCosh(x);  //Note denominator is never zero
}

float msaUtilities::uValWithMult(QString data)   //Return value of number which end with a multipler (e.g. K, M,...)
{
  data=uCompact(data);    //Remove all blanks
  int dataLen = data.length();
  QString multChar="";
  for (int i=1; i<= dataLen; i++)  //Find first char that can//t be part of the number
  {
      QString thisChar=data.mid(i,1);
      if ((QString("0123456789+-.eE").indexOf(thisChar) == -1))
          multChar=thisChar;
          break;
  }
      //Note val will calculate value up to any non-numeric data
  float baseVal=data.toFloat();
  float multVal=uMultiplierValue(multChar);
  return baseVal*multVal;  //base val times multiplier factor
}

QString msaUtilities::uRepeat(QString s, int n)
{
  //Return s repeated n times
  QString r="";
  while (n>0) { r=r+s; n=n-1; }
  return r;
}


QString msaUtilities::uGetParamText(QString s,QString key, QString delim) //Return parameter text following the text in key$, to but not including the delim$ char
{
  //delim$ can be a multi-character delimiter
  //If no delimiter is encountered, we return the text to the end of the line. If key$ is not found, we return blank
  int keyPos=s.indexOf(key);
  if (keyPos==-1) return "";
  int delimPos=s.indexOf(delim,keyPos);
  if (delimPos==-1) delimPos=s.length()  +1;    //delim position or one past string end
  int paramPos=keyPos + key.length();   //parameter starts immediately after end of key$
  return s.mid(paramPos, delimPos-paramPos); //after key$ and befor delim$
}

QString msaUtilities::uFormatByDig(float v, int maxWhole, int maxDec, int maxSig)
{
  QString retVal = "";

  //Return a formatted string for v
  //maxWhole is the max number of whole digits allowed; maxDec is the max number of
  //decimal digits. Both are limited to a max of 20, for sanity. Overriding maxDec is
  //maxSig, the max number of total significant digits. maxSig only affects the number of
  //decimal places; it does not force any whole digits to zero.
  //If v is too big, we return scientific notation
  QString signv="";
  if (v<0) {signv="-"; v=0-v;}
  if (v< pow(0.00001,4)) return "0";     //zero and tiny values are "0"
  if (maxDec<9 && v<0.000000005)  return "0";   //avoids LB bug
  if (maxWhole>20) { maxWhole=20; if (maxWhole<0) maxWhole=0;}
  if (maxDec>20) { maxDec=20; if (maxDec<0) maxDec=0;}
  if (maxSig<1) maxSig=1;

  //First find the number of whole digits; for v<1 this will be zero or negative
  //and will be the number of zeroes between the decimal point and the first significant digit
  int nWhole=(int)(uSafeLog10(v));
  if (v>1) nWhole=nWhole+1;
    //It is possible to be off by one due to rounding, so check it
  float shiftv= v * pow(0.1,nWhole); //This should make value: 0.1<=value<1
  if (shiftv>=1) { nWhole=nWhole+1; shiftv=shiftv/10;}
  if (shiftv<0.1) { nWhole=nWhole-1; shiftv=shiftv*10;}
    //We have now established nWhole
  if (v>=1)
  {
        //max decimal places is reduced if whole part uses up allowed sig dig
    maxDec=qMin(maxDec,qMax(0,maxSig-nWhole));
  }
  else
  {
    //for v<0, leading zeroes won//t count as sig dig. We know how many there are,
    //so we can figure out the extent to which maxSig limits maxDec
    int sigDigForMaxDec=maxDec+nWhole;   //We will have this many sig dig if we use maxDec decimal places
    if (sigDigForMaxDec>maxSig) maxDec=maxDec-(sigDigForMaxDec>maxSig);
  }

  if (nWhole>maxWhole)
  {
    //Value is too big; use scientific notation with only a few significant digits
    QString f=signv+uFormatByDig(shiftv*10, 2,3,4);
    retVal = f + "e" + QString::number(nWhole-1);
    return retVal;
  }

  //We now know how many whole and decimal places to use, except for one issue, arising from
  //the fact that formatting involves rounding, which can create an extra digit. The total
  //number of significant digits we plan to have is nWhole+maxDec. For numbers in the form
  //99999d * 10^N (N any integer), where the 9//s fill nWhole+maxDec positions and the "d" is
  //therefore the digit where we add the "5" to round, rounding will create an extra digit which
  //messes up our previous calculations. For such numbers, shiftv is of the
  //form 0.99999d. So if we round this off and get 1, we know we have a problem number, but we
  //also know it rounds to an exact power of 10, so we can construct its string representation.
  float roundFactor=0.5*pow(0.1,(nWhole+maxDec));
  if (roundFactor>=1)
  {
    //This occurs only if the rounding factor would be added to a fractional v which doesn//t
    //have any digits until after the place where we would add the "5", and so we
    //know it will produce zero even after rounding
    retVal = "0"; return retVal;
  }
  float roundedShiftv=shiftv+roundFactor;
  if (roundedShiftv>=1)    //SEWcal2 fixed this and the comment above regarding 0.99999d
  {
    //The value we need to represent is +/- 10^nWhole.
        //If nWhole>=0 we want a 1 followed by nWhole zeros
    if (nWhole>=0) {retVal=signv+"1"+uRepeat("0",nWhole); return retVal;}
        //If nWhole<0 we want a zero and decimal, then -1-nWhole zeroes, then the digit 1
    retVal = signv+"0."+uRepeat("0",-1-nWhole)+"1";
    return retVal;
  }
    //Create format string. Add 1 to nWhole in case of some overflow problem we didn//t cover
  QString formatStr=uRepeat("#",qMax(0,nWhole)+1)+"."+uRepeat("#",maxDec);
  QString f=(usingF(formatStr,v)).trimmed();

    //now we have to affix the sign and possibly a leading 0
  int leadingZero = 0;
  if (v<0 && maxWhole>0 && leadingZero==0) f="0"+f;    //If whole dig allowed, use "0.xxxx"
  f=signv+f;  //affix sign
    //now we have to delete trailing zeroes from the fractional part if required
  int decPos=f.indexOf(".");
  int lenF=f.length();
  if (decPos>=0)    //Do only if we have a decimal point
  {
    for (int i=1; i < 100; i++)  //Do until we are done
    {
        QString endChar=f.right(1);
        if (endChar==".") { retVal = f.left(lenF-1); return retVal;}  //Delete decimal and end
        if (endChar!="0") { retVal= f; return retVal;}    //non-zero found; end
        f= f.left(lenF-1); lenF=lenF-1;   //Delete zero and continue
    }
  }
  retVal = f;
  return retVal;
}
QString msaUtilities::uFormatted(double v, QString form)
{

//Return formatted string for v, based on form$
    //form$ is a formatting string, which can be the usual ##.## style string
    //for the using() function, or can be a series of three numbers separated
    //by spaces or commas. The first is the max number of whole digits allowed, the second is the max number
    //of decimal digits. The third is the max number of significant digits, which is used
    //only to restrict the actual number of decimal places.
    //Following the format specifier there may be additional info, in which case each
    //item of info is separated from that before it by "//". Allowed info:
    //Prefix=xxx   Text to be prepended to formatted value
    //Suffix=yyy   Text to be appended to formatted value
    //UseMultiplier    This tag is present to use P,T,G,M,k,m,u,n,p or f to scale the number
    //SuppressMilli    Used with UseMultiplier to suppress the use of "m"; instead do 0.xxx
    //Scale=ddd  value is multiplied by this scale value before formatting
    //TrimUsing      This tag signals to trim leading blanks and trailing decimal zeroes left by "using" function
    //DoCompact  Causes final value (before suffix or prefix is added to have any inner blanks removed by uCompact$()

  int doUsing=1;   //default; may change
  QString upperForm = form.toUpper(); //Upper case is used to search for tags
        //See if we need to scale the number
  int pos = upperForm.indexOf("SCALE=");
  if (pos>-1)
  {
    QString f2 = form.mid(pos+6);   //Everything right of equal sign
    QString scale = uExtractTextItem(f2,"//");
    int scaleFactor = scale.toInt(); if (scaleFactor==0) scaleFactor=1;
    v=v*scaleFactor;
  }
  QString mult = "";
  double origV=v;     //save before scaling by multiplier ver115-3b
      //See if we need to use a multiplier character

  int useMult = upperForm.indexOf("USEMULTIPLIER");    //check for UseMultiplier    ver115-2d
  if (useMult == -1) useMult = 0;
  if (useMult)     //  ver115-2d
  {
    uScaleWithMultiplier(v, mult);    //Returns multiplier and scales v
    mult=" "+mult;   //prepend space to multiplier character, even if there is none
    if ((upperForm.indexOf("SUPPRESSMILLI")>-1) && mult==" m")    //ver115-4e
    {
          //If SuppressMilli, we don't want to use the "m" prefix, but instead print in the
          //format 0.xxx, with however many decimal places are allowed
        v=v/1000 ; mult=" ";    //all mult$ have a leading blank.
    }
  }
  QString f;
  float maxWhole, maxDec, maxSig;
  if (form=="")
  {
      f="";    //No format provide so we will use str$()
  }
  else
  {
    if (form.left(1)=="#")
    {
      //form$ is in format needed for "using()", so use it directly
      f=uExtractTextItem(form, "//");  //get everything up to double slash.
      f=f.trimmed();
    }
    else
    {
    //Here the format must be a series of 3 numbers separated by spaces or commas.
      doUsing=0;   //we won//t use the using() function
      QString formText=uExtractTextItem(form, "//");
      int isErr=uExtractNumericItems(3, formText, " ,", maxWhole, maxDec, maxSig);
      if (isErr) {doUsing=1; f="";}  //Use str() if format is invalid
    }
    upperForm=form.toUpper(); //Used to search for tags
  }

    QString s;
    if (doUsing==1)
    {
        //format with the using() function or str$() function
        if (f=="")
          s=QString::number(v);
        else
          s=usingF(f, v);
        if (upperForm.indexOf("TRIMUSING")>-1)  //check for TrimUsing
        {
            //trim leading blanks and trailing decimal zeroes
            s=s.trimmed(); int sLen=s.length();
            int decPos=s.indexOf(".");
            if (decPos>-1)
            {
                //If we have a decimal we have to trim trailing zeroes, and maybe the decimal
                for (int j=sLen; j > 1;j--)  //iterate back from end of string
                {
                    QString thisChar=s.mid(j, 1);
                    if (thisChar!="0")    //keep going as long as we have zeroes
                    {
                        //j is now the position before the zeroes needing deletion
                        if (thisChar==".") j=j-1;  //Make j the position before the decimal
                        s=s.left(j);  //Keep everything through the jth position
                        break;    //ver115-2d
                    }
                }
            }
        }
    }
    else
    {
        s=uFormatByDig(v, maxWhole, maxDec, maxSig);
    }
    //The number itself is now formatted
    if (useMult) //ver115-2d
    {
        //It is possible that the multiplier was obtained with a number that starts with 9//s, and that
        //formatting rounded it to 10.... If that caused the scaled value to become 1000, then we need
        //to redo with a different multiplier.
        if (s=="1000")
        {
            double v1=2*origV;
            uScaleWithMultiplier(v1, mult);    //Get multiplier for slightly larger v //ver115-3b
            mult=" "+mult;   //prepend space to multiplier character, even if there is none
            //with the larger multiplier, we know the value should be 1 or possibly 1.0...
            v=1;
            if (doUsing && upperForm.indexOf("TRIMUSING")==-1)
                if (f=="") s=QString::number(v); else s=usingF(f, v);   //To get trailing zeroes
            else
                s="1";

        }
    }
    s=s+mult;     //Add the multiplier, if any  ver114-6f
    if (upperForm.indexOf("DOCOMPACT")>-1)   //ver114-6f
    {
      s=uCompact(s);    //Remove inner blanks (such as between value and multiplier)
    }
     //Now see if a suffix or prefix is specified
    QString pre=""; QString suf="";
    pos=upperForm.indexOf("PREFIX=");
    if (pos>-1)
    {
        QString f2=form.mid(pos+7);   //Everything right of equal sign
        pre=uExtractTextItem(f2,"//");
    }
    pos=upperForm.indexOf("SUFFIX=");
    if (pos>-1)
    {
        QString f2=form.mid(pos+7);   //Everything right of equal sign
        suf=uExtractTextItem(f2,"//");
    }
    return pre+s+suf;    //Put it all together ver114-6f
}

QString msaUtilities::uCompact(QString str)
{
  //function uCompact$(s$)    //Return s$ with all blanks deleted
      //This is useful before using the val() function on user supplied data,
      //because a space between a negative sign and the number causes val to produce zero.
  return str.replace(" ", "");
  /*
s2$="" : sLen=len(s$)
for i=1 to sLen
   thisChar$=Mid$(s$,i, 1)
   if thisChar$<>" " then s2$=s2$+thisChar$ //copy all but spaces
next
uCompact$=s2$
*/
}


void msaUtilities::uScaleWithMultiplier(double  &v, QString &mult)
{
  double absV;
  //sub uScaleWithMultiplier byref v, byref mult$     //Return appropriate multiplier and scale v
  //For example, this converts 2000 to 2 K.
  if (v==0) { mult=""; return;}

  if (v>=0) absV=v; else absV=0-v;

  if (absV>=1)     //SEWcal2 changed >1 to >=1 and made other changes  {
  {
    if (absV<1000) mult="";// : exit sub
    else if (absV<1000000) {mult="k"; v=v/1000;}// : exit sub    //ver115-2d
    else if (absV<1000000000) {mult="M"; v=v/1000000;}// : exit sub
    else if (absV<Q_UINT64_C(1000000000000)) {mult="G"; v=v/1000000000;}// : exit sub   //ver115-1e
    else if (absV< Q_UINT64_C(1000000000000000)) {mult="T"; v=v/Q_UINT64_C(1000000000000);}// : exit sub   //ver115-1e
    else { mult="P"; v=v/pow((double)100000,3);}

  }
  else
  {
    if (absV>=0.001) { mult="m"; v=v*1000;}// : exit sub
    else if (absV>=0.000001) { mult="u";  v=v*1000000;}// : exit sub
    else if (absV>=0.000000001) { mult="n"; v=(v*1000000)*1000;}// : exit sub
    else if (absV>=0.000000000001) {mult="p"; v=(v*1000000)*1000000;}// : exit sub
    else {mult="f"; v=v*pow((double)100000,3);}// : exit sub
  }
}


double msaUtilities::uMultiplierValue(QString mult)    //Return value of specified multiplier
{
  //For example, this converts "k" to 1000
  if (mult=="") return 1;
  if (mult=="P")
      return (double)100000*(double)1000000*(double)1000000;
  if (mult=="T")
      return (double)1000000*(double)1000000;
  if (mult=="G")
      return (double)100000*(double)1000000;
  if (mult=="M")
      return 1000000;
  if (mult=="K" || mult == "k")
      return 1000;
  if (mult=="m")
      return 0.001;
  if (mult=="u")
      return 0.000001;
  if (mult=="n")
      return 0.001*0.000001;
  if (mult=="p")
      return 0.000001*0.000001;
  if (mult=="f")
      return 0.00001*0.000001*0.000001;

  return 1;
}

QString msaUtilities::uPrompt(QString caption, QString msg, int doYesNo, int allowCancel)   //Post message and return user response
{
  //yes and no are allowed or alternatively OK
  //Cancel may be allowed with either
  //msg$ is the message to post. caption$ is the caption of the message box
  //Returns "yes", "no", "ok" or "cancel"
  //This code is modified from LB Workshop
  beep();
  QMessageBox msgBox;
  msgBox.setText(caption);
  msgBox.setInformativeText(msg);

  if (doYesNo == 0)
  {
    if (allowCancel)
      msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    else
      msgBox.setStandardButtons(QMessageBox::Ok);

  }
  else
  {
    if (allowCancel)
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    else
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  }

  msgBox.setDefaultButton(QMessageBox::Save);
  int ret = msgBox.exec();

  switch (ret)
  {
  case QMessageBox::Ok:
    return "ok";
    break;
  case QMessageBox::Yes:
    return "yes";
    break;
  case QMessageBox::No:
    return "no";
    break;
  case QMessageBox::Cancel:
    return "cancel";
    break;
  default:
    // should never be reached
    break;
  }
  return "WTF";
}
float msaUtilities::uRadsPerDegree()
{
  //Return radians per degree
  return uKRadsPerDegree;
}
float msaUtilities::uDegreesPerRad()
{
  //Return degrees per radan
  return uKDegreesPerRad;
}
float msaUtilities::uPi()
{
  //Return Pi
  return uKPi;
}
float msaUtilities::uNatLog10()
{
  //Return natural log of 10
  return uKNatLog10;
}
float msaUtilities::uE()
{
  //Return e, the base of nat log system
  return uKE;
}
float msaUtilities::uSafeLog10(float aVal)
{
  //Return base 10 log of aVal; special rule for small and non-positive arguments
  if (aVal<=pow(0.00001,4))    //Might be negative due to rounding; avoid crazy values
    return -20;
  else
    return log(aVal)/uKNatLog10;   //Nat log divided by nat log of 10
}
float msaUtilities::uLog10(float aVal)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Return base 10 log of aVal; special rule for non-positive arguments
    if aVal<=0 then
        uLog10=-20  'Avoids error ver116-2a
    else
        uLog10=log(aVal)/uKNatLog10   'Nat log divided by nat log of 10
    end if
    */
  return -1;
}
int msaUtilities::uRoundDown(float x)
{
  //Round to integer<= x. -2.1 rounds to -3
  if (x>=0)
    return (int)(x);
  else
  {
    if ((int)(x)==x) return x; else return (int)(x-1);
  }
}
int msaUtilities::uRoundUp(float x)
{
  //Round to integer>= x. -2.1 rounds to -2
  if (x>=0)
  {
    if ((int)x == x)
      return x ;
    else
      return (int)(x+1);
  }
  else
  {
    return (int)x;
  }
}
int msaUtilities::uRoundUpToMultiple(float X, int mult)
{
  //   RoundUpToMultiple--Round up to next higher multiple of mult if value
  //    is not already a multiple of mult. E.g. if mult=5 then
  //         3.2-->5   -3.2-->0
  int div = uRoundUp(X / mult);            //3.2-->1  -3.2-->0
  return div * mult;     //3.2-->5  -3.2-->0
}
int msaUtilities::uRoundDownToMultiple(float X, int mult)
{
  //   uRoundDownToMultiple--Round down to next lower multiple of mult if value
  //    is not already a multiple of mult. E.g. if mult=5 then
  //         3.2-->0   -3.2-->-5

  int div = uRoundDown(X / mult);   //3.2-->0  -3.2-->-1
  return div * mult;                //3.2-->0  -3.2-->-5

}
float msaUtilities::uRoundUpToPower(float X, float base)
{
//   uRoundUpToPower--Round up to next higher integral(+/-) power of base if value
//    is not already a power of base.  Negative numbers are rounded to an algebraically
//    larger number
//    E.g. if base is 10 then
//    50-->100  -50-->-10  0.5-->1   0.005-->0.01
//Round up to power of positive integral base mod ver116-4k

  if (X == 0) return 0;
  if (base <= 0) return X;
  if (X<0) return 0-uRoundDownToPower(0-X, base);
  float powf = log(X) / log(base); //power=log of X, using base of "base"
  int trunc = (int)(powf); //can reduce positive pow or increase negative pow
  if (trunc != powf && powf > 0) trunc = trunc + 1;
  //It is possible for trunc to be off by one due to rounding errors
  //in the log calculation. So we adjust if necessary
  float ceil = pow(base,trunc); //Should be >= X but <= X*base
  if (ceil < X)
      trunc = trunc + 1;
  else
      if (ceil / base >= X) trunc = trunc - 1;

  return pow(base, trunc);
}
float msaUtilities::uRoundDownToPower(float X, float base)
{
  //   uRoundDownToPower--Round down to next lower integral(+/-) power of of positive integral base if value
  //    is not already a power of base.  Negative numbers are rounded to an algebraically smaller number.
  //    E.g. if base is 10 then
  //    50-->10  -50-->-100  0.5-->0.1   0.005-->0.001
  //Round down to power of base    //modified by ver115-8b for negative numbers
  if (X == 0) return 0;
  if (base <= 0) return X;
  if (X < 0) return 0-uRoundUpToPower(0-X,base);
  float powf = log(X) / log(base); //power=log of X, using base of "base"
  int trunc = (int)(powf); //can reduce positive pow or increase negative pow
  if (trunc != powf && powf < 0) trunc = trunc-1;
  //It is possible for trunc to be off by one due to rounding errors
  //in the log calculation. So we adjust if necessary
  float flr = pow(base,trunc); //Should be <= X but >= X/base
  if (flr > X)
      trunc = trunc - 1;
  else
      if (flr * base <= X) trunc = trunc + 1;

  return pow(base,trunc);
}
float msaUtilities::uTenPower(float pow1)
{
  return pow(pow1, 10);
/*
function uTenPower(pow)  //Raise 10 to power pow; avoid hangup if pow is large integer
    //LB does not seem to use divide and conquer for large integral powers,
    //so we force use of floating point math
    if pow>=0 then absPow=pow else absPow=0-pow //ver114-6k
    if absPow<8 then uTenPower=10^pow :exit function    //this covers most cases ver114-7a
    if int(pow)<>pow then uTenPower=10^pow :exit function
    pow=pow-0.1     //SEW9
    res=10^pow*1.258925412 // the constant is 10^0.1 ver114-7a
    if pow>0 then uTenPower=int(res+0.5) else uTenPower=res //Pos integer power has integer result ver114-7a
end function
*/
}
float msaUtilities::uPower(float x, float fpow)
{

  //Raise x to power pow; avoid hangup if pow is large integer
  //LB does not seem to use divide and conquer for large integral powers,
  //so we force use of floating point math
  float absPow;
  if (fpow>=0) absPow=fpow; else absPow=0-fpow; //ver114-6k
  if (absPow<5) return pow(x,fpow); //this covers most cases
  if ((int)(fpow)!=fpow) return pow(x,fpow);
  return pow(x,(fpow-(float)0.1))*pow(x,(float)0.1);
}
float msaUtilities::uATan2(float r, float i)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'Return angle (degrees) of r+j*i whose tangent is i/r
     if r=0 then
        if i=0 then
            ang=0
        else
            if i>0 then ang=90 else ang=-90
        end if
    else
        ang = uKDegreesPerRad*atn(i/r)   'Gives angle betwee        if r < 0 then
            if i < 0 then ang = ang - 180 else ang = ang + 180
        end if
        'Put angle within bounds; rounding may have put it outside
        if ang>180 then
            ang=ang-360
        else
            if ang<=-180 then ang=ang+360
        end if
    end if
    uATan2=ang
    */
  return -1;
}
float msaUtilities::uNormalizeDegrees(float deg)
{
  //Put deg in range -180<deg<=180    'ver116-4n
  while (deg<=-180)  { deg=deg+360; }
  while (deg>180) { deg=deg-360; }
  return deg;
}
QString msaUtilities::uAlignDecimalInString(QString v, int lenInDigits, int nLeft)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Return string with decimal aligned at decPos
    'Aligns decimal in string for printing in columns. Decimal need not actually be present.
    'lenInDigits is the desired string length, if the string were filled with digits.
    'nLeft is the number of characters allowed left of the decimal, so decimal (if any) will be at nLeft+1.
    'String will be padded with spaces on both ends if necessary.
    'If v$ is too big, it is possible we return a string that is too long.
    'nWhole is the desired number of whole digits--i.e. before the decimal, if any.
    'This works with fonts like Courier New, where all digits, spaces, sign and "e" have the same width
    'values that end with a multiplier character are aligned at the right side of the field, not at the decimal

    endChar$=Right$(v$,1)
    if instr("0123456789.", endChar$)=0 then  'check for multiplier character
            'Ends with non-numeric, non-decimal, so must have multiplier character
            'We right-align these in the string.
        leftPadLen=lenInDigits-len(v$) : if leftPadLen<0 then leftPadLen=0
        uAlignDecimalInString$=space$(leftPadLen);v$
        exit function
    end if
    sep=instr(v$,".")
    if sep=0 then
        whole$=v$ : fract$="" : dec$=""
    else
        whole$=Left$(v$,sep-1) : fract$=Mid$(v$,sep+1) : dec$="."
    end if
    leftPadLen=nLeft-len(whole$) : if leftPadLen<0 then leftPadLen=0
    rightPadLen=lenInDigits-nLeft-len(fract$)-len(dec$) : if rightPadLen<0 then rightPadLen=0
    uAlignDecimalInString$=space$(leftPadLen);whole$;dec$;fract$;space$(rightPadLen)
    */
  return "fix me 7";
}
QString msaUtilities::uScientificNotation(float v, int nDec, int padZero)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Return string for v in scientific notation x.xxxEyy with nDec decimal places
    'if padZero=0 then we delete the trailing zeroes after the decimal point
    if v=0 then uScientificNotation$="0" : exit function
    if nDec<0 then nDec=0
    useForm$="##.";uRepeat$("#", nDec)
    absV=abs(v)
    vLog=uLog10(absV)
    if vLog>=0 then vLog=int(uSafeLog10(absV)) else vLog=int(uSafeLog10(absV)-1)
    scale=uTenPower(vLog)  'e.g. 12 has int of log=1 and is scaled by 10
    v$=using(useForm$,v/scale)
    v=val(v$)
    if v>=10 then v$=using(useForm$,1) : vLog=vLog+1    'Rounding caused an excess digit
    if v<=-10 then v$=using(useForm$,1) : vLog=vLog-1
    v$=Trim$(v$)
    if padZero=0 then
        sLen=len(v$)
        decPos=instr(v$,".")
        if decPos>0 then
            'If we have a decimal we have to trim trailing zeroes, and maybe the decimal
            for j=sLen to 1 step -1  'iterate back from end of string
                thisChar$=Mid$(v$, j, 1)
                if thisChar$<>"0" then   'keep going as long as we have zeroes
                    'j is now the position before the zeroes needing deletion
                    if thisChar$="." then j=j-1  'Make j the position before the decimal
                    v$=Left$(v$,j)  'Keep everything through the jth position
                    exit for
                end if
            next j
        end if
    end if
    if vLog<>0 then v$=v$;"E";vLog
    uScientificNotation$=v$
    */
  return "fix me 8";
}
void msaUtilities::uCrystalParameters(float Fs, float Fp, float PeakS21DB, float Fdb3A, float Fdb3B, float &Rm, float &Cm, float &Lm, float &Cp, float &Qu, float &QL)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
       'Calculate crystal parameters in Series or Shunt Jig. Caps in pF; Lm in mH
        'Can also use this for series RLC combinations; just provide a bogus Fp and ignore Cp
    'Fs=series resonance in MHz
    'Fp=parallel resonance in MHz
    'PeakS21DB=S21 db at Fs (a negative value in db)
    'Fdb3A, Fdb3B= -3db frequencies, in MHz, around Fs; (absolute -3dB frequencies if shunt jig)
    'S21JigR0 must be set to the R0 of the test jig
        'From these we calculate
    'For series jig
    '   Rm=Motional Resistance=(2*Ro) * (1-S21)/S21 = (2*Ro) * (1/S21 - 1) (since phase is zero, we just take negative of S21db)
    '   Reff=2*R0+Rm    (i.e. crystal Rm plus the series R0 on each side)
    'For shunt jig
    '   Rm=Motional Resistance=(Ro/2) * S21 / (1-S21) = (Ro/2)/(1/S21 - 1) (since phase is zero, we just take negative of S21db)
    '   Reff=R0/2+Rm    (i.e. crystal Rm plus the parallel R0 on each side)
    'Q=Fs/(Fdb3B-Fdb3A
    'Lm=Motional Inductance=Q*Reff/(2*pi*Fs), in uH
    'Cm=Motional Capacitance=1/(4*pi^2*Fs^2*Lm), in pF
    'Preact=Reactance at Fp=2*pi*Lm*Fp - 1/(2*pi*Cm*Fp/1000000), in ohms. This is net inductive reactance at Fp.
    'Cp=Shunt Capacitance=1/(2*pi*Fp*Preact)*1000000 in pF
    'To test: Use Fs=20.015627, Fp=20.07, PeakS21DB=-1.97,Fdb3a=20.016655, Fdb3B=20.018711
    'Results should be Rm=6.36, Cm=26.04 fF, Lm=2427.9 uH, Cp=4.79
    if Fs<=0 or Fp<=0 or Fdb3A>=Fs or Fdb3B<=Fs then
        notice "Invalid frequency data for crystal calculation" : Rm=0 : Lm=0 : Cm=0 : Cp=0 : Q=0 : exit sub 'ver115-5c
    end if
    twoPi=2*uPi()   'ver115-4a
    R0=S21JigR0 : if R0<=0 then _
        notice "Invalid R0 crystal calculation" : Rm=0 : Lm=0 : Cm=0 : Cp=0 : Q=0 : exit sub 'ver115-5c
    if S21JigAttach$="Series" then
        Rm=2*R0*(10^(0-PeakS21DB/20)-1)    'internal crystal resistance at Fs, in ohms
        if Rm<0.001 then Rm=0.001
        Reff=2*R0+Rm    'effective load seen by crystal--external plus internal
    else    'shunt fixture
        Rm=(R0/2)/(10^(0-PeakS21DB/20)-1)    'internal crystal resistance at Fs, in ohms
        if Rm<0.001 then Rm=0.001
        Reff=R0/2+Rm    'effective load seen by crystal--external plus internal
    end if
    BW=Fdb3B-Fdb3A
    QL=Fs/(Fdb3B-Fdb3A)  'Loaded Q at Fs
    Lm=(QL*Reff/(twoPi*Fs)) 'in uH
    Cm=1000000/((twoPi^2)*(Fs^2)*Lm) 'in pF
    Preact=twoPi*Lm*Fp - 1/(twoPi*Cm*Fp/1000000) ' net reactance of motional inductance and capacitance at Fp, ohms
    Cp=1000000/(twoPi*Fp*Preact)  'in pF
    Qu=QL*Reff/Rm   'Unloaded Q is L reactance divided by series resistance.
    */
}
void msaUtilities::uParallelRLCFromScalarS21(float Fp, float PeakS21DB, float Fdb3A, float Fdb3B, float &parR, float &L, float &C, float &Qu, float &QL, float &serR)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calculate parallel RLC values in Series or Shunt Jig. Values are ohms, H and F
    'Fp=parallel resonance in MHz
    'PeakS21DB=S21 db at Fp (a negative value in db)
    'Fdb3A, Fdb3B= -3db frequencies, in MHz, around Fs; (absolute -3dB frequencies if series jig)
        'From these we calculate
    'The resistance is a combination of inductor resistance and the actual parallel resistance.
    'For parallel RLC, the resistance at resonance is the Q*React, where React is reactance of C or L.
    'QL=Fs/(Fdb3B-Fdb3A
    'For series jig
    '   parR=(2*Ro) * (1-S21)/S21 = (2*Ro) * (1/S21 - 1) (since phase is zero, we just take negative of S21db)
    '   Reff=2*R0+Rm    (i.e. R of the RLC plus the series R0 on each side)
    'For shunt jig
    '   parR=(Ro/2) * S21 / (1-S21) = (Ro/2)/(1/S21 - 1) (since phase is zero, we just take negative of S21db)
    '   Reff=R0/2+Rm    (i.e. R of the RLC plus the parallel R0 on each side)
    'netLoadR=parR in parallel with combined sourc and load resistances
    'resonReact=netLoadR/QL
    'L=resonReact/(2*pi*Fp) 'in uH
    'C=1/(2*pi*resonReact*Fp    'in uF
    'Qu=parR/resonReact     'Unloaded Q
    'serR=resonReact/Qu     'Series resistance that would provide the same Qu as parR produces

    R0=S21JigR0
    if S21JigAttach$="Series" then
       parR=2*R0*(10^(0-PeakS21DB/20)-1)    'This is the R of the RLC itself at resonance, not counting source and load
    else    'shunt fixture
       parR=(R0/2)/(10^(0-PeakS21DB/20)-1)
    end if
    if parR<0.001 then R=0.001
    BW=Fdb3B-Fdb3A : if BW<=0 then notice "Invalid bandwidth for parallel RLC calculation" : R=0 : L=0 : C=0 : Q=0 : exit sub
    QL=Fp/BW  'Q at Fp   'This is the loaded Q
    if S21JigAttach$="Series" then sourceLoadR=2*R0 else sourceLoadR=R0/2
    netLoadR=sourceLoadR*parR/(sourceLoadR+parR)  'parR (of the RLC circuit) and sourceLoadR are in parallel
    resonReact=netLoadR/QL     'reactance of L, and -reactance of C, at resonance
    Qu=parR/resonReact     'Unloaded Q is based on parR, much larger than netLoadR
    serR=resonReact/Qu      'The series resistance that would produce the same Qu
    twoPiF=2*uPi()*Fp   'ver115-4a
    L=(resonReact/twoPiF)/1000000    'in H
    C=1e-6/(twoPiF*resonReact) 'in F
    if L<1e-12 then L=0 'Force to 0 if less than 1 pH.
    if C<1e-15 then C=0 'Force to 0 if less than 1 fF.
    */
}
void msaUtilities::uRLCFromTwoImpedances(QString connectType, float F1, float R1, float X1, float F2, float R2, float X2, float &R, float &L, float &C)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Return values for series RLC given two impedances ver116-4h
    'Given two frequencies F1 and F2 (MHz) and the impedance (R+jX) of the RLC circuit at those frequencies,
    'calculate the R(ohms), L(H) and C(F) values. Frequencies near resonant, such as at resonance and at a frequency
    'with response about 3 dB different from resonance are ideal for best accuracy.
    'connectType$="S" for series RLC and "P" for parallel.
    'The key to deriving these equations is that we know at resonance the reactances of L and C are equal but opposite,
    'and the data at the second frequency gives us a second equation
    R=0: L=0 : C=0   'In case of error
    if F1<=0 or F2<=0 or F1=F2 or X1=X2 or R1<0 or R2<0 then notice "Invalid data for RLC calculation" : exit sub
    if R1<0.001 then R1=0.001   'for stability
    if R2<0.001 then R2=0.001   'for stability
    F1=Fo*1e6 : F2=F2*1e6 : F1Sq=F1^2 : F2Sq=F2^2 'convert to Hz and square
    twoPi=2*uPi()
    if connectType$="S" then
        C=(F2^2-F1Sq)/(twoPi*(F2*F1Sq*X2-F1*F2Sq*X2)) : if C>1 then C=1    'series C component, max 1F
            'Use R measured at lowest reactance for best accuracy. Calc L from C at that frequency too.
        if abs(X1)<=abs(X2) then R=R1 : FL=F1 : XL=X1 else R=R2 : FL=F2 : XL=X2
    else    'Parallel RLC
        C=(F2*X1-F1*X2)/(twoPi*X1*X2*(F1^2-F2^2)) : if C>1 then C=1    'Parallel C value, max 1F
        'Use R measured at lowest reactance for best accuracy. Calc L from C at that frequency too.
        if abs(X1)<=abs(X2) then R=R1+X1^2/R1 : FL=F1 : XL=X1 else R=R2+X1^2/R2 : FL=F2 : XL=X2
    end if
    if C<=0 then notice "Invalid data for RLC calculation" : C=0 : exit sub
    L=(twoPi*FL*XL*C+1)/(((twoPi*FL)^2)*C) : if L>1 then L=1   'series L component, max 1H. Calc from FL and XL from above
    if L<1e-12 then L=0 'Force to 0 if less than 1 pH.
    if C<1e-15 then C=0 'Force to 0 if less than 1 fF.
*/
}
void msaUtilities::uShuntJigImpedance(float R0, float S21DB, float S21Deg, float delay, float freq, float *Res, float &React)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc impedance from S21; delay is ns; freq is Hz
'If a source and load of impedance Ro are attached to a grounded DUT, and S21 is measured,
'the impedance of the DUT is
'   Z(DUT)= (Ro/2) * S21 / (1-S21) = (Ro/2)/(1/S21 - 1) 'The second form works best for S21 originally in db
'Special case : if S21Mag=1 (or close to 1), treat the impedance as huge resistance
'if delay<>0, then we adjust for the connector delay (ns), which is assumed to occur in a 50-ohm line (not S21JigR0)
'If delay=0, we don't need to know freq here.
'Freq is in Hz.

    if S21Deg>90 then S21Deg=90   'possible only through noise/rounding ver115-1e
    if S21Deg<-90 then S21Deg=-90  'possible only through noise/rounding ver115-1e
    extremeVal=0
        '
    if S21DB>-0.005 then 'ver115-3e
        'For S21 mag near 1, the impedance is very large, and can be a small capacitor, or a large
        'resistor or inductor, or a mix. In a real fixture S21 mag can even be slightly greater than one,
        'and the angle seems to be a better indicator of what we have. For large reactive Z,
        'tan(S21Deg)=R0/(2*Z), so Z=R0/(2*tan(S21Deg))
        if abs(S21Deg)<0.25 then
            'Angle less than 0.25 degrees; assume resistance
            'Process resistance in normal way unless it is very small, but make angle zero
            if S21DB>-0.001 then Res=constMaxValue : React=constMaxValue : exit sub    'Near-zero angle; treat as zero resistance
            S21Deg=0
        else
            Res=0 : React=R0/(2*Tan(S21Deg*uRadsPerDegree())) : exit sub
        end if
    end if

    if S21DB<-100 then Res=0 : React=0 : extremeVal=1
    if extremeVal=0 then
        'To invert S21 while in db/angle form, negate db and angle
        lossMag=uTenPower(0-S21DB/20)    'loss Mag
        lossAngle=(0-S21Deg)*uRadsPerDegree()   'loss angle in Radians
        a=lossMag*cos(lossAngle) : b=lossMag*sin(lossAngle) 'a+jb is 1/S21
        call cxInvert a-1, b, invR, invI   'Inversion of denominator
        halfR0=R0/2
        Res=halfR0*invR  'half R0 times real part of inverted denominator
        React=halfR0*invI 'half R0 times imag part of inverted denominator
    end if
    'if delay<>0, then we adjust for the connector length of delay ns
    'the delay in radians is theta=2*pi*delay*freq/1e9, where delay is ns and freq is Hz
    if delay<>0 then    'ver115-1e
        'The impedance Res+j*React is the result of transformation by the transmission line, assumed to be 50 ohms (not R0).
        'We find the terminating impedance that produced that transformed impedance.
        'We use the same formula as is used to do the transformation, but with negative length (theta).
        theta=-0.000000360*delay*freq    'degrees; note it is negative to "undo" the delay
        call uPhaseShiftImpedance 50, theta, Res,React  'We assume a 50-ohm connector no matter what the jig R0. ver115-4a
    end if
    if Res<0.001 then Res=0 'avoids printing tiny values
    if React>-0.001 and React<0.001 then React=0 'avoids printing tiny values
    */
}
void msaUtilities::uAdjustS21ForConnectorDelay(float freq, float &S21DB, float &S21Ang)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'adjust specified S21 to what it would have been w/o connector delay
    'This is used to compensate for the connector delay in the Shunt fixture.
    'freq is in Hz
        'We first find the impedance presented by the open connector as a transmission line
    call uShuntImpedanceToS21DB S21JigR0,0, 1e9, S21JigShuntDelay, freq, openDB, openDeg    'ver116-4k
        'The true open reference should be higher than it is by -openDB and shifted -openDeg
        'This would increase our S21DB and S21Ang by that amount
    S21DB=S21DB-openDB : S21Ang=S21Ang-openDeg
        'Use this corrected S21 to find the DUT impedance
    call uShuntJigImpedance S21JigR0, S21DB, S21Ang, S21JigShuntDelay, _
                                    freq, Res, React
        'Find the S21 which that impedance would have generated with no delay
    call uShuntImpedanceToS21DB S21JigR0,Res, React, 0, freq, S21DB, S21Ang
    */
}
void msaUtilities::uSeriesJigImpedance(float R0, float S21DB, float S21Deg, float &Res, float &React)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'Calc impedance from S21
'If a source and load of impedance Ro are attached to a series DUT, and S21 is measured,
'the impedance of the DUT is
'   Z(DUT)= (2*Ro) * (1-S21)/S21 = (2*Ro) * (1/S21 - 1)
'The second formula is best when we start with S21 in db, because we can do the S21 inversion
'by taking the negative of the db value and adding 180 to the phase
'Special case : if S21Mag=0 (or close to 0), treat the impedance as huge resistance
    if S21DB<-80 then Res=constMaxValue : React=constMaxValue : exit sub   'Treat as huge resistance+reactance 'ver115-3e
    if S21Deg>90 then S21Deg=90   'possible only through noise/rounding ver115-1e
    if S21Deg<-90 then S21Deg=-90  'possible only through noise/rounding ver1
    if S21DB>-0.005 then 'ver115-3e
        'For S21 mag near 1, the impedance is very small, and can be a large capacitor, or a small
        'resistor or inductor, or a mix. In a real fixture S21 mag can even be slightly greater than one,
        'and the angle seems to be a better indicator of what we have. For small reactive Z,
        'tan(S21Deg)=-Z/(2*R0), so Z=-2*R0*tan(S21Deg)
        if abs(S21Deg)<0.25 then
            'Angle less than 0.25 degrees; assume resistance
            'Process resistance in normal way unless it is very small, but make angle zero
            if S21DB>-0.001 then Res=0 : React=0 : exit sub    'Near-zero angle; treat as zero resistance
            S21Deg=0
        else
            Res=0 : React=-2*R0*Tan(S21Deg*uRadsPerDegree()) : exit sub
        end if
    end if
    'To invert S21 while in db/angle form, negate db and angle
    lossMag=uTenPower(0-S21DB/20)    'loss Mag
    lossAngle=(0-S21Deg)*uRadsPerDegree()   'loss angle in Radians
    a=lossMag*cos(lossAngle) : b=lossMag*sin(lossAngle) 'a+jb is inverted S21
    doubleR0=2*R0
    Res=doubleR0*(a-1) : React=doubleR0*b
    if Res<0.001 then Res=0 'avoids printing tiny values
    if React>-0.001 and React<0.001 then React=0 'avoids printing tiny values
    */
}
void msaUtilities::uS11DBToImpedance(float R0, float S11DB, float S11Deg, float &Res, float &React)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc impedance from S11
    if S11DB>0 then S11DB=0 'Error condition; could happen from noise or rounding 'ver115-1e
    m=uTenPower(S11DB/20)    'S11 Mag
    call uRefcoToImpedance R0, m, S11Deg, Res, React    'ver115-1e
    */
}
void msaUtilities::uRefcoToImpedance(float R0, float rho, float theta, float &Res, float &React)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc impedance from refco: rho, theta (deg)
'   Z(DUT)= Ro*(1+G)/(1-G), where G is the reflection coefficient
'Special case : if a=1 and b=0 (or close), treat the impedance as huge resistance
    if rho<0 then rho=0    'rho<0 is error but could happen due to rounding
    if rho>1 then rho=1 'rho should never exceed 1 but might due to noise or rounding ver115-1e
    p=theta*uRadsPerDegree()   'S11 Radians
    a=rho*cos(p) : b=rho*sin(p)
        'a near +1 means rho is near 1 and theta is near zero. We treat these as large resistances
    if 0.999999<a then Res=constMaxValue : React=0 : exit sub   'ver115-4b

    'ver115-1h deleted additional test for a>0.99
        'Similarly, a near -1 means we have a very small resistance or reactance. It doesn't make much difference
        'which way we go.
    if -0.999999>a then Res=0 : React=0 : exit sub   'ver115-4b
    Rnum=a*R0+R0 : Inum=R0*b : Rden=1-a : Iden=0-b  'ver115-1e
    'Now do the divide, copying the procedure of cxDivide; faster than calling cxDivide
    'First invert the denominator
    D=Rden^2+Iden^2
    if D<0.0000000001 then Res=constMaxValue : React=0: exit sub    'a near 1 and b near 0 ver115-1e
    Rinv=Rden/D : Iinv=0-Iden/D
    'Now multiply Rnum+jInum times Rinv+jIinv
    Res=Rnum*Rinv-Inum*Iinv
    React=Rnum*Iinv+Inum*Rinv
    if Res<0.001 then Res=0 'avoids printing tiny values
    if React>-0.001 and React<0.001 then React=0 'avoids printing tiny values
    */
}
void msaUtilities::uImpedanceToRefco(float R0, float R, float I, float &rho, float &theta)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc reflection coefficient as mag/angle from impedance
    '   Refco= (Z-R0)/(Z+R0)
    if R<0 then rho=1 : theta=180 : exit sub  'Negative resistance is error; we return refco of -1, which is right for R=0 ver115-1e
    Rnum=R-R0 : Inum=I : Rden=R+R0 : Iden=I
    'Now do the divide, copying the procedure of cxDivide; faster than calling cxDivide
    'First invert the denominator
    D=Rden^2+Iden^2
    if D=0 then Rres=constMaxValue : Ires=0: exit sub
    Rinv=Rden/D : Iinv=0-Iden/D
    'Now multiply Rnum+jInum times Rinv+jIinv
    refR=Rnum*Rinv-Inum*Iinv
    refI=Rnum*Iinv+Inum*Rinv
        'Convert to polar form
    rho=sqr(refR^2+refI^2)
    theta=uATan2(refR, refI)
    */
}
void msaUtilities::uImpedanceToRefcoRI(float R0, float R, float I, float &GR, float &GI)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc reflection coefficient as real/imag from impedance
    '   Refco= (Z-R0)/(Z+R0)
    if R<0 then GR=-1 : GI=0 : exit sub  'Negative resistance is error; we return refco of -1, which is right for R=0 ver115-1e
    Rnum=R-R0 : Inum=I : Rden=R+R0 : Iden=I
    'Now do the divide, copying the procedure of cxDivide; faster than calling cxDivide
    'First invert the denominator
    D=Rden^2+Iden^2
    if D=0 then Rres=constMaxValue : Ires=0: exit sub
    Rinv=Rden/D : Iinv=0-Iden/D
    'Now multiply Rnum+jInum times Rinv+jIinv
    GR=Rnum*Rinv-Inum*Iinv
    GI=Rnum*Iinv+Inum*Rinv
    */
}
void msaUtilities::uSeriesImpedanceToS21DB(float R0, float R, float I, float &db, float &deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc S21 as db, degrees for series impedance when source,load=R0
    'S21(Series)=2*R0/(2*R0+Z)=1/(1+Z/(2*R0)) this is in complex number format
    if R<0 then R=0 'error, but could happen from noise/rounding ver115-1e
    doubleR0=2*R0
    call cxInvert 1+R/doubleR0, I/doubleR0, Rres, Ires
    deg=uATan2(Rres, Ires)   'phase in degrees
    db=10*uSafeLog10(Rres^2+Ires^2)       'magnitude in db; mult by 10 not 20, because magnitude is squared
    */
}
void msaUtilities::uShuntImpedanceToS21DB(float R0, float R, float I, float delay, float freq, float &db, float &deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc S21 as db, degrees for shunt impedance when source,load=R0
    'if delay<>0, then we adjust for the connector length of delay ns
    'If delay=0, we don't need to know freq here.
    'delay is in ns. freq is in Hz
    'S21(Shunt)=2*Z/(2*Z+R0) this is in complex number format
    if delay<>0 then    'ver115-1e
        'The impedance R+jI is transformed by the transmission line into a different
        'impedance. We find that impedance and then apply the formula to find S21(Shunt)
        theta=0.000000360*delay*freq    'degrees
        call uPhaseShiftImpedance 50, theta, R,I    'We assume a 50-ohm connector no matter what the jig R0. ver115-4a
    end if
    if R<0 then R=0 'error, but could happen from noise/rounding ver115-1e
    call cxDivide 2*R, 2*I, R0+2*R, 2*I, Rres, Ires
    deg=uATan2(Rres, Ires)   'phase in degrees
    db=10*uSafeLog10(Rres^2+Ires^2)       'magnitude in db; mult by 10 not 20, because magnitude is squared
    */
}
void msaUtilities::uRefcoToSeriesS21DB(float rho, float theta, float &db, float &deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc S21 in series jig for impedance with specified relect coeff
    'refco is reflection coefficient, mag angle; we calculate S21 for shunt fixture in db, angle format
    'Reference impedance for S21 is whatever it was for S11; it doesn't enter into the calculation
    'S21=2(1-S11)/(2+(1-S11)); then convert S21 to db
    if rho<0 then rho=0    'rho<0 is error but could happen due to rounding
    if rho>1 then rho=1 'rho should never exceed 1 but might due to noise or rounding ver115-1e
    p=theta*uRadsPerDegree()   'S11 Radians
    a=rho*cos(p) : b=rho*sin(p) 'S11 rectangular
    if a>0.99999999 then db=-199 : deg=0 : exit sub   'close to Open so no output ver116-4k
    oneMinusS11Real=1-a : oneMinusS11Imag=0-b
    call cxDivide 2*oneMinusS11Real, 2*oneMinusS11Imag, 2+oneMinusS11Real, oneMinusS11Imag, S21Real, S21Imag  '2(1-S11)/(2+(1-S11))
    deg=uATan2(S21Real, S21Imag)   'phase in degrees
    db=10*uSafeLog10(S21Real^2+S21Imag^2)       'magnitude in db; mult by 10 not 20, because magnitude is squared
    if db<-199 then db=-199
    */
}
void msaUtilities::uRefcoToShuntS21DB(float rho, float theta, float &db, float &deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc S21 in shunt jig for impedance with specified relect coeff ver116-4k
    'refco is reflection coefficient, mag angle; we calculate S21 for shunt fixture in db, angle format
    'delayNS is the connector delay in the shunt fixture; freqHZ is the frequency in MHz.
    'Reference impedance for S21 is whatever it was for S11; it doesn't enter into the calculation
    'S21=2(1+S11)/(2+(1+S11)); then convert S21 to db
    'This assumes no connector delay in the shunt fixture. To deal with that, rho and theta should be calculated
    'from the original impedance at the end of an appropriate transmission line, with uPhaseShiftImpedance.
    if rho<0 then rho=0    'rho<0 is error but could happen due to rounding
    if rho>1 then rho=1 'rho should never exceed 1 but might due to noise or rounding ver115-1e
     p=theta*uRadsPerDegree()   'S11 Radians
    a=rho*cos(p) : b=rho*sin(p) 'S11 rectangular
    if a<-0.999999999 then db=-199 : deg=0 : exit sub   'close to short so no output ver116-4k
    onePlusS11Real=1+a : onePlusS11Imag=b
    call cxDivide 2*onePlusS11Real, 2*onePlusS11Imag, 2+onePlusS11Real, onePlusS11Imag, S21Real, S21Imag  '2(1+S11)/(2+(1+S11))
    deg=uATan2(S21Real, S21Imag)   'phase in degrees
    db=10*uSafeLog10(S21Real^2+S21Imag^2)       'magnitude in db; mult by 10 not 20, because magnitude is squared
    if db<-199 then db=-199
    */
}
int  msaUtilities::uParseRLC(QString spec, QString &connect, float &R, float &L, float &C, float &QL, float &QC, float &D, QString &coaxSpecs)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Parse spec for parallel or series RLC; return 1 if error
    'spec$ describes a series or parallel combo of R, L and C, possibly at the end of a transmission
    'line described per CoaxParseSpecs or by the delay factor D. It is in this form:    'ver116-4i added delay D
    'RLC[S, R25, L10n, C200p,QL10,QC10,D2], Coax[xxx,xxx...]
    'First item is S for series or P for parallel, referring to the RLC combination
    'Remaining items are optional; one R,L,C QL, QC, and D are allowed; remaining items are data for coax cable.
    'Coax data is returned as a string, without the "Coax" or brackets, in coaxSpecs$
    'R, L, C and D are in ohms, Henries, Farads and seconds. Multiplier characters (k, u, n, etc.) are allowed.
    'QL and QC are the Q values for the L and C
    'D is the delay in seconds (one-way) prior to the terminating components. It can be used only if there are
    'no coax specs and is forced to 0 if there are.
    'We return the specified values, and return a function value of 0 for no error, 1 for error
    tagPos=instr(spec$, "RLC")  'find RLC tag
    if tagPos=0 then
        RLC$=""
    else
        openBracket=instr(spec$, "[", tagPos)   'find open bracket after RLC
        closeBracket=instr(spec$, "]", tagPos)  'find close bracket after RLC
        if closeBracket=0 or closeBracket=0 or openBracket>=closeBracket then uParseRLC=1 : exit function
        RLC$=Mid$(spec$, openBracket+1, closeBracket-openBracket-1) 'Get data in brackets
    end if

    tagPos=instr(spec$, "Coax")  'find Coax tag
    if tagPos=0 then
        coaxSpecs$=""
    else
        openBracket=instr(spec$, "[", tagPos)   'find open bracket after Coax
        closeBracket=instr(spec$, "]", tagPos)  'find close bracket after Coax
        if closeBracket=0 or closeBracket=0 or openBracket>closeBracket-1 then uParseRLC=1 : exit function
        coaxSpecs$=Mid$(spec$, openBracket+1, closeBracket-openBracket-1) 'Get data in brackets
    end if
     'ver116-4i changed the defaults for backward compatibility with old method to specify OSL standards
    connect$="P" : R=constMaxValue : L=constMaxValue : C=0 : D=0  'default is series RLC with high impedance and no delay
    QL=100000 : QC=100000 'ver116-4i
    commaPos=0
    if RLC$<>"" then
        specLen=len(RLC$)
        isErr=0
        while commaPos<specLen
            oldCommaPos=commaPos
            commaPos=instr(RLC$,",", commaPos+1)
            if commaPos=0 then commaPos=specLen+1     'Pretend comma follows the string
            compon$=Trim$(Mid$(RLC$,oldCommaPos+1, commaPos-oldCommaPos-1))   'get this component spec
            if compon$="" then exit while  'get next word; done if there is none
            firstChar$=Left$(compon$,1)   'data tag, single character
            if firstChar$="Q" then
                tag$=Left$(compon$,2)   'QL or QC is two characters
                data$=Mid$(compon$,3)   'From third character to end
                if data$<>"" then v=uValWithMult(data$)   'Value of everything after first char
            else
                tag$=firstChar$
                data$=Mid$(compon$,2)   'From second character to end
                if data$<>"" then v=uValWithMult(data$)   'Value of everything after first char
            end if
            select tag$   'Assign value to proper variable
                case "S"
                    connect$="S"
                    R=0 : L=0 : C=constMaxValue 'Defaults in case any components are not specified
                case "P"
                    connect$="P"
                    R=constMaxValue : L=constMaxValue : C=0 'Defaults in case any components are not specified
                case "R"
                    R=v : if R<0 then isErr=1
                case "L"
                    L=v
                case "C"
                    C=v
                case "QL"  'ver151-4b
                    QL=v : if QL<=0 then isErr=1
                case "QC"   'ver151-4b
                    QC=v : if QC<=0 then isErr=1
                case "D"   'ver16-4i
                    if coaxSpecs$="" then D=v   'Record D only if no coax specs
                case else   'Invalid component spec
                    isErr=1
            end select
            if connect$="" or isErr then uParseRLC=1 : exit function
        wend
    end if  'end RLC
    uParseRLC=0
    */
  return 1;
}
void msaUtilities::uComboImpedance(QString connect, float R, float L, float C, float QL, float QC, float freq, float &Zr, float &Zi)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc impedance Zr+j*Zi for RLC combo
    'connect$=P for parallel, S for series. R, L and C are connected in series or parallel.
    'For Series circuit, C>=constMaxvalue means treat C as a short--there is no C in the circuit
    'For Parallel circuit, L>=constMaxValue means treat L as open--there is no L in the circuit
    'Units are ohms, Henries, Farads, Hz. Same results are achieved with ohms, uh, uf and MHz.
    'QL is Q of inductor; QC is Q of capacitor (Q=reactance/resistance)
    twoPiF=2*uPi()*freq
    if QC<=0 then QC=0.000001   'avoids problems below
    if QL<=0 then QC=0.000001
    if freq<>0 and abs(L)<constMaxValue then 'If freq=0, we already have Ai=0; also, ignore huge parallel L
        ZLi=twoPiF*L :ZLr=ZLi/QL     'Inductor impedance, imag and real, with Q
    else
        ZLi=constMaxValue : ZLr=0
    end if
    if freq<>0 and C<>0 then    'Ignore C if freq or C is zero, because huge reactance in parallel is irrelevant
        ZCi=-1/(twoPiF*C) :ZCr=abs(ZCi)/QC     'Capacitor impedance, imag and real, with Q
    else    'zero freq or C; note that series connection ignores these
        ZCi=0-constMaxValue : ZCr=0
    end if

    if connect$="S" then
        Zr=R+ZLr
        Zi=ZLi
        if C=0 or freq=0 then
            Zi=0-constMaxValue
        else
            if abs(C)<constMaxValue then Zi=Zi+ZCi : Zr=Zr+ZCr
        end if
    else 'this section modver115-1a
        if R=0 or L=0 or abs(C)>=constMaxValue then  'parallel with a zero-ohm component
            Zr=0 : Zi=0
        else
            'Parallel Add admittances and then invert
            Ar=1/R
            if freq=0 or abs(L)>=constMaxValue then
                Ai=0        'Parallel inductance is huge so C will determine the reactance
            else
                call cxInvert ZLr, ZLi, YLr, YLi 'convert to admittance
                Ar=Ar+YLr : Ai=YLi      'Add to resistor admittance
            end if
            if C=0 then
                YCr=0 : YCi =0  'causes cap to be ignored
            else
                call cxInvert ZCr, ZCi, YCr, YCi 'convert to admittance
            end if
            Ar=Ar+YCr : Ai=Ai+YCi      'Add to resistor plus inductor admittance

            call cxInvert Ar, Ai, Zr, Zi     'Invert admittance to get impedance of combined circuit
        end if
    end if
    if Zi>constMaxValue then Zi=constMaxValue       'ver115-4h imposed limits
    if Zi<0-constMaxValue then Zi=0-constMaxValue
    if Zr>constMaxValue then Zr=constMaxValue
    if Zr<0 then Zr=0
    */
}
int msaUtilities::uRLCComboResponse(QString spec, float Z0, QString jig)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Calc S21 or S11 response of RLC combo; return 1 if error
    'spec$ describes the RLC combo, per uParseRLC
    'We use the frequencies in uWorkArray (MHz) and calculate S11 or S21 (db/degrees) for the specified RLC combo
    'We store the resulting db in uWorkArray(N, 1) and degrees in uWorkArray(N,2)
    'uWorkArray has uWorkNumPoints valid frequency points.
    'RLC combo connected per connect$ is tested at ref resistance R0 for S21 or S11
    'If jig$="S11" we do S11; if "S21Shunt" we do S21 for shunt connection; otherwise we do S21 for series connection.
    'We calculate the actual S21 or S11 response that would have been produced by the combination after a theoretical
    'perfect calibration is applied. Thus, we do not include the effect of S21JigShuntDelay, because that effect would be
    'removed by calibration.
    isErr=uParseRLC(spec$, connect$, R, L, C, QL,QC, D, coaxSpecs$)
    if isErr=0 then isErr=CoaxParseSpecs(coaxSpecs$, R0, VF, K1, K2, lenFeet) 'ver115-5d
    if isErr or Z0<=0 or R0<=0 then uRLCComboResponse=1 : exit function 'ver115-4a
    twoPi=2*uPi()
    'Note R0 is the impedance of any transmission line in the RLC combo; Z0 is the reference impedance for
    'calculating S11 or S21. Both are pure resistances.
    for i=1 to uWorkNumPoints       'uWorkNumPoints contains the number of frequency points in uWorkArray
        freq=uWorkArray(i,0)*1000000
        'if freq<0 then freq=0-freq  'Make frequencies positive delver115-1a
        call uComboImpedance connect$, R, L, C, QL,QC,freq, RLCZr, RLCZi  'ver115-4b
        if coaxSpecs$="" and D=0 then   'Simple case--no coax and no delay factor
            ZReal=RLCZr : ZImag=RLCZi
        else 'Get impedance of coax terminated by the RLC
            if jig$="S11" or jig$="S21Shunt" then
                if coaxSpecs$<>"" then  'Presence of coax overrides the delay factor
                    call CoaxTerminatedZFromSpecs coaxSpecs$, uWorkArray(i,0), RLCZr, RLCZi, ZReal, ZImag
                else    'Here apply delay factor D (sec) instead of a coax delay ver116-4i
                    call uImpedanceToRefco R0, RLCZr, RLCZi, rho, theta       'convert to refco
                    phaseDelay=D*360*freq         'phase delay (deg)=delay(sec)*degrees/cycle*cycles/sec
                    theta=theta-2*phaseDelay      'Delay reflection by twice phaseDelay, for round trip.
                    call uRefcoToImpedance R0, rho, theta, ZReal, ZImag
                end if
            else
                'Series S21. Any terminating impedance is deemed to be in series, but if the transmission
                'line specs are not blank the termination is ignored and the coax is used by itself
                if coaxSpecs$<>"" then call CoaxS21FromSpecs Z0, 0, coaxSpecs$, uWorkArray(i,0), db, theta 'ver115-4e
            end if
        end if
        if jig$="S11" then
            call uImpedanceToRefco Z0, ZReal, ZImag, rho, theta   'Impedance to reflection coefficient
            db=20*uSafeLog10(rho)   'rho to db
        else
            if jig$="S21Shunt" then
                call uShuntImpedanceToS21DB Z0, ZReal, ZImag, 0,freq, db, theta 'ver115-4h--Removed S21JigShuntDelay
            else
                'Series S21. If no coax, we use the RLC values in series. If coax,
                'we ignore "termination" and just use the S21 (db, theta) calculated above
                if coaxSpecs$="" then call uSeriesImpedanceToS21DB Z0, ZReal, ZImag, db, theta 'ver115-1e
            end if
        end if
        theta=theta mod 360
        if theta<=-180 then theta=theta+360  'Put angle in bounds
        if theta>180 then theta=theta-360
        uWorkArray(i, 1)=db : uWorkArray(i,2)=theta 'Store db, degrees in uWorkArray
    next i
    uRLCComboResponse=0  'no error
    */
  return 1;
}
void msaUtilities::uPhaseShiftImpedance(float R0, float theta, float &Zr, float &Zi)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc impedance of trans line terminated by Zr+j*Zi; replace Zr, Zi
    'Theta is in degrees, and can be positive or negative.
    'If an impedance Z is placed at the end of a transmission line of characteristic impedance R0, with a length
    'that causes (one-way) phase delay of theta degrees, then the impedance at the input of the transmission line is:
    ' newZ=R0*(Z + j*R0*tan(theta))/(R0 + j*Z*tan(theta))
    'This equation also works with negative theta, which effectively calculates the impedance that
    'would produce Zr+jZi after the transformation caused by the transmission line with length abs(theta).
    while theta>180 : theta=theta-360 : wend
    while theta<-180 : theta=theta+360 : wend
    TanTheta=0
    if abs(theta-90)<.00000001 then TanTheta=1e9    'near 90; use huge value for tangent
    if abs(theta+90)<.00000001 then TanTheta=-1e9   'near -90 use huge negative value for tangent
    if TanTheta=0 then TanTheta=tan(theta*uRadsPerDegree())
    Rnum=Zr : Inum=Zi+R0*TanTheta
    Rden=R0-Zi*TanTheta : Iden=Zr*TanTheta  'denominator.
    if abs(Rden)<0.000000001 and abs(Iden)<0.000000001 then
        Zr=1e9 : Zi=0   'This can only happen if TanTheta=-R0/Zi and Zr=0
    else
        call cxInvert Rden,Iden, Rinv, Iinv     'Invert denominator into Rinv+j*Iinv
        Zr=R0*(Rnum*Rinv-Inum*Iinv)  'Multiply R0 * numerator * inverted denominator
        Zi=R0*(Rnum*Iinv+Inum*Rinv)
    end if
    */
}


void msaUtilities::uHideCommandItem(int hParent, int itemID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //Delete the specified command item. It can later be re-inserted
    //hParent is the submenu containing the item. "Parent" may not be Windows terminology.
    //itemID is the ID of the item to hide by removing it
    calldll #user32, "RemoveMenu",_
        hParent as ulong,_ //handle of submenu
        itemID as ulong,_ //menu item ID
        _MF_BYCOMMAND as long,_ //says we are specifying the target item by its ID, not its position
        result as long //nonzero=success We return this, but it probably won//t be used. //ver116-4q boolean to long
    uHideCommandItem=result
*/
}

void msaUtilities::uHideSubMenu(int hParent, int position)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*    //Delete the specified submenu. It can later be re-inserted
    //hParent is the submenu containing the item. "Parent" may not be Windows terminology.
    //postion is the position of the item to hide by removing it
    calldll #user32, "RemoveMenu",_
        hParent as ulong,_ //handle of menu listing the one we want to hide
        position as ulong,_ //position of menu we want to hide
        _MF_BYPOSITION as long,_ //Says we are specifying the target menu by its position
        result as long //nonzero=success We return this, but it probably won//t be used. //ver116-4q boolean to long
    uHideSubMenu=result
*/
}
void msaUtilities::uShowMenuItem(int hParent, int itemID, int hMenu, QString caption, int precedeNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*    //Insert a menu item that was previously deleted
    //hParent is the submenu in which it will be inserted
    //itemID is the item ID of the item to insert, if it is a command item; otherwise -1
    //hMenu is the handle of the submenu to insert, if the item is a submenu; otherwise NULL
    //caption$ is the name of the item being inserted;
    //The item will be inserted prior to the item in position precedeNum (0...)--That is,
    //once it is inserted it will occupy position precedeNum.
    //the number of existing items.
    //An alternative subroutine could be written to locate the insertion
    //point by the ID of the item to be preceded, rather than by position.

        //The DLL uses the following structure to contain info about the item to be shown
    struct MENUITEMINFO,cbSize as ulong,fMask as ulong, _
        fType as ulong,fState as ulong,wID as ulong, _
        hSubMenu as ulong,hbmpChecked as ulong,hbmpUnchecked as ulong, _
        dwItemData as ulong,dwTypeData$ as ptr,cch as ulong   //Note dwTypeData$ is ptr because we have text caption
    MENUITEMINFO.cbSize.struct = len(MENUITEMINFO.struct)   //length of this structure
    MENUITEMINFO.fMask.struct = _MIIM_ID or _MIIM_SUBMENU or _MIIM_TYPE //Says to process ID, submenu handle and type
    MENUITEMINFO.wID.struct = itemID //ID of command item, or any value for submenu
    MENUITEMINFO.fType.struct = _MFT_STRING //type of caption is text
    MENUITEMINFO.hSubMenu.struct =hMenu //handle to submenu being inserted, or NULL if not a submenu
    MENUITEMINFO.dwTypeData$.struct = caption$ //caption for menu. This is represented in the struct as a pointer to the text
    MENUITEMINFO.cch.struct = len(caption$) //length of new caption
        //we don//t mess here with the struct members fState, hbmChecked, hbmpUnchecked or dwItemData.

        //Actual DLL call
    calldll #user32, "InsertMenuItemA",_
        hParent as ulong,_ //handle of menu into which we are inserting. "Parent" may not be Windows terminology.
        precedeNum as ulong,_ //pos of item that new item is to precede
        _MF_BYPOSITION as long,_ //Says to find locate the insertion point by position
        MENUITEMINFO as struct,_ //struct with item info
        result as long //nonzero=success We return this, but it probably won//t be used. //ver116-4q boolean to long
    uShowMenuItem=result
*/
}

void msaUtilities::uDrawMenu(int hWind  )
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Redraw menu after making modifications
    calldll #user32, "DrawMenuBar",_
        hWind as ulong,_ //window handle
        result as long //nonzero=success We don//t bother to return this //ver116-4q boolean to long
*/
}

void msaUtilities::uSubMenuHandle(int hParent, int subPosition)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Return handle of submenu in specified position
    //hParent is handle of the menu bar, or whatever menu contains the submenu
    //subPosition is position (0...) of the desired submenu
    calldll #user32, "GetSubMenu",_
        hParent as ulong,_ //menu handle
        subPosition as long,_ //0-indexed pos of submenu whose handle is sought
        hSub as ulong //returns submenu handle
    uSubMenuHandle=hSub
*/
}

void msaUtilities::uMenuItemID(int hSub, int itemPosition)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Return ID of item in submenu in menubar
    //This returns -1 if the specified menu item is not a command item, so
    //it is pointless to use it for submenus.
    calldll #user32, "GetMenuItemID", _
        hSub as ulong, _ //handle of the submenu
        itemPosition as long, _ //position of the menu item
        menuID as ulong //the handle (or ID) of the menu item
    uMenuItemID=menuID
*/
}

void msaUtilities::uGrayMenu(int hSub, int hItemID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     uGrayMenu=uChangeMenuState(hSub, hItemID, _MFS_GRAYED)
*/
}

void msaUtilities::uEnableMenu(int hSub, int hItemID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     uEnableMenu=uChangeMenuState(hSub, hItemID, _MFS_ENABLED)
*/
}

void msaUtilities::uCheckMenu(int hSub, int hItemID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     uGrayMenu=uChangeMenuState(hSub, hItemID, _MFS_CHECKED)
*/
}

void msaUtilities::uUncheckMenu(int hSub, int hItemID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     uGrayMenu=uChangeMenuState(hSub, hItemID, _MFS_UNCHECKED)
*/
}

void msaUtilities::uChangeMenuState(int hSub, int hItemID, int newStateFlags)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Flags can be any of these, and can be or//d together
    //_MFS_CHECKED Checks the menu item.
    //_MFS_DEFAULT Specifies that the menu item is the default, which is displayed in bold.
    //_MFS_DISABLED Disables the menu item so that it cannot be selected, but does not gray it.
    //_MFS_ENABLED Enables the menu item so that it can be selected. This is the default state.
    //_MFS_GRAYED Disables the menu item and grays it so that it cannot be selected.
    //_MFS_HILITE Highlights the menu item.
    //_MFS_UNCHECKED Unchecks the menu item.
    //_MFS_UNHILITE Remove highlight.

    struct MENUITEMINFO,cbSize as ulong,fMask as ulong, _
        fType as ulong,fState as ulong,wID as ulong, _
        hSubMenu as ulong,hbmpChecked as ulong,hbmpUnchecked as ulong, _
        dwItemData as ulong,dwTypeData as ulong,cch as ulong
    MENUITEMINFO.cbSize.struct = len(MENUITEMINFO.struct)
    MENUITEMINFO.fMask.struct = _MIIM_STATE //change state
    MENUITEMINFO.fState.struct = newStateFlags //flags to set state
    calldll #user32, "SetMenuItemInfoA",_
        hSub as ulong,_ //handle to submenu
        hItemID as ulong,_ //menu item
        0 as long,_ //0=ID,1=by position  //ver116-4q boolean to long
        MENUITEMINFO as struct,_ //struct
        result as long //nonzero = success //ver116-4q boolean to long

    uChangeMenuState=result
*/
}

void msaUtilities::uCheckMenuRadioItem(int hSubMenu, int posFirst, int posLast, int posCheck)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Set group of menu items as radio button group
    //In the submenu with handle hSubMenu, the items from posFirst to posLast (0...) will be grouped
    //as radio button items, and the item at position posCheck will be checked.
    uFlags = _MF_BYPOSITION
    Calldll #user32, "CheckMenuRadioItem",_
        hSubMenu as ulong,_ //handle of submenu containing items
        posFirst as ulong,_ //pos of first item in radiogroup
        posLast as ulong,_ //pos of last item in radiogroup
        posCheck as ulong,_ //pos of item to check
        uFlags as ulong,_ //MF_BYCOMMAND or MF_BYPOSITION--here we use by position
        result as long //nonzero=success  //ver116-4q boolean to long
*/
}

void msaUtilities::uSetWindowText(int hWin, QString newText)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Set window caption
    //Note for text window or graphics window, hWin must be the parent handle
    calldll #user32, "SetWindowTextA",_
        hWin as ulong,_ //handle of window or control
        newText$ as ptr,_ //new text string
        result as long //nonzero = success  //ver116-4q boolean to long
*/
}

void msaUtilities::uBringWindowToFront(int hWin)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Put specified window in front
    calldll #user32, "BringWindowToTop",_
        hWin as ulong,_ //handle of window or control
        res as long  //nonzero=success We don//t bother to return this  //ver116-4q boolean to long
*/
}

void msaUtilities::uGetWindowBounds(int hWin, int &topLeftX, int &topLeftY, int &botRightX, int &botRightY)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Get window bounds
    STRUCT Rect, topLeftX as long, topLeftY as long, botRightX as long, botRightY as long //To hold the returned data
    calldll #user32,"GetWindowRect", hWind as ulong, Rect as struct, r as long  //Fill Rect with size info
*/
}

void msaUtilities::uGetWindowWidthHeight(int hWindow, int &width, int &height)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Get height and width of window
    STRUCT Rect, topLeftX as long, topLeftY as long, botRightX as long, botRightY as long //To hold the returned data
    calldll #user32,"GetWindowRect", hWindow as ulong, Rect as struct, r as long  //Fill Rect with size info
    width=Rect.botRightX.struct-Rect.topLeftX.struct : height=Rect.botRightY.struct-Rect.topLeftY.struct
*/
}

void msaUtilities::uMoveWindow(int hWindow, int xOrg, int yOrg, int width, int height)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Move and possibly resize window
    //If width or height is <=0, we find the current width and height and keep it, so all we
    //do is move the window.
    if width<=0 or height<=0 then
        call uGetWindowWidthHeight hWindow, width, height    //get current width and height
    end if
    calldll #user32, "MoveWindow", _
        hWindow as ulong, _ //window or control handle
        xOrg as long, _ //new upper left x position
        yOrg as long, _ //new upper left y position
        width as long, _ //new width
        height as long, _ //new height
        1 as long, _ //1 = repaint window now  //ver116-4q boolean to long
        result as long //nonzero (true) if successful  //ver116-4q boolean to long
*/
}

void msaUtilities::uBestFitLines(int dataType1, int dataType2, int nPoints, int startStep, int endStep)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Creates an array of slopes m and intercepts b that represent the best fit line
    //at each step of ReflectArray, using the data dataType1 (e.g. constRho).
    //nPoints is the number of points used for the fit.
    //We will use the current point plus nLeft=int(nPoints/2) on the left and nPoints-nLeft-1 on the right,
    //except that at the start and end we will use the first or last nPoints when necessary.
    //We do the entire graph, starting at startPoint and ending at endPoint.
    //We put data into uWorkArray for each step, starting with uWorkArray(startStep,)
    //We put the frequency in uWorkArray(p,0) data of dataType1 in uWorkArray(p,1) and data of dataType2 in uWorkArray(p,2)
    //We put the result for step p in uWorkArray(p,3) (m) and uWorkArray(p,4)
    //sumXY will be the sum of x*y for the points (x,y)
    //sumX will be the sum of x; sum Y will be the sum of y
    //sumXSquared will be the sum of x^2
    call gGetMinMaxPointNum pMin, pMax  //Actual data available for points pMin to pMax (not steps)
    minStep=pMin-1 : maxStep=pMax-1
    call uSetMaxWorkPoints pMax, 2   //Make sure uWorkArray has room
    if nPoints<2 then nPoints=2
    if startStep<minStep then startStep=minStep
    if endStep>maxStep then endStep=maxStep
    if nPoints>endStep-startStep then nPoints=endStep-startStep+1
    nLeft=min(startStep-minStep, int(nPoints/2))
    //To start, we must calculate the sums for the first point
    sumX=0 : sumY=0 : sumXY=0 : sumXSquared=0
    saveY1DataType=Y1DataType :saveY2DataType=Y2DataType
    Y1DataType=dataType1 : Y2DataType=dataType2 //So CalcGraphData will return what we want as y1.

    currStep=startStep-nLeft
    if currStep<minStep then currStep=minStep
    for i=0 to nPoints-1
        call CalcGraphData i+currStep, y1, y2,0    //CalcGraphData goes by step numbers, not point numbers
        x=ReflectArray(i+currStep,0)
        sumX=sumX+x : sumY=sumY+y1
        sumXY=sumXY+x*y1 : sumXSquared=sumXSquared+x*x
    next i
    nLeft=int(nPoints/2)    //num of points on left and right to analyze
    nRight=nPoints-nLeft-1
    //Now we calculate m and b for the current step, then move to the next step, etc.
    for currStep=startStep to endStep
        m=(nPoints*sumXY-sumX*sumY)/(nPoints*sumXSquared-sumX*sumX)
        b=(sumY-m*sumX)/nPoints
        f=gGetPointXVal(currStep+1) //converts to point num
        call CalcGraphData currStep, thisY1, thisY2,0
        uWorkArray(currStep,0)=f : uWorkArray(currStep,1)=thisY1 : uWorkArray(currStep,2)=thisY2
        uWorkArray(currStep,3)=m : uWorkArray(currStep,4)=b
        if currStep-nLeft>=minStep and currStep+nRight<maxStep then
            //We now calculate the sums for the next step, by dropping the bottom point and adding
            //a new top point. We don//t do this if too close to beginning or end
            call CalcGraphData currStep-nLeft, bottomY, dum,0   //Y value to drop
            bottomX=ReflectArray(currStep-nLeft,0)  //Freq to drop
            call CalcGraphData currStep+nRight+1, topY, dum,0   //Y value to add
            topX=ReflectArray(currStep+nRight+1,0)  //Freq to add
            sumX=sumX+topX-bottomX : sumY=sumY+topY-bottomY
            sumXY=sumXY+topX*topY-bottomX*bottomY
            sumXSquared=sumXSquared+topX*topX-bottomX*bottomX
        end if
    next currStep
    Y1DataType=saveY1DataType : Y2DataType=saveY2DataType
*/
}

void msaUtilities::uBestFitLine(int dataType, int startStep, int endStep, int &m, int &b)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Calculates slope m and intercept b that represent the best fit line y=mx+b
    //at each step of ReflectArray, using the data dataType (e.g. constAngle).
    //Note that slope is calculated with frequency in MHz.
    //This is somewhat less efficient than doing the best fit calculation all at once for all steps, as
    //is done in uBestFitLines.
    //sumXY will be the sum of x*y for the points (x,y)
    //sumX will be the sum of x; sum Y will be the sum of y
    //sumXSquared will be the sum of x^2
    if startStep>=endStep or startStep<0 or endStep>globalSteps then m=0 : b=0 : exit sub   //Can//t do slope
    sumX=0 : sumY=0 : sumXY=0 : sumXSquared=0
    saveY1DataType=Y1DataType
    Y1DataType=dataType //So CalcGraphData will return what we want as y1.
    isAngle=DataTypeIsAngle(dataType)
    for i=startStep to endStep
        call CalcGraphData i, y1, y2,0       //Get data; we only use y1
        if i>startStep and isAngle then
            //For angles, we must deal with wraparound. To do this with best fit lines, we must make y1
            //change by less than 180 from the previous angle, by adding or subtracting 360 as many times
            //as necessary.
            angleChange= y1-prevAngle
            while y1-prevAngle>180 : y1=y1-360 :wend
            while y1-prevAngle<-180 : y1=y1+360 :wend
        end if
        x=ActualSignalFrequency(datatable(i,1), datatable(i,4)) //actual signal frequency, not equivalent 1G freq ver116-4m //ver116-4s
        sumX=sumX+x : sumY=sumY+y1
        sumXY=sumXY+x*y1 : sumXSquared=sumXSquared+x*x
        prevAngle=y1    //harmless if y1 is not an angle
    next i
    nPoints=endStep-startStep+1
    denom=nPoints*sumXSquared-sumX*sumX
    if denom=0 then m=constMaxValue else m=(nPoints*sumXY-sumX*sumY)/denom  //slope
    b=(sumY-m*sumX)/nPoints         //intercept
    Y1DataType=saveY1DataType   //Restore to original
*/
}

void msaUtilities::uExtendCalPlane(float freq, float &phase, float extend, int isReflect)
{
  //Extend calibration plane by extend ns. freq in MHz. phase in deg.
    //phase is original phase; we replace it with extended phase
    //isReflect=1 for reflection mode, 0 for transmission
    //isShunt=1 for shunt fixture, which complicates the calculation
    //Rotations for one way travel is extend(ns) * freq(MHz)
    //For reflection with series jig, plane extension makes no sense.

  //For transmission mode we rotate the phase by the phase delay of the extension
  float rotate = .001 * extend * freq;    //decimal number of rotations; 0.001 is net of extend in ns and freq in MHz
    //For reflection mode, the rotation is twice that.
  if (isReflect) rotate=2*rotate;
    //drop whole number of rotations and change to degrees (0 to 360)
  float adddegrees = 360*(rotate - (int)(rotate)); //+360 to 0 degrees. ver115-2d eliminated rounding
  phase = phase + adddegrees;  //extension advances the phase
  while (phase>180) {phase=phase-360;}   //ver116-4s
  while (phase<=-180) {phase=phase+360;} //ver116-4s

  //Note this does not properly deal with Shunt fixture, where the effect on S11 is not simply a phase shift
  //At the moment, plane extension just doesn//t make sense for the series fixture in reflection mode or the shunt
  //fixture in any mode.
}

void msaUtilities::uTransformShuntJigRefResist(int origRef, int S21DB, int S21Deg, int newRef, int &newS21DB, int &newS21Deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Find S21 in Shunt Jig with new ref resist
    //Let K=newRef/oldRef
    //newS21=S21/(K+S21*(1-K))
    m=uTenPower(S21DB/20)    //S11 Mag
    p=S21Deg*uRadsPerDegree()   //S11 Radians
    a=m*cos(p) : b=m*sin(p)     //S21=a+jb
    K=newRef/origRef
    Dreal=K+a-a*K
    Dimag=b-b*K
    if Dreal=0 and Dimag=0 then newS21DB=0 : newS21Deg=0   //Huge value, not really possible. Use biggest allowed S21=0
    call cxDivide a, b, Dreal, Dimag, newR, newI
    newS21Deg=uATan2(newR, newI)   //phase in degrees
    newS21DB=10*uSafeLog10(newR^2+newI^2)       //magnitude in db; mult by 10 not 20, because magnitude is squared
*/
}

void msaUtilities::uTransformSeriesJigRefResist(int origRef, int S21DB, int S21Deg, int newRef, int &newS21DB, int &newS21Deg)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Find S21 in Series Jig with new ref resist
    //Let K=oldRef/newRef
    //newS21=S21/(K+S21*(1-K))
    m=uTenPower(S21DB/20)    //S11 Mag
    p=S21Deg*uRadsPerDegree()   //S11 Radians
    a=m*cos(p) : b=m*sin(p)     //S21=a+jb
    K=origRef/newRef
    Dreal=K+a-a*K
    Dimag=b-b*K
    if Dreal=0 and Dimag=0 then newS21DB=0 : newS21Deg=0   //Huge value, not really possible. Use biggest allowed S21=0
    call cxDivide a, b, Dreal, Dimag, newR, newI
    newS21Deg=uATan2(newR, newI)   //phase in degrees
    newS21DB=10*uSafeLog10(newR^2+newI^2)       //magnitude in db; mult by 10 not 20, because magnitude is squared
*/
}

void msaUtilities::uEquivParallelRLC(int freq, int serR, int serReact, int &parR, int &parL, int &parC)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Calc equiv parallel circuit of impedance
    //Compute equivalent parallel circuit RLC components for imped of R+j*X.
    //Freq is in MHz, R in ohms, L in uh and C in uf.
    //The actual equivalent circuit can be formed by R and L or by R and C,
    //not all components together.
    //Note that unless L and C are both zero, one will be positive and the other negative.

    if freq=0 then parR=constMaxValue: parL=constMaxValue: parC=0 :exit sub    //Set for max impedance
    call uEquivParallelImped serR, serReact, parR, parReact //Convert imped to equivalent parallel resistance and reactance ver114-7b
    twoPiF = 2.0*uPi() * freq
    if parReact>=constMaxValue then parL=constMaxValue else  parL = parReact/twoPiF
    if parReact=0 then parC=constMaxValue else parC = -1.0/(twoPiF * parReact)
*/
}

void msaUtilities::uEquivSeriesLC(int freq, int serR, int serReact,  int &serL, int &serC)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Calc equiv series circuit of impedance
    //Compute equivalent series circuit RLC components for imped of R+j*X. R itself is the series resistance
    //and does not need computing. To generate the desired impedance, R must be combined with either an L or a C,
    //but not both. Both L and C are possibilities, though unless L and C are both zero, one will be positive
    //and the other negative. The positive one is the "real" component.

    if freq=0 then serL=0: serC=constMaxValue : exit sub     //Set for min impedance
    twoPiF = 2.0*uPi() * freq
    if serReact>=constMaxValue then serL=constMaxValue else serL = serReact/twoPiF
    if serReact=0 then serC=constMaxValue else serC = -1.0/(twoPiF * serReact)
*/
}

void msaUtilities::uEquivParallelImped(int sR, int sX, int &pR, int &pX)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /* //Calc parallel impedances that would produce this imped
    //Calculates a resistance and reactance pR and pX that when placed in parallel
    //would produce an impedance of sR+j*sX.
    magSquared = sR^2+sX^2
    if sR=0 then
        if sX=0 then
            pR=0 : pX=constMaxValue: exit sub //target imped is zero; do small R and large X
        else
            pR=constMaxValue  //target resistance is 0 but react is not; we need no parallel resistor
        end if
    else
        pR=magSquared/sR    //Nothing is zero so parallel res is simple formula
    end if
    if sX=0 then pX=constMaxValue else pX=magSquared/sX
*/
}

float msaUtilities::NormalizePhase(float p)
{
  //Return phase in range -180<phase<=180
  while (p>180) { p=p-360; }
  while (p<=-180) { p=p+360; }
  return p;
}
//==========================UTILITIES MODULE===========================================



int  msaUtilities::uArrayFromFile(QFile *fHndl, int nPerLine)   //get points from file into uWorkArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Return 1 if error; otherwise zero
    //fHndl$ is the handle of an already open file. We read our data but do not close the file.
    //The file likely contains a title, some other preliminary info (such as Touchstone info) and then
    //many lines of numeric info, possibly ending with "EndContext". If the first lines begin with !, then
    //up to 3 lines are considered title info, and are put into uWorkTitle(1-3).
    //Once we encounter a line beginning with numeric data, we read it and all succeeding lines into uWorkArray()
    //beginning with entry 1. We stop at the end of file or when we read the line "!EndContext"
    //The required number of data items per line is nPerLine, but if nPerLine>=100 we get the required number
    //from the first point we process.
    //We put the number of points read into uWorkNumPoints and the number of items per line into uWorkNumPerPoint.
    //If all lines do not have the same number of data items, we end with an error.
    //Data on a line is delimited by spaces, tabs or commas.
    nLines=0
    //Break the file into separate lines, listed in uTextPointArray$()
    while EOF(#fHndl$)=0
        Line Input #fHndl$, tLine$
        tLine$=Trim$(tLine$)
        nLines=nLines+1
        if nLines>gMaxNumPoints()+maxPointExtraLines then notice "Too Many Lines" : uArrayFromFile=1 : exit void msaUtilities::
        if Left$(tLine$,1)="!EndContext" then nLines=nLines-1 : exit while    //End if we find !EndContext
        uTextPointArray$(nLines)=tLine$ //Add this line to uTextPointArray$()
    wend
    uArrayFromFile=uWorkArrayFromTextArray(nLines, nPerLine) //Transfer data from strings to uWorkArray
*/
  return 0;
}

void msaUtilities::uArrayFromString(QString s, int startN, int &startPos, int nPerLine)   //get points from string into uWorkArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Return 1 for error; otherwise 0.
    //s$ is a string with lines delimited by carriage returns.
    //The lines likely contains a title, some other preliminary info (such as Touchstone info) and then
    //many lines of numeric info, possibly ending with "EndContext".
    //We start at position startPos. We update startPos tothe beginning of the first line after the last one
    //we process, or one past the end of the string.
    //If the first lines begin with !, then
    //up to 3 lines are considered title info, and are put into uWorkTitle(1-3).
    //Once we encounter a line beginning with numeric data, we read it and all succeeding lines into uWorkArray()
    //beginning with entry startN. We stop at the end of string or when we read the line "!EndContext"
    //The required number of data items per line is nPerLine, but if nPerLine>=100 we get the required number
    //from the first point we process.
    //We put the number of points read into uWorkNumPoints and the number of items per line into uWorkNumPerPoint.
    //If all lines do not have the same number of data items, we end with an error.
    //Data on a line is delimited by spaces, tabs or commas.
    nLines=0
    //Break the string into separate lines, listed in uTextPointArray$()
    sLen=len(s$)
    //ver114-5o added lines to skip over !StartContext if it is the first line
    saveStartPos=startPos
    firstLine$=uGetLine$(s$, startPos) //get data line and increment startPos to next line
    //If first line is !StartContext, skip it and start at the next line. startPos was already incremented
    if Upper$(Left$(firstLine$,12))<>"!STARTCONTEXT" then startPos=saveStartPos     //backup to start

    while startPos<=sLen
        sepPos=instr(s$, chr$(13), startPos)
        oldStartPos=startPos
        if sepPos=0 then
            tLine$=Trim$(Mid$(s$, startPos)) : startPos=sLen+1 //Entire remainder is final line
        else
            tLine$=Trim$(Mid$(s$, startPos, sepPos-startPos)) : startPos=sepPos+1  //This line goes to next CR  //ver114-2a
        end if
        if tLine$="!EndContext" then uArrayFromString=startPos : exit while  //ver114-2a
        nLines=nLines+1
        uTextPointArray$(nLines)=tLine$ //Add this line to uTextPointArray$()
    wend
    uArrayFromString=uWorkArrayFromTextArray(nLines, nPerLine) //Transfer data from strings to uWorkArray
*/
}

void msaUtilities::uHighlightText(int handle)  //handle$ is handle variable for the target text box to highlight
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     handle = hWnd(#handle$)
    #handle$, "!Contents? txt$"
    param1 = Len(txt$)
    CallDLL #user32, "SendMessageA", _
    handle as Ulong, _
    _EM_SETSEL as Long, _
    param1 as Long, _
    0 as Long, _
    result as Long
*/
}
/*
void msaUtilities::uSleep tms //wait for tms milliseconds
{    tms=int(tms+0.5)    //avoids crash if fractional
    if tms>0 then CALLDLL #kernel32, "Sleep", tms as ulong, Sleep as void   //ver116-1b

}*/
void msaUtilities::uSleep(int ms)
{
  QMutex dummy;
  dummy.lock();
  QWaitCondition waitCondition;
  waitCondition.wait(&dummy, ms);
}


void msaUtilities::uTickCount()   //Return windows tick count ver116-1b
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //ms since system started, max about 49 days.
    CALLDLL #kernel32, "GetTickCount", tick as ulong
    uTickCount=tick
*/
}

void msaUtilities::uParsePath(QString fullPath, QString &folder, QString &file) //Parse full path name of file into the file name and folder name (no slash at end)
{
  //check to make sure we don't have a blank string as absolutePath is undfined if blank
  if (fullPath.trimmed() == "")
  {
    folder = "";
    file = "";
  }
  QFileInfo info1(fullPath);

  folder = info1.absolutePath();
  file = info1.fileName();
}

int msaUtilities::uConfirmSave(QString fullPath) //if file fullPath$ exists, then ask user to confirm saving file. Return 1 if save, 0 if cancel.
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     call uParsePath fullPath$, folder$, file$   //separate into folder and file name
    if file$="" then uConfirmSave=0 : exit void msaUtilities:://No valid file name
    files folder$, file$, fileInfo$()   //Look for matching files in this folder
    if val(fileInfo$(0, 0)) > 0 then   //This is number of matching files
        Confirm "File "+file$+" already exists. Do you want to replace it?"; response$
        if response$="yes" then uConfirmSave=1 else uConfirmSave=0
    else
        uConfirmSave=1
    end if
*/
  return 0;
}
QString msaUtilities::uAddExtension(QString name, QString exten)    //Add extension to path or file name if there is no extension
{
  if (name.indexOf(".") > -1)   //There already is an extension
    return name;

  return name + "." + exten;
  /*
  //exten$ should not have a dot in it. e.g. bitmap exten$ is bmp, not .bmp.
  int L=name.length();
  dotPos=0;
  for (int i=L; i > 1; i--)  //scan character by character from end
  {
    QString thisChar=name.mid(i, 1);
    if (thisChar==".") {dotPos=i; break; } //looking for dot
    if (thisChar=="/") break;  //looking for backslash--indicates there is no extension
  }
  if (dotPos>0) { uAddExtension$=name$ : exit void msaUtilities::  //There already is an extension
  uAddExtension$=name$;".";exten$  //append dot and exten$
        */
}

void msaUtilities::uDeleteFile(QString fullName)   //Kill file if it exists; no error if it doesn//t
{
  QFile::remove(fullName);
}

int msaUtilities::uFileOrFolderExists(QString Path) // checks for the existence of the given file or folder
{
  int retVal = 0;
  QFileInfo info(Path);
  if (info.exists())
    retVal = 1;

  return retVal;
}

QString  msaUtilities::uSaveFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle)
{
  QString retVal;
  retVal = QFileDialog::getSaveFileName(parent, windTitle, initialDir, filter);
  return retVal;
}

QString msaUtilities::uOpenFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle)
{/*
  QStringList filt;
  filt = filter.split(";;");
  int filterIndex = filt.indexOf(defaultExt);
*/
  //QFileInfo ttt(initDir);
  //initalDir = ttt.absoluteFilePath();
  QString retVal;
  retVal = QFileDialog::getOpenFileName(parent, windTitle, initialDir, filter);
  //retVal = "C:\\Qt\\projects\\MSA\\MSA\\debug\\test_save_data.s1p";
  return retVal;
}

bool msaUtilities::uVerifyDLL(QString dllName)
{

  //Return 1 if specified DLL exists; otherwise 0
  //dllName$ can include or exclude the .dll extension
  //First turn off Windows error message for missing dll

  QLibrary *Lib = new QLibrary(dllName);
  if (!Lib->load())
  {
    qDebug() << dllName << " " << Lib->errorString();
    delete Lib;
    return false;
  }
  Lib->unload();
  delete Lib;
  return true;
}


QString msaUtilities::uExtractFontColor(QString font)  //Return the color from a font specification
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  int pos=font.indexOf("color");
  if (pos=-1) return "";
  int semiPos=font.indexOf(";", pos);  //Find semicolon after "color"
  if (semiPos=-1) semiPos=font.length()+1;  //Pretend it is one past end if we didn//t find it
  //QString colorSpec=font.mid(pos, semiPos-pos);
  return  font.mid(6).trimmed(); //Everything except "color" is the actual color
  */
  return "fix me";
}

void msaUtilities::uSeriesRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C)   //From Z at two points, calculate series RLC components
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //The Z's are real and imaginary impedance at points 1 and 2; f1 and f2 are frequency in MHz
    //We return R (ohms), L (Henries) and C (Farads).
    //In theory Z1r=Z2r, but we use the one with the lower reactance value
    //because it gets measured most accurately.
    if abs(Z1i)<=abs(Z2i) then R=Z1r else R=Z2r
    if R<0 then R=0
    freqScale=1000000*2*uPi() //converts MHz to radians/sec
    w1=f1*freqScale : w2=f2*freqScale

    den=w1^2*w2*Z2i-w2^2*w1*Z1i
    if den=0 then C=constMaxValue else C=(w2^2-w1^2)/den
    //We determine L from C at one of the points. The best accuracy would be from the point with
    //the higher frequency, where the L is more dominant.
    if w1>w2 then
        w=w1 : X=X1
    else
        w=w2 : X=X2
    end if
    den=w^2*C
    if den=0 then L=constMaxValue else L=(w*X*C+1)/den
    if C<0 then C=0
    if L<0 then L=0
    if C<1e-14 then C=0
    if L<1e-11 then L=0
    if C>10 then C=10
    if L>10 then L=10
*/
}

void msaUtilities::uParallelRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C)   //From Z at two points, calculate parallel RLC components
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //The Z's are real and imaginary impedance at points 1 and 2; f1 and f2 are frequency in MHz
    //We return R (ohms), L (Henries) and C (Farads).
    //Convert the impedances to equivalent parallel resistance||reactance
    if Z1i=0 then X1=constMaxValue else X1=(Z1r^2+Z1i^2)/Z1i
    if Z2i=0 then X2=constMaxValue else X2=(Z2r^2+Z2i^2)/Z2i

    if Z1r=0 then R1=constMaxValue else R1=(Z1r^2+Z1i^2)/Z1r
    if Z2r=0 then R2=constMaxValue else R2=(Z2r^2+Z2i^2)/Z2r

    freqScale=1000000*2*uPi() //converts MHz to radians/sec
    w1=f1*freqScale : w2=f2*freqScale

    //In theory R1=R2, but we pick the one with the best measurement accuracy, meaning the one
    //with the highest parallel X.
    if abs(X1)>abs(X2) then R=R1 else R=R2
    if R<0 then R=0

        //Calculate C from X
    den=X1*X2*(w1^2-w2^2)
    if den=0 then C=constMaxValue else C=(w2*X1-w1*X2)/den

    //We determine L from C at one of the points. The best accuracy would be from the point with
    //the higher frequency, where the L is more dominant.
    if w1>w2 then
        w=w1 : X=X1
    else
        w=w2 : X=X2
    end if
    den=w+X*w^2*C
    if den=0 then L=constMaxValue else L=X/den
    if C<0 then C=0
    if L<0 then L=0
    if C<1e-14 then C=0
    if L<1e-11 then L=0
    if C>10 then C=10
    if L>10 then L=10
*/
}

QString msaUtilities::fixColor(QString col)
{
  col = col.trimmed();
  if (col.indexOf("#") == 0)
  {
    //assume it's correct as there is a # at the start of the line
    return col;
  }
  else if (col.indexOf(" ") != -1)
  {
    QStringList items;
    items = col.split(QRegExp("\\s+"));
    if (items.count() != 3)
    {
      // wrong number of items so set to magenta
      return QColor(Qt::magenta).name();
    }
    else
    {
      return QColor(items.at(0).toInt(), items.at(1).toInt(), items.at(2).toInt()).name();
    }
  }
  // assume color is a name eg black, white
  return col;
}
