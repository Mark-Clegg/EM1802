#ifndef MEMORY_H
#define MEMORY_H

#include <QDockWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QStringRef>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

namespace Ui {
class Memory;
}

class MemProxy
{
private:
    uint8_t (&M)[65536];
    uint16_t Address;
    Ui::Memory *ui;

public:
    explicit MemProxy(uint8_t (&Mem)[65536], Ui::Memory *);

    void setAddress(uint16_t);

    void operator=(uint8_t);

    operator uint8_t();
};


class Memory : public QDockWidget
{
    Q_OBJECT

private:
    Ui::Memory *ui;
    uint8_t M[65536];
    MemProxy *P;
    void LoadIntelHex(QFile &);

public:
    explicit Memory(QWidget *parent = nullptr);
    ~Memory();
    MemProxy operator [] (uint16_t) const;
    MemProxy& operator [] (uint16_t);
    void setPosition(uint16_t);

signals:

public slots:
    void LoadFile(QString);

};
#endif // MEMORY_H
