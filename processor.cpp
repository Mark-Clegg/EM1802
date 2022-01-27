#include "processor.h"
#include "ui_processor.h"
#include <cmath>

Processor::Processor(QWidget *parent, Memory &RAM) :
    QDockWidget(parent),
    ui(new Ui::Processor),
    M(RAM)
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
    connect(Clock, &QTimer::timeout, this, &Processor::ExecuteInstruction);
    connect(ui->ClockSpeed, &QSlider::valueChanged, this, [this](int i) { Clock->setInterval(pow(2,10-i)-1);});

    MasterReset();
}

void Processor::MasterReset()
{
    Clock->stop();
    *P = 0;
    *X = 0;
    *Q = 0;
    *IE = 1;
    *R[0] = 0;
    Idle = false;
    M.setPosition(0);
    emit Reset();
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

void Processor::PChanged()
{
    // Set Coloured Indicator next to R[x]
    for(int i=0;i<16;i++)
        if(i == *P)
            R[i]->setRIndicator(Qt::red);
        else
            R[i]->setRIndicator(Qt::black);
}

void Processor::Interrupt()
{
    *T = (*X << 4) | *P;
    *X = 2;
    *P = 1;
    *IE = false;
    Idle = false;
}

void Processor::DMAIn(uint8_t i)
{
    M[*R[0]] = i;
    *R[0] = *R[0] + 1;
    Idle = false;
}

void Processor::DMAOut(uint8_t & i)
{
    i = M[*R[0]];
    *R[0] = *R[0] + 1;
    Idle = false;
}

void Processor::Run()
{
    Idle = false;
    Clock->start();
}

void Processor::Stop()
{
    Clock->stop();
}

void Processor::ExecuteInstruction()
{
    if(Idle)
        return;

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
            Idle = true;
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

            break;

        case 0x9: /* INP P1 */ emit Inp1(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xA: /* INP P2 */ emit Inp2(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xB: /* INP P3 */ emit Inp3(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xC: /* INP P4 */ emit Inp4(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xD: /* INP P5 */ emit Inp5(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xE: /* INP P6 */ emit Inp6(Temp); d = Temp; M[*R[*X]] = Temp; break;
        case 0xF: /* INP P7 */ emit Inp7(Temp); d = Temp; M[*R[*X]] = Temp; break;
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
        case 0x6: /* SHRC */ d |= df << 8; df = d & 0x01; d = d >> 1; break;
        case 0x7: /* SMB  */ d |= 0x0100; d = d - M[*R[*X]] - (df^1); df = d >> 8; break;
        case 0x8: /* SAV  */ M[*R[*X]] = *T; break;
        case 0x9: /* MARK */ *T = (*X << 4) + *P; M[*R[2]] = *T; *X = P->value(); *R[2] = *R[2] -1; break;
        case 0xA: /* REQ  */ *Q = false; emit QSignal(false); break;
        case 0xB: /* SEQ  */ *Q = true; emit QSignal(true); break;
        case 0xC: /* ADCI */ d = d + M[*R[*P]] + df; df = d >> 8; *R[*P] = *R[*P] +1; break;
        case 0xD: /* SDBI */ d |= 0x0100; d = M[*R[*P]] - d - (df^1); df = d >> 8; *R[*P] = *R[*P] +1; break;
        case 0xE: /* SHLC */ d = (d << 1) + df; df = d >> 8; break;
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
        case 0x6: /* SHR  */ df = d & 1; d = d >> 1; break;
        case 0x7: /* SM   */ d |= 0x0100; d = d - M[*R[*X]]; df = d >> 8; break;
        case 0x8: /* LDI  */ d = M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0x9: /* ORI  */ d = d | M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xA: /* ANI  */ d = d & M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xB: /* XRI  */ d = d ^ M[*R[*P]]; *R[*P] = *R[*P] + 1; break;
        case 0xC: /* ADI  */ d = d + M[*R[*P]]; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        case 0xD: /* SDI  */ d |= 0x0100; d = M[*R[*P]] - d; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        case 0xE: /* SHL  */ d = d << 1;  df = d >> 8; break;
        case 0xF: /* SMI  */ d |= 0x0100; d = d - M[*R[*P]]; df = d >> 8; *R[*P] = *R[*P] + 1; break;
        }
    }
    // Update D and DF in the UI
    *D = d & 0xFF;
    *DF = df & 0x01;
    M.setPosition(*R[*P]);
}

Processor::~Processor()
{
    delete ui;
}
