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
#ifndef DIALOGGRIDAPPEARANCE_H
#define DIALOGGRIDAPPEARANCE_H

#include <QDialog>
#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include <QGraphicsScene>
#include <QCloseEvent>
#include <QShowEvent>

#include "msautilities.h"

struct colors
{

};

namespace Ui {
class dialogGridappearance;
}

class dialogGridappearance : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogGridappearance(QWidget *parent = 0);
  ~dialogGridappearance();

  void AppearanceShow(int currentAxis);
  void gGetGridColors(QColor &backCol, QColor &lineCol, QColor &boundsCol);
  void gSetGridColors(QColor backCol, QColor lineCol, QColor boundsCol);
  void gSetFonts(QString xFont, QString y1Font, QString y2Font, QString gridFont);
  void gGetFonts(QString &xFont, QString &y1Font, QString &y2Font, QString &gridFont);
  void gSetTextColors(QColor xText, QColor y1Text, QColor y2Text, QColor gridText);
  void gGetTextColors(QColor &xText, QColor &y1Text, QColor &y2Text, QColor &gridText);
  void gGetInfoColors(QColor &fore, QColor &back);
  void SetCycleColors();
  QString gGetLastPresetColors();
  void FillAppearancesArray(QStringList &list);


  QString customPresetNames[6];   //User names for custom color presets (1-5) ver115-2a
  void getcustomPresetNames(QStringList &list);




  QColor gTrace1Color, gTrace2Color;  //Color of graph lines.
  QColor gTrace1AColor, gTrace2AColor;
  QColor gTrace1BColor, gTrace2BColor;    //color of supplemental traces 1A, 1B, 2A and 2B ver116-4b
  QString gGraphColorPreset;  //Last selected graph color preset; may have been partially overridden since ver114-2a
  QString gGraphTextPreset;   //Last selected graph text preset; may have been partially overridden since ver114-2a

  QColor gBackColor;  //Grid background color.
  QString gXAxisFont, gY1AxisFont, gY2AxisFont, gGridFont;   //Text specs for 3 axes and grid interior
  QColor gXTextColor, gY1TextColor, gY2TextColor, gGridTextColor;   //Text color for 3 axes and grid interior
  QColor gGridLineColor, gGridBoundsColor;    //Color of grid interior lines and boundary; default gray and black
  QColor cycleColorsAxis1[3], cycleColorsAxis2[3];    //colors for cycling in Stick mode. Index 1 is standard trace color ver116-4s

  QColor gCustomColors[6][13];    //Custom colors(1-5); gGridLineColor$(0),gGridBoundsColor$(1),gBackColor$(2),
                              //gGridTextColor$(3),gXTextColor$(4),gY1TextColor$(5),gY2TextColor$(6),
                              //gTrace1Color$(7), gTrace2Color$(8),gTrace1AColor$(9), gTrace2AColor$(10),
                              //gTrace1BColor$(11), gTrace2BColor$(12)

  void gSetTraceColors(QColor &col1, QColor &col2);
  void gGetTraceColors(QColor &col1, QColor &col2);
  void gGetSupplementalTraceColors(QColor &col1A, QColor &col2A, QColor &col1B, QColor &col2B);
  void gSetSupplementalTraceColors(QColor col1A, QColor col2A, QColor col1B, QColor col2B);

  void gUsePresetColors(QString btn, int primaryAxis);
  void gUseCustomPresetColors(int N);
  void gSetCustomPresetColors(int N, QColor grid, QColor bounds, QColor back, QColor gridText
                              , QColor XText, QColor Y1Text, QColor Y2Text, QColor trace1
                              , QColor trace2, QColor trace1A, QColor trace2A, QColor trace1B, QColor trace2B);
  void gGetCustomPresetColors(int N, QColor &grid,QColor &bounds,QColor &back,QColor &gridText,QColor &XText
                              ,QColor &Y1Text,QColor &Y2Text, QColor &trace1, QColor &trace2
                              , QColor &trace1A, QColor &trace2A, QColor &trace1B, QColor &trace2B);
  void gInitCustomColors();


private slots:


  void on_BoxY1_clicked(const QModelIndex &index);

  void on_Cancel_clicked();

  void on_BoxY1A_clicked(const QModelIndex &index);

  void on_Use_clicked();

  void on_BoxY2_clicked(const QModelIndex &index);

  void on_BoxBack_clicked(const QModelIndex &index);

  void on_BoxY1B_clicked(const QModelIndex &index);

  void on_BoxX_clicked(const QModelIndex &index);

  void on_BoxBounds_clicked(const QModelIndex &index);

  void on_BoxGrid_clicked(const QModelIndex &index);

  void on_BoxText_clicked(const QModelIndex &index);

  void on_BoxY2A_clicked(const QModelIndex &index);

  void on_BoxY2B_clicked(const QModelIndex &index);


  void on_Save_clicked();

private:
  Ui::dialogGridappearance *ui;
  QGraphicsScene *scene;
  bool allowClose;
  msaUtilities util;

  void AppearanceDrawSample();
  void PresetClosebox();
  void AppearanceList();
  void AppearanceDeletePreset();
  void AppearanceSavePreset();
  void AppearanceCancelPreset();

  void addText(qreal x,qreal y, QString text, QColor color);

  QColor Y1Col;
  QColor Y1ACol;
  QColor Y1BCol;

  QColor Y2Col;
  QColor Y2ACol;
  QColor Y2BCol;

  QColor BackCol;
  QColor LineCol;
  QColor BoundsCol;

  QColor XCol;
  QColor TextCol;

  //QString Appearances[11];    //Names of Appearances

  int currentPriAxis;
protected:
  virtual void closeEvent(QCloseEvent *e);
  virtual void showEvent ( QShowEvent * event );
};

#endif // DIALOGGRIDAPPEARANCE_H
