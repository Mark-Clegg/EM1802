#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RAM = new Memory(this);
    CPU = new Processor(this, *RAM);

    addDockWidget(Qt::RightDockWidgetArea, CPU);
    addDockWidget(Qt::LeftDockWidgetArea, RAM);

    ui->Console->installEventFilter(this);

    connect(this, &MainWindow::Input, this, &MainWindow::Output);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        emit Input(keyEvent->text().front());
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


