#ifndef MEMORY_H
#define MEMORY_H

#include <QDockWidget>

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

public:
    explicit Memory(QWidget *parent = nullptr);
    MemProxy operator [] (uint16_t) const;
    MemProxy& operator [] (uint16_t);
    void setPosition(uint16_t);

signals:

public slots:

};
#endif // MEMORY_H
