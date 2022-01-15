#ifndef REGISTER_H
#define REGISTER_H
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

class Register : public QWidget
{
    Q_OBJECT

private:
    union {
        uint16_t Word;
        struct __attribute__ ((packed)) {
            uint8_t LowByte;
            uint8_t HighByte;
        };
    } Value;

    QHBoxLayout *Layout;
    QLabel *Identifier;
    QLineEdit *RegisterValue;
    QLabel *LIndicator;
    QLabel *RIndicator;
    int digits = 4;

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

    int nibbleCount();
    void setNibbleCount(int);
    void setInputMask(char);

    uint8_t high();
    void setHigh(uint8_t);
    uint8_t low();
    void setLow(uint8_t);

    uint16_t value();
    void setValue(uint16_t);

    void setLIndicator(Qt::GlobalColor);
    void setRIndicator(Qt::GlobalColor);

    void operator=(int);
    operator int();

public slots:
    void set();

signals:
    void valueChanged(int);
};
#endif // REGISTER_H
