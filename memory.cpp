#include "memory.h"

Memory::Memory(QWidget *parent)
    : QWidget{parent}
{
    // Dummy porogram - toggle Q
    M[0] = 0x71;
    M[1] = 0x23;
    M[2] = 0x7B;
    M[3] = 0x7A;
    M[4] = 0x30;
    M[5] = 0x00;
}

uint8_t Memory::operator [] (uint8_t i) const
{
    return M[i];
}
uint8_t& Memory::operator [] (uint8_t i)
{
    return M[i];
}
