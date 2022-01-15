#ifndef MEMORY_H
#define MEMORY_H
#include <QWidget>


class Memory : public QWidget
{
    Q_OBJECT

private:
    uint8_t M[65536];

public:
    explicit Memory(QWidget *parent = nullptr);
    uint8_t operator [] (uint8_t) const;
    uint8_t& operator [] (uint8_t);

signals:

};

#endif // MEMORY_H
