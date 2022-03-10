#include "ide_backgroundworker.h"

IDE_BackgroundWorker::IDE_BackgroundWorker(QObject *parent)
    : QObject{parent}
{

}

void IDE_BackgroundWorker::ReadSector(QFile *Handle, uint8_t Buffer[215], long SectorNumber)
{
    if(Handle->seek(SectorNumber * 512))
    {
        long ReadCount = Handle->read((char *)(Buffer), 512);
        emit ReadComplete(ReadCount == 512);
    }
    else
        emit ReadComplete(false);
}

void IDE_BackgroundWorker::WriteSector(QFile *Handle, uint8_t Buffer[512], long SectorNumber)
{
    if(Handle->seek(SectorNumber * 512))
    {
        long WriteCount = Handle->write((char *)Buffer, 512);
        emit WriteComplete(WriteCount == 512);
    }
    else
        emit WriteComplete(true);
}
