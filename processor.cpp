#include "processor.h"
#include "ui_processor.h"
#include <cmath>

Processor::Processor(QWidget *parent, Memory &RAM) :
    QDockWidget(parent),
    DisassemblyTemplate("ADDR  -- -- -- --  OPCODE--------"),
    ui(new Ui::Processor),
    M(RAM)
{
    ui->setupUi(this);

    QTextDocument *Doc = ui->Disassembly->document();
    QFont font = Doc->defaultFont();
    font.setFamily("Courier New");
    Doc->setDefaultFont(font);

    QFontMetrics fm(font);
    ui->Disassembly->setMinimumWidth(fm.horizontalAdvance(DisassemblyTemplate)+35);
    LineHeight = fm.height();
    ui->Disassembly->installEventFilter(this);

    SetType(CDP1802);

    Clock = new QTimer(this);
    Clock->setInterval(0);
    ui->ClockSpeed->setSliderPosition(10);

    R[ 0] = ui->R0; R[ 0]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 1] = ui->R1; R[ 1]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 2] = ui->R2; R[ 2]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 3] = ui->R3; R[ 3]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 4] = ui->R4; R[ 4]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 5] = ui->R5; R[ 5]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 6] = ui->R6; R[ 6]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 7] = ui->R7; R[ 7]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 8] = ui->R8; R[ 8]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[ 9] = ui->R9; R[ 9]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[10] = ui->RA; R[10]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[11] = ui->RB; R[11]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[12] = ui->RC; R[12]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[13] = ui->RD; R[13]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[14] = ui->RE; R[14]->setStyle(4, 30, 50, Qt::AlignCenter);
    R[15] = ui->RF; R[15]->setStyle(4, 30, 50, Qt::AlignCenter);

    EF1 = ui->EF1;
    EF2 = ui->EF2;
    EF3 = ui->EF3;
    EF4 = ui->EF4;

    D   = ui->D;   D->setStyle(2, 30, 50, Qt::AlignCenter);
    DF  = ui->DF; DF->setStyle(1, 30, 50, Qt::AlignCenter);
    X   = ui->X;   X->setStyle(1, 30, 50, Qt::AlignCenter);
    P   = ui->P;   P->setStyle(1, 30, 50, Qt::AlignCenter);
    T   = ui->T;   T->setStyle(2, 30, 50, Qt::AlignCenter);
    N   = ui->N;   N->setStyle(1, 30, 50, Qt::AlignCenter);
    I   = ui->I;   I->setStyle(1, 30, 50, Qt::AlignCenter);

    Q   = ui->Q;
    IE  = ui->IE;
    XIE = ui->XIE;
    CIE = ui->CIE;
    XI  = ui->XI;
    CI  = ui->CI;

    Counter = ui->Counter; Counter->setStyle(2, 50, 0, Qt::AlignRight);

    DF->setInputMask('B');

    Q->setOnColour(Qt::red);
    Q->setOffColour(Qt::black);

    IE->setOnColour(Qt::green);
    IE->setOffColour(Qt::red);

    XIE->setOnColour(Qt::green);
    XIE->setOffColour(Qt::red);

    CIE->setOnColour(Qt::green);
    CIE->setOffColour(Qt::red);

    XI->setOnColour(Qt::green);
    XI->setOffColour(Qt::black);

    CI->setOnColour(Qt::green);
    CI->setOffColour(Qt::black);

    // Enable Click on Register, X and P Labels to scroll address into view in Memory widget
    for(int i=0; i<0x10; i++)
    {
        connect(R[i], &Register::doubleClick, &M, &Memory::setPosition);
        R[i]->setIdentifierCursor(Qt::PointingHandCursor);
    }
    connect(X, &Register::doubleClick, this, [this](uint16_t X) { M.setPosition(*R[X]); });
    X->setIdentifierCursor(Qt::PointingHandCursor);
    connect(P, &Register::doubleClick, this, [this](uint16_t P) { M.setPosition(*R[P]); });
    P->setIdentifierCursor(Qt::PointingHandCursor);

    connect(X, &Register::valueChanged, this, &Processor::XChanged);
    connect(P, &Register::valueChanged, this, &Processor::PChanged);
    connect(Clock, &QTimer::timeout, this, &Processor::ExecuteInstruction);
    connect(ui->ClockSpeed, &QSlider::valueChanged, this, [this](int i) { Clock->setInterval(pow(2,10-i)-1);});

    connect(EF1, &ExternalFlag::stateChanged, this, [this](bool State)
    {
        switch(CounterMode)
        {
        case CounterModeEventCounter1: if(State) DecrementCounter(); break;
        case CounterModePulseMeasure1:

            if(!State)
            {
                SetCounterMode(CounterModeStopped);
                SetCounterInterrupt(true);
            }
            break;

        default: break;
        }
    });

    connect(EF2, &ExternalFlag::stateChanged, this, [this](bool State)
    {
        switch(CounterMode)
        {
        case CounterModeEventCounter2: if(State) DecrementCounter(); break;
        case CounterModePulseMeasure2:

            if(!State)
            {
                SetCounterMode(CounterModeStopped);
                SetCounterInterrupt(true);
            }
            break;

        default: break;
        }
    });

    MasterReset();
}

bool Processor::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize && obj == ui->Disassembly)
    {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        DisassemblerLines = resizeEvent->size().height() / LineHeight - 1;
        ui->Disassembly->setMaximumBlockCount(DisassemblerLines);
        ui->Disassembly->appendPlainText(DisassemblyTemplate);
        ui->Disassembly->appendPlainText(Disassemble(*R[*P], DisassemblerLines - 1));
        QTextCursor C = ui->Disassembly->textCursor();
        C.setPosition(0);
        C.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
        C.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        ui->Disassembly->setTextCursor(C);
    }
    return QWidget::eventFilter(obj, event);
}

void Processor::MasterReset()
{
    Clock->stop();

    X->setLIndicator(Qt::green);
    X->setRIndicator(Qt::green);
    P->setLIndicator(Qt::red);
    P->setRIndicator(Qt::red);

    *P = 0;
    *X = 0;
    *Q = 0;
    *IE = 1;
    *XIE = 1;
    *CIE = 1;
    SetCounterMode(CounterModeStopped);
    *R[0] = 0;
    Running = false;
    ExternalInterrupt = 0;
    SetCounterInterrupt(false);
    CounterToggleQ = false;
    M.setPosition(0);
    ui->Disassembly->clear();
    ui->Disassembly->appendPlainText(DisassemblyTemplate);
    ui->Disassembly->appendPlainText(Disassemble(*R[*P], DisassemblerLines - 1));
    QTextCursor C = ui->Disassembly->textCursor();
    C.setPosition(0);
    C.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    C.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    ui->Disassembly->setTextCursor(C);

    emit Reset();
}

void Processor::SetType(enum ProcessorType ProcessorType)
{
    this->ProcessorType = ProcessorType;

    ui->CDP1806->setVisible(ProcessorType != CDP1802);
    ui->XIE->setVisible(ProcessorType != CDP1802);
    ui->CIE->setVisible(ProcessorType != CDP1802);
    ui->CI->setVisible(ProcessorType != CDP1802);
}

void Processor::SetCounterMode(CounterModeEnum Mode)
{
    CounterMode = Mode;
    switch(CounterMode)
    {
    case CounterModeStopped:       ui->CounterMode->setText("Counter Stopped"); break;
    case CounterModeTimer:         ui->CounterMode->setText("Timer Mode");     break;
    case CounterModeEventCounter1: ui->CounterMode->setText("Event Counter 1"); break;
    case CounterModeEventCounter2: ui->CounterMode->setText("Event Counter 2"); break;
    case CounterModePulseMeasure1: ui->CounterMode->setText("Pulse Measure 1"); break;
    case CounterModePulseMeasure2: ui->CounterMode->setText("Pulse Measure 2"); break;
    }
}

void Processor::SetCounterInterrupt(bool State)
{
    CounterInterrupt = State;
    *CI = State;
}

void Processor::DecrementCounter()
{
    *Counter = *Counter - 1;
    if(*Counter <= 0)
    {
        *Counter = CounterHoldingRegister;
        SetCounterInterrupt(true);

        if(CounterToggleQ)
            *Q = !*Q;
    }
}

void Processor::XChanged()
{
    // Set Coloured Indicator next to R[x]
    for(int i=0;i<16;i++)
        if(i == *X)
            R[i]->setLIndicator(Qt::green);
        else
            R[i]->setLIndicator(Qt::black);
}

void Processor::PChanged()
{
    // Set Coloured Indicator next to R[x]
    for(int i=0;i<16;i++)
        if(i == *P)
            R[i]->setRIndicator(Qt::red);
        else
            R[i]->setRIndicator(Qt::black);
}

void Processor::Interrupt(bool State)
{
    if(State)
        ExternalInterrupt ++;
    else
        if(ExternalInterrupt > 0)
            ExternalInterrupt --;
    *ui->XI = ExternalInterrupt > 0;

    if(Running)
        Clock->start();
}

void Processor::DMAIn(uint8_t i)
{
    M[*R[0]] = i;
    *R[0] = *R[0] + 1;
    if(Running)
        Clock->start();
}

void Processor::DMAOut(uint8_t & i)
{
    i = M[*R[0]];
    *R[0] = *R[0] + 1;
    if(Running)
        Clock->start();
}

void Processor::Run()
{
    Running = true;
    Clock->start();
}

void Processor::Stop()
{
    Running = false;
    Clock->stop();
}

void Processor::IllegalInstruction()
{
    Clock->stop();
    *R[*P] = *R[*P] - 1;
    emit Break("Illegal Instruction: 68");
}

void Processor::ExecuteInstruction()
{
    if(*IE && ((*XIE && ExternalInterrupt > 0) | (*CIE && CounterInterrupt)))
    {
        *T = (*X << 4) | *P;
        *X = 2;
        *P = 1;
        *IE = false;
    }

    int Ticks = 2;
    // Fetch Instruction

    uint8_t Instruction = M[*R[*P]];
    *R[*P] = *R[*P] + 1;
    *N = Instruction & 0x0F;
    *I = Instruction >> 4;

    // Execute
    uint8_t Temp;

    // Cache D and DF to avoid updating UI during execution
    uint16_t d = *D;
    uint16_t df = *DF;

    switch(*I)
    {
    case 0x0: /* IDL / LDN */

        if(*N == 0)
            Clock->stop();
        else
            d = M[*R[*N]];
        break;

    case 0x1: /* INC */ *R[*N] = *R[*N] + 1; break;

    case 0x2: /* DEC */ *R[*N] = *R[*N] - 1; break;

    case 0x3: /* Short Branch */

        switch(*N)
        {
        case 0x0: /* BR  */ R[*P]->setLow(M[*R[*P]]); break;
        case 0x1: /* BQ  */

            if(*Q)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x2: /* BZ */

            if(d == 0)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x3: /* BDF */

            if(df)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x4: /* B1 */

            if(*EF1)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x5: /* B2 */

            if(*EF2)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x6: /* B3 */

            if(*EF3)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x7: /* B4 */

            if(*EF4)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x8: /* NBR / SKP */ *R[*P] = *R[*P] + 1; break;

        case 0x9: /* BNQ */
            if(!*Q)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xA: /* BNZ */

            if(d != 0)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xB: /* BNF */

            if(!df)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xC: /* BN1 */

            if(!*EF1)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xD: /* BN2 */

            if(!*EF2)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xE: /* BN3 */

            if(!*EF3)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xF: /* BN4 */

            if(!*EF4)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;
        }
        break;

    case 0x4: /* LDA */ d = M[*R[*N]]; *R[*N] = *R[*N] + 1; break;

    case 0x5: /* STR */ M[*R[*N]] = *D; break;

    case 0x6: /* Input / Output */

        switch(*N)
        {
        case 0x0: /* IRX */ *R[*X] = *R[*X] + 1; break;
        case 0x1: /* OUT P1 */ emit Out1(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x2: /* OUT P2 */ emit Out2(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x3: /* OUT P3 */ emit Out3(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x4: /* OUT P4 */ emit Out4(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x5: /* OUT P5 */ emit Out5(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x6: /* OUT P6 */ emit Out6(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;
        case 0x7: /* OUT P7 */ emit Out7(M[*R[*X]]); *R[*X] = *R[*X] + 1; break;

        case 0x8: // Extended 1806 Instructions

            if(ProcessorType == CDP1802)
                IllegalInstruction();
            else
            {
                Instruction = M[*R[*P]];
                *R[*P] = *R[*P] + 1;
                *N = Instruction & 0x0F;
                *I = Instruction >> 4;

                switch(*I)
                {
                case 0x0: /* Counter Timer */

                    switch(*N)
                    {
                    case 0x0: /* STPC */ SetCounterMode(CounterModeStopped); break;
                    case 0x1: /* DTC  */ DecrementCounter(); break;
                    case 0x2: /* SPM2 */ SetCounterMode(CounterModePulseMeasure2); break;
                    case 0x3: /* SCM2 */ SetCounterMode(CounterModeEventCounter2); break;
                    case 0x4: /* SPM1 */ SetCounterMode(CounterModePulseMeasure1); break;
                    case 0x5: /* SCM1 */ SetCounterMode(CounterModeEventCounter1); break;

                    case 0x6: /* LDC  */

                        *Counter = d;
                        if(CounterMode == CounterModeStopped)
                        {
                            CounterHoldingRegister = d;
                            SetCounterInterrupt(false);
                            CounterToggleQ = false;
                        }
                        break;

                    case 0x7: /* STM */ SetCounterMode(CounterModeTimer); TotalTicks = 0; break;
                    case 0x8: /* GEC */ d = Counter->value(); break;
                    case 0x9: /* ETQ */ CounterToggleQ = true; break;

                    case 0xA: /* XIE */ *XIE = true;  break;
                    case 0xB: /* XID */ *XIE = false; break;
                    case 0xC: /* CIE */ *CIE = true;  break;
                    case 0xD: /* CID */ *CIE = false; break;
                    default: IllegalInstruction(); break;
                    }
                    Ticks += 1;
                    break;

                case 0x1: IllegalInstruction(); break;

                case 0x2: /* DBNZ */

                    if(ProcessorType == CDP1806A)
                    {
                        *R[*N] = *R[*N] - 1;
                        if(*R[*N] != 0)
                            *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
                        else
                            *R[*P] = *R[*P] + 2;
                        Ticks += 3;
                    }
                    else
                        IllegalInstruction();
                    break;

                case 0x3: /* BCI / BXI */

                    switch(*N)
                    {
                    case 0xE: /* BCI */

                        if(CounterInterrupt)
                        {
                            R[*P]->setLow(M[*R[*P]]);
                            SetCounterInterrupt(false);
                            CounterToggleQ = false;
                        }
                        else
                            *R[*P] = *R[*P] + 1;
                        break;

                    case 0xF: /* BXI */

                        if(ExternalInterrupt > 0)
                            R[*P]->setLow(M[*R[*P]]);
                        else
                            *R[*P] = *R[*P] + 1;
                        break;

                    default:

                        IllegalInstruction();
                        break;
                    }
                    Ticks += 1;
                    break;

                case 0x4: IllegalInstruction(); break;
                case 0x5: IllegalInstruction(); break;

                case 0x6: /* RLXA */

                    *R[*N] = (M[*R[*X]] << 8) + M[*R[*X] + 1];
                    *R[*X] = *R[*X] + 2;
                    Ticks += 3;
                    break;

                case 0x7:

                    if(ProcessorType == CDP1806A)
                    {
                        switch(*N)
                        {
                        case 0x4: /* DADC */ d = BCD_Add(d, M[*R[*X]], df); break;
                        case 0x6: /* DSAV */

                            M[*R[*X] - 1] = *T;
                            M[*R[*X] - 2] = d;
                            Temp = d & 1; d >>= 1; d |= df << 7; df = Temp;
                            M[*R[*X] - 3 ] = d;
                            *R[*X] = *R[*X] - 3;
                            Ticks += 2;
                            break;

                        case 0x7: /* DSMB */ d = BCD_Subtract(d, M[*R[*X]], df); break;
                        case 0xC: /* DACI */ d = BCD_Add(d, M[*R[*P]], df); *R[*P] = *R[*P] + 1; break;
                        case 0xF: /* DSBI */ d = BCD_Subtract(d, M[*R[*P]], df); *R[*P] = *R[*P] + 1; break;

                        default:

                            IllegalInstruction();
                            break;
                        }
                        Ticks += 2;
                    }
                    else
                        IllegalInstruction();
                    break;

                case 0x8: /* SCAL */

                    M[*R[*X]] = R[*N]->low();
                    M[*R[*X] - 1] = R[*N]->high();
                    *R[*X] = *R[*X] - 2;
                    *R[*N] = R[*P]->value();
                    *R[*P] = (M[*R[*N]] << 8) + M[*R[*N] + 1];
                    *R[*N] = *R[*N] + 2;
                    Ticks += 8;
                    break;

                case 0x9: /* SRET */

                    *R[*P] = R[*N]->value();
                    *R[*N] = (M[*R[*X] + 1] << 8) + M[*R[*X] + 2];
                    *R[*X] = *R[*X] + 2;
                    Ticks += 6;
                    break;

                case 0xA: /* RSXD */

                    M[*R[*X]] = R[*N]->low();
                    M[*R[*X] - 1] = R[*N]->high();
                    *R[*X] = *R[*X] - 2;
                    Ticks += 3;
                    break;

                case 0xB: /* RNX */

                    *R[*X] = R[*N]->value();
                    Ticks += 2;
                    break;

                case 0xC: /* RLDI */

                    R[*N]->setHigh(M[*R[*P]]);
                    R[*N]->setLow(M[*R[*P] + 1]);
                    *R[*P] = *R[*P] + 2;
                    Ticks += 3;
                    break;

                case 0xD: IllegalInstruction(); break;
                case 0xE: IllegalInstruction(); break;
                case 0xF:

                    if(ProcessorType == CDP1806A)
                    {
                        switch(*N)
                        {
                        case 0x4: /* DADD */ df = 0; d = BCD_Add(d, M[*R[*X]], df); break;
                        case 0x7: /* DSM  */ df = 1; d = BCD_Subtract(d, M[*R[*X]], df); break;
                        case 0xC: /* DADI */ df = 0; d = BCD_Add(d, M[*R[*P]], df); *R[*P] = *R[*P] + 1; break;
                        case 0xF: /* DSMI */ df = 1; d = BCD_Subtract(d, M[*R[*P]], df); *R[*P] = *R[*P] + 1; break;
                        default: IllegalInstruction(); break;
                        }
                        Ticks += 2;
                    }
                    else
                        IllegalInstruction();
                    break;
                }
            }
            break;

        case 0x9: /* INP P1 */ Temp=0xFF; emit Inp1(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xA: /* INP P2 */ Temp=0xFF; emit Inp2(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xB: /* INP P3 */ Temp=0xFF; emit Inp3(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xC: /* INP P4 */ Temp=0xFF; emit Inp4(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xD: /* INP P5 */ Temp=0xFF; emit Inp5(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xE: /* INP P6 */ Temp=0xFF; emit Inp6(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xF: /* INP P7 */ Temp=0xFF; emit Inp7(Temp); d = Temp; M[*R[*X]] = Temp; break;
        }
        break;

    case 0x7: /* Misc */

        switch(*N)
        {
        case 0x0: /* RET  */ Temp = M[*R[*X]]; *R[*X] = *R[*X] + 1; *X = Temp >> 4; *P = Temp & 0xF; *IE = true; break;
        case 0x1: /* DIS  */ Temp = M[*R[*X]]; *R[*X] = *R[*X] + 1; *X = Temp >> 4; *P = Temp & 0xF; *IE = false; break;
        case 0x2: /* LDXA */ d = M[*R[*X]]; *R[*X] = *R[*X] + 1; break;
        case 0x3: /* STXD */ M[*R[*X]] = d; *R[*X] = *R[*X] - 1; break;
        case 0x4: /* ADC  */ d = d + M[*R[*X]] + df; df = d >> 8; break;
        case 0x5: /* SDB  */ d |= 0x0100; d = M[*R[*X]] - d - (df^1); df = d >> 8; break;
        case 0x6: /* SHRC */ Temp = d & 1; d >>= 1; d |= df << 7; df = Temp; break;
        case 0x7: /* SMB  */ d |= 0x0100; d = d - M[*R[*X]] - (df^1); df = d >> 8; break;
        case 0x8: /* SAV  */ M[*R[*X]] = *T; break;
        case 0x9: /* MARK */ *T = (*X << 4) + *P; M[*R[2]] = *T; *X = P->value(); *R[2] = *R[2] -1; break;
        case 0xA: /* REQ  */ *Q = false; emit QSignal(false); break;
        case 0xB: /* SEQ  */ *Q = true; emit QSignal(true); break;
        case 0xC: /* ADCI */ d = d + M[*R[*P]] + df; df = d >> 8; *R[*P] = *R[*P] +1; break;
        case 0xD: /* SDBI */ d |= 0x0100; d = M[*R[*P]] - d - (df^1); df = d >> 8; *R[*P] = *R[*P] +1; break;
        case 0xE: /* SHLC */ Temp = (d & 0x80) >> 7; d <<= 1; d |= df; df = Temp; break;
        case 0xF: /* SMBI */ d |= 0x0100; d = d - M[*R[*P]] - (df^1); df = d >> 8; *R[*P] = *R[*P] +1; break;
        }
        break;

    case 0x8: /* GLO */ d = R[*N]->low(); break;
    case 0x9: /* GHI */ d = R[*N]->high(); break;
    case 0xA: /* PLO */ R[*N]->setLow(d); break;
    case 0xB: /* PHI */ R[*N]->setHigh(d); break;

    case 0xC: /* Long Branch / Skip */

        switch(*N)
        {
        case 0x0: /* LBR  */ *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1]; break;
        case 0x1: /* LBQ  */

            if(*Q)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x2: /* LBZ  */

            if(d == 0)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x3: /* LBDF */

            if(df)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x4: /* NOP */ break;

        case 0x5: /* LSNQ */

            if(!*Q)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x6: /* LSNZ */

            if(d != 0)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x7: /* LSNF  */

            if(!df)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x8: /* LSKP */

            *R[*P] = *R[*P] + 2;
            break;

        case 0x9: /* LBNQ */

            if(!*Q)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0xA: /* LBNZ */

            if(d != 0)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0xB: /* LBNF */

            if(!df)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P] + 1];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0xC: /* LSIE */

            if(*IE)
                *R[*P] = *R[*P] + 2;
            break;

        case 0xD: /* LSQ  */

            if(*Q)
                *R[*P] = *R[*P] + 2;
            break;

        case 0xE: /* LSZ  */

            if(d == 0)
                *R[*P] = *R[*P] + 2;
            break;

        case 0xF: /* LSDF */

            if(df == 1)
                *R[*P] = *R[*P] + 2;
            break;
        }
        Ticks += 1;
        break;

    case 0xD: /* SEP */ *P = N->value(); break;
    case 0xE: /* SEX */ *X = N->value(); break;

    case 0xF: /* Misc */

        switch(*N)
        {
        case 0x0: /* LDX  */ d = M[*R[*X]]; break;
        case 0x1: /* OR   */ d = d | M[*R[*X]]; break;
        case 0x2: /* AND  */ d = d & M[*R[*X]]; break;
        case 0x3: /* XOR  */ d = d ^ M[*R[*X]]; break;
        case 0x4: /* ADD  */ d = d + M[*R[*X]]; df = d >> 8; break;
        case 0x5: /* SD   */ d |= 0x0100; d = M[*R[*X]] - d; df = d >> 8; break;
        case 0x6: /* SHR  */ df = d & 1; d >>= 1; break;
        case 0x7: /* SM   */ d |= 0x0100; d = d - M[*R[*X]]; df = d >> 8; break;
        case 0x8: /* LDI  */ d = M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0x9: /* ORI  */ d = d | M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xA: /* ANI  */ d = d & M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xB: /* XRI  */ d = d ^ M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xC: /* ADI  */ d = d + M[*R[*P]]; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        case 0xD: /* SDI  */ d |= 0x0100; d = M[*R[*P]] - d; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        case 0xE: /* SHL  */ df = (d & 0x80) >> 7; d <<= 1; break;
        case 0xF: /* SMI  */ d |= 0x0100; d = d - M[*R[*P]]; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        }
    }
    // Update D and DF in the UI
    *D = d;
    *DF = df & 0x01;
    M.setPosition(*R[*P]);

    // Update Disassembly
    QTextCursor C = ui->Disassembly->textCursor();
    // Delete all except the first two lines
    C.setPosition(0);
    C.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    C.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    C.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    C.removeSelectedText();
    // Append new Disassembly
    ui->Disassembly->appendPlainText(Disassemble(*R[*P], DisassemblerLines - 1));
    // Select the second line
    C.setPosition(0);
    C.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    C.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    ui->Disassembly->setTextCursor(C);


    TotalTicks += Ticks;
    if(CounterMode == CounterModeTimer && TotalTicks >= 32)
    {
        TotalTicks = 0;
        DecrementCounter();
    }

    if(((CounterMode == CounterModePulseMeasure1) & *EF1) || ((CounterMode == CounterModePulseMeasure2) & *EF2))
        for(int i=0; i < Ticks; i++)
            DecrementCounter();
}

Processor::~Processor()
{
    delete ui;
}
