#ifndef UART_H
#define UART_H

#include <QDockWidget>
#include <QLabel>

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
    Ui::UART *ui;
    ParityFlag Parity;
    bool RSel;
    void SetIndicator(QLabel *, Qt::GlobalColor);

public:
    explicit UART(QWidget *parent = nullptr);
    ~UART();

signals:
    void TxChar(QChar);
    void Interrupt(bool);

public slots:
    void RxChar(QChar);
    void Reset();
    void SetQ(bool);
    void Read(uint8_t &);
    void Write(uint8_t);

private:
};



#endif // UART_H
