#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDockWidget>

namespace Ui {
class Console;
}

class Console : public QDockWidget
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console();

private:
    Ui::Console *ui;
    bool eventFilter(QObject *, QEvent *);

signals:
    void Input(QChar);

public slots:
    void Output(QChar);
    void Clear();
};

#endif // CONSOLE_H
