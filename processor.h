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
    explicit Processor(QWidget *parent = nullptr);
    ~Processor();

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

    void Reset();

signals:
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

public slots:
    void XChanged();
    void PChanged();
    void ExecuteInstruction();
    void RunStop();
    void Load();

private:
    Ui::Processor *ui;

    Memory M;

};

#endif // PROCESSOR_H
