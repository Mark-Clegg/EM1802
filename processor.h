#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QDockWidget>
#include <QTimer>
#include <QPlainTextEdit>
#include "register.h"
#include "indicatorled.h"
#include "externalflag.h"
#include "memory.h"

namespace Ui {
class Processor;
}

class Processor : public QDockWidget
{
    Q_OBJECT

public:
    explicit Processor(QWidget *parent, Memory &);
    ~Processor();

private:
    QTimer *Clock;
    QString Disassemble(uint16_t, int);
    QString DisassemblyLine(const int &, const QStringList &, const QString &, const QStringList &);
    const QString DisassemblyTemplate;

    Register *R[16];

    Register *D;
    Register *X;
    Register *P;
    Register *T;
    Register *N;
    Register *I;
    Register *DF;

    IndicatorLED *Q;
    IndicatorLED *IE;

    ExternalFlag *EF1;
    ExternalFlag *EF2;
    ExternalFlag *EF3;
    ExternalFlag *EF4;


signals:
    void Reset();

    void Out1(uint8_t);
    void Out2(uint8_t);
    void Out3(uint8_t);
    void Out4(uint8_t);
    void Out5(uint8_t);
    void Out6(uint8_t);
    void Out7(uint8_t);

    void Inp1(uint8_t &);
    void Inp2(uint8_t &);
    void Inp3(uint8_t &);
    void Inp4(uint8_t &);
    void Inp5(uint8_t &);
    void Inp6(uint8_t &);
    void Inp7(uint8_t &);

    void QSignal(bool);

private slots:
    void XChanged();
    void PChanged();

public slots:
    void Interrupt();
    void DMAIn(uint8_t);
    void DMAOut(uint8_t &);

    void ExecuteInstruction();
    void Run();
    void Stop();
    void Load();
    void MasterReset();

private:
    Ui::Processor *ui;
    bool Idle = false;
    Memory &M;

};

#endif // PROCESSOR_H
