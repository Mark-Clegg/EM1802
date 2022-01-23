#include "memory.h"
#include "ui_memory.h"

Memory::Memory(QWidget *parent) :
    QDockWidget{parent},
    ui(new Ui::Memory)
{
    ui->setupUi(this);

    // Dummy porogram - toggle Q
    M[0] = 0x7B;
    M[1] = 0x7A;
    M[2] = 0x30;
    M[3] = 0x00;

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

void Memory::LoadFile(QString FileName)
{
    QFileInfo fi(FileName);
    QString Extension = fi.completeSuffix().toLower();

    QFile file(FileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("Unable to read file.");
        msgBox.exec();
        return;
    }

    if(Extension == "hex")
    {
        LoadIntelHex(file);
    }
    file.close();
}

void Memory::LoadIntelHex(QFile &File)
{
    int Lines = 0;
    int InvalidLines = 0;
    int ChecksumErrors = 0;
    int UnsupportedRecords = 0;

    QRegularExpression Parser("^:(?<bytes>[0-9A-F]{2})(?<address>[0-9A-F]{4})(?<type>[0-9A-F]{2})(?<data>(?:[0-9A-F]{2})+)(?<checksum>[0-9A-F]{2})$");
    QTextStream in(&File);
    while (!in.atEnd()) {
        Lines++;
        QString line = in.readLine().toUpper();
        QRegularExpressionMatch Match = Parser.match(line);
        if(Match.hasMatch())
        {
            QVector<uint8_t> Data;

            int Bytes = Match.captured("bytes").toUInt(nullptr,16);
            int Checksum = Bytes;
            uint16_t Address = Match.captured("address").toUInt(nullptr,16);
            Checksum += (Address & 0xFF) + ((Address & 0xFF00) >> 8);
            int Type = Match.captured("type").toUInt(nullptr,16);
            Checksum += Type;
            QString DataSegment = Match.captured("data");
            for(int i = 0; i < Bytes; i++)
            {
                QStringRef DataByte(&DataSegment, i * 2, 2);
                uint8_t Byte = DataByte.toUInt(nullptr,16);
                Data.append(Byte);
                Checksum += Byte;
            }
            int ExpectedChecksum = Match.captured("checksum").toUInt(nullptr,16);
            Checksum = -Checksum & 0xFF;
            if(Checksum == ExpectedChecksum)
            {
                switch(Type)
                {
                case 00: // Data record

                    for(int i=0; i < Bytes; i++)
                        (*this)[Address+i]=Data[i];
                    break;

                case 01: // EOF

                    break;

                default:

                    UnsupportedRecords++;
                    break;
                }
            }
            else
                ChecksumErrors ++;
        }
        else
            InvalidLines++;
    }
    QMessageBox msgBox;
    msgBox.setText("File Load Statistics.");
    msgBox.setInformativeText(QString("Lines: %1\nInvalid Lines: %2\nChecksun Errors: %3\nUnsupported Records: %4")
                              .arg(Lines)
                              .arg(InvalidLines)
                              .arg(ChecksumErrors)
                              .arg(UnsupportedRecords));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

Memory::~Memory()
{
    delete ui;
    delete P;
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
