#include "uart.h"
#include "ui_uart.h"

UART::UART(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::UART)
{
    ui->setupUi(this);

    ui->DA->setOnColour(Qt::green);
    ui->OE->setOnColour(Qt::red);
    ui->PE->setOnColour(Qt::red);
    ui->FE->setOnColour(Qt::red);
    ui->ES->setOnColour(Qt::green);
    ui->PSI->setOnColour(Qt::green);
    ui->TSRE->setOnColour(Qt::green);
    ui->THRE->setOnColour(Qt::green);

    ui->IE->setOnColour(Qt::green);
    ui->Break->setOnColour(Qt::green);
    ui->TR->setOnColour(Qt::green);

    ui->InterruptFlag->setOnColour(Qt::green);

    SetIndicator(ui->StatusLIndicator, Qt::black);
    SetIndicator(ui->StatusRIndicator, Qt::black);
    SetIndicator(ui->ControlLIndicator, Qt::black);
    SetIndicator(ui->ControlRIndicator, Qt::black);
    ui->TxHR->setLIndicator(Qt::black);
    ui->TxHR->setRIndicator(Qt::black);
    ui->RxHR->setLIndicator(Qt::black);
    ui->RxHR->setRIndicator(Qt::black);
}

void UART::Reset()
{
    *ui->DA = false;
    *ui->OE = false;
    *ui->PE = false;
    *ui->FE = false;
    *ui->ES = false;
    *ui->PSI = false;
    *ui->TSRE = true;
    *ui->THRE = true;

    *ui->TR = false;
    *ui->Break = false;
    *ui->IE = false;

    ui->TxHR->setNibbleCount(2);
    ui->RxHR->setNibbleCount(2);

    SetQ(false);

    *ui->InterruptFlag = false;
}

void UART::SetIndicator(QLabel *Label, Qt::GlobalColor c)
{
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, c);
    Label->setAutoFillBackground(true);
    Label->setPalette(pal);
}

void UART::SetQ(bool Q)
{
    RSel = Q;

    if(RSel)
    {
        SetIndicator(ui->StatusLIndicator, Qt::green);
        SetIndicator(ui->StatusRIndicator, Qt::green);
        SetIndicator(ui->ControlLIndicator, Qt::red);
        SetIndicator(ui->ControlRIndicator, Qt::red);
        ui->TxHR->setLIndicator(Qt::black);
        ui->TxHR->setRIndicator(Qt::black);
        ui->RxHR->setLIndicator(Qt::black);
        ui->RxHR->setRIndicator(Qt::black);
}
    else
    {
        SetIndicator(ui->StatusLIndicator, Qt::black);
        SetIndicator(ui->StatusRIndicator, Qt::black);
        SetIndicator(ui->ControlLIndicator, Qt::black);
        SetIndicator(ui->ControlRIndicator, Qt::black);
        ui->TxHR->setLIndicator(Qt::red);
        ui->TxHR->setRIndicator(Qt::red);
        ui->RxHR->setLIndicator(Qt::green);
        ui->RxHR->setRIndicator(Qt::green);
    }
}

void UART::RxChar(QChar c)
{
    if(*ui->DA)
        *ui->OE = true;
    *ui->RxHR = c.toLatin1();
    *ui->DA = true;
    if(*ui->IE & !*ui->InterruptFlag)
    {
        *ui->InterruptFlag = true;
        emit Interrupt(true);
    }
}

void UART::Read(uint8_t & d)
{
    if(RSel)   // Read Status Register
    {
        d = 0;
        d |= *ui->THRE ? 0x80 : 0;
        d |= *ui->TSRE ? 0x40 : 0;
        d |= *ui->PSI ? 0x20 : 0;
        d |= *ui->ES ? 0x10 : 0;
        d |= *ui->FE ? 0x08 : 0;
        d |= *ui->PE ? 0x04 : 0;
        d |= *ui->OE ? 0x02 : 0;
        d |= *ui->DA ? 0x01 : 0;
        if(*ui->IE && *ui->InterruptFlag)
        {
            *ui->InterruptFlag = false;
            emit Interrupt(false);
        }
    }
    else    // Read Receive Holding Register
    {
        d = *ui->RxHR;
        *ui->DA = false;
        *ui->OE = false;
    }
}

void UART::Write(uint8_t d)
{
    static QString BitString[8] = { ",5,1", ",5,1.5", ",6,1", ",6,2", ",7,1", ",7,2", ",8,1", ",8,2" };
    static QString ParityString[3] = { "N", "O", "E" };

    if(RSel)   // Write Control Register
    {
        if(!*ui->TR || (d & 0x80) == 0x00)
        {
            if((d & 0x01) == 1)
                Parity = ParityFlag::None;
            else
                Parity = ((d & 0x02) == 2) ? ParityFlag::Even : ParityFlag::Odd;

            ui->Format->setText("9600," + ParityString[Parity] + BitString[(d & 0x1C) >> 2]);

            *ui->IE = (d & 0x20) == 0x20;
            *ui->Break = (d & 0x40) == 0x40;
            *ui->TR = (d & 0x80) == 0x80;

            if(!*ui->IE && *ui->InterruptFlag)
            {
                *ui->InterruptFlag = false;
                emit Interrupt(false);
            }
        }
    }
    else    // Write Transmit Holding Register
    {
        *ui->THRE = false;
        *ui->TxHR = d;
        emit TxChar(d);
        *ui->THRE = true;
        if(*ui->IE && *ui->InterruptFlag)
        {
            *ui->InterruptFlag = false;
            emit Interrupt(false);
        }
    }
}

UART::~UART()
{
    delete ui;
}
