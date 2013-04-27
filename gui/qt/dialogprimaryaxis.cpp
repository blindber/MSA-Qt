#include "dialogprimaryaxis.h"
#include "ui_dialogprimaryaxis.h"

dialogPrimaryAxis::dialogPrimaryAxis(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogPrimaryAxis)
{
  ui->setupUi(this);
}

dialogPrimaryAxis::~dialogPrimaryAxis()
{
  delete ui;
}
void dialogPrimaryAxis::menuPrimaryAxis()
{/*
[menuPrimaryAxis]
    if haltsweep=0 then gosub [FinishSweeping]
    if ChoosePrimaryAxis()=1 then   'Returns 0 if cancelled ver115-3c
        call gUsePresetColors gGetLastPresetColors$()  'Reselect same appearance in case primary axis change had effect.
        call SetCycleColors 'ver116-4s
        select case msaMode$
            case "Reflection"
                goto [RestartReflectionMode]
            case "ScalarTrans", "VectorTrans"
                goto [RestartTransmissionMode]
            case else   ' "SA"
                if gentrk=1 then goto [RestartSATGmode] else goto [RestartPlainSAmode] 'ver115-4f
        end select
    end if
    wait
*/
}

void dialogPrimaryAxis::ChoosePrimaryAxis()
{/*
function ChoosePrimaryAxis()   'Lets user choose axis 1 or 2 as the primary axis. Return 0 if cancelled; otherwise 1
    WindowWidth = 465
    WindowHeight = 300
    UpperLeftX=100
    UpperLeftY=100
    ForegroundColor$="black"
    BackgroundColor$="buttonface"
    statictext #primaryaxis, "Your may choose either the left axis (axis Y1) or the right axis (axis Y2)",   5,   5, 425,  20
    statictext #primaryaxis, "to be the primary axis. When starting up or changing modes, magnitude",   5,  25, 430,  20
    statictext #primaryaxis, "will be graphed on the primary axis. Certain analyses, such as Filter",   5,  45, 415,  20
    statictext #primaryaxis, "and Crystal Analysis, require dB to be graphed on the primary axis.",   5,  65, 415,  20
        statictext #primaryaxis, "You must Restart after changing the primary axis.",   5,  85, 415,  20
    checkbox #primaryaxis.Y1, "Left Axis (Y1)", [primarySelY1], [primarySelY1], 145, 147, 101,  25
    checkbox #primaryaxis.Y2, "Right Axis (Y2)", [primarySelY2], [primarySelY2], 145, 177, 111,  25
    button #primaryaxis.Done,"Done",[primaryDone], UL,  35, 217, 105,  35
    button #primaryaxis.Cancel,"Cancel",[primaryCancel], UL, 245, 217, 100,  35
    statictext #primaryaxis.statictext9, "Choose Primary Axis", 130, 122, 124,  20

    open "Primary Axis" for dialog_modal as #primaryaxis
    print #primaryaxis, "font ms_sans_serif 10"
    print #primaryaxis, "trapclose [primaryDone]"
    if primaryAxisNum=1 then goto [primarySelY1] else goto [primarySelY2]
    wait
*/
}

void dialogPrimaryAxis::primarySelY1()
{/*
[primarySelY1]   'Perform action for the checkbox named 'Y1'
    #primaryaxis.Y1, "set" : #primaryaxis.Y2, "reset"
    wait
*/
}

void dialogPrimaryAxis::primarySelY2()
{/*
[primarySelY2]   'Perform action for the checkbox named 'Y2'
    #primaryaxis.Y2, "set" : #primaryaxis.Y1, "reset"
    wait
*/
}

void dialogPrimaryAxis::primaryDone()
{/*
[primaryDone]   'Perform action for the button named 'Done'
    #primaryaxis.Y1, "value? primaryY1Val$"
    if primaryY1Val$="set" then primaryAxisNum=1 else primaryAxisNum=2
    call gSetPrimaryAxis primaryAxisNum 'ver115-3c
    lastCol$=gGetLastPresetColors$
    if lastCol$="DARK" or lastCol$="LIGHT" then _
            call gUsePresetColors lastCol$    'Reset colors; may be affected by primary axis change ver115-3c
            call gridappearance.gGetSupplementalTraceColors referenceColor1$, referenceColor2$, dum1$, dum2$   'ver116-4b
            if primaryAxisNum=1 then referenceColorSmith$=referenceColor1$ else referenceColorSmith$=referenceColor2$  'ver116-4b
            call SetCycleColors 'ver116-4s
    close #primaryaxis
    ChoosePrimaryAxis=1
    exit function
*/
}

void dialogPrimaryAxis::primaryCancel()
{/*
[primaryCancel]   'Perform action for the button named 'Cancel'
    close #primaryaxis
    ChoosePrimaryAxis=0
    exit function
end function
*/
}
