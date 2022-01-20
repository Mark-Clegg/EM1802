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
}

void UART::RxChar(QChar c)
{
    if(*ui->DA)
        *ui->OE = true;
    *ui->RxHR = c.toLatin1();
    *ui->DA = true;
    //emit TxChar(c);
}

void UART::Read(uint8_t & d, bool Q)
{
    if(Q)   // Read Status Register
    {
        d = 0;
        d |= ui->THRE ? 0x80 : 0;
        d |= ui->TSRE ? 0x40 : 0;
        d |= ui->PSI ? 0x20 : 0;
        d |= ui->ES ? 0x10 : 0;
        d |= ui->FE ? 0x08 : 0;
        d |= ui->PE ? 0x04 : 0;
        d |= ui->OE ? 0x02 : 0;
        d |= ui->DA ? 0x01 : 0;
    }
    else    // Read Receive Holding Register
    {
        d = *ui->RxHR;
        *ui->DA = false;
        *ui->OE = false;
    }
}

void UART::Write(uint8_t d, bool Q)
{
    if(Q)   // Write Control Register
    {

    }
    else    // Write Transmit Holding Register
    {
        *ui->THRE = false;
        emit TxChar(d);
        *ui->THRE = true;
    }
}

UART::~UART()
{
    delete ui;
}
