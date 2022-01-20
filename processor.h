#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QDockWidget>
#include <QTimer>
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

    void MasterReset();

signals:
    void Reset();

    void Out1(uint8_t, bool);
    void Out2(uint8_t, bool);
    void Out3(uint8_t, bool);
    void Out4(uint8_t, bool);
    void Out5(uint8_t, bool);
    void Out6(uint8_t, bool);
    void Out7(uint8_t, bool);

    void Inp1(uint8_t &, bool);
    void Inp2(uint8_t &, bool);
    void Inp3(uint8_t &, bool);
    void Inp4(uint8_t &, bool);
    void Inp5(uint8_t &, bool);
    void Inp6(uint8_t &, bool);
    void Inp7(uint8_t &, bool);

private slots:
    void XChanged();
    void PChanged();
    void ExecuteInstruction();
    void RunStop();
    void Load();

public slots:
    void Interrupt();
    void DMAIn(uint8_t);
    void DMAOut(uint8_t &);

private:
    Ui::Processor *ui;
    bool Idle = false;
    Memory &M;

};

#endif // PROCESSOR_H
