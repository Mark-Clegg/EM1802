#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CPU = new Processor(this);
    CPU->setFeatures(CPU->features() & ~QDockWidget::DockWidgetClosable);
    CPU->setMinimumWidth(500);

    addDockWidget(Qt::RightDockWidgetArea, CPU);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete CPU;
}
