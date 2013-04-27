#include "dialogmarkeroptions.h"
#include "ui_dialogmarkeroptions.h"

dialogMarkerOptions::dialogMarkerOptions(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogMarkerOptions)
{
  ui->setupUi(this);
}

dialogMarkerOptions::~dialogMarkerOptions()
{
  delete ui;
}
void dialogMarkerOptions::mMenuMarkerOptions()
{/*
sub mMenuMarkerOptions     'Button handler to set marker options
    WindowHeight=255 : WindowWidth=200
    BackgroundColor$="buttonface"   'ver116-4g
    ForegroundColor$="black"  'ver116-4g
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c

        'ver115-1b separated the captions from the checkboxes and radiobuttons so the text color comes out right
    checkbox #mark.Show, "", [markNil], [markNil],10,10,20,20
    statictext #mark, "Show Markers On Graph",30,13,140,20
    groupbox #mark.LR, "L,R Markers",5,40,160,140
    radiobutton #mark.Indep, "", [markNil], [markNil],10,70,20,20
    statictext #mark, "L,R Independent", 30,73,140,20
    radiobutton #mark.Bounded, "", [markNil], [markNil],10,95,20,20
    statictext #mark, "P+, P- bounded by L,R", 30,98,140,20
    radiobutton #mark.Down3, "", [markNil], [markNil],10,120,20,20
    statictext #mark, "L,R -3db re P+", 30,123,140,20
    radiobutton #mark.Up3, "", [markNil], [markNil],10,145,20,20
    statictext #mark, "L,R 3db re P-", 30,148,140,20

    button #mark.OK "OK",[markFinished], UL, 20, 200, 50, 25
    button #mark.Cancel "Cancel",[markBtnCancel], UL, 100, 200, 50, 25

    open "Marker Options" for Dialog_modal as #mark
    #mark, "trapclose [markFinished]"
    #mark.Indep, "set"  'may change in next few lines
    if doGraphMarkers then #mark.Show, "set" else #mark.Show, "reset"
    if doPeaksBounded then #mark.Bounded, "set"
    if doLRRelativeTo$="P+" then #mark.Down3, "set"
    if doLRRelativeTo$="P-" then #mark.Up3, "set"
    wait
*/
}

void dialogMarkerOptions::markNil()
{/*
[markNil]
    wait
*/
}

void dialogMarkerOptions::markBtnCancel()
{/*
  [markBtnCancel]
      close #mark
      exit sub
*/
}

void dialogMarkerOptions::markFinished()
{/*
[markFinished]
    #mark.Show, "value? box$"
    if box$="set" then doGraphMarkers=1 else doGraphMarkers=0
    #mark.Bounded, "value? box$"
    if box$="set" then doPeaksBounded=1 else doPeaksBounded=0
    doLRRelativeTo$="" : doLRRelativeAmount=0 : doLRAbsolute=0   'ver115-3f. User has no way to set doLRAbsolute
    #mark.Down3, "value? box$"
    if box$="set" then doLRRelativeTo$="P+" : doLRRelativeAmount=-3
    #mark.Up3, "value? box$"
    if box$="set" then doLRRelativeTo$="P-" : doLRRelativeAmount=3
        'Auto locating of markers prevents other automatic uses of the markers
    if doLRRelativeTo$<>"" then doFilterAnalysis=0

    close #mark
    if haltsweep=0 then call RefreshGraph 0   'if not sweeping redraw graph ver114-7d
end sub
*/
}
