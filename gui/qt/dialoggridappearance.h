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
  void gGetGridColors(QString &backCol, QString &lineCol, QString &boundsCol);
  void gSetGridColors(QString backCol, QString lineCol, QString boundsCol);
  void gSetFonts(QString xFont, QString y1Font, QString y2Font, QString gridFont);
  void gGetFonts(QString &xFont, QString &y1Font, QString &y2Font, QString &gridFont);
  void gSetTextColors(QString xText, QString y1Text, QString y2Text, QString gridText);
  void gGetTextColors(QString &xText, QString &y1Text, QString &y2Text, QString &gridText);
  void gGetInfoColors(QString &fore, QString &back);
  void SetCycleColors();
  QString gGetLastPresetColors();
  void FillAppearancesArray();


  QString customPresetNames[6];   //User names for custom color presets (1-5) ver115-2a




  QString gTrace1Color, gTrace2Color;  //Color of graph lines.
  QString gTrace1AColor, gTrace2AColor;
  QString gTrace1BColor, gTrace2BColor;    //color of supplemental traces 1A, 1B, 2A and 2B ver116-4b
  QString gGraphColorPreset;  //Last selected graph color preset; may have been partially overridden since ver114-2a
  QString gGraphTextPreset;   //Last selected graph text preset; may have been partially overridden since ver114-2a

  QString gBackColor;  //Grid background color.
  QString gXAxisFont, gY1AxisFont, gY2AxisFont, gGridFont;   //Text specs for 3 axes and grid interior
  QString gXTextColor, gY1TextColor, gY2TextColor, gGridTextColor;   //Text color for 3 axes and grid interior
  QString gGridLineColor, gGridBoundsColor;    //Color of grid interior lines and boundary; default gray and black
  QString cycleColorsAxis1[3], cycleColorsAxis2[3];    //colors for cycling in Stick mode. Index 1 is standard trace color ver116-4s

  QString gCustomColors[6][13];    //Custom colors(1-5); gGridLineColor$(0),gGridBoundsColor$(1),gBackColor$(2),
                              //gGridTextColor$(3),gXTextColor$(4),gY1TextColor$(5),gY2TextColor$(6),
                              //gTrace1Color$(7), gTrace2Color$(8),gTrace1AColor$(9), gTrace2AColor$(10),
                              //gTrace1BColor$(11), gTrace2BColor$(12) //ver116-4b
  void gSetTraceColors(QString &col1, QString &col2);
  void gGetTraceColors(QString &col1, QString &col2);
  void gGetSupplementalTraceColors(QString &col1A, QString &col2A, QString &col1B, QString &col2B);
  void gSetSupplementalTraceColors(QString col1A, QString col2A, QString col1B, QString col2B);

  void gUsePresetColors(QString btn, int primaryAxis);
  void gUseCustomPresetColors(int N);
  void gSetCustomPresetColors(int N, QString grid, QString bounds, QString back, QString gridText
                              ,QString XText, QString Y1Text, QString Y2Text, QString trace1
                              ,QString trace2,QString trace1A, QString trace2A, QString trace1B, QString trace2B);
  void gGetCustomPresetColors(int N, QString &grid,QString &bounds,QString &back,QString &gridText,QString &XText
                              ,QString &Y1Text,QString &Y2Text, QString &trace1, QString &trace2
                              , QString &trace1A, QString &trace2A, QString &trace1B, QString &trace2B);
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

  void AppearanceDrawSample();
  void PresetClosebox();
  void AppearanceList();
  void AppearanceDeletePreset();
  void AppearanceSavePreset();
  void AppearanceCancelPreset();

  void addText(qreal x,qreal y, QString text, QColor color);

  QString Y1Col;
  QString Y1ACol;
  QString Y1BCol;

  QString Y2Col;
  QString Y2ACol;
  QString Y2BCol;

  QString BackCol;
  QString LineCol;
  QString BoundsCol;

  QString XCol;
  QString TextCol;

  QString Appearances[11];    //Names of Appearances

  int currentPriAxis;
protected:
  virtual void closeEvent(QCloseEvent *e);
  virtual void showEvent ( QShowEvent * event );
};

#endif // DIALOGGRIDAPPEARANCE_H
