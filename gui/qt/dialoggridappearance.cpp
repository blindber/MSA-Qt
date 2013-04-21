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
#include "dialoggridappearance.h"
#include "ui_dialoggridappearance.h"
#include <QColorDialog>
#include <QGraphicsScene>
#include <QGraphicsTextItem>


dialogGridappearance::dialogGridappearance(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogGridappearance)
{
  ui->setupUi(this);
  allowClose = false;

  scene = new QGraphicsScene(this);
  scene->setSceneRect(scene->itemsBoundingRect());
  ui->graphicsView->fitInView(scene->sceneRect());  //,Qt::KeepAspectRatio);
  ui->graphicsView->setScene(scene);

  // the QListWidget needs at least one item to get a click event
  ui->BoxY1->addItem("");
  ui->BoxY1A->addItem("");
  ui->BoxY1B->addItem("");
  ui->BoxY2->addItem("");
  ui->BoxY2A->addItem("");
  ui->BoxY2B->addItem("");
  ui->BoxX->addItem("");
  ui->BoxBack->addItem("");
  ui->BoxBounds->addItem("");
  ui->BoxGrid->addItem("");
  ui->BoxText->addItem("");
}

dialogGridappearance::~dialogGridappearance()
{
  delete ui;
}
void dialogGridappearance::AppearanceShow()
{
  gGetGridColors(BackCol, LineCol, BoundsCol);

  QString temp = QString("background: %1;").arg(BackCol);
  ui->BoxGrid->setStyleSheet( QString("background: %1;").arg(LineCol) );
  ui->BoxBack->setStyleSheet( temp );
  ui->BoxBounds->setStyleSheet( QString("background: %1;").arg(BoundsCol) );

  gGetTextColors(XCol, Y1Col, Y2Col, TextCol);
  gGetSupplementalTraceColors(Y1ACol, Y2ACol, Y1BCol, Y2BCol);

  ui->BoxText->setStyleSheet( QString("background: %1;").arg(TextCol) );
  ui->BoxY1->setStyleSheet( QString("background: %1;").arg(Y1Col) );
  ui->BoxY1A->setStyleSheet( QString("background: %1;").arg(Y1ACol) );
  ui->BoxY1B->setStyleSheet( QString("background: %1;").arg(Y1BCol) );

  ui->BoxY2->setStyleSheet( QString("background: %1;").arg(Y2Col) );
  ui->BoxY2A->setStyleSheet( QString("background: %1;").arg(Y2ACol) );
  ui->BoxY2B->setStyleSheet( QString("background: %1;").arg(Y2BCol) );

  ui->BoxX->setStyleSheet( QString("background: %1;").arg(XCol) );

  AppearanceDrawSample();
  exec();
}
void dialogGridappearance::AppearanceDrawSample()
 {
  scene->clear();
  scene->setBackgroundBrush(QColor(BackCol)); // set background to black

  //Draw sample chart
  int originX=25, originY=125, ht=100, width=200;
  for (int i=1; i <= 3; i++)    //vertical lines
  {
    scene->addLine(originX+50*i, originY, originX+50*i, originY-ht, QPen(QColor(LineCol),1));
  }
  for (int i=1; i <= 3; i++)    //horizontal lines
  {
    scene->addLine(originX, originY-25*i, originX+width, originY-25*i, QPen(QColor(LineCol),1));
  }

  scene->addLine(originX, originY, originX, originY-ht, QPen(QColor(BoundsCol),3));
  scene->addLine(originX, originY-ht, originX+width, originY-ht, QPen(QColor(BoundsCol),3));
  scene->addLine(originX+width, originY-ht, originX+width, originY, QPen(QColor(BoundsCol),3));
  scene->addLine(originX+width, originY, originX, originY, QPen(QColor(BoundsCol),3));

  //Draw Sample Text
  addText(originX+90,originY-ht, "Title", TextCol);

  //Draw graph lines
  addText(originX-10,originY, "1", Y1Col);
  addText(originX-20,originY-ht+5, "10", Y1Col);

  addText(originX+width+5,originY, "1", Y2Col);
  addText(originX+width+5,originY-ht+5, "10", Y2Col);

  addText(originX,originY+15, "1", XCol);
  addText(originX+width-5,originY+15, "10", XCol);

  scene->addLine(originX, originY-30, originX+width, originY-40, QPen(QColor(Y1Col),1));
  scene->addLine(originX, originY-20, originX+width, originY-30, QPen(QColor(Y1ACol),1));
  scene->addLine(originX, originY-10, originX+width, originY-20, QPen(QColor(Y1BCol),1));


  scene->addLine(originX, originY-80, originX+width, originY-90, QPen(QColor(Y2Col),1));
  scene->addLine(originX, originY-70, originX+width, originY-80, QPen(QColor(Y2ACol),1));
  scene->addLine(originX, originY-60, originX+width, originY-70, QPen(QColor(Y2BCol),1));

  scene->setSceneRect(scene->itemsBoundingRect());
  ui->graphicsView->fitInView(scene->sceneRect());  //,Qt::KeepAspectRatio);
  ui->graphicsView->ensureVisible(scene->sceneRect(),10,10);
}
void dialogGridappearance::AppearanceSave()
 {
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'Use colors and save them as a custom preset
    saveCancelled=0
    gosub void dialogGridappearance::AppearanceSubToSave()
    if saveCancelled then wait
    'ver116-4b deleted setting graph module colors; gUsePresetColors will do that
    call gUsePresetColors "Custom";savedAsPresetNum  'To register that we are using this preset and implement it
    call SetCycleColors 'ver116-4s
    close #gridappearance
    call RedrawGraph 0
    exit sub

 '---------------------------Start of AppearanceSubToSave-------
*/
}
void dialogGridappearance::AppearanceSubToSave()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    WindowWidth = 465
    WindowHeight = 335
    UpperLeftX=100
    UpperLeftY=100

    statictext #colorpreset.statictext1, "Select the Custom Appearance to be replaced by the new colors.",  25,  17, 384,  20
    statictext #colorpreset.statictext2, "You may also rename the Custom Appearance.",  60,  37, 350,  20
    ListboxColor$ = "white"

    listbox #colorpreset.List, customPresetNames$(), void dialogGridappearance::AppearanceList(),  145,  82, 160,  90
    textbox #colorpreset.Name, 145, 200,120, 20
    statictext #colorpreset, "Name", 105, 200, 40, 20
    button #colorpreset.Delete,"Delete",void dialogGridappearance::AppearanceDeletePreset(), UL,  70, 240,  90,  40
    button #colorpreset.Save,"Save",void dialogGridappearance::AppearanceSavePreset(), UL,  180, 240,  90,  40
    button #colorpreset.Cancel,"Cancel",void dialogGridappearance::AppearanceCancelPreset(), UL, 290, 240,  90,  40

    open "Save Custom Appearance" for dialog_modal as #colorpreset
    print #colorpreset, "font ms_sans_serif 10"
    print #colorpreset, "trapclose void dialogGridappearance::PresetClosebox()"
    print #colorpreset.List, "singleClickSelect"
    wait
*/
}
void dialogGridappearance::PresetClosebox()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    wait
*/
}
void dialogGridappearance::AppearanceList()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'Perform action for the listbox named 'List'
    #colorpreset.List, "selection? name$"
    #colorpreset.Name, name$
    #colorpreset.Name, "!setfocus"
    call uHighlightText "#colorpreset.Name" 'to allow changing it immediately
    wait
*/
}
void dialogGridappearance::AppearanceDeletePreset()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    #colorpreset.List, "selectionindex? sel"    'sel (1...) is index of selected item
    if sel=0 then notice "No Appearance is selected." : wait
    customPresetNames$(sel)="Empty"
    #colorpreset.List, "Reload"
    #colorpreset.Name, "Empty"
    wait
*/
}
void dialogGridappearance::AppearanceSavePreset()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    #colorpreset.List, "selectionindex? savedAsPresetNum"    'savedAsPresetNum (1...) is index of selected item
    if savedAsPresetNum=0 then notice "No Appearance is selected." : wait
    #colorpreset.Name, "!contents? newName$"
    if newName$="" or newName$="Empty" then notice "You must provide a Name." : wait
    call gSetCustomPresetColors savedAsPresetNum,LineCol$, BoundsCol$, BackCol$,TextCol$, XCol$, Y1Col$, Y2Col$, _
                Y1Col$, Y2Col$, Y1ACol$, Y2ACol$, Y1BCol$, Y2BCol$  'ver116-4b added extra trace colors
    customPresetNames$(savedAsPresetNum)=newName$
    call FillAppearancesArray
    close #colorpreset
    return
*/
}
void dialogGridappearance::AppearanceCancelPreset()
 {
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'Perform action for the button named 'Cancel'
    close #colorpreset
    saveCancelled=1
    return
 '---------------------------End of AppearanceSubToSave-------
*/
}
void dialogGridappearance::addText(qreal x, qreal y, QString text, QColor color)
{
  QFontMetrics fm(scene->font());
  QGraphicsTextItem *item;
  item = scene->addText(text);
  item->setDefaultTextColor(color);
  item->setPos(x,y-fm.height()*1.5);
}

void dialogGridappearance::closeEvent(QCloseEvent *e)
{
  if (!allowClose)
    e->ignore();
}

void dialogGridappearance::showEvent(QShowEvent *event)
{
  ui->graphicsView->fitInView(0, 0, scene->width(), scene->height());
  QWidget::showEvent(event);
}
void dialogGridappearance::gGetGridColors(QString &backCol, QString &lineCol, QString &boundsCol)
{
  //Colors for drawing grid
  lineCol=gGridLineColor;
  boundsCol=gGridBoundsColor;
  backCol=gBackColor;
}

void dialogGridappearance::gSetGridColors(QString backCol, QString lineCol, QString boundsCol)
{
  //Colors for drawing grid
  gGridLineColor=lineCol;
  gGridBoundsColor=boundsCol;
  gBackColor=backCol;
}
void dialogGridappearance::gSetFonts(QString xFont, QString y1Font, QString y2Font, QString gridFont)
{
  //Text specs
  //Text specs include font name, attributes and color in the same
  //form used by LB to send commands to a raphic box or window
  gXAxisFont=xFont;
  gY1AxisFont=y1Font;
  gY2AxisFont= y2Font;
  gGridFont=gridFont;
}

void dialogGridappearance::gGetFonts(QString &xFont, QString &y1Font, QString &y2Font, QString &gridFont)
{
  //Text specs
  //Text specs include font name, size and attributesin the same
  //form used by LB to send commands to a graphic box or window
  xFont=gXAxisFont;
  y1Font=gY1AxisFont;
  y2Font=gY2AxisFont;
  gridFont=gGridFont;
}
void dialogGridappearance::gSetTextColors(QString xText, QString y1Text, QString y2Text, QString gridText)
{
  //Text color
  //examples: "red" or "100 250 60"
  gXTextColor=xText;
  gY1TextColor=y1Text;
  gY2TextColor=y2Text;
  gGridTextColor=gridText;
}

void dialogGridappearance::gGetTextColors(QString &xText, QString &y1Text, QString &y2Text, QString &gridText)
{
  gridText=gGridTextColor;
  xText=gXTextColor;
  y1Text=gY1TextColor;
  y2Text=gY2TextColor;
}
void dialogGridappearance::gGetInfoColors(QString &fore, QString &back)
{
  //Get text fore and back colors for printing info below or at side of grid
  fore=gGridTextColor;
  back=gBackColor;
}
void dialogGridappearance::gSetTraceColors(QString &col1, QString &col2)
{
  //Set color of graph traces
  gTrace1Color=col1; gTrace2Color=col2;
}
void dialogGridappearance::gGetTraceColors(QString &col1, QString &col2)
{
  //Get color of main graph traces
  col1=gTrace1Color;
  col2=gTrace2Color;
}
void dialogGridappearance::gGetSupplementalTraceColors(QString &col1A, QString &col2A, QString &col1B, QString &col2B)
{
//Get color of supplemental graph traces ver116-4b
    col1A=gTrace1AColor;
    col2A=gTrace2AColor;
    col1B=gTrace1BColor;
    col2B=gTrace2BColor;
}

void dialogGridappearance::gSetSupplementalTraceColors(QString col1A, QString col2A, QString col1B, QString col2B)
{
  //Set color of supplemental graph traces ver116-4b
  gTrace1AColor=col1A;
  gTrace2AColor=col2A;
  gTrace1BColor=col1B;
  gTrace2BColor=col2B;
}
void dialogGridappearance::SetCycleColors()
{
  //Set up the trace colors for cycling 'ver116-4s
  QString Y1ACol, Y2ACol, Y1BCol, Y2BCol;
  gGetSupplementalTraceColors(Y1ACol, Y2ACol, Y1BCol, Y2BCol);
  QString Y1Col, Y2Col;
  gGetTraceColors(Y1Col, Y2Col);
  cycleColorsAxis1[0]=Y1Col; cycleColorsAxis1[1]=Y1ACol; cycleColorsAxis1[2]=Y1BCol;
  cycleColorsAxis2[0]=Y2Col; cycleColorsAxis2[1]=Y2ACol; cycleColorsAxis2[2]=Y2BCol;
}

void dialogGridappearance::on_Cancel_clicked()
{
  allowClose = true;
  close();
}
void dialogGridappearance::on_BoxY1_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y1Col = color.name();
    ui->BoxY1->setStyleSheet( QString("background: %1;").arg(Y1Col) );
    AppearanceDrawSample();
  }
  ui->BoxY1->setCurrentRow(-1);
}


void dialogGridappearance::on_BoxY1A_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y1ACol = color.name();
    ui->BoxY1A->setStyleSheet( QString("background: %1;").arg(Y1ACol) );
    AppearanceDrawSample();
  }
  ui->BoxY1A->setCurrentRow(-1);
}
void dialogGridappearance::on_Use_clicked()
{
  //Use but don't save the selected colors
  gSetGridColors(BackCol, LineCol, BoundsCol);
  gSetTextColors(XCol, Y1Col, Y2Col, TextCol);
  gSetTraceColors(Y1Col, Y2Col);
  gSetSupplementalTraceColors(Y1ACol, Y2ACol, Y1BCol, Y2BCol);
  SetCycleColors();
  allowClose = true;
  close();
  //call RedrawGraph 0
}

void dialogGridappearance::on_BoxY2_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y2Col = color.name();
    ui->BoxY2->setStyleSheet( QString("background: %1;").arg(Y2Col) );
    AppearanceDrawSample();
  }
  ui->BoxY2->setCurrentRow(-1);
}


void dialogGridappearance::on_BoxBack_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    BackCol = color.name();
    ui->BoxBack->setStyleSheet( QString("background: %1;").arg(BackCol) );
    AppearanceDrawSample();
  }
  ui->BoxBack->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxY1B_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y1BCol = color.name();
    ui->BoxY1B->setStyleSheet( QString("background: %1;").arg(Y1BCol) );
    AppearanceDrawSample();
  }
  ui->BoxY1B->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxX_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    XCol = color.name();
    ui->BoxX->setStyleSheet( QString("background: %1;").arg(XCol) );
    AppearanceDrawSample();
  }
  ui->BoxX->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxBounds_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    BoundsCol = color.name();
    ui->BoxBounds->setStyleSheet( QString("background: %1;").arg(BoundsCol) );
    AppearanceDrawSample();
  }
  ui->BoxBounds->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxGrid_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    LineCol = color.name();
    ui->BoxGrid->setStyleSheet( QString("background: %1;").arg(LineCol) );
    AppearanceDrawSample();
  }
  ui->BoxGrid->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxText_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    TextCol = color.name();
    ui->BoxText->setStyleSheet( QString("background: %1;").arg(TextCol) );
    AppearanceDrawSample();
  }
  ui->BoxText->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxY2A_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y2ACol = color.name();
    ui->BoxY2A->setStyleSheet( QString("background: %1;").arg(Y2ACol) );
    AppearanceDrawSample();
  }
  ui->BoxY2A->setCurrentRow(-1);
}

void dialogGridappearance::on_BoxY2B_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor();
  if (color.isValid())
  {
    Y2BCol = color.name();
    ui->BoxY2B->setStyleSheet( QString("background: %1;").arg(Y2BCol) );
    AppearanceDrawSample();
  }
  ui->BoxY2B->setCurrentRow(-1);
}
