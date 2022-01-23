#include "uart.h"
#include "ui_uart.h"

UART::UART(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::UART)
{
    ui->setupUi(this);

    ui->RSel->setOnColour(Qt::green);
    ui->RSel->setOffColour(Qt::black);

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

    *ui->RSel = false;
}

void UART::SetQ(bool Q)
{
    *ui->RSel = Q;
}

void UART::RxChar(QChar c)
{
    if(*ui->DA)
        *ui->OE = true;
    *ui->RxHR = c.toLatin1();
    *ui->DA = true;
    if(*ui->IE)
        emit Interrupt();
}

void UART::Read(uint8_t & d)
{
    if(*ui->RSel)   // Read Status Register
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

    if(*ui->RSel)   // Write Control Register
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
        }
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
