//*******************************************************************************
//
//  -- Modular Spectrum Analyzer , in QT.
//
// Copyright (c) 2013 Bill Lindbergs
//
// The majority of this code is from spectrumanalyzer.bas, written by
// Scotty Sprowls and modified by Sam Wetterlin and Dave Roberts
//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//******************************************************************************

#include <QApplication>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QString version = "x.x.x.x";

  QFile mFile(":/res/res_file");
  if (!mFile.open(QFile::ReadOnly | QFile::Text))
  {
    qDebug() << __FILE__ << __LINE__  << "Failed opening res file";
  }
  QTextStream in(&mFile);

  while (!in.atEnd())
  {
    QString mText = in.readLine();
    if (mText.contains("FILEVERSION"))
    {
      mText.remove("FILEVERSION");
      mText.remove(" ");
      mText.replace(",",".");
      version = mText;
      break;
    }
  }

  a.setApplicationVersion(version);

  if (argc > 1)
  {
    QString strArgs = argv[1];
    strArgs = strArgs.toUpper();
    if (strArgs.indexOf("--HELP") != -1
        || strArgs.indexOf("--VERSION") != -1
      || strArgs.indexOf("-VERSION") != -1)
    {
      qDebug() << "MSA-Qt Version " << a.applicationVersion();
      return 0;
    }
  }

  qDebug() << "Version " << a.applicationVersion();
  MainWindow w;
  w.show();
  
  return a.exec();
}
