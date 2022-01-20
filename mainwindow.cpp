#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RAM = new Memory(this);
    CPU = new Processor(this, *RAM);
    Uart = new UART(this);

    addDockWidget(Qt::RightDockWidgetArea, CPU);
    addDockWidget(Qt::LeftDockWidgetArea, RAM);
    addDockWidget(Qt::LeftDockWidgetArea, Uart);

    ui->Console->installEventFilter(this);

    connect(CPU, &Processor::Reset, Uart, &UART::Reset);
    connect(CPU, &Processor::Reset, ui->Console, [=](){ ui->Console->clear();});

    connect(Uart, &UART::TxChar, this, &MainWindow::Output);
    connect(this, &MainWindow::Input, Uart, &UART::RxChar);

    connect(CPU, &Processor::Inp3, Uart, &UART::Read);
    connect(CPU, &Processor::Out3, Uart, &UART::Write);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QString Keys = keyEvent->text();
        if(Keys.size() > 0)
            emit Input(Keys.front());
        return true;
    }
    else
        return QObject::eventFilter(obj, event);
}

void MainWindow::Output(QChar c)
{
    QString s(c);
    ui->Console->moveCursor(QTextCursor::End);
    ui->Console->insertPlainText(s);
    ui->Console->moveCursor (QTextCursor::End);
}

MainWindow::~MainWindow()
{
    delete ui; ui = nullptr;
}


