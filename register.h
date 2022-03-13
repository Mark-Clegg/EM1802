#ifndef REGISTER_H
#define REGISTER_H
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QRegularExpression>
#include <QMouseEvent>

class Register : public QWidget
{
    Q_OBJECT

private:
    union {
        uint16_t Word;
        struct /* __attribute__ ((packed)) */ {
            uint8_t LowByte;
            uint8_t HighByte;
        };
    } Value;

    static constexpr uint16_t Mask[5] = { 0x0000, 0x000F, 0x00FF, 0x0FFF, 0xFFFF };

    QHBoxLayout *Layout;
    QLabel *Identifier;
    QLineEdit *RegisterValue;
    QLabel *LIndicator;
    QLabel *RIndicator;
    int digits = 4;

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

    bool eventFilter(QObject*, QEvent*);

    int nibbleCount();
    void setNibbleCount(int);
    void setInputMask(char);

    void setStyle(int, int, int, Qt::Alignment);

    uint8_t high();
    void setHigh(uint8_t);
    uint8_t low();
    void setLow(uint8_t);

    uint16_t value();
    void setValue(uint16_t);

    void setLIndicator(Qt::GlobalColor);
    void setRIndicator(Qt::GlobalColor);

    bool isReadOnly();
    void setReadOnly(bool);

    void setIdentifierCursor(QCursor);

    void operator=(int);
    operator int();

public slots:
    void set();

signals:
    void valueChanged(int);
    void doubleClick(uint16_t);
};
#endif // REGISTER_H
