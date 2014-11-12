#include "dialogrlc.h"
#include "ui_dialogrlc.h"
#include "globalvars.h"
#include <QMessageBox>

dialogRLC::dialogRLC(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogRLC)
{
  ui->setupUi(this);

  ui->lineEdit_QL->setValidator(new QDoubleValidator(this));
  ui->lineEdit_QC->setValidator(new QDoubleValidator(this));

  ui->lineEdit_R0->setValidator(new QDoubleValidator(this));
  ui->lineEdit_VF->setValidator(new QDoubleValidator(this));
  ui->lineEdit_K1->setValidator(new QDoubleValidator(this));
  ui->lineEdit_K2->setValidator(new QDoubleValidator(this));
  ui->lineEdit_Len->setValidator(new QDoubleValidator(this));

  ui->lineEdit_FixR0->setValidator(new QDoubleValidator(this));



  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(RLCDialogFinished()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(RLCSpecHelp()));
}

dialogRLC::~dialogRLC()
{
  delete ui;
}
void dialogRLC::RLCDialog()
{
  //    call GetDialogPlacement //set UpperLeftX and UpperLeftY ver115-1c

  //Copy standard coax names from coaxNames$, but add None at beginning and Custom at end.
  ui->comboBoxCoaxTypes->clear();
  ui->comboBoxCoaxTypes->addItem("None");

  QStringListIterator i(mSAconfig::coax.coaxNames);
  while(i.hasNext())
  {
    ui->comboBoxCoaxTypes->addItem(i.next());
  }
  ui->comboBoxCoaxTypes->addItem("Custom");

  //    combobox #RLCDialog.Coax, RLCDialogCoaxTypes$(),void dialogRLC::RLCSelectCoax(),175, tranLineTop+5, 150, 250

  if (settings.msaMode == modeReflection)
  {
    ui->A3->setVisible(false);
    ui->A4->setVisible(false);
  }
  else
  {
    ui->A3->setVisible(true);
    ui->A4->setVisible(true);
  }

  //Display existing values
  QString form="3,3,4//UseMultiplier";
  QString resForm="3,3,4//UseMultiplier//SuppressMilli";
  QString QForm="######.###";
  QString R=util.uFormatted(settings.dialogRValue, resForm);
  QString L=util.uFormatted(settings.dialogLValue, form);
  QString C=util.uFormatted(settings.dialogCValue, form);
  QString QL=util.uFormatted(settings.dialogQLValue, QForm);
  QString QC=util.uFormatted(settings.dialogQCValue, QForm);
  double cR0,cVF,cK1,cK2,cLen;
  int isErr = mSAconfig::coax.CoaxParseSpecs(settings.dialogCoaxSpecs,cR0,cVF,cK1,cK2,cLen);    //If error, default values are returned
  QString R0=util.uFormatted(cR0, resForm);
  QString VF=util.usingF("#.#####", cVF);
  QString K1=util.usingF("#.#####", cK1);
  QString K2=util.usingF("#.#####", cK2);
  QString lenFeet=util.uFormatted(cLen, resForm);

  ui->lineEditSelR->setText(R);
  ui->lineEditSelL->setText(L);
  ui->lineEditSelC->setText(C);
  ui->lineEdit_QL->setText(QL);
  ui->lineEdit_QC->setText(QC);
  ui->lineEdit_R0->setText(R0);
  ui->lineEdit_VF->setText(VF);
  ui->lineEdit_K1->setText(K1);
  ui->lineEdit_K2->setText(K2);
  ui->lineEdit_Len->setText(lenFeet);

  ui->checkBox_R->setChecked(true);
  ui->checkBox_L->setChecked(true);
  ui->checkBox_C->setChecked(true);
  //R, L and C only if they are meaningful

  if (settings.dialogRLCConnect=="S")
  {
    ui->radioButtonSeries->setChecked(true);
    //For series components, zero impedance component means there is no such component,
    //except missing R is set to high impedance if there is no L or C.
    //a=DialogRValue;
    //b=DialogLValue;
    //c=DialogCValue;
    if (settings.dialogRValue>=constMaxValue)
    {
      ui->checkBox_R->setChecked(false);
    }
    if (settings.dialogRValue==0 && (settings.dialogLValue!=0 || settings.dialogCValue<constMaxValue))
    {
      ui->checkBox_R->setChecked(false);
      ui->lineEditSelR->setVisible(false);
    }
    if (settings.dialogLValue==0)
    {
      ui->checkBox_L->setChecked(false);
      ui->lineEditSelL->setVisible(false);
      ui->lineEdit_QL->setVisible(false);
      ui->label_QL->setVisible(false);
    }
    if (settings.dialogCValue>=constMaxValue)
    {
      ui->checkBox_C->setChecked(false);
      ui->lineEditSelC->setVisible(false);
      ui->lineEdit_QC->setVisible(false);
      ui->label_QC->setVisible(false);
    }
  }
  else    //parallel
  {
    ui->radioButtonParallel->setChecked(true);
    if (settings.dialogRValue>=constMaxValue)
    {
      ui->checkBox_R->setChecked(false);
      ui->lineEditSelR->setVisible(false);
    }
    if (settings.dialogLValue>=constMaxValue)
    {
      ui->checkBox_L->setChecked(false);
      ui->lineEditSelL->setVisible(false);
      ui->lineEdit_QL->setVisible(false);
      ui->label_QL->setVisible(false);
    }
    if (settings.dialogCValue==0)
    {
      ui->checkBox_C->setChecked(false);
      ui->lineEditSelC->setVisible(false);
      ui->lineEdit_QC->setVisible(false);
      ui->label_QC->setVisible(false);
    }
  }
  if (settings.dialogCoaxName=="")
    settings.dialogCoaxName="None";

  //Select current coax in combobox
  ui->comboBoxCoaxTypes->setCurrentIndex(ui->comboBoxCoaxTypes->findText(settings.dialogCoaxName));

  if (settings.msaMode == modeScalarTrans ||  settings.msaMode == modeVectorTrans)
  {
    //We only do fixture type for Transmission mode
    ui->lineEdit_FixR0->setText(QString::number(settings.S21JigR0));
    if (settings.S21JigAttach=="Series")
    {
      ui->radioButton_SeriesFix->setChecked(true);
    }
    else
    {
      ui->radioButton_ShuntFix->setChecked(true);
    }
  }
  else
  {
    ui->groupBoxFix->setVisible(false);
  }
  ui->lineEdit_Len->setFocus();
  on_comboBoxCoaxTypes_currentIndexChanged(ui->comboBoxCoaxTypes->currentText());
}

void dialogRLC::setSettings(dialogRLCStruct settings)
{
  this->settings = settings;
  RLCDialog();
}

void dialogRLC::getSettings(dialogRLCStruct &settings)
{
  settings = this->settings;
}

void dialogRLC::DialogRLCConnect(QString t)
{
  settings.dialogRLCConnect = t;
}

void dialogRLC::DialogRValue(float t)
{
  ui->lineEditSelR->setText(QString::number(t));
}

void dialogRLC::DialogLValue(float t)
{
  ui->lineEditSelL->setText(QString::number(t));
}

void dialogRLC::DialogQLValue(float t)
{
  ui->lineEdit_QL->setText(QString::number(t));
}

void dialogRLC::DialogQCValue(float t)
{
  ui->lineEdit_QC->setText(QString::number(t));
}

void dialogRLC::DialogCValue(float t)
{
  ui->lineEditSelC->setText(QString::number(t));
}

void dialogRLC::DialogCoaxSpecs(QString t)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

void dialogRLC::DialogCoaxName(QString t)
{
  ui->comboBoxCoaxTypes->setCurrentIndex(ui->comboBoxCoaxTypes->findText(t));
}

QString dialogRLC::DialogRLCConnect()
{
  return settings.dialogRLCConnect;
}

float dialogRLC::DialogRValue()
{
  return ui->lineEditSelR->text().toFloat();
}

float dialogRLC::DialogLValue()
{
  return ui->lineEditSelL->text().toFloat();
}

float dialogRLC::DialogQLValue()
{
  return ui->lineEdit_QL->text().toFloat();
}

float dialogRLC::DialogQCValue()
{
  return ui->lineEdit_QC->text().toFloat();
}

float dialogRLC::DialogCValue()
{
  return ui->lineEditSelC->text().toFloat();
}

QString dialogRLC::DialogCoaxSpecs()
{
  return "Fix me";
}

QString dialogRLC::DialogCoaxName()
{
  return ui->comboBoxCoaxTypes->currentText();
}
void dialogRLC::RLCDialogFinished()
{
  if (settings.msaMode == modeScalarTrans || settings.msaMode == modeVectorTrans)
  {
    //We only do fixture info for Transmission mode
    float v = util.uValWithMult(ui->lineEdit_FixR0->text());
    if (v<=0)
    {
      QMessageBox::information(this, "Notice","Invalid Fixture R0; 50 ohms used.");
      v=50;
    }
    settings.S21JigR0=v;

    if (ui->radioButton_SeriesFix)
      settings.S21JigAttach="Series";
    else
      settings.S21JigAttach="Shunt";
  }

  settings.dialogCoaxName = ui->comboBoxCoaxTypes->currentText();
  //Retrieve and check coax data
  float cR0=util.uValWithMult(ui->lineEdit_R0->text());  //R0
  if (cR0<=0)
  {
    QMessageBox::information(this, "Notice","Invalid coax R0");
    return;
  }
  float cVF=util.uCompact(ui->lineEdit_VF->text()).toFloat(); //velocity Factor
  if (cVF <= 0 || cVF > 1)
  {
    QMessageBox::information(this, "Notice","Invalid coax VF");
    return;
  }
  float cK1=util.uCompact(ui->lineEdit_K1->text()).toFloat(); //K1--conductor loss factor
  if (cK1<0 || cK1>=10)
  {
    QMessageBox::information(this, "Notice","Invalid coax K1");
    return;
  }
  float cK2=util.uCompact(ui->lineEdit_K2->text()).toFloat(); //K2--dielectric loss factor
  if (cK2<0 || cK2>=10)
  {
    QMessageBox::information(this, "Notice","Invalid coax K2");
    return;
  }
  float cLen=util.uValWithMult(util.uCompact(ui->lineEdit_Len->text())); //length in feet
  if (cLen<0)
  {
    QMessageBox::information(this, "Notice","Invalid coax length");
    return;
  }
  if (cLen>50000)
  {
    //big value creates overflow
    QMessageBox::information(this, "Notice","Coax length may not exceed 50 k feet");
    return;
  }
  //Use blank coax specs if no coax is used,
  //as a signal for Series Fixture in transmission mode that the RLC values are to be used.
  if (cLen==0 || settings.dialogCoaxName == "None")
    settings.dialogCoaxSpecs="";
  else
    settings.dialogCoaxSpecs = mSAconfig::coax.CoaxSpecs(cR0,cVF,cK1,cK2,cLen);


  if (ui->radioButtonParallel->isChecked())
    settings.dialogRLCConnect = "P";
  else
    settings.dialogRLCConnect = "S";
  //Get component values. For shunt fixture, these are deemed the termination. For series
  //fixture, any RLC combo is in series but is ignored if there is a transmission line. To indicate
  //no transmission line for this purpose, we made the spec blank.

  bool haveR = ui->checkBox_R->isChecked();
  bool haveL = ui->checkBox_L->isChecked();
  bool haveC = ui->checkBox_C->isChecked();

  if (haveR)
  {
    settings.dialogRValue=util.uValWithMult(ui->lineEditSelR->text());
    if (settings.dialogRValue<0)
      settings.dialogRValue=0;
  }
  else
  {
    //if no R for parallel, use large value
    if (settings.dialogRLCConnect=="P")
      settings.dialogRValue = constMaxValue;
    else
    {
      //if no R for series, use 0 if L or C are specified; otherwise use large value
      if (haveL || haveC)
        settings.dialogRValue=0;
      else
        settings.dialogRValue=constMaxValue;
    }
  }

  if (haveL)
  {
    settings.dialogLValue=util.uValWithMult(ui->lineEditSelL->text());
    if (settings.dialogLValue<0)
      settings.dialogLValue=0;

    settings.dialogQLValue=util.uValWithMult(ui->lineEdit_QL->text());
    if (settings.dialogQLValue<=0)
      settings.dialogQLValue=0.001;
    if (settings.dialogQLValue>100000)
      settings.dialogQLValue=100000;   //max of 1e5 so it can display properly ver115-5f
  }
  else
  {
    if (settings.dialogRLCConnect=="P")
      settings.dialogLValue=constMaxValue;
    else
      settings.dialogLValue=0;
    settings.dialogQLValue=1e4;
  }

  if (haveC)
  {
    settings.dialogCValue=util.uValWithMult(ui->lineEditSelC->text());
    if (settings.dialogCValue<0)
      settings.dialogCValue=0;
    settings.dialogQCValue=util.uValWithMult(ui->lineEdit_QC->text());
    if (settings.dialogQCValue<=0)
      settings.dialogQCValue=0.001;
    if (settings.dialogQCValue>100000)
      settings.dialogQCValue=100000;   //max of 1e5 so it can display properly ver115-5f
  }
  else
  {
    //if no C, use large value for series and 0 for parallel
    if (settings.dialogRLCConnect=="P")
      settings.dialogCValue=0;
    else
      settings.dialogCValue=constMaxValue;
    settings.dialogQCValue=1e4;
  }
  accept();

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
void dialogRLC::RLCSpecHelp()
{
  //Display help info for RLC spec dialog

  QString s = "The RLC spec dialog lets you specify the characteristics of a circuit consisting of a resistor, inductor"
      " and capacitor. One or two of those components can be omitted. You may specify the RLC values, and for"
      " the inductor and capacitor you may specify the Q value. Low Q values represent a loss in the component"
      " that in effect puts a resistor in series with the component, whose resistance changes with frequency."
      " You also specify whether the RLC components are in series or in parallel with each other."
      "\n\n"
      " The RLC circuit may be simulated as though it is attached via a transmission line, so the RLC combination"
      " becomes the termination of the transmission line. You specify the length of the transmission"
      " line in feet, which may be zero. You may select a type of coaxial cable,"
      " or you may select Custom and enter your own parameters, which incude characteristic impedance (R0), velocity"
      " factor, conductor loss (K1), and dielectric loss (K2). The K loss factors are specified in accordance with the"
      " equation:"
      "\n\n"
      "\tMatched Loss (dB per hundred feet)=K1*sqrt(F)+K2*F"
      "\t\twhere F is the frequency in MHz.";

  if (settings.msaMode != modeReflection) //Reflection doesn't care what the fixture is
  {
    s = s + "\n\n"
        " You must specify whether the fixture is series or shunt, and its R0."
        " For the shunt fixture, you may enter the time delay of the connection between the actual fixture and"
        " the components; typically on the order of 0.125 ns per inch. For the series fixture, if the"
        " transmission line is used the RLC components are ignored.";
  }

  QMessageBox::information(0, "RLC Analysis Help", s);
}

void dialogRLC::on_checkBox_R_clicked(bool checked)
{
  ui->lineEditSelR->setVisible(checked);
}

void dialogRLC::on_checkBox_L_clicked(bool checked)
{
  ui->lineEditSelL->setVisible(checked);
  ui->label_QL->setVisible(checked);
  ui->lineEdit_QL->setVisible(checked);
}

void dialogRLC::on_checkBox_C_clicked(bool checked)
{
  ui->lineEditSelC->setVisible(checked);
  ui->label_QC->setVisible(checked);
  ui->lineEdit_QC->setVisible(checked);
}

void dialogRLC::on_comboBoxCoaxTypes_currentIndexChanged(const QString &cName)
{
  //disable/enable, hide/show and fill in what needs to be done for coax cName$
  if (cName=="None" || cName=="")
  {
    ui->lineEdit_R0->setText("50");
    ui->lineEdit_VF->setText("1");
    ui->lineEdit_K1->setText("0");
    ui->lineEdit_K2->setText("0");
    ui->lineEdit_Len->setText("0");
    ui->groupBox_R0->setVisible(false);
  }
  else
  {
    if (cName!="Custom")     //For custom, we don't change the existing data
    {
      float cR0, cVF, cK1, cK2;
      mSAconfig::coax.CoaxGetData(cName, cR0, cVF, cK1, cK2);
      ui->lineEdit_R0->setText(QString::number(cR0));
      ui->lineEdit_VF->setText(QString::number(cVF));
      ui->lineEdit_K1->setText(QString::number(cK1));
      ui->lineEdit_K2->setText(QString::number(cK2));

      ui->lineEdit_R0->setEnabled(false);
      ui->lineEdit_VF->setEnabled(false);
      ui->lineEdit_K1->setEnabled(false);
      ui->lineEdit_K2->setEnabled(false);
    }
    else
    {
      ui->lineEdit_R0->setEnabled(true);
      ui->lineEdit_VF->setEnabled(true);
      ui->lineEdit_K1->setEnabled(true);
      ui->lineEdit_K2->setEnabled(true);
    }
    ui->groupBox_R0->setVisible(true);
  }
}
