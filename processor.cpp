#include "processor.h"
#include "ui_processor.h"
#include <cmath>

Processor::Processor(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::Processor)
{
    ui->setupUi(this);

    Clock = new QTimer(this);
    Clock->setInterval(1023);

    R[0] = ui->R0;
    R[1] = ui->R1;
    R[2] = ui->R2;
    R[3] = ui->R3;
    R[4] = ui->R4;
    R[5] = ui->R5;
    R[6] = ui->R6;
    R[7] = ui->R7;
    R[8] = ui->R8;
    R[9] = ui->R9;
    R[10] = ui->RA;
    R[11] = ui->RB;
    R[12] = ui->RC;
    R[13] = ui->RD;
    R[14] = ui->RE;
    R[15] = ui->RF;

    EF1 = ui->EF1;
    EF2 = ui->EF2;
    EF3 = ui->EF3;
    EF4 = ui->EF4;

    D = ui->D;
    DF = ui->DF;
    X = ui->X;
    P = ui->P;
    T = ui->T;
    N = ui->N;
    I = ui->I;

    Q = ui->Q;
    IE = ui->IE;

    D->setNibbleCount(2);
    DF->setNibbleCount(1);
    DF->setInputMask('B');
    X->setNibbleCount(1);
    P->setNibbleCount(1);
    T->setNibbleCount(2);
    N->setNibbleCount(1);
    I->setNibbleCount(1);

    X->setLIndicator(Qt::green);
    X->setRIndicator(Qt::green);

    P->setLIndicator(Qt::red);
    P->setRIndicator(Qt::red);

    Q->setOnColour(Qt::red);
    Q->setOffColour(Qt::black);

    IE->setOnColour(Qt::green);
    IE->setOffColour(Qt::red);

    connect(X, &Register::valueChanged, this, &Processor::XChanged);
    connect(P, &Register::valueChanged, this, &Processor::PChanged);
    connect(ui->ButtonStep, &QPushButton::clicked, this, &Processor::ExecuteInstruction);
    connect(ui->ButtonRunStop, &QPushButton::toggled, this, &Processor::RunStop);
    connect(Clock, &QTimer::timeout, this, &Processor::ExecuteInstruction);
    connect(ui->ClockSpeed, &QSlider::valueChanged, this, [this](int i) { Clock->setInterval(pow(2,10-i)-1);});
    connect(ui->ButtonReset, &QPushButton::clicked, this, &Processor::Reset);
    connect(ui->ButtonLoad, &QPushButton::clicked, this, &Processor::Load);

    Reset();
}

void Processor::Reset()
{
    *P = 0;
    *X = 0;
    *Q = 0;
    *IE = 1;
    *R[0] = 0;
}

void Processor::Load()
{
    M[*R[0]] = *D;
    *R[0] = *R[0] + 1;
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

void Processor::Interrupt()
{
    *T = (*X << 4) | *P;
    *X = 2;
    *P = 1;
    *IE = false;
}

void Processor::DMAIn(uint8_t i)
{
    M[*R[0]] = i;
    *R[0] = *R[0] + 1;
}

void Processor::DMAOut(uint8_t & i)
{
    i = M[*R[0]];
    *R[0] = *R[0] + 1;
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

void Processor::RunStop()
{
    if(ui->ButtonRunStop->isChecked())
    {
        ui->ButtonStep->setEnabled(false);
        ui->ButtonRunStop->setText("Stop");
        Clock->start();
    }
    else
    {
        ui->ButtonStep->setEnabled(true);
        ui->ButtonRunStop->setText("Run");
        Clock->stop();
    }
}

void Processor::ExecuteInstruction()
{
    // Fetch Instruction

    uint8_t Instruction = M[*R[*P]];
    *R[*P] = *R[*P] + 1;
    *N = Instruction & 0x0F;
    *I = Instruction >> 4;

    // Execute
    uint8_t Temp;

    switch(*I)
    {
    case 0x0: /* LDN */

        if(*N == 0)
            *R[*P] = *R[*P] - 1;
        else
            *D = M[*R[*N]];
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

            if(*D == 0)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0x3: /* BDF */

            if(*DF)
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

        case 0x8: /* NBR */ *R[*P] = *R[*P] + 1; break;

        case 0x9: /* BNQ */
            if(!*Q)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xA: /* BNZ */

            if(*D != 0)
                R[*P]->setLow(M[*R[*P]]);
            else
                *R[*P] = *R[*P] + 1;
            break;

        case 0xB: /* BNF */

            if(!*DF)
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

    case 0x4: /* LDA */ *D = M[*R[*N]]; *R[*N] = *R[*N] + 1; break;

    case 0x5: /* STR */ M[*R[*N]] = *D; break;

    case 0x6: /* Input / Output */

        switch(*N)
        {
        case 0x0: /* IRX */ *R[*X] = *R[*X] + 1; break;
        case 0x1: /* OUT P1 */ emit Out1(*D); *R[*X] = *R[*X] + 1; break;
        case 0x2: /* OUT P2 */ emit Out2(*D); *R[*X] = *R[*X] + 1; break;
        case 0x3: /* OUT P3 */ emit Out3(*D); *R[*X] = *R[*X] + 1; break;
        case 0x4: /* OUT P4 */ emit Out4(*D); *R[*X] = *R[*X] + 1; break;
        case 0x5: /* OUT P5 */ emit Out5(*D); *R[*X] = *R[*X] + 1; break;
        case 0x6: /* OUT P6 */ emit Out6(*D); *R[*X] = *R[*X] + 1; break;
        case 0x7: /* OUT P7 */ emit Out7(*D); *R[*X] = *R[*X] + 1; break;

        case 0x8: // Extended 1806 Instructions

            break;

        case 0x9: /* INP P1 */ emit Inp1(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xA: /* INP P2 */ emit Inp2(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xB: /* INP P3 */ emit Inp3(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xC: /* INP P4 */ emit Inp4(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xD: /* INP P5 */ emit Inp5(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xE: /* INP P6 */ emit Inp6(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        case 0xF: /* INP P7 */ emit Inp7(Temp); *D = Temp; M[*R[*X]] = Temp; break;
        }
        break;

    case 0x7: /* Misc */

        switch(*N)
        {
        case 0x0: /* RET  */ Temp = M[*R[*X]]; *R[*X] = *R[*X] + 1; *X = Temp >> 4; *P = Temp & 0xF; *IE = true; break;
        case 0x1: /* DIS  */ Temp = M[*R[*X]]; *R[*X] = *R[*X] + 1; *X = Temp >> 4; *P = Temp & 0xF; *IE = false; break;
        case 0x2: /* LDXA */ *D = M[*R[*X]]; *R[*X] = *R[*X] + 1; break;
        case 0x3: /* STXD */ M[*R[*X]] = *D; *R[*X] = *R[*X] - 1; break;
        case 0x4: /* ADC  */ *D = *D + M[*R[*X]] + *DF; *DF = D->high() & 1; *D = *D & 0xFF; break;
        case 0x5: /* SDB  */ *D = M[*R[*X]] - *D - (*DF^1); *DF = (D->high() & 1)^1; *D = *D & 0xFF; break;
        case 0x6: /* SHRC */ *D = *D << 1; *D = *D + *DF; *DF = D->high() & 1; *D = *D & 0xFF; break;
        case 0x7: /* SMB  */ *D = *D - M[*R[*X]] - (*DF^1); *DF = (D->high() & 1)^1; *D = *D & 0xFF; break;
        case 0x8: /* SAV  */ M[*R[*X]] = *T; break;
        case 0x9: /* MARK */ *T = *X << 4 + *P; M[*R[2]] = *T; *P = X->value(); *R[2] = *R[2] -1; break;
        case 0xA: /* REQ  */ *Q = false; break;
        case 0xB: /* SEQ  */ *Q = true; break;
        case 0xC: /* ADCI */ *D = *D + M[*R[*P]] + *DF; *DF = D->high() & 1; *D = *D & 0xFF; *R[*P] = *R[*P] +1; break;
        case 0xD: /* SDBI */ *D = M[*R[*P]] - *D - (*DF^1); *DF = (D->high() & 1)^1; *D = *D & 0xFF; *R[*P] = *R[*P] +1; break;
        case 0xE: /* SHLC */ *D = Temp = *D & 1; *D = *D >> 1; *D = *D + (*DF << 7); *DF = Temp; break;
        case 0xF: /* SMBI */ *D = *D - M[*R[*P]] - (*DF^1); *DF = (D->high() & 1)^1; *D = *D & 0xFF; *R[*P] = *R[*P] +1; break;
        }
        break;

    case 0x8: /* GLO */ *D = R[*N]->low(); break;
    case 0x9: /* GHI */ *D = R[*N]->high(); break;
    case 0xA: /* PLO */ R[*N]->setLow(*D); break;
    case 0xB: /* PHI */ R[*N]->setHigh(*D); break;

    case 0xC: /* Long Branch / Skip */

        switch(*N)
        {
        case 0x0: /* LBR  */ *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]]; break;
        case 0x1: /* LBQ  */

            if(*Q)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x2: /* LBZ  */

            if(*D == 0)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x3: /* LBDF */

            if(*DF)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0x4: /* NOP */ break;

        case 0x5: /* LSNQ */

            if(!*Q)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x6: /* LSNZ */

            if(*D != 0)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x7: /* LSNF  */

            if(!*DF)
                *R[*P] = *R[*P] + 2;
            break;

        case 0x8: /* LSKP */

            *R[*P] = *R[*P] + 2;
            break;

        case 0x9: /* LBNQ */

            if(!*Q)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0xA: /* LBNZ */

            if(*D != 0)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
            else
                *R[*P] = *R[*P] + 2;
            break;

        case 0xB: /* LBNF */

            if(*DF != 0)
                *R[*P] = (M[*R[*P]] << 8) + M[*R[*P + 1]];
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

            if(*D == 0)
                *R[*P] = *R[*P] + 2;
            break;

        case 0xF: /* LSDF */

            if(*DF == 1)
                *R[*P] = *R[*P] + 2;
            break;
        }
        break;

    case 0xD: /* SEP */ *P = N->value(); break;
    case 0xE: /* SEX */ *X = N->value(); break;

    case 0xF: /* Misc */

        switch(*N)
        {
        case 0x0: /* LDX  */ *D = M[*R[*X]]; break;
        case 0x1: /* OR   */ *D = *D | M[*R[*X]]; break;
        case 0x2: /* AND  */ *D = *D & M[*R[*X]]; break;
        case 0x3: /* XOR  */ *D = *D ^ M[*R[*X]]; break;
        case 0x4: /* ADD  */ *D = *D + M[*R[*X]]; *DF = D->high() & 1; *D = *D & 0xFF; break;
        case 0x5: /* SD   */ *D = M[*R[*X]] - *D; *DF = (D->high() & 1)^1; *D = *D & 0xFF; break;
        case 0x6: /* SHR  */ *DF = *D & 1; *D = *D >> 1; break;
        case 0x7: /* SM   */ *D = *D - M[*R[*X]]; *DF = (D->high() & 1)^1; *D = *D & 0xFF; break;
        case 0x8: /* LDI  */ *D = M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0x9: /* ORI  */ *D = *D | M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xA: /* ANI  */ *D = *D & M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xB: /* XRI  */ *D = *D ^ M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xC: /* ADI  */ *D = *D + M[*R[*P]]; *DF = D->high() & 1; *D = *D & 0xFF; *R[*P] = *R[*P] + 1; break;
        case 0xD: /* SDI  */ *D = M[*R[*P]] - *D; *DF = (D->high() & 1)^1; *D = *D & 0xFF; *R[*P] = *R[*P] + 1; break;
        case 0xE: /* SHL  */ *D = *D << 1; *DF = D->high() & 1; break;
        case 0xF: /* SMI  */ *D = *D - M[*R[*P]]; *DF = (D->high() & 1)^1; *D = *D & 0xFF; *R[*P] = *R[*P] + 1; break;
        }
    }
}

Processor::~Processor()
{
    delete ui;
}
