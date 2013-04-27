#include "dialogvnaref.h"
#include "ui_dialogvnaref.h"

dialogVNARef::dialogVNARef(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogVNARef)
{
  ui->setupUi(this);
}

dialogVNARef::~dialogVNARef()
{
  delete ui;
}
void dialogVNARef::menuVNARef()
{
  /*
[menuVNARef]
    if haltsweep=1 then gosub [FinishSweeping]
    WindowWidth = 150 : WindowHeight = 150
    call GetDialogPlacement 'ver115-1c
    BackgroundColor$="gray" 'ver115-5b
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

    VNAinfoLeft=15 : VNAapplyTop=10
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then
        statictext #VNAref.Apply, "Reference Transmission Measurements To:",VNAinfoLeft,VNAapplyTop+10,120, 35
    else
        statictext #VNAref.Apply, "Reference Reflection Measurements To:",VNAinfoLeft,VNAapplyTop+10,120, 35
    end if
    checkbox #VNAref.Line, "Band Sweep Cal", [VNAapplyLineOn], [VNAapplyLineOff], VNAinfoLeft+5,VNAapplyTop+45, 150, 15
    checkbox #VNAref.BaseLine, "Baseline Cal", [VNAapplyBaseLineOn], [VNAapplyBaseLineOff], VNAinfoLeft+5,VNAapplyTop+65, 150, 15
    checkbox #VNAref.None, "No Reference", [VNAapplyNoneOn], [VNAapplyNoneOff], VNAinfoLeft+5,VNAapplyTop+85, 150, 15

    open "Ref" for dialog_modal as #VNAref  'ver115-1b
    print #VNAref, "trapclose [VNARefFinished]"   'goto [finished] if xit is clicked

    savedesiredCalLevel=desiredCalLevel 'ver114-5L
    if desiredCalLevel=0 then #VNAref.None, "set" : wait
    if desiredCalLevel=2 then #VNAref.Line, "set" : wait
    #VNAref.BaseLine, "set"
    wait
*/
}

void dialogVNARef::VNARefFinished()
{
  /*
[VNARefFinished]
    #VNAref.None, "value? calLevel$"
        'ver115-1b applied same procedure to reflection as for transmission
    if calLevel$="set" then
        desiredCalLevel=0
    else
        #VNAref.BaseLine, "value? calLevel$"
        if calLevel$="set" then desiredCalLevel=1 else desiredCalLevel=2
    end if
        'Implement selected calibration if anything changed
    if savedesiredCalLevel<>desiredCalLevel then    'ver114-5L created if...
        call SignalNoCalInstalled   'ver116-4b
        call RequireRestart
    end if

    close #VNAref
    wait
*/
}

void dialogVNARef::VNAapplyLineOn()
{
  /*
  [VNAapplyLineOn]
      #VNAref.BaseLine, "reset" : #VNAref.None, "reset" :wait
*/
}

void dialogVNARef::VNAapplyLineOff()
{
  /*
  [VNAapplyLineOff]
      #VNAref.Line, "set" : wait
*/
}

void dialogVNARef::VNAapplyBaseLineOn()
{
  /*
  [VNAapplyBaseLineOn]
      #VNAref.Line, "reset" : #VNAref.None, "reset" : wait
      */
}

void dialogVNARef::VNAapplyBaseLineOff()
{/*
  [VNAapplyBaseLineOff]
      #VNAref.BaseLine, "set" : wait
   */
}

void dialogVNARef::VNAapplyNoneOn()
{/*
[VNAapplyNoneOn]
    #VNAref.BaseLine, "reset" : #VNAref.Line, "reset" : wait
*/
}

void dialogVNARef::VNAapplyNoneOff()
{/*
[VNAapplyNoneOff]
    #VNAref.None, "set" : wait
*/
}

