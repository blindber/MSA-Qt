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
#include "dialogsavecustappearance.h"

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
void dialogGridappearance::AppearanceShow(int currentAxis)
{
  currentPriAxis = currentAxis;
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
  //Perform action for the button named 'Cancel'
//    close #colorpreset
/*    saveCancelled=1;
    return;
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
  gTrace1Color=col1;
  gTrace2Color=col2;
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
QString dialogGridappearance::gGetLastPresetColors()
{
  //Get last selected color preset
  return gGraphColorPreset;
}
void dialogGridappearance::FillAppearancesArray(QStringList &list)
{
  list.clear();
  list.append("DARK");
  list.append("LIGHT");

  for (int i=1; i < 5; i++)
  {
    QString customName=customPresetNames[i];
    if (customName!="Empty")
    {
      list.append(customName);
    }
  }

/*  for (int i=0; i <= 10; i++)
  {
    Appearances[i]="";
  *
  } //Clear
  Appearances[0]="DARK";
  Appearances[1]="LIGHT";
  int k=1;     //max currently used appearance index ver115-2a
  for (int i=1; i < 5; i++)    //add custom names  ver115-2a
  {
    QString customName=customPresetNames[i];
    if (customName!="Empty")
    {
      k=k+1;
      Appearances[k]=customName;
    }
  }*/
}

void dialogGridappearance::getcustomPresetNames(QStringList &list)
{
  list.clear();

  for (int i=1; i < 5; i++)
  {
    QString customName=customPresetNames[i];
    if (customName!="Empty")
    {
      list.append(customName);
    }
  }
}

void dialogGridappearance::on_Cancel_clicked()
{
  allowClose = true;
  close();
}
void dialogGridappearance::on_BoxY1_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor(Y1Col, this);
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
  QColor color = QColorDialog::getColor(Y1ACol, this);
  if (color.isValid())
  {
    Y1ACol = color.name();
    ui->BoxY1A->setStyleSheet( QString("background: %1;").arg(Y1ACol) );
    AppearanceDrawSample();
  }
  ui->BoxY1A->setCurrentRow(-1);
}


void dialogGridappearance::on_BoxY2_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor(Y2Col, this);
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
  QColor color = QColorDialog::getColor(BackCol, this);
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
  QColor color = QColorDialog::getColor(Y1BCol, this);
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
  QColor color = QColorDialog::getColor(XCol, this);
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
  QColor color = QColorDialog::getColor(BoundsCol, this);
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
  QColor color = QColorDialog::getColor(LineCol, this);
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
  QColor color = QColorDialog::getColor(TextCol, this);
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
  QColor color = QColorDialog::getColor(Y2ACol, this);
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
  QColor color = QColorDialog::getColor(Y2BCol, this);
  if (color.isValid())
  {
    Y2BCol = color.name();
    ui->BoxY2B->setStyleSheet( QString("background: %1;").arg(Y2BCol) );
    AppearanceDrawSample();
  }
  ui->BoxY2B->setCurrentRow(-1);
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
}
void dialogGridappearance::on_Save_clicked()
{
  QStringList names;
  for (int ii=1; ii <= 5; ii++)
  {
    names.append(customPresetNames[ii]);
  }
  //Use colors and save them as a custom preset
  DialogSaveCustAppearance getName(this);
  getName.setNameList(names);
  getName.exec();
  if (getName.saveCancelled)
  {
    return;
  }
  int savedAsPresetNum = getName.getPresetNumber();
  QString newName = getName.getPresetName();

  gSetCustomPresetColors(savedAsPresetNum, LineCol, BoundsCol, BackCol,
                         TextCol, XCol, Y1Col, Y2Col,
                         Y1Col, Y2Col, Y1ACol, Y2ACol, Y1BCol, Y2BCol);
  customPresetNames[savedAsPresetNum] = newName;
  QStringList list;
  FillAppearancesArray(list);

  gUsePresetColors("Custom" + QString::number(savedAsPresetNum), currentPriAxis);  //To register that we are using this preset and implement it
  SetCycleColors();

  allowClose = true;
  close();
}
void dialogGridappearance::gUsePresetColors(QString btn, int primaryAxis)
{
  //ver116-4b added extra trace colors
  //Light, Dark and Sepia are hard-wired. In addition, 5 custom colors may be used, and referred to as Custom1, ....
  //btn$ specifies the preset to be used. It may be in the form of a
  //button handle, so we drop everything before the period, if there is one.
  int pos=btn.indexOf(".");
  if (pos>-1)
    btn=btn.mid(pos+1);
  if (btn.left(6).toUpper()=="CUSTOM")
  {
    //We need to use a numbered custom color set. btn$ is in the form "CustomN" where N is 1-5
    gUseCustomPresetColors(btn.mid(7).toInt());
    return;
  }

  gTrace1Color = QColor(250,40,20).name();
  gTrace2Color = QColor(30,30, 230).name();
  btn=btn.trimmed().toUpper();
  gGraphColorPreset=btn;     //ver114-2a Save for later use
  if (btn == "LIGHT")       //Black on white
  {
    gGridLineColor = QColor(200, 200, 200).name();
    gGridBoundsColor=QColor(Qt::black).name();
    gBackColor = QColor(Qt::white).name();
    gXTextColor=QColor(Qt::black).name();
    gGridTextColor=QColor(Qt::black).name();
    if (primaryAxis==1)
    {
      gTrace1Color = QColor(30, 30, 230).name();
      gTrace2Color = QColor(250, 40, 20).name();   //blue, red
      gTrace1AColor = QColor(20, 160, 175).name();
      gTrace2AColor = QColor(230, 140, 20).name();   //dark cyan, orange
      gTrace1BColor = QColor(0, 150, 50).name();
      gTrace2BColor = QColor(200, 175, 20).name();   //green, gold
    }
    else
    {
      gTrace1Color = QColor(250, 40, 20).name();
      gTrace2Color = QColor(30, 30, 230).name();   //red, blue
      gTrace1AColor = QColor(230, 140, 20).name();
      gTrace2AColor = QColor(20, 160, 175).name();   //orange, dark cyan
      gTrace1BColor = QColor(200, 175, 20).name();
      gTrace2BColor = QColor(0, 150, 50).name();   //gold, green
    }
    gY1TextColor=gTrace1Color;
    gY2TextColor=gTrace2Color;
  }
  else if (btn == "DARK")       //White On black
  {
    gGridLineColor = QColor(150, 150, 140).name();
    gGridBoundsColor = QColor(190, 190, 180).name();
    gBackColor = QColor(0, 0, 0).name();
    if (primaryAxis==1)  //ver115-3c
    {
      gTrace1Color = QColor(0, 255, 255).name();
      gTrace2Color = QColor(255, 200, 90).name();  //cyan,orange
      gTrace1AColor = QColor(100, 190, 255).name();
      gTrace2AColor = QColor(255, 50, 50).name();   //light blue, red
      gTrace1BColor = QColor(0, 255, 150).name();
      gTrace2BColor = QColor(255, 255, 50).name();   //green, yellow
    }
    else
    {
      gTrace1Color = QColor(255, 200, 90).name();
      gTrace2Color = QColor(0, 255, 255).name();  //orange, cyan
      gTrace1AColor = QColor(255, 50, 50).name();
      gTrace2AColor = QColor(100, 190, 255).name();   //red, light blue
      gTrace1BColor = QColor(255, 255, 50).name();
      gTrace2BColor = QColor(0, 255, 150).name();   //yellow, green
    }
    gXTextColor = QColor(245, 245, 240).name();
    gGridTextColor = QColor(245, 245, 240).name();
    gY1TextColor=gTrace1Color;
    gY2TextColor=gTrace2Color;
  }
  else if (btn == "SEPIA")
  {
    gGridLineColor = QColor(255, 230, 180).name();
    gGridBoundsColor = QColor(115, 85, 0).name();
    gBackColor = QColor(255, 255, 255).name();
    gXTextColor=QColor(Qt::black).name();
    gGridTextColor=QColor(Qt::black).name();
    if (primaryAxis==1)
    {
      gTrace1Color = QColor(30, 30, 230).name();
      gTrace2Color = QColor(250, 40, 20).name();   //blue, red
      gTrace1AColor = QColor(20, 160, 175).name();
      gTrace2AColor = QColor(230, 140, 20).name();   //dark cyan, orange
      gTrace1BColor = QColor(200, 175, 20).name();
      gTrace2BColor = QColor(220, 220, 50).name();   //green, yellow
    }
    else
    {
      gTrace1Color = QColor(250, 40, 20).name();
      gTrace2Color = QColor(30, 30, 230).name();   //red, blue
      gTrace1AColor = QColor(230, 140, 20).name();
      gTrace2AColor = QColor(20, 160, 175).name();   //orange, dark cyan
      gTrace1BColor = QColor(220, 220, 50).name();
      gTrace2BColor = QColor(200, 175, 20).name();   //yellow, green
    }
    gY1TextColor=gTrace1Color;
    gY2TextColor=gTrace2Color;
  }
}

void dialogGridappearance::gUseCustomPresetColors(int N)
{
  //Use preset color set N (1...5) //ver116-4b added extra trace colors
  if (N<1)
    N=1;
  if (N>5)
    N=5;
  gGridLineColor=gCustomColors[N][0];
  gGridBoundsColor=gCustomColors[N][1];
  gBackColor=gCustomColors[N][2];
  gGridTextColor=gCustomColors[N][3];
  gXTextColor=gCustomColors[N][4];
  gY1TextColor=gCustomColors[N][5];
  gY2TextColor=gCustomColors[N][6];
  gTrace1Color=gCustomColors[N][7];
  gTrace2Color=gCustomColors[N][8];
  gTrace1AColor=gCustomColors[N][9];
  gTrace2AColor=gCustomColors[N][10];
  gTrace1BColor=gCustomColors[N][11];
  gTrace2BColor=gCustomColors[N][12];
  gGraphColorPreset="Custom" + QString::number(N);       //So we can identify what set of colors we last started with
}
void dialogGridappearance::gSetCustomPresetColors(int N, QString grid, QString bounds, QString back, QString gridText, QString XText, QString Y1Text, QString Y2Text, QString trace1, QString trace2, QString trace1A, QString trace2A, QString trace1B, QString trace2B)
{
  //Set Nth (1-5) preset color set; don't actually implement this set yet
  if (N<1) N=1;
  if (N>5) N=5;
  gCustomColors[N][0]=grid;
  gCustomColors[N][1]=bounds;
  gCustomColors[N][2]=back;
  gCustomColors[N][3]=gridText;
  gCustomColors[N][4]=XText;
  gCustomColors[N][5]=Y1Text;
  gCustomColors[N][6]=Y2Text;
  gCustomColors[N][7]=trace1;
  gCustomColors[N][8]=trace2;
  gCustomColors[N][9]=trace1A;
  gCustomColors[N][10]=trace2A;
  gCustomColors[N][11]=trace1B;
  gCustomColors[N][12]=trace2B;
}
void dialogGridappearance::gGetCustomPresetColors(int N, QString &grid, QString &bounds, QString &back, QString &gridText, QString &XText, QString &Y1Text, QString &Y2Text, QString &trace1, QString &trace2, QString &trace1A, QString &trace2A, QString &trace1B, QString &trace2B)
{
  //Get Nth (1-5) preset color set
  if (N<1) N=1;
  if (N>5) N=5;
  N = N -1;
  grid=gCustomColors[N][0];
  bounds=gCustomColors[N][1]; //ver116-4b fixed second index of all these
  back=gCustomColors[N][2];
  gridText=gCustomColors[N][3];
  XText=gCustomColors[N][4];
  Y1Text=gCustomColors[N][5];
  Y2Text=gCustomColors[N][6];
  trace1=gCustomColors[N][7];trace2=gCustomColors[N][8];
  trace1A=gCustomColors[N][9];trace2A=gCustomColors[N][10];
  trace1B=gCustomColors[N][11];trace2B=gCustomColors[N][12];
}
void dialogGridappearance::gInitCustomColors()
{
//Initialize the custom colors to default values just to be valid
  for (int N=0; N < 5; N++)
  {
    //White background, dark foreground
    gCustomColors[N][0]="200 200 200";   //Line
    gCustomColors[N][1]="black";         //Bounds
    gCustomColors[N][2]="white";         //Background
    gCustomColors[N][3]="black";         //GridText
    gCustomColors[N][4]="black";         //XText
    gCustomColors[N][5]="250 40 20";     //Y1Text off-red
    gCustomColors[N][6]="30 30 230";     //Y2Text off-blue
    gCustomColors[N][7]="250 40 20";     //Trace1 off-red
    gCustomColors[N][8]="30 30 230";     //Trace2 off-blue
    gTrace1AColor="230 140 20";         //Trace1A orange
    gTrace2AColor="20 160 175";         //Trace 2A dark cyan
    gTrace1BColor="220 220 50";         //Trace1B yellow
    gTrace2BColor="200 175 20";         //Trace2B green
  }
}
