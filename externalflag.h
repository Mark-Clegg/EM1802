#ifndef EXTERNALFLAG_H
#define EXTERNALFLAG_H
#include <QtWidgets/QCheckBox>

#include <QWidget>

class ExternalFlag : public QCheckBox
{
    Q_OBJECT

private:
    bool State;

public:
    explicit ExternalFlag(QWidget *parent = nullptr);

    void operator=(bool);
    operator bool();

signals:

};

#endif // EXTERNALFLAG_H
