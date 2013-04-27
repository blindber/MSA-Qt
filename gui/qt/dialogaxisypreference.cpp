#include "dialogaxisypreference.h"
#include "ui_dialogaxisypreference.h"

dialogAxisYPreference::dialogAxisYPreference(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogAxisYPreference)
{
  ui->setupUi(this);
}

dialogAxisYPreference::~dialogAxisYPreference()
{
  delete ui;
}
void dialogAxisYPreference::DisplayAxisYPreference(int axisNum, int doTwoPort)
{
  /*
sub DisplayAxisYPreference axisNum, doTwoPort   'Display dialog to select Y axis preferences
    'if doTwoPort=1 we are doing the Two-Port graph; otherwise the regular scan graph
    'ver116-1b added changes to enable this routine to handle two-port graphs
    if axisPrefHandle$<>"" then close #axisPrefHandle$  'If pref window already open; close it
    cycleNumber=1 : call gridappearance.gSetTraceColors cycleColorsAxis1$(1),cycleColorsAxis2$(1) 'start over if cycling colors ver116-4s
    WindowWidth = 425 : WindowHeight = 275  'ver115-2c
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    BackgroundColor$="buttonface"   'colors changed ver116-1b
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"
        'Ver114-6k rearranged and added graph data selection and autoscale.
    'Top and bottom limits
    staticText #axis, "Top Ref", 120,10,50,15
    textbox #axis.topref, 120, 25, 65,20     'Top Ref  ver115-2c
    staticText #axis, "Bot Ref", 120,50,50,15
    textbox #axis.botref, 120, 65, 65,20     'Bot Ref ver115-2c
            'ver115-1b separated the captions from the checkboxes so the text color comes out right
    if doTwoPort=0 then
        checkbox #axis.auto, "",[axisAutoscaleOn], [axisYDoNothing],120, 100, 25,15  'Auto Scale
        staticText #axis, "Auto Scale",140, 100, 60,15  'Auto Scale
    end if

        'Trace Color
    staticText #axis, "Trace Color", 10,10,70,15
    graphicbox #axis.color, 10, 25, 50, 20  'Trace Color

        'Trace width
    staticText #axis "Trace Width", 10,50,70,15
    TraceWidths$(0)="0" : TraceWidths$(1)="1" : TraceWidths$(2)="2" : TraceWidths$(3)="3"
    Stylebits #axis.width, _CBS_DROPDOWNLIST, 0, 0, 0   'ver115-1a
    combobox #axis.width, TraceWidths$(), [axisYDoNothing],10, 65, 50, 90   'Trace Width

        'Trace style (Norm Erase,...)
    if doTwoPort=0 then
        staticText #axis "Trace Style", 15,95,70,15
        if (msaMode$="SA" or msaMode$="ScalarTrans") and axisNum=primaryAxisNum then    'ver115-3b
            'histo modes are only in non-phase modes, and only on primary axis
            TraceStyles$(0)="Off" : TraceStyles$(1)="Norm Erase" :  TraceStyles$(2)="Norm Stick"
            TraceStyles$(4)="Histo Erase" : TraceStyles$(5)="Histo Stick"
        else    'Phase modes and secondary axis have no histo
            TraceStyles$(0)="Off" : TraceStyles$(1)="Erase" :  TraceStyles$(2)="Stick"
            TraceStyles$(4)="" : TraceStyles$(5)=""
        end if
        'Y1DisplayMode, Y2DisplayMode  0=off  1=NormErase  2=NormStick  3=HistoErase  4=HistoStick
        Stylebits #axis.style, _CBS_DROPDOWNLIST, 0, 0, 0   'ver115-1a
        combobox #axis.style, TraceStyles$(), [axisYDoNothing],10, 110, 80, 120   'Trace Style
    end if

        'Number of vertical divisions
    staticText #axis "Number of Divisions", 10,140,140,15
    staticText #axis "(Affects both Y axes)", 10,155,140,15
    NumVertDiv$(0)="4" : NumVertDiv$(1)="5" : NumVertDiv$(2)="6"    'ver115-1b changed to NumVertDiv$
    NumVertDiv$(3)="8" : NumVertDiv$(4)="10" : NumVertDiv$(5)="12"
    combobox #axis.NDiv, NumVertDiv$(), [axisYDoNothing],25, 170, 60, 120   'Number of vert divisions

    checkbox #axis.cycleColors, "Cycle colors in Stick modes.",[axisYDoNothing], [axisYDoNothing], 120, 120, 175,15  'Cycling ver116-4s

        'SuppressPhase box   'ver116-1b
    if doTwoPort=0 and msaMode$<>"SA" and msaMode$<>"ScalarTrans" then
        doingPhase=1
        checkbox #axis.suppressPhase, "Force phase to zero.",[axisYDoNothing], [axisYDoNothing],220, 100, 140,15  'suppressPhase
    else
        doingPhase=0
    end if

        'OK and Cancel buttons
    button #axis.OK, "OK", [axisYFinished],UL, 220, 175,50,25
    button #axis.Cancel, "Cancel", [axisYCancel],UL, 300, 175,50,25

    axisDataLeft=220 : axisDataTop=25
    dim axisGraphData$(40) : dim axisDataType(40)   'ver115-4a

    if doTwoPort=0 then     'ver116-1b
        numGraphs=FillRegularGraphData(axisNum)
    else
        numGraphs=TwoPortFillGraphData(axisNum) 'Fill axisGraphData$ and axisDataType
        if numGraphs<2 then
            Notice "There are no valid parameters to graph."
            call TwoPortSetPrimSecTypes constNoGraph, constNoGraph
            call gSetNumPoints 0
            call TwoPortCalcAndGraph
            exit sub
        end if
    end if

            'List of graphs
    staticText #axis.Instruct1, "Graph Data",axisDataLeft, axisDataTop, 270,15
    'ver115-3a omitted the stylebits, because we have to take action when the graph changes
    comboBox #axis.GraphData, axisGraphData$(),[axisYChangeGraph], axisDataLeft, axisDataTop+15, 175, 350 'ver115-4a
    button #axis.doDefault, "Default Graph",[axisYDefaultOn], UL, axisDataLeft+20, axisDataTop+40, 120, 25 'ver116-4h

    open "Axis Y";axisNum for dialog_modal as #axis 'Open preference dialog
    axisPrefHandle$="#axis"
    #axis, "trapclose [axisYFinished]"
    #axis.color, "when leftButtonDown [PickColor]"
        'Get some existing values
    call gGetTraceWidth w1, w2  'Trace widths
    call gGetTraceColors c1$, c2$   'Trace colors
    if axisNum=1 then
        tWidth=w1
        tColor$=c1$
        if doTwoPort=0 then
            if autoScaleY1=1 then #axis.auto, "set" else #axis.auto, "reset"    'ver114-7a
        end if
    else
        tWidth=w2
        tColor$=c2$
        if doTwoPort=0 then
            if autoScaleY2=1 then #axis.auto, "set" else  #axis.auto, "reset"    'ver114-7a
        end if
    end if
    'Fill in preference choices with current values
    call gGetYAxisRange axisNum, yMin, yMax     'Previously specified min and max
    call gGetAxisFormats xForm$, y1Form$, y2Form$
    if axisNum=1 then yForm$=y1Form$ else yForm$=y2Form$
    topref$= uFormatted$(yMax, yForm$): botref$=uFormatted$(yMin, yForm$)
    print #axis.topref, topref$ : print #axis.botref, botref$

    if doTwoPort=0 and doingPhase and suppressPhase=1 then #axis.suppressPhase, "set"   'ver116-1b
    if doCycleTraceColors=1 then #axis.cycleColors, "set" else #axis.cycleColors, "reset"  'ver116-4s

    #axis.color, "fill "; tColor$;";flush"  'Fill box with current trace color
    #axis.width, "select ";tWidth
    #axis.width, "setfocus" 'So selection is visually active
    call gGetNumDivisions nHorDiv, nVertDiv
    #axis.NDiv, "!";nVertDiv
    #axis.NDiv, "setfocus" 'So selection is visually active

    if doTwoPort=0 then
        'select initial trace style based on Y2DisplayMode or Y1DisplayMode. Note those variables run from 0 but
        'combobox selection indices run from 1; hence the +1
        if axisNum=1 then #axis.style, "selectindex ";Y1DisplayMode+1 _
                                else #axis.style, "selectindex ";Y2DisplayMode+1
        #axis.style, "setfocus" 'Visually activate
    end if

    'Select current graph data. Set sel to the index corresponding to the entries in axisGraphData$(),
    'but sel is numbered from 1 whereas the arrays are indexed from 0.
    if doTwoPort then
        if axisNum=1 then origData=TwoPortGetY1Type() else origData=TwoPortGetY2Type()
    else
        if axisNum=1 then origData=Y1DataType else origData=Y2DataType
    end if
    restoreData=origData 'used to restore on cancel. origData gets changed when graph data is changed ver115-3b
    selectDataType=origData : gosub [SelectGraphType]
    call RememberState  'Remember variables so we can detect changes
    wait    'Wait for user to make choices
*/
}

void dialogAxisYPreference::SelectGraphType()
{
  /*
  [SelectGraphType]   'select graph to match selectDataType
      sel=0
      for i=0 to 40
          if axisDataType(i)=selectDataType then sel=i+1 : exit for  'Look for match
      next i
      if sel=0 then sel=1 'Error, use first entry
      'sel now has the index (1...) to use to select the current data type
      #axis.GraphData, "selectindex ";sel
      #axis.GraphData, "setfocus"
      #axis.topref, "!setfocus" : call uHighlightText "#axis.topref"      'Highlite axis max box  'ver115-2c
      return
*/
}

void dialogAxisYPreference::axisYDefaultOn()
{/*
[axisYDefaultOn]    'button to use default graph types ver116-4h
    call GetDefaultGraphData axisNum, selectDataType, yMin, yMax
    gosub [SelectGraphType] : gosub [axisYChangeGraph]
    wait
    */
}

void dialogAxisYPreference::TwoPortLeftButDouble()
{/*
  [TwoPortLeftButDouble]
      wait
*/
}

void dialogAxisYPreference::axisYDoNothing()
{/*
[axisYDoNothing] 'embedded in DisplayYAxisPreference
    wait
*/
}

void dialogAxisYPreference::axisYChangeGraph()
{/*
[axisYChangeGraph]   'ver115-3a
    'We need to record the change and also change the format for top and bottom references.
    #axis.GraphData, "selectionindex? sel"
    if sel=0 then sel=1 'In case user typed something into the box.
    newData=axisDataType(sel-1)   'constant indicating the data type
    if doTwoPort then 'ver116-1b
        if axisNum=1 then call TwoPortSetY1Type newData else call TwoPortSetY2Type newData
    else
        if axisNum=1 then Y1DataType=newData else Y2DataType=newData
        if newData=constNoGraph then #axis.auto, "reset"    'turn off autoscale if no graph ver115-3b
    end if

    if newData<>constNoGraph then  'ver116-1b
        if doTwoPort then
            call TwoPortDetermineGraphDataFormat newData, dum1$, dum2$, dum3, yForm$   'Get new data format
        else
            call DetermineGraphDataFormat newData, dum1$, dum2$, dum3, yForm$   'Get new data format
        end if
            'Get current range
        #axis.topref "!contents? newTop$"
        #axis.botref "!contents? newBot$"
            'uCompact deletes blanks, which can mess up negative numbers
        newTop=uValWithMult(newTop$) : newBot=uValWithMult(newBot$) : newWidth=val(uCompact$(newWidth$))
        if newTop<newBot then temp=newTop : newTop=newBot : newBot=temp 'Swap to get correct order

        topref$= uFormatted$(newTop, yForm$): botref$=uFormatted$(newBot, yForm$)
        newTop=uValWithMult(topref$) : newBot=uValWithMult(botref$)   'Do any rounding from formatting
        if doTwoPort then
            call TwoPortStartingLimits newData, origData, newBot, newTop 'ver115-3a
        else
            call StartingLimits newData, origData, newBot, newTop 'ver115-3a
        end if

        topref$= uFormatted$(newTop, yForm$): botref$=uFormatted$(newBot, yForm$)
            'Reprint the range in the new format
        print #axis.topref, topref$  'ver115-3a
        print #axis.botref, botref$  'ver115-3a
    end if
    origData=newData
    wait
*/
}

void dialogAxisYPreference::axisAutoscaleOn()
{/*
[axisAutoscaleOn]   'autoscaling turned on
    call CalcAutoScale axisNum, axisMin, axisMax
    topref$= uFormatted$(axisMin, yForm$): botref$=uFormatted$(axisMax, yForm$)  'ver115-4h
    print #axis.topref, topref$  'ver115-3a
    print #axis.botref, botref$  'ver115-3a
    wait
*/
}

void dialogAxisYPreference::PickColor()
{/*
[PickColor] 'embedded in DisplayYAxisPreference
    ColorDialog tColor$, newColor$
    if newColor$<>"" then
        tColor$=newColor$
        #axis.color, "fill "; tColor$;";flush"  'Fill box with new trace color
    end if
    wait
*/
}

void dialogAxisYPreference::axisYCancel()
{/*
[axisYCancel]    'User has cancelled; 'embedded in DisplayYAxisPreference
    close #axis
    if doTwoPort then
        if axisNum=1 then call TwoPortSetY1Type restoreData else call TwoPortSetY2Type restoreData    'restore data type ver116-1b
    else
        if axisNum=1 then Y1DataType=restoreData else Y2DataType=restoreData    'restore data type ver115-3b
    end if
    axisPrefHandle$=""
    exit sub
*/
}

void dialogAxisYPreference::axisYFinished()
{/*
[axisYFinished]  'Window is being closed; record choices; 'embedded in DisplayAxisYPreference
    'Note that if the graph type changed, action would have been taken immediately in [axisYChangeGraph]
    'So Y1DataType and Y2DataType, or TwoPortY1Type and TwoPortY2Type are current ver115-3a
    #axis.topref "!contents? newTop$"
    #axis.botref "!contents? newBot$"
    #axis.width, "selection? newWidth$"
        'uCompact deletes blanks, which can mess up negative numbers
    newTop=uValWithMult(newTop$) : newBot=uValWithMult(newBot$) : newWidth=val(uCompact$(newWidth$))
    if newWidth<0 then newWidth=0 else if newWidth>3 then newWidth=3    'ver116-4b
    if newTop<newBot then temp=newTop : newTop=newBot : newBot=temp 'Swap to get correct order
        'ver115-3a moved call to SaneLimits into [axisYChangeGraph]
    if newBot=newTop then newBot=yMin : newTop=yMax : notice "Axis range cannot be zero. Previous values retained."
    call gGetTextColors xCol$, y1Col$, y2Col$,gridCol$
    if doTwoPort=0 then #axis.auto, "value? auto$"
    if axisNum=1 then
        w1=newWidth
        c1$=tColor$ : y1Col$=tColor$    'Set trace and grid labels to same color
        if doTwoPort then call TwoPortSetY1Range newBot, newTop else call SetY1Range newBot, newTop 'ver116-1b
        if doTwoPort=0 then
            if auto$="set" then autoScaleY1=1 else autoScaleY1=0
        end if
    else
        w2=newWidth
        c2$=tColor$ : y2Col$=tColor$    'Set trace and grid labels to same color
        if doTwoPort then call TwoPortSetY2Range newBot, newTop else call SetY2Range newBot, newTop   'ver116-1b
        if doTwoPort=0 then
            if auto$="set" then autoScaleY2=1 else autoScaleY2=0
        end if
    end if

    if doTwoPort=0 and doingPhase then 'ver116-1b
        #axis.suppressPhase, "value? sup$"
        if sup$="set" then suppressPhase=1 else suppressPhase=0
    end if
    #axis.cycleColors, "value? cycleVal$" : if cycleVal$="set" then doCycleTraceColors=1 else doCycleTraceColors=0  'ver116-4s
    call gridappearance.gSetTraceColors c1$, c2$
    call gSetTraceWidth w1, w2
    call gSetTextColors xCol$, y1Col$, y2Col$,gridCol$
    call SetCycleColors     'Set trace colors for color cycling ver116-4s
    #axis.NDiv, "contents? nDiv$"
    nVertDiv=val(nDiv$) : if nVertDiv<1 then nVertDiv=1 else if nVertDiv>12 then nVertDiv=12
    call gSetNumDivisions nHorDiv, nVertDiv
    'ver115-1b deleted call gCalcGraphParams; DetectChanges will handle that

    if doTwoPort=0 then
        #axis.style, "selectionIndex? axisStyle"
        if axisStyle<1 then axisStyle=1
        if axisNum=1 then   'Set Y1DisplayMode or Y2DisplayMode from contents of axis style box; to Norm Erase if no graph
            if Y1DataType=constNoGraph then Y1DisplayMode=1 else Y1DisplayMode=axisStyle-1 'ver115-4e
        else
            if Y2DataType=constNoGraph then Y2DisplayMode=1 else Y2DisplayMode=axisStyle-1 'ver115-4e
        end if
        call ImplementDisplayModes    'ver114-6e
                'Save some sweep settings for reflection and transmission for use when changing
            'back to a previously used mode
        if msaMode$="Reflection" then   'ver116-1b
            refLastY1Type=Y1DataType : refLastY1Top=Y1Top : refLastY1Bot=Y1Bot : refLastY1AutoScale=autoScaleY1
            refLastY2Type=Y2DataType : refLastY2Top=Y2Top : refLastY2Bot=Y2Bot : refLastY2AutoScale=autoScaleY2
        else
            if msaMode$="VectorTrans" then
                transLastY1Type=Y1DataType : transLastY1Top=Y1Top : transLastY1Bot=Y1Bot : transLastY1AutoScale=autoScaleY1
                transLastY2Type=Y2DataType : transLastY2Top=Y2Top : transLastY2Bot=Y2Bot : transLastY2AutoScale=autoScaleY2
            end if
        end if
    end if

    if axisPrefHandle$<>"" then close #axis
    axisPrefHandle$=""
    if doTwoPort=0 then
        call DetectChanges 0  'Redraw as required and/or signal to restart via continueCode
    else
        call TwoPortAdjustToYChanges (origData<>restoreData)
    end if
end sub     'end DisplayAxisYPreference
*/
}
