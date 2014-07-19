#include "debugmodule.h"
#include <QMessageBox>

DebugModule::DebugModule()
{
}

void DebugModule::setDefaultDir(QString dir)
{
  DefaultDir = dir;
}
//================================START DEBUG MODULE=======================================
//This module has routines to save/reload info to allow one person to partially recreate
//the state of another person//s MSA to help in debugging. Saved context files can be reloaded, and
//the copied MSA_Info can be examined or used as needed.

void DebugModule::DebugSaveData()
{
  QString folder=DefaultDir+"/xxDebug";
  if (!QDir().mkpath(folder))
  {
    QMessageBox::warning(0, "Error", "Cannot create Debug folder.");
    return;
  }
  DebugSaveArrays(folder);    //Save various debug arrays as individual files
  /*
      //Save preferences file
  for i=0 to 30 : contextTypes(i)=0: next i
  contextTypes(constGrid)=1   //Grid
  contextTypes(constTrace)=1   //Trace
  contextTypes(constSweep)=1   //Sweep
  errMsg$=SaveContextFile$(folder$;"\Prefs.txt")
  if errMsg$<>"" then notice "Unable to save preferences."
      //Save configuration file
  open folder$;"\config.txt" for output as #configOut
  print #configOut, configHardwareContext$()      //ver114-3h
  close #configOut

  //call DebugCopyFile DefaultDir$;"\MSA_Info\config.txt", DefaultDir$;"\xxDebug\config.txt"
  call DebugCopyDirectory DefaultDir$, "MSA_Info", DefaultDir$;"\xxDebug"
end sub
*/
}
void DebugModule::DebugCopyFile(QString source, QString dest)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*    open source$ for input as #copySource
    open dest$ for output as #copyDest
    print #copyDest, input$(#copySource, lof(#copySource));
    close #copySource
    close #copyDest
end sub
*/
}
void DebugModule::DebugCopyDirectory(QString sourcePath, QString dirName, QString destPath)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*   //Copy the directory in path sourcePath$ whose name is dirName$, into the path destPath$
    //These path names do not end with slashes.
    dest$=destPath$;"\";dirName$
    isErr=mkdir(dest$)
    source$=sourcePath$;"\";dirName$
    files source$, "*", fileInfo$()  //get list of all files and directories
        //copy the files
    numFiles=val(fileInfo$(0,0))    //0,0 has the number of files, which are listed first, from 1,x to numFiles,x
    numFolders=val(fileInfo$(0,1))  //0,1 has the number of folders, which are listd next, from numFiles+1,x to numFiles+numFolders,x

    for i=1 to numFiles
        fileName$=fileInfo$(i,0)  //file name (no path info) is in (i,0)
        call DebugCopyFile source$;"\";fileName$, dest$;"\";fileName$   //copy file from source to dest
    next  i

        //We want to recursively copy all folders in this folder. However, because LB makes arrays global,
        //each recursive call would use the same copy of fileInfo$(). So we save the names of the folders in local variables.
        //This means there will be a max number of allowed folders, but that works for us
        //folder name (no path info) is in (i,1)
    if numFolders>0 then folder1$=fileInfo$(numFiles+1,1)
    if numFolders>1 then folder2$=fileInfo$(numFiles+2,1)
    if numFolders>2 then folder3$=fileInfo$(numFiles+3,1)
    if numFolders>3 then folder4$=fileInfo$(numFiles+4,1)
    if numFolders>4 then folder5$=fileInfo$(numFiles+5,1)
    if numFolders>5 then folder6$=fileInfo$(numFiles+6,1)
    if numFolders>6 then folder7$=fileInfo$(numFiles+7,1)
    if numFolders>7 then folder8$=fileInfo$(numFiles+8,1)
    if numFolders>8 then folder9$=fileInfo$(numFiles+9,1)

    for i=1 to numFolders
        select i
            case 1
                folderName$=folder1$
            case 2
                folderName$=folder2$
            case 3
                folderName$=folder3$
            case 4
                folderName$=folder4$
            case 5
                folderName$=folder5$
            case 6
                folderName$=folder6$
            case 7
                folderName$=folder7$
            case 8
                folderName$=folder8$
            case 9
                folderName$=folder9$
            case else
                notice "Too many folders to copy." : exit sub
        end select
        call DebugCopyDirectory source$, folderName$, dest$  //Copy the contents of the source directory
    next  i
end sub
*/
}
void DebugModule::DebugLoadData()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Menu item to load debug data
    if haltsweep=1 then gosub void DebugModule::FinishSweeping()
    filter$="Text files" + chr$(0) + "*.txt" + chr$(0) + _
                "All files" + chr$(0) + "*.*"   //ver115-6b
    defaultExt$="txt"
    initialDir$=DefaultDir$;"\xxDebug\"
    initialFile$="Prefs.txt"
    fName$=uOpenFileDialog$(filter$, defaultExt$, initialDir$, initialFile$, _
                        "Load Debug Files--Select Any File in the Debug Folder")
    if fName$="" then wait  //Blank means cancelled
    for i=len(fName$) to 1 step -1
        //Delete the actual file name so we just have the directory info
        thisChar$=Right$(fName$,1)
        fName$=Left$(fName$, i-1)   //Drop last character
        if thisChar$="\" or thisChar$="/" then exit for  //Done when we find the separator character
    next i
    //fName$ now has the directory name
    call mClearMarkers
        //Load the preference file
    restoreFileName$=fName$;"\Prefs.txt"
        //Load preference file in restoreFileName$; set restoreErr$ with any error message
    gosub void DebugModule::LoadPreferenceFile()  

    //Data is loaded from a folder called "xxDebug" in the default directory
    //We load the array info. The configuration info
    //is not used unless it is moved to the proper folder before startup.
    //This takes a long time because of the huge number of data items in the hardware arrays.
    cursor hourglass
    isErr=DebugLoadArrays(fName$)
    cursor normal
    if isErr then notice "Loading Debug Info was Unsuccessful." : wait

    //We must now recreate the graph from datatable(). We proceed as though we just gathered the data
    //point by point
    for thisstep=0 to globalSteps
        if msaMode$<>"SA" then gosub void DebugModule::ProcessDataArrays()    //fill S21DataArray and/or ReflectArray  ver115-8b
        gosub void DebugModule::PlotDataToScreen()    //Plot this point the same as though we just gathered it.
    next thisstep
    #graphBox$, "flush"
    beep
wait
*/
}
void DebugModule::DebugSaveArrays(QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*
  //Save Debug arrays as separate files in folder whose complete path is in folder$
  DebugArrayToFile(vars->magarray, folder+"magarray");
  DebugArrayToFile(vars->phaarray, folder+"phaarray");
  DebugArrayToFile(vars->PLL1array, folder+"PLL1array");
  DebugArrayToFile(vars->PLL3array, folder+"PLL3array");
  DebugArrayToFile(vars->DDS1array, folder+"DDS1array");
  DebugArrayToFile(vars->DDS3array, folder+"DDS3array");
  DebugArrayToFile(vars->cmdallarray, folder+"cmdallarray");
  //DebugArrayToFile(vars->, folder+"freqcoeff");
  //DebugArrayToFile(, folder+"magcoeff");
  DebugArrayToFile(vars->lineCalArray, folder+"linecal");
  DebugArrayToFile(vars->datatable, folder+"datatable");
  */
}
void DebugModule::DebugLoadArrays(QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Save Debug arrays as separate files in folder whose complete path is in folder$
    //return 1 if error; otherwise 0
    message$="Loading..."
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("magarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("phaarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("PLL1array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("PLL3array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("DDS1array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("DDS3array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("cmdallarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("freqcoeff", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("magcoeff", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("linecal", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("datatable", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    message$="" : call PrintMessage
end function
*/
}
void DebugModule:: DebugArrayAsTextArray(QString arrayID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Put data points into uTextPointArray$, with header info
    //We save the full datatable() even if data for some steps has not been collected yet.
    //return number of lines placed into uTextPointArray$
    //First line is "!Array=Name", where Name is arrayID$
    //Then comes each array entry as its own string
    uTextPointArray$(1)="!Name=";arrayID$
    if arrayID$="magcoeff" then limit=100 else limit=globalSteps    //Do globalSteps steps, but only 100 for magcoeff
    for i=0 to limit
        thisLine$=""
        aSpace$=""  //starts out blank for each line
        select arrayID$
            case "magarray"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;magarray(i, item) : aSpace$=" "
                next item
            case "phaarray"
                for item=0 to 4
                    thisLine$=thisLine$;aSpace$;phaarray(i, item) : aSpace$=" "
                next item
            case "PLL1array"
                for item=0 to 48
                    thisLine$=thisLine$;aSpace$;PLL1array(i, item) : aSpace$=" "
                next item
            case "PLL3array"
                for item=0 to 48
                    thisLine$=thisLine$;aSpace$;PLL3array(i, item) : aSpace$=" "
                next item
            case "DDS1array"
                for item=0 to 46
                    thisLine$=thisLine$;aSpace$;DDS1array(i, item) : aSpace$=" "
                next item
            case "DDS3array"
                for item=0 to 46
                    thisLine$=thisLine$;aSpace$;DDS3array(i, item) : aSpace$=" "
                next item
            case "cmdallarray"
                for item=0 to 39
                    thisLine$=thisLine$;aSpace$;cmdallarray(i, item) : aSpace$=" "
                next item
            case "freqcoeff"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;calFreqCoeffTable(i,item) : aSpace$=" "
                next item
            case "magcoeff"
                for item=0 to 7
                    thisLine$=thisLine$;aSpace$;calMagCoeffTable(i, item) : aSpace$=" "
                next item
            case "linecal"
                for item=0 to 2
                    thisLine$=thisLine$;aSpace$;lineCalArray(i, item) : aSpace$=" "
                next item
            case "datatable"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;datatable(i, item) : aSpace$=" "
                next item
            case else
                notice "Invalid Debug Array Name" : exit for
        end select
        uTextPointArray$(i+2)=thisLine$
    next i
    DebugArrayAsTextArray=limit+2   //Number of lines
end function
*/
}
void DebugModule:: DebugArrayToFile(QString arrayID, QString fullName)
{
  QFile debugOut(fullName);
  QFileInfo info(fullName);
  QString name = info.baseName();
  if (debugOut.open(QFile::WriteOnly))
  {
    QTextStream s(&debugOut);
    s << "!Name=" << name << "\n";
    debugOut.close();
  }
  /*
  //save array to a file in folder$ whose complete path is in folder$
  //We name the file per arrayID$ and override any existing file
  int nLines=DebugArrayAsTextArray(arrayID$)   //Assemble strings into uTextPointArray$
    fHndl$=DebugOpenOutputFile$(arrayID$, folder$)
    if isErr then notice "Could not save Debug file ";arrayID$ : exit sub
    for i=1 to nLines
        thisLine$=uTextPointArray$(i)
        print #fHndl$, thisLine$  //output each line
    next i
    close #fHndl$
    exit sub
end sub
*/
}
void DebugModule::DebugOpenInputFile$(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Open file; return handle or blank if not successful
    fName$=folder$;"\";arrayID$;".txt"
    On Error goto void DebugModule::fileError()
    open fName$ for input as #DebugArrayFile
    DebugOpenInputFile$="#DebugArrayFile"
    exit function
void DebugModule::fileError()
    notice "Could not open file ";arrayID$
    DebugOpenInputFile$=""  //error
end function
*/
}
void DebugModule::DebugOpenOutputFile$(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Open file; return handle or blank if not successful
    fName$=folder$;"\";arrayID$;".txt"
    On Error goto void DebugModule::fileError()
    open fName$ for output as #DebugArrayFile
    DebugOpenOutputFile$="#DebugArrayFile"
    exit function
*/
}

void DebugModule::fileError()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    notice "Could not open file ";arrayID$
    DebugOpenOutputFile$=""  //error
end function
*/
}
void DebugModule::DebugGetArrayFromFile(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//get points from file; return 1 if error; otherwise 0
    //Restore specified array
    //We will open file, read and close. File name is the same as arrayID$; file is in folder$
    fHndl$=DebugOpenInputFile$(arrayID$, folder$)
    if isErr then DebugGetArrayFromFile=1 : exit function
    isErr=uArrayFromFile(fHndl$,100) //Get data, determine number of items from first line
    if isErr then DebugGetArrayFromFile=1 : close #fHndl$ : exit function
        //Move the data from uWorkArray to gGraphVal
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints   //Make sure we have enough room
    for i=1 to uWorkNumPoints
        select arrayID$
            case "magarray"
                for item=0 to 3
                    magarray(i-1,item)=uWorkArray(i, item)
                next item
            case "phaarray"
                for item=0 to 4
                    phaarray(i-1,item)=uWorkArray(i, item)
                next item
            case "PLL1array"
                for item=0 to 48
                    PLL1array(i-1,item)=uWorkArray(i, item)
                next item
            case "PLL3array"
                for item=0 to 48
                    PLL3array(i-1,item)=uWorkArray(i, item)
                next item
            case "DDS1array"
                for item=0 to 46
                    DDS1array(i-1,item)=uWorkArray(i, item)
                next item
            case "DDS3array"
                for item=0 to 46
                    DDS3array(i-1,item)=uWorkArray(i, item)
                next item
            case "cmdallarray"
                for item=0 to 39
                    cmdallarray(i-1,item)=uWorkArray(i, item)
                next item
            case "freqcoeff"
                for item=0 to 3
                    calFreqCoeffTable(i-1,item)=uWorkArray(i, item)
                next item
            case "magcoeff"
                for item=0 to 7
                    calMagCoeffTable(i-1,item)=uWorkArray(i, item)
                next item
            case "linecal"
                for item=0 to 2
                    lineCalArray(i-1,item)=uWorkArray(i, item)
                next item
            case "datatable"
                for item=0 to 3
                    datatable(i-1,item)=uWorkArray(i, item)
                next item
            case else
                notice "Invalid Debug Array Name" : exit for
        end select
    next i
    DebugGetArrayFromFile=0  //no error
    close #fHndl$
    exit function
end function
*/
}

//==================================END DEBUG MODULE=======================================
