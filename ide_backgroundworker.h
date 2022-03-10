#ifndef IDE_BACKGROUNDWORKER_H
#define IDE_BACKGROUNDWORKER_H

#include <QObject>
#include "QFile"

class IDE_BackgroundWorker : public QObject
{
    Q_OBJECT

public:
    explicit IDE_BackgroundWorker(QObject *parent = nullptr);

public slots:
    void ReadSector(QFile *Handle, uint8_t Buffer[215], long SectorNumber);
    void WriteSector(QFile *Handle, uint8_t Buffer[512], long SectorNumber);

signals:
    void ReadComplete(bool Status);
    void WriteComplete(bool Status);
};

#endif // IDE_BACKGROUNDWORKER_H
