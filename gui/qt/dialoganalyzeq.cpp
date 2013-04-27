#include "dialoganalyzeq.h"
#include "ui_dialoganalyzeq.h"

dialogAnalyzeQ::dialogAnalyzeQ(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogAnalyzeQ)
{
  ui->setupUi(this);
}

dialogAnalyzeQ::~dialogAnalyzeQ()
{
  delete ui;
}
void dialogAnalyzeQ::AnalyzeQ()
{/*//Open dialog to handle Q factor analysis
    WindowWidth = 600
    WindowHeight = 350
    call GetDialogPlacement
    BackgroundColor$="gray"
    ForegroundColor$="black"
    TextboxColor$ = "white"

    statictext #analyzeQ.title, "DETERMINATION OF Q FACTORS",70, 10, 375, 20
    s$="This module determines the apparent Q, series Q and parallel Q for each step of the"
    s$=s$;" existing scan."
    s$=s$;" This involves analysis of the specified number of points around the target point."
    statictext #analyzeQ.title1, s$,20, 30, 550, 60

    controlTop=75
    s$="Apparent Q is the traditional abs(X)/R, and is meaningful only if the tested device"
    s$=s$;" has a single L or C component (including parasitics), but not both."
    statictext #analyzeQ, s$,20, controlTop, 550,35
    s$="Series Q and Parallel Q are Q values that will exist if the device is brought"
    s$=s$;" to resonance at a given frequency by combining it with an appropriate capacitor"
    s$=s$;" or inductor, in series for Series Q and in parallel for Parallel Q."
    statictext #analyzeQ, s$, 20, controlTop+40, 550,60

    s$="These Q factors will be calculated and may then be graphed. They will survive until the next Restart."
    statictext #analyzeQ, s$, 20, controlTop+100, 550,35

    textbox #analyzeQ.nPoints, 310, controlTop+150, 50, 20
    statictext #analyzeQ.nPointsLabel, "Number of points to include:", 60, controlTop+152, 225, 20

    button #analyzeQ.Analyze, "Analyze", void MainWindow::analyzeQAnalyze(), UL,250,controlTop+200, 100, 25

        //Open dialog
    open "Q Analysis" for dialog_modal as #analyzeQ

    #analyzeQ, "trapclose void MainWindow::analyzeQDone()"
    #analyzeQ, "font ms_sans_serif 10"
    pCount=gPointCount()
    if pCount<2 then notice "Not enough points for analysis." : goto void MainWindow::analyzeQDone()
    #analyzeQ.nPointsLabel, "Number of points to include (2-"; int(pCount/4);"):"

    if analyzeQLastNumPoints>=2 and analyzeQLastNumPoints<=pCount then
        defPoints=analyzeQLastNumPoints
    else
        defPoints=max(2,int(pCount/80)*2+1)     //approx pCount/40, but an odd number
    end if
    #analyzeQ.nPoints, defPoints
    wait
*/
}
void dialogAnalyzeQ::analyzeQDone()
{/*    close #analyzeQ
    exit sub
*/
}
void dialogAnalyzeQ::analyzeQAnalyze()
{/*    #analyzeQ.Analyze, "!disable"    //prevent it being pushed while we are processing.
    #analyzeQ.nPoints, "!contents? nPoints$"
    nPoints=val(nPoints$)
    if nPoints<2 or nPoints>pCount/4 then
        nPoints=defPoints : #analyzeQ.nPoints, nPoints
        notice "Invalid number of points. ";defPoints;" used."
    end if
    analyzeQLastNumPoints=nPoints //save for next time we enter this dialog
    call ClearAuxData   //Clear auxiliary graph data by blanking graph names
    call QFactors nPoints   //Put Q data into auxGraphData(0...,)
    goto void MainWindow::analyzeQDone()

end sub
*/
}
