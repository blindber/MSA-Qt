#include "dialogcalpdm.h"
#include "ui_dialogcalpdm.h"

dialogCalPDM::dialogCalPDM(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogCalPDM)
{
  ui->setupUi(this);
}

dialogCalPDM::~dialogCalPDM()
{
  delete ui;
}
void dialogCalPDM::menuCalPDM()
{/*
[menuCalPDM]
    if haltsweep=1 then gosub [FinishSweeping]
    WindowWidth = 500 : WindowHeight = 325
    UpperLeftX = INT((DisplayWidth-WindowWidth)/2)
    UpperLeftY = 75 'ver115-1a
    BackgroundColor$="buttonface"   'ver116-4L changed color
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

    s$="The actual phase shift caused by PDM inversion will differ from the theoretical"
    s$=s$;" 180 degrees. A one-time calibration is required to determine the actual phase shift."
    s$=s$;" This value will be used internally, and you will not directly need to know or use the value."
    s$=s$;" To perform this calibration you first need to do the following, which will require that"
    s$=s$;" you close this window and return to the Graph Window:"
    statictext #PDMcal.t1, s$,15,15,425, 100
    statictext #PDMcal.t2, "*Set Video Filter to NARROW bandwidth.",25,112,400, 16
    statictext #PDMcal.t3, "*Connect Tracking Generator output to MSA input with 1-2 foot cable.",25,129,425, 16
    statictext #PDMcal.t4, "*In menu Operating Cal->Transmission, set Transmission Reference to No Reference.",25,146,475, 16   'ver114-5L'ver114-5n
    statictext #PDMcal.t5, "*Sweeping 0-200 MHz, find a frequency with a phase shift near 90 or 270 deg.",25,163,475, 16
    statictext #PDMcal.t6, "*Center the sweep at that frequency, with zero sweep width.",25,180,425, 16
    statictext #PDMcal.t7, "*Return to this window and click the PDM Inversion Cal button.",25,197,425, 16

    button #PDMcal.PDM, "PDM Inversion Cal",[DoPDMCal], UL, 150, 220, 120,20
    statictext #PDMcal.PDMval, "Current Inversion=xxx deg.",140,240,170, 16

    button #PDMcal.Done, "Save New Value and Quit",[PDMCalFinished], UL, 60, 265, 160,20
    button #PDMcal.Cancel, "Cancel",[PDMCalCancel], UL, 250, 265, 95,20

    open "PDM Calibration" for dialog_modal as #PDMcal  'ver114-3g
    print #PDMcal, "trapclose [PDMCalWait]"

    #PDMcal, "font ms_sans_serif 9" 'ver116-4L
    #PDMcal.t1, "!font ms_sans_serif 10"   'ver116-4L
    #PDMcal.PDMval, "Current Inversion=";invdeg;" deg."
    doingPDMCal=0
    CalInvDeg=360     'Will change if cal is done ver114-5L
    wait
*/
}

void dialogCalPDM::PDMCalWait()
{
  /*
[PDMCalWait]
    wait*/
}

void dialogCalPDM::PDMCalCancel()
{
  /*
[PDMCalCancel]
    close #PDMcal
    wait
*/
}

void dialogCalPDM::PDMCalFinished()
{/*
[PDMCalFinished]
    if doingPDMCal=1 then wait 'Don't allow quit in middle of cal
    if CalInvDeg<>360 then invdeg=CalInvDeg 'ver114-5L use calibrated value if cal was done
    globalPort=port
    call configSaveFile
    close #PDMcal
    wait
*/
}

void dialogCalPDM::DoPDMCal()
{/*
[DoPDMCal]  'Run PDM calibration and display resulting invdeg
    if doingPDMCal=1 then goto [PostScan]  'continue on; don't stop ver114-5L
    doingPDMCal=1   'ver114-5L
    #PDMcal.PDMval, "Current Inversion= "
    #PDMcal.Done, "!disable" : #PDMcal.Cancel, "!disable"
    #PDMcal.PDM, "Be Patient"
    gosub [CalPDMinvdeg]
    #PDMcal.Done, "!enable" : #PDMcal.Cancel, "!enable"
    #PDMcal.PDMval, "Current Inversion= "; CalInvDeg ;" deg."   'Calibration result ver114-5L
    #PDMcal.PDM, "PDM Inversion Cal"
    doingPDMCal=0  'ver114-5L
    wait
*/
}

