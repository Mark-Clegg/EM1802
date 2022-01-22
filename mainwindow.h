#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit>
#include "processor.h"
#include "memory.h"
#include "uart.h"
#include "console.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSplitter *TopBottomSplitter;
    QSplitter *LeftRightSplitter;
    QSplitter *PeripheralSplitter;
    Processor *CPU;
    Memory *RAM;
    UART *Uart;
    Console *SerialConsole;
};

#endif // MAINWINDOW_H
