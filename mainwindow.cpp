#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TopBottomSplitter = new QSplitter(Qt::Vertical, this);
    LeftRightSplitter = new QSplitter(Qt::Horizontal, TopBottomSplitter);
    PeripheralSplitter = new QSplitter(Qt::Vertical, LeftRightSplitter);

    this->setStyleSheet("QSplitter::handle{background-color: black;}");
    TopBottomSplitter->setChildrenCollapsible(false);
    LeftRightSplitter->setChildrenCollapsible(false);
    PeripheralSplitter->setChildrenCollapsible(false);

    RAM = new Memory(PeripheralSplitter);
    Uart = new UART(PeripheralSplitter);
    CPU = new Processor(LeftRightSplitter, *RAM);
    SerialConsole = new Console(TopBottomSplitter);

    TopBottomSplitter->addWidget(LeftRightSplitter);
    TopBottomSplitter->addWidget(SerialConsole);

    LeftRightSplitter->addWidget(PeripheralSplitter);
    LeftRightSplitter->addWidget(CPU);

    PeripheralSplitter->addWidget(RAM);
    PeripheralSplitter->addWidget(Uart);

    setCentralWidget(TopBottomSplitter);

    ProcessorTypeActionGroup = new QActionGroup(this);
    ProcessorTypeActionGroup->addAction(ui->actionCDP1802);
    ProcessorTypeActionGroup->addAction(ui->actionCDP1806);
    ProcessorTypeActionGroup->addAction(ui->actionCDP1806A);
    ProcessorTypeActionGroup->setExclusive(true);
    ui->actionCDP1802->setChecked(true);

    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionStep->setEnabled(true);
    ui->actionLoad->setEnabled(true);

    connect(ui->actionRun, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(false);
        ui->actionStop->setEnabled(true);
        ui->actionStep->setEnabled(false);
        ui->actionLoad->setEnabled(false);
        CPU->Run();
    });

    connect(ui->actionStop, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionStep->setEnabled(true);
        ui->actionLoad->setEnabled(true);
        CPU->Stop();
    });

    connect(ui->actionStep, &QAction::triggered, CPU, &Processor::ExecuteInstruction);
    connect(ui->actionLoad, &QAction::triggered, CPU, &Processor::Load);

    connect(ui->actionReset, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionStep->setEnabled(true);
        ui->actionLoad->setEnabled(true);
        CPU->MasterReset();
    });

    connect(ui->actionCDP1802,  &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1802);});
    connect(ui->actionCDP1806,  &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1806);});
    connect(ui->actionCDP1806A, &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1806A);});

    connect(CPU, &Processor::Reset, Uart, &UART::Reset);
    connect(CPU, &Processor::Reset, SerialConsole, &Console::Clear);

    connect(CPU, &Processor::Break, this, [this](QString Reason)
    {
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionStep->setEnabled(true);
        ui->actionLoad->setEnabled(true);
        QMessageBox msgBox;
        msgBox.setText(Reason);
        msgBox.exec();
    });

    connect(CPU, &Processor::QSignal, Uart, &UART::SetQ);

    connect(Uart, &UART::Interrupt, CPU, &Processor::Interrupt);
    connect(Uart, &UART::TxChar, SerialConsole, &Console::Output);
    connect(SerialConsole, &Console::Input, Uart, &UART::RxChar);

    connect(CPU, &Processor::Inp3, Uart, &UART::Read);
    connect(CPU, &Processor::Out3, Uart, &UART::Write);

    Uart->Reset();
}

void MainWindow::FileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            "Open Memory Image", "",
            "Intel Hex (*.hex);;Binary (*.bin)");
    if(fileName != "")
        RAM->LoadFile(fileName);
    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionStep->setEnabled(true);
    ui->actionLoad->setEnabled(true);
    CPU->MasterReset();
}

MainWindow::~MainWindow()
{
    delete ui; ui = nullptr;
}


