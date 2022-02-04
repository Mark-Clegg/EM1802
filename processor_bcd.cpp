#include "processor.h"

uint8_t Processor::BCD_Add(uint8_t a, uint8_t b, uint16_t & carry)
{
    int A = (a >> 4) * 10 + (a & 0x0F);
    int B = (b >> 4) * 10 + (b & 0x0F);

    int temp = A + B + carry;

    carry = temp >99 ? 1 : 0;
    return ((((temp & 0xFF) / 10) % 10) << 4) + (temp % 10);
}

uint8_t Processor::BCD_Subtract(uint8_t a, uint8_t b, uint16_t & borrow)
{
    uint8_t b10sComp = 0x99 - b + 1;
    uint8_t ah = a >> 4;
    uint8_t al = a & 0x0F;
    uint8_t bh = b10sComp >> 4;
    uint8_t bl = b10sComp & 0x0F;

    uint8_t cl = al + bl - (borrow ^ 1);
    if(cl > 9)
    {
        cl += 6;
        borrow = 1;
        cl &= 0x0f;
    }
    else
        borrow = 0;

    uint8_t ch = ah + bh + borrow;
    if(ch > 9)
    {
        ch += 6;
        borrow = 1;
        ch &= 0x0F;
    }
    else
        borrow = 0;

    uint8_t d = (ch << 4) | cl;

    return d;
}
