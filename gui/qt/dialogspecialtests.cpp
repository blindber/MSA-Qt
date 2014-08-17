#include "dialogspecialtests.h"
#include "ui_dialogspecialtests.h"

dialogSpecialTests::dialogSpecialTests(QObject *dad, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogSpecialTests)
{
  ui->setupUi(this);

  ui->dds1out->setValidator(new QDoubleValidator(this));
  ui->dds3out->setValidator(new QDoubleValidator(this));
  ui->masclkf->setValidator(new QDoubleValidator(this));

  connect(this, SIGNAL(setSwitches(int)), dad, SLOT(setSwitches(int)));
  connect(this, SIGNAL(CommandFilterSlimCBUSB(int)), dad, SLOT(CommandFilterSlimCBUSB(int)));
  connect(this, SIGNAL(setDDS1(double)), dad, SLOT(setDDS1(double)));
}

dialogSpecialTests::~dialogSpecialTests()
{
  delete ui;
}

void dialogSpecialTests::on_DDS1_clicked()
{
  double freq;
  freq = ui->dds1out->text().toDouble();
  setDDS1(freq);
}

void dialogSpecialTests::on_DDS3_clicked()
{
  //this will recalculate DDS3, using the values in the Command DDS 3 Box, and "with DDS Clock at" Box.
      //it will insert the new DDS 3 frequency into the command arrays for all steps, leaving others alone
      //it will initiate a re-command at thisstep (where the sweep was halted)
        //only the DDS 3 is re-commanded
      //using One Step or Continue will retain the new DDS3 frequency.
      //PLO3 will be non-functional until [Restart] button is clicked. PLL3 will break lock and "slam" to extreme.
      //[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
      //Signal Generator or Tracking Generator output will be non functional.
      //Spectrum Analyzer function is not effected
      //caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
      //print #special.dds3out, "!contents? dds3out$";   //grab contents of Command DDS 3 Box
      //ddsoutput = val(dds3out$) //intended output frequency of DDS 3
      double ddsoutput = ui->dds3trktxt->text().toDouble();
      ui->dds3trktxt->setText(QString::number(ddsoutput));
      //print #special.masclkf, "!contents? msclk$";   //grab contents of "with DDS Clock at" box
      //msclk = val(msclk$) //if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
      double msclk = ui->masclkf->text().toDouble();
      double ddsclock = msclk;
      //caution: if ddsoutput >= to .5 ddsclock, the program will error out
      /*
      gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
      remember = thisstep //remember where we were when entering this subroutine
      for thisstep = 0 to steps
      gosub [FillDDS3array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
      next thisstep
      thisstep = remember
      gosub [CreateCmdAllArray]
      if cb = 0 then gosub [CommandDDS3OrigCB]//will command DDS 3, only
  //delver113-4a    if cb = 2 then gosub [CommandDDS3SlimCB]//will command DDS 3, only
      if cb = 2 then gosub [CommandAllSlims]//will command all 4 modules. ver113-4a
      if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules. ver113-4a //USB:01-08-2010
      */
}

void dialogSpecialTests::on_dds3track_clicked()
{
  /*
  //This uses DDS3 as a Tracking Generator, but is limited to 0 to 32 MHz, when MasterClock is 64 MHz
  //DDS3 spare output is rich in harmonics and aliases.
  //Tracks the values in Working Window, Center Frequency and Sweep Width (already in the command arrays)
  //The Spectrum Analyzer function is not effected.
  //PLO3, Normal Tracking Generator, and Phase portion of VNA will be non-functional
  //Operation:
  //In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
  //In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
  //Click [Restart], then halt.
  //In Special Tests Window, click [DDS 3 Track].  DDS 3 will, immediately, re-command to new frequency.
  //Click [Continue]. Sweep will resume, but with DDS 3 tracking the Spectrum Analalyzer
  //[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
  //[Restart] will reset arrays, and leave the DDS 3 Track Mode. ie, normal sweeping.
  ddsclock = masterclock
  remember = thisstep
  for thisstep = 0 to steps
  ddsoutput = datatable(thisstep,1)
  //caution: if ddsoutput >= to .5 ddsclock, the program will error out
  gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
  gosub [FillDDS3array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
  next thisstep
  thisstep = remember
  gosub [CreateCmdAllArray]
  if cb = 0 then gosub [CommandDDS3OrigCB]//will command DDS 3, only
  if cb = 2 then gosub [CommandAllSlims]//will command all 4 modules. ver113-4a
  if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules. //USB:01-08-2010
   */
}

void dialogSpecialTests::on_dds1track_clicked()
{
  /*
  //This forces the DDS 1 to the values in Working Window: Center Frequency and Sweep Width (already in the command arrays)
      //DDS1 spare output is rich in harmonics and aliases.
      //PLO1, and thus, the Spectrum Analyzer will be non-functional in this mode.
      //Signal Generator or Tracking Generator output will not be affected.
      //Operation:
      //In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
      //In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
      //Click [Restart], then halt.
      //In Special Tests Window, click [DDS 1 Sweep].  DDS 1 will, immediately, re-command to new frequency.
      //Click [Continue]. Sweep will resume, but with DDS 1 sweeping.
      //[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
      //[Restart] will reset arrays, and will leave the DDS 1 Sweep Mode. ie, normal sweeping.
      ddsclock = masterclock
      remember = thisstep
      for thisstep = 0 to steps
      ddsoutput = datatable(thisstep,1)
      //caution: if ddsoutput >= to .5 ddsclock, the program will error out
      gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
      gosub [FillDDS1array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
      next thisstep
      thisstep = remember
      gosub [CreateCmdAllArray]
      if cb = 0 then gosub [CommandDDS1OrigCB]//will command DDS 1, only
      if cb = 2 then gosub [CommandAllSlims]//will command all 4 modules. ver113-4a
      if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules.  //USB:01-08-2010 moved ver116-4f
     */
}

void dialogSpecialTests::on_pdminv_clicked()
{

}

void dialogSpecialTests::on_insert_clicked()
{

}

void dialogSpecialTests::on_prevnalin_clicked()
{

}

void dialogSpecialTests::on_lpttest_clicked()
{

}

void dialogSpecialTests::on_cftest_clicked()
{

}

void dialogSpecialTests::on_pushButtonSetSwitches_clicked()
{
  //bit 0    VS0   Video Filter Address, low order bit
  //bit 1    VS1   Video Filter Address, high order bit
  //bit 2    BS0   Band Selection, low order bit
  //bit 3    BS1   Band Selection, high order bit
  //bit 4    FR    DUT Direction, Forward (0) or Reverse
  //bit 5    TR    VNA Selection, Transmission (0) or Reflection
  //bit 6   Spare
  //bit 7    PS    Pulse Start (a/k/a Latch Pulse), common to all latching   relays.

  int switches = 0x80; // latch bit starts high

  if (ui->radioButton_1G->isChecked())
  {
    switches |= 0x00;
  }
  else if (ui->radioButton_2G->isChecked())
  {
    switches |= 0x0C;
  }
  else if (ui->radioButton_3G->isChecked())
  {
    switches |= 0x00;
  }

  if (ui->radioButton_VW->isChecked())
  {
    switches |= 0x00;
  }
  else if (ui->radioButton_VM->isChecked())
  {
    switches |= 0x01;
  }
  else if (ui->radioButton_VN->isChecked())
  {
    switches |= 0x02;
  }
  else if (ui->radioButton_VX->isChecked())
  {
    switches |= 0x03;
  }

  if (ui->checkBoxFor->isChecked())
  {
    switches |= 0x10;
  }
  if (ui->checkBoxTrans->isChecked())
  {
    switches |= 0x20;
  }

  setSwitches(switches);
}

void dialogSpecialTests::on_pushButtonSelectFilter_clicked()
{
  int fbank = 0;

  if (ui->radioButton_RBW1->isChecked())
  {
    fbank |= 0x00;
  }
  else if (ui->radioButton_RBW2->isChecked())
  {
    fbank |= 0x20;
  }
  else if (ui->radioButton_RBW3->isChecked())
  {
    fbank |= 0x40;
  }
  else if (ui->radioButton_RBW4->isChecked())
  {
    fbank |= 0x60;
  }

  CommandFilterSlimCBUSB(fbank);
}
