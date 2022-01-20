#ifndef UART_H
#define UART_H

#include <QDockWidget>

namespace Ui {
class UART;
}

class UART : public QDockWidget
{
    Q_OBJECT

public:
    explicit UART(QWidget *parent = nullptr);
    ~UART();

signals:
    void TxChar(QChar);

public slots:
    void RxChar(QChar);
    void Reset();
    void Read(uint8_t &, bool Q);
    void Write(uint8_t, bool Q);

private:
    Ui::UART *ui;
};

#endif // UART_H
