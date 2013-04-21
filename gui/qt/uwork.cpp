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
#include "uwork.h"

cWorkArray::cWorkArray()
{
  uWorkArray.mresize(801,10);
  uWorkFormats.resize(802);
}
//SEWgraph The next several routines manage uWorkArray() and uWorkFormats$(). uWorkArray()
//serves as an intermediary to transfer data between arrays and either strings or files. To
//transfer data to a string or file, the data is first transferred to uWorkArray(). When
//the data is retrieved, it will go to uWorkArray() from which the user must copy to the desired array.
//uWorkFormats$() contains formatting info to convert numeric info into text form.

void cWorkArray::uSetMaxWorkPoints(int maxPoints, int maxData)
{
  //Set max number of points in uWorkArray
  //maxPoints is the new max number of points; maxData is the max number of data entries per point
  //We create a few extra, and downsize if maxPoints is a drastic reduction, but never below 800 points
  unsigned int nPoints=qMax(800, maxPoints+10) ; unsigned int nData=qMax(9, maxData);
  int redimWork;
  if (nPoints<uWorkMaxPoints/2 || nPoints>uWorkMaxPoints)
    redimWork=1;
  else
    redimWork=0;

  if (nData!=uWorkNumPerPoint)
  {
    uWorkFormats.resize(nData+1);
    redimWork=1;
  }
  if (redimWork==1)
  {
    uWorkArray.mresize(nPoints, nData+1);
    uWorkMaxPoints=nPoints ;
    uWorkMaxPerPoint=nData;
  }

}

QString cWorkArray::uTextArrayToString(int startN, int endN)
{
  //Return uTextPointArray$ items as a string
  //Normally the user will set startN=1 and endN=final entry. We make recursive calls here with
  //other values. The number of data items per line (1 to uWorkMaxPerPoint) is in uWorkNumPerPoint.
  //We return a string containing the data, with lines delimited by chr$(13)
  QString newLine="\r";
  int nPoints=endN-startN+1;
  if (nPoints>25)
  {
    //If we have a lot of points to do, divide the job in half and then put the two resulting
    //strings together
    int mid=startN+(int)(nPoints/2);
    return uTextArrayToString(startN, mid-1) + newLine + uTextArrayToString(mid, endN);
  }
  QString s="" ; QString joint="";
  for (int i=startN; i < endN; i++)    //concatenate lines startN thru endN
  {
    s=s+joint+uTextPointArray[i];
    joint=newLine;
  }
  return s;
}

int cWorkArray::uWorkArrayFromTextArray(int nLines, int nPerLine)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'get points from uTextPointArray$() into uWorkArray()
      'Return 1 if error; otherwise zero
      'uTextPointArray$() is an array of nLines strings. The first 3 strings may contain title information. They
      'are assumed to do so if they begin with !. Title info is put into uWorkTitle$(1-6).
      'Various other strings may follow,
      'until we reach a string with numeric information (meaning the string begins with +, -, or a digit).
      'All strings after that must contain numeric information for a point. That information will be entered
      'into uWorkArray, beginning with uWorkArray(1,0). The strings must contain one nPerLine items of numeric
      'data, except that if nPerLine=100, the proper nPerLine will be determined from the first numeric string.
      'Numeric items on a line are separated by spaces, a comma, or tabs, or any combination. Any sequence
      'containing nothing but spaces, commas and tabs constitutes a single delimiter. The items so delimited
      'are evaluated with the val() function; if alpha they evaluate to zero.
      'We put the number of points read into uWorkNumPoints
      for i=1 to 6 :uWorkTitle$(i)="" : next i  'ver114-5m
      nTitles=0 : foundData=0 : firstDataFirstTime=0: doTitles=1 :nPoints=0 :isErr=0
      aTab$=chr$(9)
      for lineNum=1 to nLines
          thisLine$=uTextPointArray$(lineNum)
          if foundData=0 then
              startChar$=Left$(thisLine$,1)   'First character of line
              if instr("+-0123456789",startChar$)>0 then
                  foundData=1 'We have found the first data point
                  firstDataFirstTime=1 'First time thru on first line of data--triggers resizing below
              else
                  if startChar$="!" then
                      if doTitles then
                          nTitles=nTitles+1
                          uWorkTitle$(nTitles)=Mid$(thisLine$,2) 'Title line is everything but the !
                          if nTitles=6 then doTitles=0    'No more title lines; ver114-5L increased to 6
                      else
                          doTitles=0  'no more title lines
                      end if
                  end if
              end if
          end if
          if foundData=1 and thisLine$<>"" then   'skips blank lines
              'Once we have found a data line, process it and all non-blank lines for data points
              nPoints=nPoints+1   'count this point
              lineLen=len(thisLine$)
              nItems=0
              endItem=0
              while endItem<lineLen
                  startItem=lineLen+1 'In case we don't find a start
                  for i=endItem+1 to lineLen  'Find first non-delimiter char after endItem; put its position into startItem
                      thisChar$=Mid$(thisLine$,i,1)
                      if thisChar$<>" " and thisChar$<>"," and thisChar$<>aTab$ then startItem=i : exit for
                  next i
                  if startItem<=lineLen then  'If there is an item, find its end and then enter its value
                      endItem=lineLen+1
                      for j=startItem+1 to lineLen  'Find first delimiter char after startItem; put its position into endItem
                          thisChar$=Mid$(thisLine$,j,1)
                          if thisChar$=" " or thisChar$="," or thisChar$=aTab$ then endItem=j : exit for
                      next j
                      'Here the next item starts at startItem and ends at endItem-1
                      'Put its value into uWorkArray unless this is first line and used to measure the number of items
                      if firstDataFirstTime=0 then uWorkArray(nPoints, nItems)=val(Mid$(thisLine$, startItem, endItem-startItem)) 'ver115-1d
                      nItems=nItems+1
                  end if
              wend    'To next item in this line
              if nPerLine>=100 then nPerLine=nItems else if nPerLine<>nItems then isErr=1
              if isErr then exit for  'Incorrect number of items, so end

              if firstDataFirstTime then
                  'For first line of data on first time through, we just use nItems for resizing,
                  'then back up to do this line again
                  call uSetMaxWorkPoints nLines-lineNum+1, nItems 'Be sure we have room for all
                  lineNum=lineNum-1       'back up to repeat first data line
                  nPoints=0
                  firstDataFirstTime=0    'Don't do resizing again
              end if
          end if
      next lineNum    'To next string, containing data for the next point
      'Here nPoints is the number of points entered into uWorkArray; isErr=1 if an error was encountered
      uWorkNumPoints=nPoints : uWorkNumPerPoint=nPerLine  'Save number of points read, and number of data items per point
      uWorkArrayFromTextArray= isErr
          */
  return 1;
}
