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
#include "touchstone.h"
#include <QtCore>


touchStone::touchStone()
{
  touchMaxData=10000;
}

void touchStone::setUwork(cWorkArray *uw)
{
  uWork = uw;
}
//==================================================================
//         TOUCHSTONE MODULE ver115-5f
//==================================================================
QFile *touchStone::touchOpenInputFile(QString fullPath) //Open file; return handle or blank if not successful
{
  QFile *fFile = new QFile(fullPath);

  if (!fFile->exists())
  {
    QString ggg = "file not found";
    delete fFile;
    fFile = NULL;
  }
  else if (!fFile->open(QFile::ReadOnly))
  {
    QString eee = fFile->errorString();
    qDebug() << eee;
    delete fFile;
    fFile = NULL;
    QMessageBox::warning(0, "Error", "Could not open data file.");
  }

  return fFile;
}

//ver115-6a
QFile *touchStone::touchOpenOutputFile(QString fullPath)  //Open file; return handle or blank if not successful
{
  QFile *fFile = new QFile(fullPath);

  if (!fFile->exists())
  {
    QString ggg = "file not found";
    delete fFile;
    fFile = NULL;
  }
  else if (!fFile->open(QFile::WriteOnly))
  {
    QString eee = fFile->errorString();
    qDebug() << eee;
    delete fFile;
    fFile = NULL;
    QMessageBox::warning(0, "Error", "Could not open output file.");
  }

  return fFile;
}

void touchStone::touchWriteOnePortParameters(QStringList &fHndl, int dataMode)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //save data from VNAData in touchstone file
      //We do not include StartContext or EndContext lines
      //fHndl$ is the handle of an already open file.
      //dataMode$ corresponds to msaMode$ and indicates the type of data : SA, VectorTrans, ScalarTrans or Reflection.
      //VectorTrans and ScalarTrans are treated equally.
      //We output the data but do not close the file.
      //VNAData, VNADataNumSteps, VNADataZ0 and VNADataTitle$ must be set by the caller

         //First 3 lines are title, each preceded by !
      //Next line is sweep info
      //Next is Touchstone options line
      //Next is comment data headings
      //Then comes each point as its own string
      print #fHndl$, "!";VNADataTitle$(1)
      print #fHndl$, "!";VNADataTitle$(2)
      print #fHndl$, "!";VNADataTitle$(3)
      print #fHndl$, "!";VNADataTitle$(4)  //This line has sweep info set by user; not printed as part of title ver114-5m
      if dataMode$="SA" then
              print #fHndl$, "!  MHz        dBm"
      else
          print #fHndl$, "# MHz S DB R ";VNADataZ0 //Freq in MHz, data in DB/angle format  ver116-4a
          if dataMode$="Reflection" then
              print #fHndl$, "!  MHz       S11_dB    S11_Deg"
          else
              print #fHndl$, "!  MHz       S21_dB    S21_Deg"
          end if
      end if

      for i=0 to VNADataNumSteps   
          f=VNAData(i,0) //true freq
          db=VNAData(i,constGraphS11DB)   //dB ver116-1b
          ang=VNAData(i,constGraphS11Ang)    //deg  ver116-1b
          freq$=using("####.######",f)
          data1$=using("####.#####",db)
          if dataMode$="SA" then
              print #fHndl$, uAlignDecimalInString$(freq$,11,4); _
                      uAlignDecimalInString$(data1$,9,5)
          else
              data2$=using("####.###",ang)
              print #fHndl$, uAlignDecimalInString$(freq$,11,4); _
                      uAlignDecimalInString$(data1$,9,5); _
                      uAlignDecimalInString$(data2$,9,5)
          end if
      next i
          */
}
void touchStone::touchReadParams(QString source, int sourceType)
{
  //source$ is a handle variable with the handle of the source.
  //if sourceType=0 the source is a text editor; otherwise it is a file.

  //reads S parameters from text editor or file into array uWorkArray, starting with uWorkArray(1,...)
  //The parameters can be one-port or two-port.
  //The entries will be frequency in MHz(0), dB(1) and angle(2), and if parameters are for two-port (as
  //determined from the number of parameters per line, there will be three more dB/angle pairs. If there
  //are two or three sets of parameters, the file is treated as two-port and the missing parameters are zeroed.
  //The parameter file may be in touchstone format, or in a comma delimited file.
  //The frequency is assumed to be in Mhz, the parameters in DB, angle (degrees) format and the
  //reference 50 ohms, unless otherwise specified in an option line.
  //The option line begins with # and contains:
  //   HZ, KHZ, MHZ or GHZ to specify frequency units
  //   RI or MA to specify real, imaginary or magnitude, angle (degrees) format. (We convert to DB format)
  //   S to specify parameters are S params, though that is assumed anyway
  //   R followed by a space and then the reference resistance.
  //The option line, if present, should occur before any data, but we don//t enforce that.
  //If format is not DB, we convert it to DB.
  //The data items and option items can be separated by commas instead of spaces
  //
  //Comments are allowed, marked by "!" at the beginning of the comment. If a comment
  //is the only thing on a line, the "!" must be the first character. Comment lines
  //occurring before the options line are accumulated in the array touchComments,
  //up to a max of 10 lines.
  //The number of data lines may not exceed touchMaxData. When done, the caller should use uSetWorkNumPoints to
  //set uWorkArray to a reasonably small number of points; every routine that uses it redims it before use anyway.
  //The number of data lines entered into uWorkArray will be put into uWorkNumPoints
  //The number of comment lines entered into touchComments$ will be put into touchCommentCount
  //
  //touchSParamType$ will be set to "", "Reflection", "Transmission" or "TwoPort". It defaults to "" but is changed
  //if a comment line after the options line (which comment line is typically column headers) contains
  //any reference to Sxy: if x=y touchSParamType$ is changed to "Reflection"; otherwise to "Transmission.
  //It will be changed to "TwoPort" if more than one set of parameters is encountered in the first data line.
  //Of course, it is possible that the data being read has no column headers, or that it is actually SA data.

  uWork->uSetMaxWorkPoints(touchMaxData, 3);   //Allow touchMaxData points, with entries for freq and two parameter parts
    //If the file contains full two-port params, we will resize the work array when we discover that
  uWork->uWorkNumPoints=0;    //Will have total number of data lines in sParam
  touchCommentCount=0; //Will have total number of comment lines in touchComments$
  touchBadLine=0;      //If error occurs, will be set to the line causing the error
  int fileLine=0;          //Keeps track of which line of file we are on
  int accumComments=1;     //We save comment lines in touchComments$ until this is set to zero
  int prevLineWasOptions=0;    //Set to 1 if previous line was the options line
  touchFreq="MHZ"; touchForm="DB"; QString touchParam="S"; touchRef=50;  //Defaults--We use DB as default, not normal MA
  touchFreqMult=1000000;    //For MHZ
  touchSParamType="";    //Default. Will end up empty, Reflection, Transmission or TwoPort ver116-4a
  //if (sourceType=0) then print #source$, "!lines sourceLines"
  int filePos = 0;
  QStringList ttt = source.split("\r");
  int sourceLines = ttt.count();
  while (1==1)
  {
    fileLine=fileLine+1;
    if (fileLine > sourceLines)
      break;
    QString tLine = util.uGetLine(source, filePos);

    QString startChar=tLine.left(1);
    if (startChar=="!")
    {
        if (prevLineWasOptions==1)
        {
        //Comment line after options may contain column headings for the data,
        //which can indicate what parameters we have.
            touchProcessPostOptionComments(tLine.toUpper());
            prevLineWasOptions=0;
        }
            //Accumulate up to the first 10 lines of comments, without the exclamation
        if (accumComments==1)
        {
          touchComments[fileLine]=tLine.mid(1);
          touchCommentCount=fileLine;
        }
        if (fileLine>10)
          accumComments=0;    //Stop accumulating comments
    }
    else
    {
        accumComments=0;    //Stop accumulating comments
        int tLen=tLine.length();
        if (tLen>0)   //Ignore empty lines or lines of all blanks
        {
            if (tLen<3) {touchBadLine=fileLine; return;}
            if (startChar=="#")
            {
                tLine=tLine.mid(2);  //all but #
                touchProcessOptionsLine(fileLine,tLine.toUpper()); //Process options, e.g # MHZ S DB R 50
                prevLineWasOptions=1;
            }
            else
            {
                uWork->uWorkNumPoints=uWork->uWorkNumPoints+1;
                if (uWork->uWorkNumPoints>=touchMaxData)
                {
                  touchBadLine=fileLine;
                  return;
                } //Error if more points than we have room for ver116-1b
                touchProcessDataLine(fileLine, tLine);
                prevLineWasOptions=0;
            }  //end of processing data line
        }
    }     //end of processing non-comment line
    if (touchBadLine !=0)
      return;
  }    //go to next file line
  if (touchBadLine==0)
    touchConvertParamForm("DB");  //we always want DB/angle format
  touchForm="DB"; //we just converted
}

void touchStone::touchReadParams(QFile *source, int sourceType)
{
  QString str = "";

  QTextStream in(source);

  while(!in.atEnd())
  {
    QString line = in.readLine();
    str = str + line + "\r";
  }
  touchReadParams(str, 0);
}

void touchStone::touchProcessPostOptionComments(QString tLine)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //Comment line after options may contain column headings for the data,
    //which can indicate what parameters we have. We look for a word
    //beginning with Sxy, where x and y are numerals (most likely 1 or 2). If x and y
    //are equal, we have reflection parameters, otherwise transmission.
    tLine$=Upper$(tLine$)
    while Len(tLine$)>0
        item$=touchStringInput$(tLine$)
        if Len(item$)>2 then if Left$(item$,1)="S" then
            c1$=Mid$(item$,2,1): c2$=Mid$(item$,3,1)
            v1=val(c1$) :v2=val(c2$)
            if v1>0 and v1<=9 and v2>0 and v2<=9 then
                if v1=v2 then touchSParamType$="Reflection" else touchSParamType$="Transmission"
                exit sub
            end if
        end if
    wend
*/
}

void touchStone::touchProcessOptionsLine(int fileLine, QString tLine)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //Analyze touchstone options line for frequency, data format, and reference R0
    if Len(tLine$)<2 then exit sub    //nothing to do
    tLine$=Mid$(tLine$,2)   //drop leading "#"
    tLen=Len(tLine$)
    while tLen>0
        item$=touchStringInput$(tLine$)
        tLen=Len(tLine$)
        select case item$
            case "HZ"
                touchFreq$=item$: touchFreqMult=1
            case "KHZ"
                touchFreq$=item$: touchFreqMult=1000
            case "MHZ"
                touchFreq$=item$: touchFreqMult=1000000
            case "GHZ"
                touchFreq$=item$: touchFreqMult=1e9
            case "MA","DB","RI"
                touchForm$=item$
            case "S",""
                //no need to do anything
            case "R"
                item$=touchStringInput$(tLine$)      //Get reference resistance
                touchRef=val(item$)
                if touchRef<=0 then touchBadLine=fileLine:exit while
            case else
                touchBadLine=fileLine: exit while
        end select
    wend
*/
}

void touchStone::touchProcessDataLine(int fileLine, QString tLine)   //mod 116-1b
{
  //Get freq and param from this line; save param as S param in whatever form the file uses; we convert later
  //uWorkNumPoints marks the current entry
  //For the first data line, we also determine the number of parameters
  QString item=touchStringInput(tLine);  //Get Frequency
  if (item=="")
  {
    touchBadLine=fileLine;
    return;
  }    //must have frequency
  float freq=item.toFloat();
  float freqHz=freq * touchFreqMult;
  uWork->uWorkArray[uWork->uWorkNumPoints][0]=freqHz;
  for (int pNum=1; pNum <= 4; pNum++) //ver116-1b  //four possible parameters, of two parts each
  {
    item=touchStringInput(tLine);  //Get first part of parameter number pNum
    if (item=="")
      item="0";  //Force missing first param to zero

    float p1=item.toFloat();
    item=touchStringInput(tLine);  //Get second part of parameter
    if (item=="")
      item="0";  //Force missing second param to zero
    float p2=item.toFloat();
    if (uWork->uWorkNumPoints==1 && pNum==1)
    {
      //For first parameter of first point, we need to see if we have full two-port parameters
      //based on how much data we have. We assume that if we have more than one parameter, we
      //have a full set of four. If there are any missing parameters, they are read as zero.
      tLine=tLine.trimmed();
      if (tLine.length()!=0 && tLine.left(1)!="!")
      {
        touchSParamType="TwoPort"; //We have more data that is not a comment
        uWork->uSetMaxWorkPoints(touchMaxData, 9);  //9 items per point--freq and two parts each for S11, S21,S12 and S22, indexed 0-8
        uWork->uWorkArray[uWork->uWorkNumPoints][0]=freqHz; //restore frequency
      }
    }
    uWork->uWorkArray[uWork->uWorkNumPoints][2*pNum-1]=p1;
    uWork->uWorkArray[uWork->uWorkNumPoints][2*pNum]=p2;
    if (touchSParamType!="TwoPort")
      break;    //Only one param (with two parts) if not two port
  } //next pNum
}

QString touchStone::touchStringInput(QString &s)
{
  QString retVal;
  //Get next item from comma or space or tab delimited string s$ and delete from s$
  //Spaces are trimmed. If a comma is the delimiter and there are two commas
  //separated by nothing or by one or more spaces, an empty item will be returned
  s=s.trimmed();
  int pos=0;
  int sLen=s.length();
  QString tab="\t";
  if (sLen==0)
    return "";
  QString currChar;
  while (pos<=sLen) //Find delimiter.
  {
    currChar=s.mid(pos,1); //get character
    if (currChar==" " || currChar=="," || currChar==tab)
      break; //ver116-2a
    pos=pos+1;
  }
  if (pos>sLen)
  {
    retVal=s;
    s="";
    return retVal;
  }  //end of string found
  QString item=s.left(pos);  //Get chars up to but not including delimiter
  if (pos==sLen)
  {
    s="";
  }
  else
  {
    s=s.mid(pos+1);  //Drop those chars and delimiter
    //If delimiter was space, delete all successive spaces and a single comma after them
    if (currChar==" ")
    {
      s=s.trimmed();
      sLen=s.length();
      if (sLen>0)
      {
        if (s.left(1)==",")
        {
          if (sLen==1)
            s="";
          else s=s.mid(2);
        }
      }
    }
  }
  return item;
}

void touchStone::touchConvertSingleParam(QString oldForm, QString newForm, float &p1, float &p2)  //convert single param in place //ver116-4m
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //p1 and p2 are the two parts of the parameter. Forms are RA, DB or RI
    if newForm$="RI" then
        //new form is real, imaginary; current form not RI and is therefore polar
        if oldForm$="DB" then p1=10^(p1/20)   //put in MA form
        m=p1   //magnitude
        p2=p2*uRadsPerDegree()    //Trig is done in radians
        p1=m*cos(p2): p2=m*sin(p2)
    else
        //new form is polar
        pForm$=oldForm$
        if oldForm$="RI" then
            //Put in MA form
            m=sqr(p1^2+p2^2)   //magnitude
            p2=uATan2(p1, p2)   //angle (degrees)
            p1=m
            pForm$="MA"
        end if
        if pForm$<>newForm$ then
            //p is db and new form is MA, or vice-versa
            if newForm$="DB" then
                p1=20*uSafeLog10(p1) //convert to DB
            else
                p1=10^(p1/20)   //convert to MA
            end if
        end if
    end if
*/
}

void touchStone::touchConvertParamForm(QString newForm)
{
  //convert the S parameters in uWorkArray into form specified by newForm$
  // RI=real, imaginary;  DB= db, angle (degrees); MA=magnitude, angle (degrees)
  if (newForm==touchForm)
    return;    //Already in desired form
  int nParam;
  if (touchSParamType=="TwoPort")
    nParam=4;
  else
    nParam=1;   //Number of S-parameters per line ver116-1b
  for (int i=1; i <= uWork->uWorkNumPoints; i++)   //Process each frequency
  {
    for (int pNum=1; pNum <= nParam; nParam++)
    {
      int index1=2*pNum-1;
      int index2=index1+1;   //Index for the two parameter parts in uWorkArray
      float p1=uWork->uWorkArray[i][index1];
      float p2=uWork->uWorkArray[i][index2];  //Get the two parts of current parameter
      touchConvertSingleParam(touchForm, newForm, p1, p2);   //ver116-4m
      uWork->uWorkArray[i][index1]=p1;
      uWork->uWorkArray[i][index2]=p2; //Put converted param back where it came from
    }   //Next parameter for this frequency
  }
  touchForm = newForm;
}
//===============End Touchstone Module==============
