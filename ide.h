#ifndef IDE_H
#define IDE_H

#include <QDockWidget>
#include <QFileInfo>
#include <QThread>
#include <QFutureWatcher>
#include "register.h"
#include "ideregister.h"
#include "ide_backgroundworker.h"

#define IDE_MODE_LBA28 0xE0
#define IDE_MODE_LBA48 0x40
#define IDE_MODE_CHS 0xA0

namespace Ui {

class IDE;
}

class IDE : public QDockWidget
{
    Q_OBJECT

    QFile *DiskFile = nullptr;
    long LBASectorCount; // For LBA28 Mode
    int CylinderCount;  // For CHS Mode (Assuming 63 sectors per track, and 16 heads)
    long GetFilePointer(int);

public:
    explicit IDE(QWidget *parent = nullptr);
    ~IDE();
    void Mount(QString);
    void UnMount();

private:
    Ui::IDE *ui;
    IDE_Register RegisterNumber;
    QString FileName = "";
    void ResetIndicators();
    void SetIndicator(QLabel *, Qt::GlobalColor);
    bool EightBitMode = false;
    int Mode;

    IDE_BackgroundWorker *BackgroundWorker;
    QThread *BackgroundThread;
    QFutureWatcher<bool> *FutureWatcher;

    bool ReadMode = false;
    bool WriteMode = false;
    int NumSectors;
    int BytePointer;

    static QMap<uint8_t, QString> const CommandNameLookup;
    Register *Command;
    Register *SectorCount;
    Register *StartSector;
    Register *CylinderHigh;
    Register *CylinderLow;
    Register *Head;
    Register *Data;

    uint8_t SectorBuffer[512];

signals:
    void Break(QString);
    void ReadSector(QFile *, uint8_t[512], long);
    void WriteSector(QFile *, uint8_t[512], long);

    void test();

public slots:
    void Reset();
    void RegisterSelect(uint8_t);
    void ReadComplete(bool);
    void WriteComplete(bool);
    void Read(uint8_t &);
    void Write(uint8_t);
};

#endif // IDE_H
