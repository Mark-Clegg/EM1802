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

enum ProcessorType {
    CDP1802,
    CDP1806,
    CDP1806A
};

enum CounterModeEnum {
    CounterModeStopped,
    CounterModeEventCounter1,
    CounterModeEventCounter2,
    CounterModeTimer,
    CounterModePulseMeasure1,
    CounterModePulseMeasure2
};

class Processor : public QDockWidget
{
    Q_OBJECT

public:
    explicit Processor(QWidget *parent, Memory &);
    ~Processor();

    void SetType(enum ProcessorType);

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QTimer *Clock;
    void IllegalInstruction();
    QString Disassemble(uint16_t, int);
    QString DisassemblyLine(const int &, const QStringList &, const QString &, const QStringList &);
    const QString DisassemblyTemplate;
    int LineHeight;
    int DisassemblerLines;

    int ExternalInterrupt = 0;
    bool CounterInterrupt = false;
    bool CounterToggleQ = false;
    int TotalTicks = 0;

    CounterModeEnum CounterMode = CounterModeStopped;
    uint8_t CounterHoldingRegister;
    void DecrementCounter();
    void SetCounterMode(CounterModeEnum);
    void SetCounterInterrupt(bool);

    enum ProcessorType ProcessorType = CDP1802;

    uint8_t BCD_Add(uint8_t, uint8_t, uint16_t &);
    uint8_t BCD_Subtract(uint8_t, uint8_t, uint16_t &);

    Register *R[16];

    Register *D;
    Register *X;
    Register *P;
    Register *T;
    Register *N;
    Register *I;
    Register *DF;
    Register *Counter;

    IndicatorLED *Q;
    IndicatorLED *IE;
    IndicatorLED *XIE;
    IndicatorLED *CIE;
    IndicatorLED *XI;
    IndicatorLED *CI;

    ExternalFlag *EF1;
    ExternalFlag *EF2;
    ExternalFlag *EF3;
    ExternalFlag *EF4;


signals:
    void Reset();
    void Break(QString);

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
    void Interrupt(bool);
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
