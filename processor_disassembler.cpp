#include "processor.h"

enum NType
{
    Register,
    Port,
    None
};

struct Decode
{
    const QString OpCode;
    const NType Type;
    const int Bytes;
    const int XequalPBytes;
};

static const Decode OpCodes[] = {
    { "IDL",  None,     0, 0 },  /*  0 */
    { "LDN",  Register, 0, 0 },  /*  1 */
    { "INC",  Register, 0, 0 },  /*  2 */
    { "DEC",  Register, 0, 0 },  /*  3 */
    { "BR",   None,     1, 0 },  /*  4 */
    { "BQ",   None,     1, 0 },  /*  5 */
    { "BZ",   None,     1, 0 },  /*  6 */
    { "BDF",  None,     1, 0 },  /*  7 */
    { "B1",   None,     1, 0 },  /*  8 */
    { "B2",   None,     1, 0 },  /*  9 */
    { "B3",   None,     1, 0 },  /* 10 */
    { "B4",   None,     1, 0 },  /* 11 */
    { "SKP",  None,     0, 0 },  /* 12 */
    { "BNQ",  None,     1, 0 },  /* 13 */
    { "BNZ",  None,     1, 0 },  /* 14 */
    { "BNF",  None,     1, 0 },  /* 15 */
    { "BN1",  None,     1, 0 },  /* 16 */
    { "BN2",  None,     1, 0 },  /* 17 */
    { "BN3",  None,     1, 0 },  /* 18 */
    { "BN4",  None,     1, 0 },  /* 19 */
    { "LDA",  Register, 0, 0 },  /* 20 */
    { "STR",  Register, 0, 0 },  /* 21 */
    { "IRX",  None,     0, 0 },  /* 22 */
    { "OUT",  Port,     0, 1 },  /* 23 */
    { "INP",  Port,     0, 0 },  /* 24 */
    { "RET",  None,     0, 1 },  /* 25 */
    { "DIS",  None,     0, 1 },  /* 26 */
    { "LDXA", None,     0, 1 },  /* 27 */
    { "STXD", None,     0, 0 },  /* 28 */
    { "ADC",  None,     0, 0 },  /* 29 */
    { "SDB",  None,     0, 0 },  /* 30 */
    { "SHRC", None,     0, 0 },  /* 31 */
    { "SMB",  None,     0, 0 },  /* 32 */
    { "SAV",  None,     0, 0 },  /* 33 */
    { "MARK", None,     0, 0 },  /* 34 */
    { "REQ",  None,     0, 0 },  /* 35 */
    { "SEQ",  None,     0, 0 },  /* 36 */
    { "ADCI", None,     1, 0 },  /* 37 */
    { "SDBI", None,     1, 0 },  /* 38 */
    { "SHLC", None,     0, 0 },  /* 39 */
    { "SMBI", None,     1, 0 },  /* 40 */
    { "GLO",  Register, 0, 0 },  /* 41 */
    { "GHI",  Register, 0, 0 },  /* 42 */
    { "PLO",  Register, 0, 0 },  /* 43 */
    { "PHI",  Register, 0, 0 },  /* 44 */
    { "LBR",  None,     2, 0 },  /* 45 */
    { "LBQ",  None,     2, 0 },  /* 46 */
    { "LBZ",  None,     2, 0 },  /* 47 */
    { "LBDF", None,     2, 0 },  /* 48 */
    { "NOP",  None,     0, 0 },  /* 49 */
    { "LSNQ", None,     0, 0 },  /* 50 */
    { "LSNZ", None,     0, 0 },  /* 51 */
    { "LSNF", None,     0, 0 },  /* 52 */
    { "LSKP", None,     0, 0 },  /* 53 */
    { "LBNQ", None,     2, 0 },  /* 54 */
    { "LBNZ", None,     2, 0 },  /* 55 */
    { "LBNF", None,     2, 0 },  /* 56 */
    { "LSIE", None,     0, 0 },  /* 57 */
    { "LSQ",  None,     0, 0 },  /* 58 */
    { "LSZ",  None,     0, 0 },  /* 59 */
    { "LSDF", None,     0, 0 },  /* 60 */
    { "SEP",  Register, 0, 0 },  /* 61 */
    { "SEX",  Register, 0, 0 },  /* 62 */
    { "LDX",  None,     0, 0 },  /* 63 */
    { "OR",   None,     0, 0 },  /* 64 */
    { "AND",  None,     0, 0 },  /* 65 */
    { "XOR",  None,     0, 0 },  /* 66 */
    { "ADD",  None,     0, 0 },  /* 67 */
    { "SD",   None,     0, 0 },  /* 68 */
    { "SHR",  None,     0, 0 },  /* 69 */
    { "SM",   None,     0, 0 },  /* 70 */
    { "LDI",  None,     1, 0 },  /* 71 */
    { "ORI",  None,     1, 0 },  /* 72 */
    { "ANI",  None,     1, 0 },  /* 73 */
    { "XRI",  None,     1, 0 },  /* 74 */
    { "ADI",  None,     1, 0 },  /* 75 */
    { "SDI",  None,     1, 0 },  /* 76 */
    { "SHL",  None,     0, 0 },  /* 77 */
    { "SMI",  None,     1, 0 }   /* 78 */
};

static const int OpCodeLookup[256] = {
    /* 0x0- */  0,  1,  1,  1,      1,  1,  1,  1,      1,  1,  1,  1,      1,  1,  1,  1,
    /* 0x1- */  2,  2,  2,  2,      2,  2,  2,  2,      2,  2,  2,  2,      2,  2,  2,  2,
    /* 0x2- */  3,  3,  3,  3,      3,  3,  3,  3,      3,  3,  3,  3,      3,  3,  3,  3,
    /* 0x3- */  4,  5,  6,  7,      8,  9, 10, 11,     12, 13, 14, 15,     16, 17, 18, 19,
    /* 0x4- */ 20, 20, 20, 20,     20, 20, 20, 20,     20, 20, 20, 20,     20, 20, 20, 20,
    /* 0x5- */ 21, 21, 21, 21,     21, 21, 21, 21,     21, 21, 21, 21,     21, 21, 21, 21,
    /* 0x6- */ 22, 23, 23, 23,     23, 23, 23, 23, 000000, 24, 24, 24,     24, 24, 24, 24,
    /* 0x7- */ 25, 26, 27, 28,     29, 30, 31, 32,     33, 34, 35, 36,     37, 38, 39, 40,
    /* 0x8- */ 41, 41, 41, 41,     41, 41, 41, 41,     41, 41, 41, 41,     41, 41, 41, 41,
    /* 0x9- */ 42, 42, 42, 42,     42, 42, 42, 42,     42, 42, 42, 42,     42, 42, 42, 42,
    /* 0xA- */ 43, 43, 43, 43,     43, 43, 43, 43,     43, 43, 43, 43,     43, 43, 43, 43,
    /* 0xB- */ 44, 44, 44, 44,     44, 44, 44, 44,     44, 44, 44, 44,     44, 44, 44, 44,
    /* 0xC- */ 45, 46, 47, 48,     49, 50, 51, 52,     53, 54, 55, 56,     57, 58, 59, 60,
    /* 0xD- */ 61, 61, 61, 61,     61, 61, 61, 61,     61, 61, 61, 61,     61, 61, 61, 61,
    /* 0xE- */ 62, 62, 62, 62,     62, 62, 62, 62,     62, 62, 62, 62,     62, 62, 62, 62,
    /* 0xF- */ 63, 64, 65, 66,     67, 68, 69, 70,     71, 72, 73, 74,     75, 76, 77, 78
};

QString Processor::Disassemble(uint16_t Address, int Lines)
{
    QStringList Disassembly;
    int ExtraBytes = 0;

    for(int i=0; i < Lines; i++)
    {
        int Addr = Address;
        if(ExtraBytes > 0)
        {
            QStringList Operands;
            for(int b = 0; b < ExtraBytes; b++)
                Operands += QString("%1").arg(M[Address++], 2, 16, QChar('0'));

            Disassembly += DisassemblyLine(Addr, Operands, "DB", Operands);
            ExtraBytes = 0;
        }
        else
        {
            int Instruction = M[Address++];
            int N = Instruction & 0x0F;
            QStringList CodeBytes;

            CodeBytes += QString("%1").arg(Instruction, 2, 16, QChar('0'));

            const Decode * Decoder = &OpCodes[OpCodeLookup[Instruction]];

            QStringList Operands;

            switch(Decoder->Type)
            {
            case Register:  Operands += QString("R%1").arg(N, 1, 16).toUpper();
                            break;

            case Port:      Operands += QString("P%1").arg(N & 7, 1, 16).toUpper();
                            break;

            case None:      break;
            }

            if(Decoder->Bytes > 0)
            {
                uint16_t Data = 0;
                for(int b = 0; b < Decoder->Bytes; b++)
                {
                    uint8_t Byte = M[Address++];
                    Data = (Data << 8) + Byte;
                    CodeBytes += QString("%1").arg(Data, 2, 16, QChar('0'));
                }
                Operands += QString("%1").arg(Data, Decoder->Bytes * 2, 16, QChar('0'));
            }
            Disassembly += DisassemblyLine(Addr, CodeBytes, Decoder->OpCode, Operands);

            if(*X == P->value())
                ExtraBytes = Decoder->XequalPBytes;
        }
    }
    return Disassembly.join(QChar('\n'));
}

QString Processor::DisassemblyLine(const int &Addr, const QStringList &CodeBytes, const QString &OpCode, const QStringList&Operands)
{
    return QString("%1  %2  %3  %4")
                .arg(Addr, 4, 16, QChar('0'))
                .arg(CodeBytes.join(QChar(' ')), -11)
                .arg(OpCode, -4)
                .arg(Operands.join(QString(", ")), -8);

}
