#include "ide.h"
#include "ui_ide.h"
#include <QThread>
#include <QtConcurrent>

IDE::IDE(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::IDE)
{
    ui->setupUi(this);

    Command      = ui->Command;      Command->setStyle(2, 80, 50, Qt::AlignRight);
    Data         = ui->Data;         Data->setStyle(2, 80, 50, Qt::AlignRight);
    SectorCount  = ui->SectorCount;  SectorCount->setStyle(2, 100, 50, Qt::AlignRight);
    StartSector  = ui->StartSector;  StartSector->setStyle(2, 100, 50, Qt::AlignRight);
    CylinderHigh = ui->CylinderHigh; CylinderHigh->setStyle(2, 100, 50, Qt::AlignRight);
    CylinderLow  = ui->CylinderLow;  CylinderLow->setStyle(2, 100, 50, Qt::AlignRight);
    Head         = ui->Head;         Head->setStyle(2, 100, 50, Qt::AlignRight);

    ui->ErrorBBK->setOnColour(Qt::red);
    ui->ErrorUNC->setOnColour(Qt::red);
    ui->ErrorMC->setOnColour(Qt::red);
    ui->ErrorIDNF->setOnColour(Qt::red);
    ui->ErrorMCR->setOnColour(Qt::red);
    ui->ErrorABRT->setOnColour(Qt::red);
    ui->ErrorTZNF->setOnColour(Qt::red);
    ui->ErrorAMNF->setOnColour(Qt::red);

    ui->StatusBSY->setOnColour(Qt::green);
    ui->StatusDRDY->setOnColour(Qt::green);
    ui->StatusDF->setOnColour(Qt::red);
    ui->StatusDSC->setOnColour(Qt::green);
    ui->StatusDRQ->setOnColour(Qt::green);
    ui->StatusCORR->setOnColour(Qt::green);
    ui->StatusIDX->setOnColour(Qt::red);
    ui->StatusERR->setOnColour(Qt::red);

    ui->Feature8Bit->setOnColour(Qt::green);
    ui->ControlReset->setOnColour(Qt::red);
    ui->ControlNIEN->setOnColour(Qt::red);
    ui->ControlNIEN->setOffColour(Qt::green);

    ui->Interrupt->setOnColour(Qt::green);
    ui->Master->setOnColour(Qt::green);

    ui->FileName->setText("-- No Disk --");
}

QMap<uint8_t, QString> const IDE::CommandNameLookup = QMap<uint8_t,QString>
{
    { 0x10, "Recalibrate" },
    { 0x11, "Recalibrate" },
    { 0x12, "Recalibrate" },
    { 0x13, "Recalibrate" },
    { 0x14, "Recalibrate" },
    { 0x15, "Recalibrate" },
    { 0x16, "Recalibrate" },
    { 0x17, "Recalibrate" },
    { 0x18, "Recalibrate" },
    { 0x19, "Recalibrate" },
    { 0x1A, "Recalibrate" },
    { 0x1B, "Recalibrate" },
    { 0x1C, "Recalibrate" },
    { 0x1D, "Recalibrate" },
    { 0x1E, "Recalibrate" },
    { 0x1F, "Recalibrate" },
    { 0x20, "Read Sectors" },
    { 0x24, "Read Sectors Ext" },
    { 0x30, "Write Sectors" },
    { 0x34, "Write Sectors Ext" },
    { 0x70, "Seek" },
    { 0x71, "Seek" },
    { 0x72, "Seek" },
    { 0x73, "Seek" },
    { 0x74, "Seek" },
    { 0x75, "Seek" },
    { 0x76, "Seek" },
    { 0x77, "Seek" },
    { 0x78, "Seek" },
    { 0x79, "Seek" },
    { 0x7A, "Seek" },
    { 0x7B, "Seek" },
    { 0x7C, "Seek" },
    { 0x7D, "Seek" },
    { 0x7E, "Seek" },
    { 0x7F, "Seek" },
    { 0xE0, "Spin Down" },
    { 0xE1, "Spin Up" },
    { 0xE2, "Auto Power Down (5s)" },
    { 0xE3, "Auto Power Down (5s)" },
    { 0xEC, "Identify" },
    { 0xEF, "Set Features" },
    { 0xF2, "Auto Power Down (0.1s)" },
    { 0xF3, "Auto Power Down (0.1s)" }
};

void IDE::Reset()
{
    ReadMode = false;
    WriteMode = false;

    if(DiskFile == nullptr)
    {
        *ui->ErrorBBK   = true;
        *ui->ErrorUNC   = true;
        *ui->ErrorMC    = true;
        *ui->ErrorIDNF  = true;
        *ui->ErrorMCR   = true;
        *ui->ErrorABRT  = true;
        *ui->ErrorTZNF  = true;
        *ui->ErrorAMNF  = true;

        *ui->StatusBSY  = true;
        *ui->StatusDRDY = true;
        *ui->StatusDF   = true;
        *ui->StatusDSC  = true;
        *ui->StatusDRQ  = true;
        *ui->StatusCORR = true;
        *ui->StatusIDX  = true;
        *ui->StatusERR  = true;
    }
    else
    {
        *ui->ErrorBBK   = false;
        *ui->ErrorUNC   = false;
        *ui->ErrorMC    = false;
        *ui->ErrorIDNF  = false;
        *ui->ErrorMCR   = false;
        *ui->ErrorABRT  = false;
        *ui->ErrorTZNF  = false;
        *ui->ErrorAMNF  = true;

        *ui->StatusBSY  = false;
        *ui->StatusDRDY = true;
        *ui->StatusDF   = false;
        *ui->StatusDSC  = true;
        *ui->StatusDRQ  = false;
        *ui->StatusCORR = false;
        *ui->StatusIDX  = false;
        *ui->StatusERR  = false;
    }

    ResetIndicators();
}

void IDE::ResetIndicators()
{
    SetIndicator(ui->StatusLIndicator, Qt::black);
    SetIndicator(ui->StatusRIndicator, Qt::black);
    SetIndicator(ui->ErrorLIndicator, Qt::black);
    SetIndicator(ui->ErrorRIndicator, Qt::black);
    SetIndicator(ui->FeaturesLIndicator, Qt::black);
    SetIndicator(ui->FeaturesRIndicator, Qt::black);
    SetIndicator(ui->DeviceControlLIndicator, Qt::black);
    SetIndicator(ui->DeviceControlRIndicator, Qt::black);
    SetIndicator(ui->ModeLIndicator, Qt::black);
    SetIndicator(ui->ModeRIndicator, Qt::black);

    SectorCount->setLIndicator(Qt::black);
    SectorCount->setRIndicator(Qt::black);
    StartSector->setLIndicator(Qt::black);
    StartSector->setRIndicator(Qt::black);
    CylinderLow->setLIndicator(Qt::black);
    CylinderLow->setRIndicator(Qt::black);
    CylinderHigh->setLIndicator(Qt::black);
    CylinderHigh->setRIndicator(Qt::black);
    Head->setLIndicator(Qt::black);
    Head->setRIndicator(Qt::black);
    Command->setLIndicator(Qt::black);
    Command->setRIndicator(Qt::black);
    Data->setLIndicator(Qt::black);
    Data->setRIndicator(Qt::black);
}

void IDE::SetIndicator(QLabel *Label, Qt::GlobalColor c)
{
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, c);
    Label->setAutoFillBackground(true);
    Label->setPalette(pal);
}

void IDE::RegisterSelect(uint8_t R)
{
    // OUT P2

    RegisterNumber = (IDE_Register)R;

    ResetIndicators();
    switch(RegisterNumber)
    {
    case 0:
        Data->setLIndicator(Qt::green);
        Data->setRIndicator(Qt::red);
        break;
    case 1:
        SetIndicator(ui->ErrorLIndicator, Qt::green);
        SetIndicator(ui->ErrorRIndicator, Qt::green);
        SetIndicator(ui->FeaturesLIndicator, Qt::red);
        SetIndicator(ui->FeaturesRIndicator, Qt::red);
        break;
    case 2:
        SectorCount->setLIndicator(Qt::green);
        SectorCount->setRIndicator(Qt::red);
        break;
    case 3:
        StartSector->setLIndicator(Qt::green);
        StartSector->setRIndicator(Qt::red);
        break;
    case 4:
        CylinderLow->setLIndicator(Qt::green);
        CylinderLow->setRIndicator(Qt::red);
        break;
    case 5:
        CylinderHigh->setLIndicator(Qt::green);
        CylinderHigh->setRIndicator(Qt::red);
        break;
    case 6:
        Head->setLIndicator(Qt::green);
        Head->setRIndicator(Qt::red);
        SetIndicator(ui->ModeLIndicator, Qt::green);
        SetIndicator(ui->ModeRIndicator, Qt::red);
        break;
    case 7:
        Command->setLIndicator(Qt::red);
        Command->setRIndicator(Qt::red);
        SetIndicator(ui->StatusLIndicator, Qt::green);
        SetIndicator(ui->StatusRIndicator, Qt::green);
        break;
    case 0xE:
        SetIndicator(ui->DeviceControlLIndicator, Qt::red);
        SetIndicator(ui->DeviceControlRIndicator, Qt::red);
        SetIndicator(ui->StatusLIndicator, Qt::green);
        SetIndicator(ui->StatusRIndicator, Qt::green);
        break;
    default:
        break;
    }
}

void IDE::Read(uint8_t & Value)
{
    // INP P1

    if(DiskFile == nullptr)
        Value = 0xFF;
    else
    {
        switch(RegisterNumber)
        {
        case IDE_Reg_Data:

            if(ReadMode)
            {
                Value = SectorBuffer[BytePointer++];
                if(BytePointer == 512)
                {
                    BytePointer = 0;
                    NumSectors--;
                    if(NumSectors == 0)
                    {
                        ReadMode = false;
                        *ui->StatusDRQ = false;
                        *ui->StatusBSY = false;
                        *ui->StatusDRDY = true;
                    }
                    else
                    {
                        // Read next sector...
                    }
                }
            }
            else
                emit Break("Reading Data register when no Data Read in progress");
            break;

        case IDE_Reg_Error:

            Value = (*ui->ErrorBBK  ? 0 : 0x80)
                  | (*ui->ErrorUNC  ? 0 : 0x40)
                  | (*ui->ErrorMC   ? 0 : 0x20)
                  | (*ui->ErrorIDNF ? 0 : 0x10)
                  | (*ui->ErrorMCR  ? 0 : 0x08)
                  | (*ui->ErrorABRT ? 0 : 0x04)
                  | (*ui->ErrorTZNF ? 0 : 0x02)
                  | (*ui->ErrorAMNF ? 0 : 0x01);
            break;

        case IDE_Reg_SectorCount:

            Value = SectorCount->value();
            break;

        case IDE_Reg_StartSector:

            Value = StartSector->value();
            break;

        case IDE_Reg_CylinderLow:

            Value = CylinderLow->value();
            break;

        case IDE_Reg_CylinderHigh:

            Value = CylinderHigh->value()
                  | (*ui->Master ? 0 : 0x10)
                  | Mode;
            break;

        case IDE_Reg_HeadDevice:

            Value = Head->value();
            break;

        case IDE_Reg_Status:

            Value = (*ui->StatusBSY  ? 0x80 : 0)
                  | (*ui->StatusDRDY ? 0x40 : 0)
                  | (*ui->StatusDF   ? 0x20 : 0)
                  | (*ui->StatusDSC  ? 0x10 : 0)
                  | (*ui->StatusDRQ  ? 0x08 : 0)
                  | (*ui->StatusCORR ? 0x04 : 0)
                  | (*ui->StatusIDX  ? 0x02 : 0)
                  | (*ui->StatusERR  ? 0x01 : 0);
            *ui->Interrupt = false;
            break;

        case IDE_Reg_AltStatus:

            Value = (*ui->StatusBSY  ? 0x80 : 0)
                  | (*ui->StatusDRDY ? 0x40 : 0)
                  | (*ui->StatusDF   ? 0x20 : 0)
                  | (*ui->StatusDSC  ? 0x10 : 0)
                  | (*ui->StatusDRQ  ? 0x08 : 0)
                  | (*ui->StatusCORR ? 0x04 : 0)
                  | (*ui->StatusIDX  ? 0x02 : 0)
                  | (*ui->StatusERR  ? 0x01 : 0);
            break;

        case IDE_Reg_ActiveStatus:
        default:
            emit Break(QString("Attempted to read unsupported IDE Register: %1").arg(RegisterNumber));
            break;
        }
    }
}

void IDE::Write(uint8_t Value)
{
    // OUT P1
    if(DiskFile != nullptr)
    {
        switch(RegisterNumber)
        {
        case IDE_Reg_Data:

            if(WriteMode)
            {
                SectorBuffer[BytePointer++] = Value;
                if(BytePointer == 512)
                {
                    *ui->StatusBSY = true;
                    emit WriteSector(DiskFile, SectorBuffer, GetFilePointer(Mode));
                }
             }
            else
                emit Break("Writing Data register when no Data Write in progress");
            break;

        case IDE_Reg_Feature:

            *ui->Feature8Bit = (Value & 0x01) == 0x01;
            break;

        case IDE_Reg_SectorCount:

            SectorCount->setValue(Value);
            break;

        case IDE_Reg_StartSector:

            StartSector->setValue(Value);
            break;

        case IDE_Reg_CylinderLow:

            CylinderLow->setValue(Value);
            break;

        case IDE_Reg_CylinderHigh:

            CylinderHigh->setValue(Value);
            break;

        case IDE_Reg_HeadDevice:
        {
            Head->setValue(Value & 0x0F);
            *ui->Master = (Value & 0x10) == 0;
            Mode = (Value & 0xE0);
            switch(Mode)
            {
            case IDE_MODE_LBA28: ui->Mode->setText("LBA 28");  break;
            case IDE_MODE_CHS:   ui->Mode->setText("CHS");     break;
            case IDE_MODE_LBA48: ui->Mode->setText("LBA 48");  break;
            default:             ui->Mode->setText("Unknown"); break;
            }
            break;
        }

        case IDE_Reg_Command:
        {
            Command->setValue(Value);

            if(CommandNameLookup.contains(Value))
                ui->CommandName->setText((CommandNameLookup[Value]));
            else
                ui->CommandName->setText("");

            switch(Value)
            {
            case 0x20: // Read Sector
            {
                *ui->StatusBSY = true;
                if(EightBitMode)
                {
                    switch(Mode)
                    {
                    case IDE_MODE_CHS:
                    case IDE_MODE_LBA28:
                    {
                        *ui->StatusBSY = true;
                        NumSectors = *ui->SectorCount;
                        emit ReadSector(DiskFile, SectorBuffer, GetFilePointer(Mode));
                        break;
                    }

                    case IDE_MODE_LBA48:
                    {
                        emit Break("LBA48 mode not supported");
                        break;
                    }
                    }
                }
                else
                    emit Break("16 bit operations not supported");
                break;
            }

            case 0x30: // Write Sector
            {
                ui->CommandName->setText("Write Sector");
                if(EightBitMode)
                {
                    switch(Mode)
                    {
                    case IDE_MODE_CHS:
                    case IDE_MODE_LBA28:
                    {
                        *ui->StatusDRQ = true;
                        WriteMode = true;
                        BytePointer = 0;
                        break;
                    }

                    case IDE_MODE_LBA48:
                    {
                        emit Break("LBA48 mode not supported");
                        break;
                    }
                    }
                }
                else
                    emit Break("16 bit operations not supported");
                break;
            }

            case 0xEC: // Identify
            {
                ui->CommandName->setText("Identify");
                if(EightBitMode)
                {
                    *ui->StatusBSY = true;

                    // Blank the sector buffer
                    for(int i=0; i < 512; i++)
                        SectorBuffer[i] = 0;

                    // Write C/H/S, LBA Count and FileName into sector buffer

                    SectorBuffer[  2] = CylinderCount & 0xFF;
                    SectorBuffer[  3] = CylinderCount >> 8;
                    SectorBuffer[  6] = 16;
                    SectorBuffer[  7] = 0;
                    SectorBuffer[  2] = 63;
                    SectorBuffer[ 13] = 0;
                    SectorBuffer[120] = LBASectorCount & 0xFF;
                    SectorBuffer[121] = (LBASectorCount >> 8) & 0xFF;
                    SectorBuffer[122] = (LBASectorCount >> 16) & 0xFF;
                    SectorBuffer[123] = (LBASectorCount >> 24) & 0xFF;

                    int i;
                    for(i=0; i < FileName.size() && i< 39; i++)
                        SectorBuffer[54 + (i ^ 1)] = FileName[i].cell();
                    SectorBuffer[54 + (i ^ 1)] = 0;

                    ReadMode = true;
                    WriteMode = false;
                    BytePointer = 0;
                    NumSectors = 1;

                    *ui->StatusBSY = false;
                    *ui->StatusDRDY = true;
                    *ui->StatusDSC = true;
                    *ui->StatusDRQ = true;
                }
                else
                    emit Break("16 bit operations not supported");
                break;
            }

            case 0xEF: // Set Features
            {
                EightBitMode = *ui->Feature8Bit;
                break;
            }

            default:
            {
                emit Break(QString("IDE Command not implemented: %1").arg(Value));
                break;
            }
            }
            break;
        }

        case IDE_Reg_IRQReset:
        {
            *ui->ControlNIEN = (Value & 0x02) != 0;
            *ui->ControlReset = (Value & 0x04) != 0;
            Reset();
            *ui->ControlReset = false;
            break;
        }

        default:
        {
            emit Break(QString("Attempted to write unsupported IDE Register: %1").arg(RegisterNumber));
            break;
        }
        }
    }
}

void IDE::Mount(QString FileName)
{
    if(DiskFile != nullptr)
        UnMount();
    DiskFile = new QFile(FileName);
    if(DiskFile->open(QIODevice::ReadWrite))
    {
        // LBA 28 Mode
        LBASectorCount = DiskFile->size() / 512;

        // CHS Mode
        CylinderCount = DiskFile->size() / (63 * 16 * 512);
        if(CylinderCount > 1023)
            CylinderCount = 1023;

        this->FileName = QFileInfo(FileName).fileName();
        ui->FileName->setText(this->FileName);

        *ui->ErrorBBK   = false;
        *ui->ErrorUNC   = false;
        *ui->ErrorMC    = false;
        *ui->ErrorIDNF  = false;
        *ui->ErrorMCR   = false;
        *ui->ErrorABRT  = false;
        *ui->ErrorTZNF  = false;
        *ui->ErrorAMNF  = true;

        *ui->StatusBSY  = false;
        *ui->StatusDRDY = true;
        *ui->StatusDF   = false;
        *ui->StatusDSC  = true;
        *ui->StatusDRQ  = false;
        *ui->StatusCORR = false;
        *ui->StatusIDX  = false;
        *ui->StatusERR  = false;

        BackgroundWorker = new IDE_BackgroundWorker(this);
        BackgroundThread = new QThread();
        BackgroundWorker->moveToThread(BackgroundThread);
        FutureWatcher = new QFutureWatcher<bool>();

        connect(this, &IDE::ReadSector,  BackgroundWorker, &IDE_BackgroundWorker::ReadSector);
        connect(BackgroundWorker, &IDE_BackgroundWorker::ReadComplete, this, &IDE::ReadComplete);

        connect(this, &IDE::WriteSector, BackgroundWorker, &IDE_BackgroundWorker::WriteSector);
        connect(BackgroundWorker, &IDE_BackgroundWorker::WriteComplete, this, &IDE::WriteComplete);

        BackgroundThread->start();
    }
}

void IDE::UnMount()
{
    LBASectorCount = 0;
    FileName = "";
    if(DiskFile != nullptr)
    {
        DiskFile->close();
        delete DiskFile;
        DiskFile = nullptr;

        BackgroundWorker->deleteLater();
        BackgroundThread->quit();
        BackgroundThread->wait();
        delete BackgroundThread;
        delete FutureWatcher;
    }
    ui->FileName->setText("-- No Disk --");

    *ui->ErrorBBK   = true;
    *ui->ErrorUNC   = true;
    *ui->ErrorMC    = true;
    *ui->ErrorIDNF  = true;
    *ui->ErrorMCR   = true;
    *ui->ErrorABRT  = true;
    *ui->ErrorTZNF  = true;
    *ui->ErrorAMNF  = true;

    *ui->StatusBSY  = true;
    *ui->StatusDRDY = true;
    *ui->StatusDF   = true;
    *ui->StatusDSC  = true;
    *ui->StatusDRQ  = true;
    *ui->StatusCORR = true;
    *ui->StatusIDX  = true;
    *ui->StatusERR  = true;
}

void IDE::ReadComplete(bool)
{
    *ui->StatusBSY = false;
    *ui->StatusDRQ = true;
    BytePointer = 0;
    ReadMode = true;
}

void IDE::WriteComplete(bool)
{
    *ui->StatusBSY = false;
    *ui->StatusDRQ = false;
    WriteMode = false;
}

long IDE::GetFilePointer(int Mode)
{
    switch(Mode)
    {
    case IDE_MODE_CHS:

        if(StartSector->value() == 0)
            emit Break(QString("Sector 0 is invalid in CHS mode."));
        return (((CylinderHigh->value() << 8) + CylinderLow->value()) * 16 + Head->value()) * 63 + (StartSector->value() - 1);

    case IDE_MODE_LBA28:

        return (CylinderHigh->value() << 24) + (CylinderLow->value() << 16) + (Head->value() << 8) + StartSector->value();
    }
    return 0;
}

IDE::~IDE()
{
    if(DiskFile != nullptr)
        UnMount();
    delete ui;
}
