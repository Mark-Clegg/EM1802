#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ColumnSplitter = new QSplitter(Qt::Horizontal, this);

    LeftSplitter = new QSplitter(Qt::Vertical, ColumnSplitter);
    CentralSplitter = new QSplitter(Qt::Vertical, ColumnSplitter);
    RightSplitter = new QSplitter(Qt::Vertical, ColumnSplitter);

    this->setStyleSheet("QSplitter::handle{background-color: black;}");
    ColumnSplitter->setChildrenCollapsible(false);
    LeftSplitter->setChildrenCollapsible(false);
    CentralSplitter->setChildrenCollapsible(false);
    RightSplitter->setChildrenCollapsible(false);

    RAM = new Memory(LeftSplitter);
    CPU = new Processor(RightSplitter, *RAM);
    Uart = new UART(CentralSplitter);
    SerialConsole = new Console(CentralSplitter);
    HardDrive = new IDE(LeftSplitter);

    DMALoader = new QLineEdit(this);
    ErrorResetTimer = new QTimer(this);
    ErrorResetTimer->setSingleShot(true);

    ColumnSplitter->addWidget(LeftSplitter);
    ColumnSplitter->addWidget(CentralSplitter);
    ColumnSplitter->addWidget(RightSplitter);

    setCentralWidget(ColumnSplitter);

    LeftSplitter->addWidget(CPU);

    CentralSplitter->addWidget(Uart);
    CentralSplitter->addWidget(SerialConsole);

    RightSplitter->addWidget(RAM);
    RightSplitter->addWidget(HardDrive);

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
    DMALoader->setEnabled(true);

    DMALoader->setFont(QFont("DEC Terminal"));
    DMALoader->setMaximumWidth(125);
    DMALoader->setPlaceholderText("Hex/Assembler");

    ui->toolBar->insertWidget(ui->actionLoad, DMALoader);

    connect(ui->actionRun, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(false);
        ui->actionStop->setEnabled(true);
        ui->actionStep->setEnabled(false);
        ui->actionLoad->setEnabled(false);
        DMALoader->setEnabled(false);
        CPU->Run();
    });

    connect(ui->actionStop, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionStep->setEnabled(true);
        ui->actionLoad->setEnabled(true);
        DMALoader->setEnabled(true);
        CPU->Stop();
    });

    connect(ui->actionStep, &QAction::triggered, CPU, &Processor::ExecuteInstruction);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::Load);
    connect(DMALoader, &QLineEdit::returnPressed, this, &MainWindow::Load);
    connect(ErrorResetTimer, &QTimer::timeout, this, [this]()
    {
        DMALoader->setStyleSheet("");
    });

    connect(ui->actionReset, &QAction::triggered, this, [this](bool)
    {
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionStep->setEnabled(true);
        ui->actionLoad->setEnabled(true);
        DMALoader->setEnabled(true);
        CPU->MasterReset();
    });

    connect(ui->actionCDP1802,  &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1802);});
    connect(ui->actionCDP1806,  &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1806);});
    connect(ui->actionCDP1806A, &QAction::toggled, this, [this](bool state){ if(state) CPU->SetType(CDP1806A);});

    connect(CPU, &Processor::Reset, HardDrive, &IDE::Reset);
    connect(CPU, &Processor::Reset, Uart, &UART::Reset);
    connect(CPU, &Processor::Reset, SerialConsole, &Console::Clear);

    connect(CPU, &Processor::Break, this, &MainWindow::Break);
    connect(HardDrive, &IDE::Break, this, &MainWindow::Break);

    // Connect the Console to the UART
    connect(Uart, &UART::TxChar, SerialConsole, &Console::Output);
    connect(SerialConsole, &Console::Input, Uart, &UART::RxChar);

    // Connect the UART to the Processor
    connect(CPU, &Processor::QSignal, Uart, &UART::SetQ);
    connect(Uart, &UART::Interrupt, CPU, &Processor::Interrupt);
    connect(CPU, &Processor::Inp3, Uart, &UART::Read);
    connect(CPU, &Processor::Out3, Uart, &UART::Write);

    // Connect the IDE Drive to the Processor
    connect(CPU, &Processor::Inp1, HardDrive, &IDE::Read);
    connect(CPU, &Processor::Out1, HardDrive, &IDE::Write);
    connect(CPU, &Processor::Out2, HardDrive, &IDE::RegisterSelect);

    CPU->MasterReset();
}

void MainWindow::Break(QString Reason)
{
    CPU->Stop();
    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionStep->setEnabled(true);
    ui->actionLoad->setEnabled(true);
    QMessageBox msgBox;
    msgBox.setText(Reason);
    msgBox.exec();
};

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
    DMALoader->setEnabled(true);
    CPU->MasterReset();
}

void MainWindow::CreateDiskImage()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            "Create Disk Image", "",
            "Elf Disk image (*.disk)");
    if(fileName != "")
    {
        QFile ImageFile(fileName);
        // Create the file
        if(ImageFile.open(QIODevice::ReadWrite))
        {
            ImageFile.resize(64 * 1024 * 1024);
            ImageFile.close();
        }
        HardDrive->Mount(fileName);
    }
}

void MainWindow::MountDiskImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            "Mount Disk Image", "",
            "Elf Disk image (*.disk)");
    if(fileName != "")
        HardDrive->Mount(fileName);

}

void MainWindow::UnMountDiskImage()
{
    HardDrive->UnMount();
}

void MainWindow::Load()
{
    static const QRegularExpression HexByteMatcher("^\\s*([0-9a-f]{1,2})\\s*$", QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression HexWordMatcher("^\\s*([0-9a-f]{1,4})\\s*$", QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression AssemblyMatcher("^\\s*([a-z1-4]+)(\\s+([a-frp0-9]+))?(\\s*,?\\s*([a-f0-9]+))?\\s*$", QRegularExpression::CaseInsensitiveOption);

    QString Line = DMALoader->displayText().toLower();
    bool Error = false;

    QRegularExpressionMatch HexMatch = HexByteMatcher.match(Line);
    if(HexMatch.hasMatch())
    {
        bool conversionstatus;
        CPU->DMAIn(HexMatch.captured(0).toUInt(&conversionstatus, 16));
    }
    else
    {
        QRegularExpressionMatch AssemblyMatch = AssemblyMatcher.match(Line);
        if(AssemblyMatch.hasMatch())
        {
            QString OpCodeName = AssemblyMatch.captured(1);
            QString Param1 = AssemblyMatch.captured(3);
            QString Param2 = AssemblyMatch.captured(5);

            if(OpCodeTable.contains(OpCodeName))
            {
                OpCode & OpCodeParams = OpCodeTable[OpCodeName];
                uint16_t Temp;
                switch(OpCodeParams.Type)
                {
                case None:

                    if(Param1.length() > 0 || Param2.length() > 0)
                        Error = true;
                    else
                        LoadWord(OpCodeParams.Code);
                    break;

                case Register:

                    if(Param1.length() != 2 || Param2.length() > 0)
                        Error = true;
                    else
                    {
                        if(ParseRegister(Param1, Temp))
                        {
                            Temp |= OpCodeParams.Code;
                            if(Temp != 0)
                                LoadWord(Temp);
                            else
                                Error = true;
                        }
                        else
                            Error = true;
                    }
                    break;

                case Port:

                    if(Param1.length() == 0 || Param2.length() > 0)
                        Error = true;
                    else
                    {
                        if(ParsePort(Param1, Temp))
                            LoadWord(OpCodeParams.Code | Temp);
                        else
                            Error = true;
                    }
                    break;

                case Byte:

                    if(Param1.length() == 0 || Param2.length() > 0)
                        Error = true;
                    else
                    {
                        QRegularExpressionMatch HexMatch = HexByteMatcher.match(Param1);
                        if(HexMatch.hasMatch())
                        {
                            LoadWord(OpCodeParams.Code);
                            bool conversionStatus;
                            LoadWord(Param1.toUInt(&conversionStatus, 16));
                        }
                        else
                            Error = true;
                    }
                    break;

                case Word:

                    if(Param1.length() == 0 || Param2.length() > 0)
                        Error = true;
                    else
                    {
                        QRegularExpressionMatch HexMatch = HexWordMatcher.match(Param1);
                        if(HexMatch.hasMatch())
                        {
                            bool conversionStatus;
                            Temp = Param1.toUInt(&conversionStatus, 16);
                            LoadWord(OpCodeParams.Code);
                            CPU->DMAIn(Temp >> 8);
                            CPU->DMAIn(Temp & 0xFF);
                        }
                        else
                            Error = true;
                    }
                    break;

                case RegWord:

                    if(Param1.length() !=2 || Param2.length() == 0)
                        Error = true;
                    else
                    {
                        if(ParseRegister(Param1, Temp))
                        {
                            QRegularExpressionMatch HexMatch = HexWordMatcher.match(Param2);
                            if(HexMatch.hasMatch())
                            {
                                LoadWord(OpCodeParams.Code | Temp);
                                bool conversionStatus;
                                Temp = Param2.toUInt(&conversionStatus, 16);
                                CPU->DMAIn(Temp >> 8);
                                CPU->DMAIn(Temp & 0xFF);
                            }
                            else
                                Error = true;
                        }
                        else
                            Error = true;
                    }
                    break;
                }
            }
            else
                Error = true;
        }
        else
            Error = true;
    }
    DMALoader->setFocus();
    if(Error)
    {
        DMALoader->setStyleSheet("QLineEdit { background-color: red; }");
        ErrorResetTimer->setInterval(125);
        ErrorResetTimer->start();
    }
    else
        DMALoader->clear();
}

void MainWindow::LoadWord(uint16_t word)
{
    if((word & 0xff00) == 0)
        CPU->DMAIn(word);
    else
    {
        CPU->DMAIn(word >> 8);
        CPU->DMAIn(word & 0xFF);
    }
}

bool MainWindow::ParsePort(QString Port, uint16_t & out)
{
    static const QRegularExpression Matcher("^\\s*p([1-7])\\s*$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch PortMatch = Matcher.match(Port);
    if(PortMatch.hasMatch())
    {
        bool conversionStatus;
        out = PortMatch.captured(1).toUInt(&conversionStatus, 16);
        return true;
    }
    else
        return false;
}

bool MainWindow::ParseRegister(QString Register, uint16_t & out)
{
    static const QRegularExpression Matcher("^\\s*r([0-9a-f])\\s*$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch RegisterMatch = Matcher.match(Register);
    if(RegisterMatch.hasMatch())
    {
        bool conversionStatus;
        out = RegisterMatch.captured(1).toUInt(&conversionStatus, 16);
        return true;
    }
    else
        return false;
}

MainWindow::~MainWindow()
{
    delete ui; ui = nullptr;
}
