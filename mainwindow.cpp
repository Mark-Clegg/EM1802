#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RAM = new Memory(this);
   // RAM->setFeatures(RAM->features() & ~QDockWidget::DockWidgetClosable);
    CPU = new Processor(this, *RAM);
   // CPU->setFeatures(CPU->features() & ~QDockWidget::DockWidgetClosable);
    CPU->setMinimumWidth(500);
    RAM->setMinimumWidth(500);

    addDockWidget(Qt::RightDockWidgetArea, CPU);
    addDockWidget(Qt::LeftDockWidgetArea, RAM);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete CPU;
}
