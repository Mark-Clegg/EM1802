#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "processor.h"
#include "memory.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void Input(QChar);

public slots:
    void Output(QChar);

private:
    Ui::MainWindow *ui;
    Processor *CPU;
    Memory *RAM;

    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAINWINDOW_H
