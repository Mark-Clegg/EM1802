#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QActionGroup>
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

public slots:
    void FileOpen();

private:
    Ui::MainWindow *ui;
    QActionGroup *ProcessorTypeActionGroup;
    QSplitter *TopBottomSplitter;
    QSplitter *LeftRightSplitter;
    QSplitter *PeripheralSplitter;
    Processor *CPU;
    Memory *RAM;
    UART *Uart;
    Console *SerialConsole;
};

#endif // MAINWINDOW_H
