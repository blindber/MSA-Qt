#include "dialogrlc.h"
#include "ui_dialogrlc.h"

dialogRLC::dialogRLC(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogRLC)
{
  ui->setupUi(this);
}

dialogRLC::~dialogRLC()
{
  delete ui;
}
void dialogRLC::RLCDialog()
{/*
    WindowWidth = 450 : WindowHeight = 445  'ver115-4e
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    BackgroundColor$="buttonface"   'colors changed ver115-4j
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"
        'ver115-4e changed instructions.
    staticText #RLCDialog.A1, "Specify type and length of transmission line, if any, and a combined", 10,10,400,19
    staticText #RLCDialog.A2, "Resistor, Inductor and Capacitor as line termination.", 10,30,400,19
    if msaMode$<>"Reflection" then 'ver115-4d
        staticText #RLCDialog.A3, "For the series fixture the RLC values are ignored if a transmission", 10,50,400,19
        staticText #RLCDialog.A4, "line is specified.", 10,70,400,19
    end if

    RLCTop=120  'ver115-4e
    'ver115-1b separated the captions from the checkboxes so the text color comes out right
    groupbox #RLCDialog.group, "RLC",10, RLCTop-20, 220, 170
    checkbox #RLCDialog.Parallel, "", void dialogRLC::RLCParOn(), void dialogRLC::RLCParReset(), 20, RLCTop-5, 20, 19
    staticText #RLCDialog.NamePar, "Parallel", 41, RLCTop-1, 45, 19
    checkbox #RLCDialog.Series, "", void dialogRLC::RLCSerOn(), void dialogRLC::RLCSerReset(), 90, RLCTop-5, 20, 19
    staticText #RLCDialog.NameSer, "Series", 111, RLCTop-1, 45, 19

    checkbox #RLCDialog.selR, "", void dialogRLC::RLCROn(), void dialogRLC::RLCROff(), 30, RLCTop+20, 20, 19
    staticText #RLCDialog.NameR, "Resistance (ohms)", 51, RLCTop+22, 100, 19
    checkbox #RLCDialog.selL, "", void dialogRLC::RLCLOn(), void dialogRLC::RLCLOff(), 30, RLCTop+45, 20, 19
    staticText #RLCDialog.NameL, "Inductance (henries)", 51, RLCTop+47, 100, 19
    checkbox #RLCDialog.selC, "", void dialogRLC::RLCCOn(), void dialogRLC::RLCCOff(), 30, RLCTop+95, 20, 19
    staticText #RLCDialog.NameC, "Capacitance (farads)", 51, RLCTop+97, 100, 19
    textbox #RLCDialog.R, 155, RLCTop+20, 60, 19
    textbox #RLCDialog.L, 155, RLCTop+45, 60, 19
    staticText #RLCDialog.LabelQL, "QL=", 130, RLCTop+72, 22, 19      'ver115-4b
    textbox #RLCDialog.QL, 155, RLCTop+70, 60, 19      'ver115-4b
    textbox #RLCDialog.C, 155, RLCTop+95, 60, 19
    staticText #RLCDialog.LabelQC, "QC=", 130, RLCTop+122, 22, 19      'ver115-4b
    textbox #RLCDialog.QC, 155, RLCTop+120, 60, 19       'ver115-4b

    fixLeft=250
    statictext #RLCDialog.FixR0Label, "R0 (ohms)", fixLeft+45, RLCTop+3, 52,  16
    textbox #RLCDialog.FixR0, fixLeft+103, RLCTop, 50, 20
    groupbox #RLCDialog.FixGroup, "Fixture Type", fixLeft-10, RLCTop-20, 190, 100
    checkbox #RLCDialog.SeriesFix, "Series", void dialogRLC::RLCDialogSetSeries(), void dialogRLC::RLCDialogSetSeries(), fixLeft, RLCTop+25, 60, 20
    checkbox #RLCDialog.ShuntFix, "Shunt", void dialogRLC::RLCDialogSetShunt(), void dialogRLC::RLCDialogSetShunt(), fixLeft, RLCTop+50, 60, 20
    'ver115-4b deleted the delay items

    tranLineTop=RLCTop+155
    staticText #RLCDialog.LabR0, "R0",45,tranLineTop+45,55,15
    staticText #RLCDialog.LabVF, "VF", 105,tranLineTop+45,55,15
    staticText #RLCDialog.LabK1, "K1", 165,tranLineTop+45,55,15
    staticText #RLCDialog.LabK2, "K2", 225,tranLineTop+45,55,15
    staticText #RLCDialog.LabLen, "Len (ft)", 268,tranLineTop+45,55,15
    textbox #RLCDialog.R0, 25, tranLineTop+60, 55, 19
    textbox #RLCDialog.VF, 85, tranLineTop+60, 55, 19
    textbox #RLCDialog.K1, 145, tranLineTop+60, 55, 19
    textbox #RLCDialog.K2, 205, tranLineTop+60, 55, 19
    textbox #RLCDialog.Len, 265, tranLineTop+60, 55, 19

                 'List of Coax choices
    staticText #RLCDialog.A5, "Transmission Line Connection", 25,tranLineTop+8,150,19

    'Copy standard coax names from coaxNames$, but add None at beginning and Custom at end.
    RLCDialogCoaxTypes$(0)="None"
    for i=1 to numCoaxEntries
        aName$=coaxNames$(i) : if aName$="" then aName$="None"  'So blank names don't mess up the sequence
        RLCDialogCoaxTypes$(i)=aName$
    next i
    RLCDialogCoaxTypes$(numCoaxEntries+1)="Custom"
    combobox #RLCDialog.Coax, RLCDialogCoaxTypes$(),void dialogRLC::RLCSelectCoax(),175, tranLineTop+5, 150, 250

         'OK and Cancel buttons
    button #RLCDialog.OK, "OK", void dialogRLC::RLCDialogFinished(),UL, 60, RLCTop+245,80,35  'ver115-4a
    button #RLCDialog.Cancel, "Cancel", void dialogRLC::RLCCancel(),UL, 160, RLCTop+245,80,35  'ver115-4a
    button #RLCDialog.Cancel, "Help", RLCSpecHelp,UL, 260, RLCTop+245,80,35  'ver115-4b

    open "RLC Specification" for dialog_modal as #RLCDialog 'Open preference dialog
    #RLCDialog, "trapclose void dialogRLC::RLCDialogFinished()"

    #RLCDialog.A1, "!font ms_sans_serif 10"
    #RLCDialog.A2, "!font ms_sans_serif 10"

    if msaMode$<>"Reflection" then 'ver115-4e
        #RLCDialog.A3, "!font ms_sans_serif 10"
        #RLCDialog.A4, "!font ms_sans_serif 10"
    end if

    DialogCancelled=0

        'Display existing values
    form$="3,3,4//UseMultiplier"
    resForm$="3,3,4//UseMultiplier//SuppressMilli" 'ver115-4e
    QForm$="######.###"   'ver115-4b
    R$=uFormatted$(DialogRValue, resForm$) 'ver115-4e
    L$=uFormatted$(DialogLValue, form$)
    C$=uFormatted$(DialogCValue, form$)
    QL$=uFormatted$(DialogQLValue, QForm$)   'ver115-4b
    QC$=uFormatted$(DialogQCValue, QForm$)   'ver115-4b
    isErr=CoaxParseSpecs(DialogCoaxSpecs$,cR0,cVF,cK1,cK2,cLen)    'If error, default values are returned
    R0$=uFormatted$(cR0, resForm$)  'ver115-7a
    VF$=using("#.#####", cVF)
    K1$=using("#.#####", cK1)
    K2$=using("#.#####", cK2)
    lenFeet$=uFormatted$(cLen, resForm$)  'ver115-7a
    #RLCDialog.R, R$
    #RLCDialog.L, L$
    #RLCDialog.C, C$
    #RLCDialog.QL, QL$        'ver115-4b
    #RLCDialog.QC, QC$        'ver115-4b
    #RLCDialog.R0, R0$
    #RLCDialog.VF, VF$
    #RLCDialog.K1, K1$
    #RLCDialog.K2, K2$
    #RLCDialog.Len, lenFeet$
    #RLCDialog.selR, "set" : #RLCDialog.selL, "set" : #RLCDialog.selC, "set"
    'R, L and C only if they are meaningful
    if DialogRLCConnect$="S" then
        #RLCDialog.Series, "set"
        #RLCDialog.Parallel, "reset"
        'For series components, zero impedance component means there is no such component,
        'except missing R is set to high impedance if there is no L or C.
        a=DialogRValue : b=DialogLValue : c=DialogCValue
        if DialogRValue>=constMaxValue then #RLCDialog.selR, "reset" : #RLCDialog.R, "!hide"    'ver115-4j
        if DialogRValue=0 and (DialogLValue<>0 or DialogCValue<constMaxValue) then _
                                        #RLCDialog.selR, "reset" : #RLCDialog.R, "!hide" 'ver115-5a
        if DialogLValue=0 then #RLCDialog.selL, "reset" : #RLCDialog.L, "!hide" : _
                                #RLCDialog.QL, "!hide" : #RLCDialog.LabelQL, "!hide" 'ver115-4b
        if DialogCValue>=constMaxValue then #RLCDialog.selC, "reset" : #RLCDialog.C, "!hide" : _
                                #RLCDialog.QC, "!hide" : #RLCDialog.LabelQC, "!hide" 'ver115-4b
    else    'parallel
        #RLCDialog.Parallel, "set"
        #RLCDialog.Series, "reset"
        if DialogRValue>=constMaxValue then #RLCDialog.selR, "reset" : #RLCDialog.R, "!hide"  'ver115-4b
        if DialogLValue>=constMaxValue then #RLCDialog.selL, "reset" : #RLCDialog.L, "!hide": _
                                #RLCDialog.QL, "!hide" : #RLCDialog.LabelQL, "!hide" 'ver115-4b
        if DialogCValue=0 then #RLCDialog.selC, "reset" : #RLCDialog.C, "!hide" : _
                                #RLCDialog.QC, "!hide" : #RLCDialog.LabelQC, "!hide" 'ver115-4b
    end if
    if DialogCoaxName$="" then DialogCoaxName$="None"
    cName$=DialogCoaxName$
        'Select current coax in combobox
        'Selection of combobox by text never works, so look up the index
    sel=0
    for i=0 to numCoaxEntries+1 'two extra for None at beginning and Custom at end
        thisName$=RLCDialogCoaxTypes$(i)
        if cName$=thisName$ then sel=i+1 : exit for  '0 in array is 1 in combobox
    next i
    #RLCDialog.Coax, "selectindex ";sel
    #RLCDialog.Coax, "setfocus"

    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then
        'We only do fixture type for Transmission mode
        #RLCDialog.FixR0, S21JigR0
        if S21JigAttach$="Series" then
            #RLCDialog.SeriesFix, "set" : #RLCDialog.ShuntFix, "reset"
        else
            #RLCDialog.SeriesFix, "reset" : #RLCDialog.ShuntFix, "set"
        end if
    else
        #RLCDialog.FixR0, "!hide"
        #RLCDialog.FixR0Label, "!hide"
        #RLCDialog.SeriesFix, "hide"
        #RLCDialog.ShuntFix, "hide"
        #RLCDialog.FixGroup, "!hide"
    end if
    #RLCDialog.Len, "!setfocus" 'to get focus off combobox
    gosub void dialogRLC::RLCAdjustItemsToCoax()
    'if msaMode$<>"Reflection" and S21JigAttach$="Series" then goto void dialogRLC::RLCDialogSetSeries() 'To hide termination items ver115-4d delver115-4e
    wait
*/
}
void dialogRLC::RLCDialogSetSeries()
{/*
    #RLCDialog.SeriesFix, "set"
    #RLCDialog.ShuntFix, "reset"
        'We can have a coax in series fixture, but we can't have a termination
    'call RLCSetTermStatus "hide" 'hide termination items  ver115-4d    'delver115-4e
    wait
*/
}
void dialogRLC::RLCDialogSetShunt()
{/*
    #RLCDialog.SeriesFix, "reset"
    #RLCDialog.ShuntFix, "set"
    'call RLCSetTermStatus "show" 'show termination items ver115-4d   'delver115-4e
    wait
*/
}
void dialogRLC::RLCCancel()
{/*
    DialogCancelled=1
    close #RLCDialog
    exit sub
*/
}
void dialogRLC::RLCParOn()
{/*
   #RLCDialog.Parallel, "set"
   #RLCDialog.Series, "reset"
   wait
*/
}
void dialogRLC::RLCParReset()
{/*
   #RLCDialog.Parallel, "set"   'Prevents reset
   wait
*/
}
void dialogRLC::RLCSerOn()
{/*
   #RLCDialog.Series, "set"
   #RLCDialog.Parallel, "reset"
   wait
*/
}
void dialogRLC::RLCSerReset()
{/*
   #RLCDialog.Series, "set"   'Prevents reset
   wait
*/
}
void dialogRLC::RLCROn()
{/*
    #RLCDialog.R, "!show"
    wait
*/
}
void dialogRLC::RLCROff()
{/*
    #RLCDialog.R, "!hide"
    wait
*/
}
void dialogRLC::RLCLOn()
{/*
    #RLCDialog.L, "!show"
    #RLCDialog.QL, "!show" : #RLCDialog.LabelQL, "!show" 'ver115-4b
    wait
*/
}
void dialogRLC::RLCLOff()
{/*
    #RLCDialog.L, "!hide"
    #RLCDialog.QL, "!hide" : #RLCDialog.LabelQL, "!hide" 'ver115-4b
    wait
*/
}
void dialogRLC::RLCCOn()
{/*
    #RLCDialog.C, "!show"
    #RLCDialog.QC, "!show" : #RLCDialog.LabelQC, "!show" 'ver115-4b
    wait
*/
}
void dialogRLC::RLCCOff()
{/*
    #RLCDialog.C, "!hide"
    #RLCDialog.QC, "!hide" : #RLCDialog.LabelQC, "!hide" 'ver115-4b
    wait
*/
}
void dialogRLC::RLCAdjustItemsToCoax()
{/*'disable/enable, hide/show and fill in what needs to be done for coax cName$
    if cName$="None" or cName$="" then
        cName$="None"
        #RLCDialog.R0, "50"
        #RLCDialog.VF, "1"
        #RLCDialog.K1, "0"
        #RLCDialog.K2, "0"
        #RLCDialog.Len, "0"
        stat$="!hide"
    else
        if cName$<>"Custom" then    'For custom, we don't change the existing data
            call CoaxGetData cName$, cR0, cVF, cK1, cK2
            #RLCDialog.R0, cR0 : #RLCDialog.R0, "!disable"
            #RLCDialog.VF, cVF : #RLCDialog.VF, "!disable"
            #RLCDialog.K1, cK1 : #RLCDialog.K1, "!disable"
            #RLCDialog.K2, cK2 : #RLCDialog.K2, "!disable"
        else
            #RLCDialog.R0, "!enable"
            #RLCDialog.VF, "!enable"
            #RLCDialog.K1, "!enable"
            #RLCDialog.K2, "!enable"
        end if
        stat$="!show"
    end if
    #RLCDialog.LabR0, stat$
    #RLCDialog.LabVF, stat$
    #RLCDialog.LabK1, stat$
    #RLCDialog.LabK2, stat$
    #RLCDialog.LabLen, stat$
    #RLCDialog.R0, stat$
    #RLCDialog.VF, stat$
    #RLCDialog.K1, stat$
    #RLCDialog.K2, stat$
    #RLCDialog.Len, stat$
    return
*/
}
void dialogRLC::RLCSelectCoax()
{/*'Coax type was selected
    #RLCDialog.Coax, "selectionIndex? sel"
    if sel=0 then cName$="None" else cName$=RLCDialogCoaxTypes$(sel-1)    'Get name from the array that filled #RLCDialog.Coax
    gosub void dialogRLC::RLCAdjustItemsToCoax()
    wait
*/
}
void dialogRLC::RLCDialogFinished()
{/*
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then
        'We only do fixture info for Transmission mode
        #RLCDialog.FixR0, "!contents? s$"
        v=uValWithMult(s$)
        if v<=0 then notice "Invalid Fixture R0; 50 ohms used." : v=50
        S21JigR0=v

        #RLCDialog.SeriesFix, "value? s$"
        if s$="set" then S21JigAttach$="Series" else S21JigAttach$="Shunt"
    end if

    DialogCoaxName$=cName$   'cName$ was set when selection was made
        'Retrieve and check coax data
    #RLCDialog.R0, "!contents? s$"
    cR0=uValWithMult(s$)  'R0
    if cR0<=0 then notice "Invalid coax R0" : wait
    #RLCDialog.VF, "!contents? s$"
    cVF=val(uCompact$(s$)) 'velocity Factor
    if cVF<=0 or cVF>1 then notice "Invalid coax VF" : wait
    #RLCDialog.K1, "!contents? s$"
    cK1=val(uCompact$(s$)) 'K1--conductor loss factor
    if cK1<0 or cK1>=10 then notice "Invalid coax K1" : wait
    #RLCDialog.K2, "!contents? s$"
    cK2=val(uCompact$(s$)) 'K2--dielectric loss factor
    if cK2<0 or cK2>=10 then notice "Invalid coax K2" : wait
    #RLCDialog.Len, "!contents? s$"
    cLen=uValWithMult(uCompact$(s$)) 'length in feet
    if cLen<0 then notice "Invalid coax length" : wait
    if cLen>50000 then notice "Coax length may not exceed 50 k feet" : wait 'big value creates overflow ver115-4d
        'Use blank coax specs if no coax is used,
        'as a signal for Series Fixture in transmission mode that the RLC values are to be used.
    if cLen=0 or cName$="None" then 'ver115-5d
        DialogCoaxSpecs$=""
    else
        DialogCoaxSpecs$=CoaxSpecs$(cR0,cVF,cK1,cK2,cLen)
    end if

    #RLCDialog.Parallel, "value? s$"
    if s$="reset" then DialogRLCConnect$="S" else DialogRLCConnect$="P"
        'Get component values. For shunt fixture, these are deemed the termination. For series
        'fixture, any RLC combo is in series but is ignored if there is a transmission line. To indicate
        'no transmission line for this purpose, we made the spec blank.
    #RLCDialog.selR, "value? s$"
    haveR=(s$="set")
    #RLCDialog.selL, "value? s$"
    haveL=(s$="set")
    #RLCDialog.selC, "value? s$"
    haveC=(s$="set")
        'ver115-5a rearranged the following
    if haveR then
        #RLCDialog.R, "!contents? s$"
        DialogRValue=uValWithMult(s$) : if DialogRValue<0 then DialogRValue=0
    else
        'if no R for parallel, use large value
        if DialogRLCConnect$="P" then
            DialogRValue=constMaxValue  'ver115-5a
        else
            'if no R for series, use 0 if L or C are specified; otherwise use large value
            if (haveL or haveC) then DialogRValue=0 else DialogRValue=constMaxValue  'ver115-5a
        end if
    end if

    if haveL then
        #RLCDialog.L, "!contents? s$"
        DialogLValue=uValWithMult(s$) : if DialogLValue<0 then DialogLValue=0
        #RLCDialog.QL, "!contents? s$"    'ver115-4b
        DialogQLValue=uValWithMult(s$) : if DialogQLValue<=0 then DialogQLValue=0.001   'ver115-4b
        if DialogQLValue>100000 then DialogQLValue=100000   'max of 1e5 so it can display properly ver115-5f
    else
        if DialogRLCConnect$="P" then DialogLValue=constMaxValue else DialogLValue=0
        DialogQLValue=1e4   'ver115-4b
    end if

    if haveC then
        #RLCDialog.C, "!contents? s$"
        DialogCValue=uValWithMult(s$) : if DialogCValue<0 then DialogCValue=0
        #RLCDialog.QC, "!contents? s$"    'ver115-4b
        DialogQCValue=uValWithMult(s$) : if DialogQCValue<=0 then DialogQCValue=0.001   'ver115-4b
        if DialogQCValue>100000 then DialogQCValue=100000   'max of 1e5 so it can display properly ver115-5f
    else
        'if no C, use large value for series and 0 for parallel
        if DialogRLCConnect$="P" then DialogCValue=0 else DialogCValue=constMaxValue
        DialogQCValue=1e4   'ver115-4b
    end if

    close #RLCDialog

end sub
*/
}
void dialogRLC::RLCSetTermStatus(QString stat)
{/*'Set status of termination items to show or hide ver115-4d

    bangStat$="!";stat$
    #RLCDialog.selR, "value? s$"
    if s$="reset" then #RLCDialog.R, "!hide" else #RLCDialog.R, bangStat$
    #RLCDialog.selL, "value? s$"
    if s$="reset" then
        #RLCDialog.L, "!hide" : #RLCDialog.QL, "!hide" : #RLCDialog.LabelQL, "!hide"
    else
        #RLCDialog.L, bangStat$ : #RLCDialog.QL, bangStat$ : #RLCDialog.LabelQL, bangStat$
    end if
    #RLCDialog.selC, "value? s$"
    if s$="reset" then
        #RLCDialog.C, "!hide" : #RLCDialog.QC, "!hide" : #RLCDialog.LabelQC, "!hide"
    else
        #RLCDialog.C, bangStat$ : #RLCDialog.QC, bangStat$ : #RLCDialog.LabelQC, bangStat$
    end if
    #RLCDialog.NameR, bangStat$ : #RLCDialog.NameL, bangStat$ : #RLCDialog.NameC, bangStat$
    #RLCDialog.selR, stat$ : #RLCDialog.selL, stat$ : #RLCDialog.selC, stat$
    #RLCDialog.group, bangStat$
    #RLCDialog.NamePar, bangStat$ : #RLCDialog.NameSer, bangStat$
    #RLCDialog.Parallel, stat$ : #RLCDialog.Series, stat$
end sub
*/
}
void dialogRLC::RLCSpecHelp(QString h)
{/*'Display help info for RLC spec dialog
    WindowWidth = 600
    if msaMode$="Reflection" then WindowHeight = 300 else WindowHeight = 375
    call GetDialogPlacement 'determine position on screen
    BackgroundColor$="gray"
    ForegroundColor$="black"

    s$="The RLC spec dialog lets you specify the characteristics of a circuit consisting of a resistor, inductor"
    s$=s$;" and capacitor. One or two of those components can be omitted. You may specify the RLC values, and for"
    s$=s$;" the inductor and capacitor you may specify the Q value. Low Q values represent a loss in the component"
    s$=s$;" that in effect puts a resistor in series with the component, whose resistance changes with frequency."
    s$=s$;" You also specify whether the RLC components are in series or in parallel with each other."
    statictext #RLCSpecHelp, s$, 10, 10, 570, 80

    s$="The RLC circuit may be simulated as though it is attached via a transmission line, so the RLC combination"
    s$=s$;" becomes the termination of the transmission line. You specify the length of the transmission"
    s$=s$;" line in feet, which may be zero. You may select a type of coaxial cable,"
    s$=s$;" or you may select Custom and enter your own parameters, which incude characteristic impedance (R0), velocity"
    s$=s$;" factor, conductor loss (K1), and dielectric loss (K2). The K loss factors are specified in accordance with the"
    s$=s$;" equation:"
    statictext #RLCSpecHelp, s$, 10, 110, 570, 105

    s$="   Matched Loss (dB per hundred feet)=K1*sqrt(F)+K2*F, where F is the frequency in MHz."
    statictext #RLCSpecHelp, s$, 10, 210, 570, 20

    if msaMode$<>"Reflection" then  'Reflection doesn't care what the fixture is
        s$="You must specify whether the fixture is series or shunt, and its R0."
        s$=s$;" For the shunt fixture, you may enter the time delay of the connection between the actual fixture and"
        s$=s$;" the components; typically on the order of 0.125 ns per inch. For the series fixture, if the"
        s$=s$;" transmission line is used the RLC components are ignored."
        statictext #RLCSpecHelp, s$, 10, 240, 570, 65
    end if

    open "RLC Analysis Help" for dialog_modal as #RLCSpecHelp
    print #RLCSpecHelp, "font ms_sans_serif 10"
    print #RLCSpecHelp, "trapclose void dialogRLC::RLCSpecHelpEnd()"

    wait
*/
}
 void dialogRLC::RLCSpecHelpEnd()
 {/*
    close #RLCSpecHelp
    exit sub

end sub
*/
}
