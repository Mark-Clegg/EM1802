#include "memory.h"
#include "ui_memory.h"

Memory::Memory(QWidget *parent) :
    QDockWidget{parent},
    ui(new Ui::Memory)
{
    ui->setupUi(this);

    // Dummy porogram - toggle Q
    M[0] = 0x71;
    M[1] = 0x23;
    M[2] = 0x7B;
    M[3] = 0x7A;
    M[4] = 0x00;
    M[5] = 0x00;


    P = new MemProxy(M, ui);

    QTextDocument *Doc = ui->MemoryView->document();
    QFont font = Doc->defaultFont();
    font.setFamily("Courier New");
    Doc->setDefaultFont(font);

    for(int i = 0; i< 65536; i+=16)
    {
        char Line[70];

        sprintf(Line, "%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                i,
                M[i], M[i+1],M[i+2],M[i+3],M[i+4], M[i+5],M[i+6],M[i+7],M[i+8], M[i+9],M[i+10],M[i+11],M[i+12], M[i+13],M[i+14],M[i+15]);
        ui->MemoryView->appendPlainText(Line);
    }
}

void Memory::setPosition(uint16_t Address)
{
    int pos = Address / 16 * 54 + 6 + (Address & 0x0F) * 3;
    QTextCursor C = ui->MemoryView->textCursor();
    C.setPosition(pos);
    C.setPosition(pos+2, QTextCursor::KeepAnchor);
    ui->MemoryView->setTextCursor(C);
}

MemProxy Memory::operator [] (uint16_t i) const
{
    P->setAddress(i);
    return *P;
}

MemProxy& Memory::operator [] (uint16_t i)
{
    P->setAddress(i);
    return *P;
}

// MemProxy "acts" like a memory element, except we have control
// over the Read Write operation, so regions can be set as ROM

MemProxy::MemProxy(uint8_t (&Mem)[65536], Ui::Memory *Ui) : M(Mem), ui(Ui)
{
}

void MemProxy::setAddress(uint16_t A)
{
    Address = A;
}

void MemProxy::operator=(uint8_t value)
{
    M[Address]=value;

    QString hexvalue = QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();

    int pos = Address / 16 * 54 + 6 + (Address & 0x0F) * 3;
    QTextCursor C = ui->MemoryView->textCursor();
    C.setPosition(pos);
    C.setPosition(pos+2, QTextCursor::KeepAnchor);
    ui->MemoryView->setTextCursor(C);

    if(C.hasSelection())
        C.insertText(hexvalue);
}

MemProxy::operator uint8_t()
{
    return M[Address];
}
