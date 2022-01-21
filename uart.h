#ifndef UART_H
#define UART_H

#include <QDockWidget>

namespace Ui {
class UART;
}

class UART : public QDockWidget
{
    Q_OBJECT

    enum ParityFlag
    {
        None,
        Odd,
        Even
    };

private:
    ParityFlag Parity;

public:
    explicit UART(QWidget *parent = nullptr);
    ~UART();

signals:
    void TxChar(QChar);
    void Interrupt();

public slots:
    void RxChar(QChar);
    void Reset();
    void SetQ(bool);
    void Read(uint8_t &);
    void Write(uint8_t);

private:
    Ui::UART *ui;
};



#endif // UART_H
