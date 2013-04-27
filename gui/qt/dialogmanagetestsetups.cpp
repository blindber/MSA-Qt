#include "dialogmanagetestsetups.h"
#include "ui_dialogmanagetestsetups.h"

dialogManageTestSetups::dialogManageTestSetups(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogManageTestSetups)
{
  ui->setupUi(this);
}

dialogManageTestSetups::~dialogManageTestSetups()
{
  delete ui;
}
void dialogManageTestSetups::ManageTestSetups()
{/*
[ManageTestSetups] 'Menu item to load/save test setups
    WindowWidth = 575 : WindowHeight = 435
    UpperLeftX=int((DisplayWidth-WindowWidth)/2) : UpperLeftY=int((DisplayHeight-WindowHeight)/2)
    ForegroundColor$="black" : BackgroundColor$="buttonface" : ListboxColor$ = "white"
    if haltsweep then gosub [FinishSweeping]
    setupTotalNum=10
    for i=0 to setupTotalNum-1 : setupList$="" : next i
    listbox #testsetup.List, setupList$(), [setupListClick],  105,  92, 355, 170
    statictext #testsetup, "To save a test setup consisting of the current sweep settings and calibration data,",  20,  12, 500,  20
    statictext #testsetup, "select a slot, change the name if desired, and click Save.",  20,  32, 450,  20
    statictext #testsetup, "To load a test setup, select it and click Load.",  20,  57, 450,  20
    TextboxColor$ = "white"
    textbox #testsetup.Name, 140, 307, 300,  25
    button #testsetup.Create,"Create Name",[setupCreateName], UL,  445, 300, 100,  30
    button #testsetup.Save,"Save",[setupSave], UL,  40, 352, 100,  40
    button #testsetup.Load,"Load",[setupLoad], UL, 160, 352, 100,  40
    button #testsetup.Delete,"Delete Setup",[setupDelete], UL, 280, 352, 100,  40

    button #testsetup.Done,"Done",[setupDone], UL, 400, 352, 100,  40
    statictext #testsetup, "Name:",  90, 307,  40,  20

    '-----End GUI objects code

    open "Test Setups" for dialog_modal as #testsetup
    print #testsetup, "font ms_sans_serif 10"
    print #testsetup, "trapclose [setupDone]"
    #testsetup.List, "singleclickselect"
    gosub [setupFillList]
    setupCurrentSelection=-1    'start with none selected
    #testsetup.Load, "!disable" : #testsetup.Delete, "!disable" : #testsetup.Save, "!disable"   'Can't act without selection
    setupHaveDonePartialRestart=0   'see setupSave
   wait
*/
}

void dialogManageTestSetups::setupSave()
{/*
[setupSave]   'Save button
       'We do a restart but return before taking any data. This makes sure any changes the user has
    'made before coming here, such as frequency range, are actually implemented so they can be saved properly.But this only
    'needs to be done once while in this dialog.
    #testsetup.Load, "!disable" : #testsetup.Delete, "!disable" : #testsetup.Save, "!disable" : #testsetup.Done, "!disable"
    if setupHaveDonePartialRestart=0 then
        setupHaveDonePartialRestart=1
        gosub [PartialRestart] 'ver115-3c
    end if

    fHndl$=SetupOpenFile$(setupCurrentSelection, 0)  '0 means output
    if fHndl$="" then
        notice "Error in opening file"
        #testsetup.Save, "!enable" : #testsetup.Done, "!enable"
        wait
    end if
    'We save the file description as line 1, preceded by a "!"
    #testsetup.Name, "!contents? setupName$"
    newLine$=chr$(13)
    print #fHndl$, "!";setupName$

    'We then save the necessary info as contexts.
    'First, the sweep context. We don't start with StartContext because we know what the file starts with
    print #fHndl$, TraceContext$();newLine$;"EndContext";newLine$;"StartContext Sweep"; newLine$;SweepContext$();newLine$;"EndContext" 'ver115-4a

        'Next any applicable Band cal file.
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then
        if BandLineCalIsCurrent() then call BandLineCalContextToFile fHndl$
    end if
    if msaMode$="Reflection" then
        if BandOSLCalIsCurrent() then call OSLCalContextToFile fHndl$, 1    '1 means band cal
    end if
    close #fHndl$ : fHndl$=""
    setupList$(setupCurrentSelection-1)=setupCurrentSelection;". ";setupName$
    #testsetup.List, "Reload"
    #testsetup.Load, "!enable" : #testsetup.Delete, "!enable" : #testsetup.Save, "!enable" : #testsetup.Done, "!enable"
    wait
*/
}

void dialogManageTestSetups::setupLoad()
{/*
[setupLoad]   'Load button
    #testsetup.Load, "!disable" : #testsetup.Delete, "!disable" : #testsetup.Save, "!disable" : #testsetup.Done, "!disable"
    fHndl$=SetupOpenFile$(setupCurrentSelection, 1)  '1 means input
    if fHndl$="" then
        notice "Error in opening file"
        #testsetup.Load, "!enable" : #testsetup.Delete, "!enable" : #testsetup.Save, "!enable" : #testsetup.Done, "!enable"
        wait
    end if
    joint$=""
    trace$="" : setupIsFirstLine=1
    while EOF(#fHndl$)=0    'Assemble the trace context lines into a single string
        Line Input #fHndl$, s1$
        if Upper$(Trim$(s1$))="ENDCONTEXT" then exit while
        if setupIsFirstLine=0 then 'Skip first line, which is the descriptive file name
            trace$=trace$;joint$;s1$
            joint$=chr$(13)
        end if
        setupIsFirstLine=0
    wend
    joint$=""
    restoreContext$="" :setupIsFirstLine=1
    while EOF(#fHndl$)=0    'Assemble the sweep context lines into a single string for [RestoreSweepContext]
        Line Input #fHndl$, s1$
        if Upper$(Trim$(s1$))="ENDCONTEXT" then exit while
        if setupIsFirstLine=0 then 'Skip first line, which is StartContext Sweep
            restoreContext$=restoreContext$;joint$;s1$
            joint$=chr$(13)
        end if
        setupIsFirstLine=0
    wend

    call RememberState   'So we can detect any change ver116-4d
    restoreIsValidation=0   'Indicates to actually restore data
    traceStart=1
    restoreErr$=RestoreTraceContext$(trace$,traceStart,0)    'Actual trace restoration
    if restoreErr$="" then gosub [RestoreSweepContext]     'Actual sweep restoration
    gosub [DetectFullChanges]    'ver116-4d

    if restoreErr$<>"" then
        notice "Error in loading context: ";restoreErr$
    else
        setupNumCalPoints=0
        if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then setupNumCalPoints=GetBandLineCalContextFromFile(fHndl$)
        if msaMode$="Reflection" then setupNumCalPoints=OSLGetCalContextFromFile(fHndl$, 1)    '1 means band cal
        if setupNumCalPoints>0 then desiredCalLevel=2   'So band cal gets used
    end if
    close #fHndl$ : fHndl$=""
    close #testsetup
        'We do a restart but return before taking any data. This makes sure everything is implemented
        'consistently, in case we end up resaving the data we just loaded.
    gosub [PartialRestart] 'ver115-3c
    setupHaveDonePartialRestart=1   'In case of Save, we don't need to do the partial restart again
    'call RequireRestart    'delver115-2a
    wait
*/
}

void dialogManageTestSetups::setupDelete()
{/*
[setupDelete]   'Delete button
    fName$=DefaultDir$;"\MSA_Info\TestSetups\TestSetup";setupCurrentSelection;".txt"
    call uDeleteFile fName$ 'Delete the file
    setupList$(setupCurrentSelection-1)=setupCurrentSelection;". Empty"
    #testsetup.List, "Reload"
    #testsetup.Load, "!disable" : #testsetup.Delete, "!disable" 'No existing file; can't load or delete
    wait
*/
}

void dialogManageTestSetups::setupDone()
{/*
[setupDone]   'Done button or dialog closed
    #testsetup.Done, "!disable" 'Prevents double click which causes trouble in debugger
    close #testsetup
    wait
*/
}

void dialogManageTestSetups::setupFillList()
{/*
[setupFillList]     'fill setupList$ with names of existing files
    'The files are in a folder called TestSetups in the MSA_Info folder
    setupPath$=DefaultDir$;"\MSA_Info\TestSetups"
    files DefaultDir$;"\MSA_Info", "", fileInfo$()  'get directory list
    nFolders=val(fileInfo$(0,1))
    folderFound=0
    for i=1 to nFolders
        if fileInfo$(i,1)="TestSetups" then folderFound=1 : exit for    'We found the TestSetups folder
    next i
    if folderFound=0 then
        if mkDir(setupPath$)<>0 then notice "Error creating Setup folder.": return  'Folder doesn't exist, so make one
    end if
    for i=1 to setupTotalNum  'Check for each of the setupTotalNum possible files
        'File exists; we need to get the descriptive name
        if SetupOpenFile$(i,1)="" then
            setupList$(i-1)=i;". Empty"
        else
            'File is open. Get its description and close it
            setupName$=SetupGetDescription$("#setupFile") 'Get description of file #setupFile
            setupList$(i-1)=i;". ";setupName$
            close #setupFile
        end if
    next i
    #testsetup.List, "Reload"   'transfer array info to the visible list
    return
*/
}

void dialogManageTestSetups::setupListClick()
{/*
[setupListClick]    'User clicked an item on the list
    'Enter name of selected item into name box.
    #testsetup.List, "selectionIndex? setupCurrentSelection"
    setupFullName$=setupList$(setupCurrentSelection-1)   'setupCurrentSelection starts with one; array starts with 0
    dotPos=instr(setupFullName$, ".")   'Name starts with number then period. Find the period
    setupShortName$=Mid$(setupFullName$,dotPos+2) 'omit the number, the dot, and the space after the dot.
    #testsetup.Name, setupShortName$
    #testsetup.Save, "!enable"
    if setupShortName$="Empty" then
        #testsetup.Load, "!disable" : #testsetup.Delete, "!disable" 'No existing file; can't load or delete
        #testsetup.Name, SetupName$()   'Print useful name, not "Empty"
    else
        #testsetup.Load, "!enable" : #testsetup.Delete, "!enable"   'file exists
    end if
    wait
*/
}

void dialogManageTestSetups::setupCreateName()
{/*
[setupCreateName]   'Create name for current setup; put it in name box
    #testsetup.Name, SetupName$()
    wait
*/
}

QString dialogManageTestSetups::SetupName()
{/*
  //Return descriptive name for current setup
    select msaMode$
        case "SA"
            s$="SA/"
        case "ScalarTrans"
            s$="SA/TG"
        case "VectorTrans"
            s$="VNA Trans/"
        case else           ' "Reflection"
            s$="VNA Reflect/"
    end select
    if gGetXIsLinear() then s$=s$;"Linear/" else s$=s$;"Log/"
    SetupName$=s$;startfreq;" to ";endfreq;"/";path$
*/
  return "fix me";
}

void dialogManageTestSetups::SetupOpenFile()
{/*
function SetupOpenFile$(N, isInput)  'Open setup file; return its handle as text or blank if error
    'If isInput, open for input, otherwise for output
    fName$=DefaultDir$;"\MSA_Info\TestSetups\TestSetup";N;".txt"   'Name of file N
    On Error goto [noFile]
    if isInput then open fName$ for input as #setupFile else open fName$ for output as #setupFile
    SetupOpenFile$="#setupFile"
    exit function
[noFile]
    SetupOpenFile$="" 'ver114-2f
end function
*/
}

void dialogManageTestSetups::SetupGetDescription()
{/*
function SetupGetDescription$(fHndl$) 'Get description of open file whose handle is in fHndl$
    'File exists; we need to get the descriptive name
    'The description is the first line of the file, but includes a leading "!" which we delete here
    SetupGetDescription$=""
    if EOF(#fHndl$) then notice "Error getting Setup file description" : SetupGetDescription$="" : exit function
    Line Input #fHndl$, tLine$
    SetupGetDescription$=Mid$(tLine$, 2)    'Everything except the exclamation.
end function
*/
}
