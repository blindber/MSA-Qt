#include "dialogfilteranalysis.h"
#include "ui_dialogfilteranalysis.h"

dialogFilterAnalysis::dialogFilterAnalysis(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogFilterAnalysis)
{
  ui->setupUi(this);
}

dialogFilterAnalysis::~dialogFilterAnalysis()
{
  delete ui;
}
void dialogFilterAnalysis::SetFilterAnalysis()
{/*//Display dialog for filter analysis and set parameters
    //This assumes that Trace 2 displays a peak with a marker at the peak. The user
    //specifies the marker ID and two db levels: db1 and db2. For example, db1=-3 and
    //db2=-40. We locate the frequencies where the response is down by those amounts,
    //marking them with markers if requested. We calculate insertion loss,
    //bandwidth at both db levels, Q, and shape factor.
    //We can also calculate unloaded Q--i.e. the Q when not loaded
    //by the VNA, if the user specifies the loads.

    WindowWidth = 350 : WindowHeight = 220
    call GetDialogPlacement //set UpperLeftX and UpperLeftY ver115-1c
    BackgroundColor$="buttonface"
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

        //checkbox to activate or deactivate filter analysis
        //ver115-1b separated the captions from the checkboxes so the text color comes out right
    checkbox #filt.doFilt, "Analyze filter spectrum for bandwidth, Q and shape factor.", void MainWindow::filtNOP(), void MainWindow::filtNOP(),10, 10, 320, 15 //ver114-7d
    statictext #filt.Instruct1, "Ref Marker is considered the peak. X1DB (typically 3 dB) ", 25, 35, 300, 15
    statictext #filt.Instruct2, "and X2DB (perhaps 30 dB, or 0 dB to ignore) are the dB", 25, 52, 300, 15
    statictext #filt.Instruct3, "levels to evaluate.", 25, 69, 300, 15
    textTop=95
    statictext #filt.peakLab, "Ref Marker", 30, textTop, 70, 15
        //SEWgraph2 Allowed reference markers are 1, L, R and P+.
        //But markers other than P+ are allowed only if they have been placed
    if gMarkerPointNum(mMarkerNum("1"))<0 then markIDs$(0)="" else markIDs$(i)="1"
    if hasMarkL then markIDs$(1)="L" else markIDs$(1)=""
    if hasMarkR then markIDs$(2)="R" else markIDs$(2)=""
    markIDs$(3)="P+"
    Stylebits #filt.peakMark, _CBS_DROPDOWNLIST, 0, 0, 0   //ver115-1a
    combobox #filt.peakMark, markIDs$(), void MainWindow::filtNOP(),30,textTop+15,60, 90     //combo box to select reference marker

    statictext #filt.db1Lab, "X1DB Down", 120, textTop, 60, 15
    textbox #filt.db1, 130, textTop+15, 40, 20      //textbox for x1 point
    statictext #filt.db2Lab, "X2DB Down", 210, textTop, 60, 15
    textbox #filt.db2, 220, textTop+15, 40, 20      //textbox for x2 point

    button #filt.OK, "OK", void MainWindow::filtFinished(), UL, 70, 150, 50, 25         //OK
    button #filt.Cancel, "Cancel", void MainWindow::filtCancel(), UL, 175, 150, 50, 25   //Cancel
        //Open dialog
    open "Filter Analysis" for dialog_modal as #filt

    #filt, "trapclose void MainWindow::filtCancel()"
    if doFilterAnalysis=0 then   //If we start out w/o analysis, set some defaults but still set #filt.doFilt
        filterPeakMarkID$="P+"
        x1DBDown=3 : x2DBDown=0
    end if
    #filt.doFilt, "set" //Assume we will be doing the analysis

    print #filt.db1, str$(x1DBDown) //Display existing values for db points
    print #filt.db2, str$(x2DBDown)
    if filterPeakMarkID$="" then filterPeakMarkID$="P+"
    #filt.peakMark, "select ";filterPeakMarkID$     //Select the current or default reference marker
    #filt.peakMark, "setfocus"
    #filt.db1, "!setfocus"
    wait
*/
}
//If the user double clicks on the menu when invoking this routine, the second click can be
//interpreted by LB as a click on the graph, so it will call void MainWindow::LeftButDown(). But it doesn//t do
//so until it gets into this routine, at which point the real void MainWindow::LeftButDown() is hidden. So
//we provide one here that just exits.
void dialogFilterAnalysis::LeftButDown()
{/*
    goto void MainWindow::filtCancel()   //ver115-1e
*/
}
void dialogFilterAnalysis::filtNOP()
{/*
    wait
*/
}
void dialogFilterAnalysis::filtCancel()
{/*
    close #filt
    exit sub
*/
}
void dialogFilterAnalysis::filtFinished()
{/*//Window is closing or OK was clicked
    //Note that if #filt.doFilt is reset, indicating no filter analysis, we exit immediately and never get
    //to here. Therefore, we know here that we want to do filter analysis
    #filt.doFilt, "value? doFilt$"    //ver114-7d
    if doFilt$="set" then doFilterAnalysis=1 else doFilterAnalysis=0    //ver114-7d
        //Filter analysis will use many markers, so we can//t have other automatic uses of the markers
    if doFilterAnalysis=1 then doLRRelativeTo$=""   //turn off auto locating of L and R    //ver114-7d
    #filt.peakMark, "selection? filterPeakMarkID$" //selected marker ID for marker at peak
    if filterPeakMarkID$="" then doFilterAnalysis=0 : close #filt : exit sub    //invalid marker-user typed into combobox
        //Get the db values for the x1 and x2 analysis points; force them positive
    #filt.db1, "!contents? c$" : x1DBDown=val(uCompact$(c$)) : if x1DBDown<0 then x1DBDown=0-x1DBDown
    #filt.db2, "!contents? c$" : x2DBDown=val(uCompact$(c$)) : if x2DBDown<0 then x2DBDown=0-x2DBDown
        //If P+ is the reference marker, we add it if necessary. Other reference markers must already
        //exist, because we wouldn//t know where to add them
    if doFilterAnalysis=1 and filterPeakMarkID$="P+" and gMarkerPointNum(mMarkerNum("P+"))<0 then   //ver115-1b
        saveSel$=selMarkerID$   //We want to save and restore the current selected marker
        call mAddMarker "P+", 1, "2"   //place P+ marker on trace 2  //It will be moved on Redraw ver115-1e
        call mMarkSelect saveSel$    //ver114-5L
    end if
    close #filt
    //If halted, redraw with markers; otherwise wait to end. ver114-7d
    if haltsweep=0 then call RefreshGraph 0
end sub
*/
}
