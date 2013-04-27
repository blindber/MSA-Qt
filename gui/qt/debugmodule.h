#ifndef DEBUGMODULE_H
#define DEBUGMODULE_H

#include <QtCore>

class DebugModule
{
public:
  DebugModule();
  void setDefaultDir(QString dir);
  void DebugSaveData();
  void DebugCopyFile(QString source, QString dest);
  void DebugCopyDirectory(QString sourcePath, QString dirName, QString destPath);
  void DebugLoadData();
  void DebugSaveArrays(QString folder);
  void DebugLoadArrays(QString folder);
  void  DebugArrayAsTextArray(QString arrayID);
  void  DebugArrayToFile(QString arrayID, QString fullName);
  void DebugOpenInputFile$(QString arrayID, QString folder);
  void DebugOpenOutputFile$(QString arrayID, QString folder);
  void fileError();
  void DebugGetArrayFromFile(QString arrayID, QString folder);

private:
  QString DefaultDir;

};

#endif // DEBUGMODULE_H
