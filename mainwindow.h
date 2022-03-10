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
#include "ide.h"
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
    void Break(QString);
    void FileOpen();
    void CreateDiskImage();
    void MountDiskImage();
    void UnMountDiskImage();

private:
    Ui::MainWindow *ui;
    QActionGroup *ProcessorTypeActionGroup;
    QSplitter *ColumnSplitter;
    QSplitter *LeftSplitter;
    QSplitter *CentralSplitter;
    QSplitter *RightSplitter;
    Processor *CPU;
    Memory *RAM;
    IDE *HardDrive;
    UART *Uart;
    Console *SerialConsole;
    QLineEdit *DMALoader;
    void Load();
    void LoadWord(uint16_t);
    QTimer *ErrorResetTimer;
    bool ParsePort(QString, uint16_t &);
    bool ParseRegister(QString, uint16_t &);

    enum OpCodeType
    {
        None,
        Register,
        Port,
        Byte,
        Word,
        RegWord
    };

    class OpCode
    {
    public:
        OpCodeType Type;
        uint16_t   Code;

        OpCode() { }

        OpCode(OpCodeType T, uint16_t value)
        {
            Type = T;
            Code = value;
        }
    };


    static QMap<QString, OpCode> OpCodeTable;
};

#endif // MAINWINDOW_H
