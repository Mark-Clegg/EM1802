#ifndef IDEREGISTERS_H
#define IDEREGISTERS_H

enum IDE_Register
{
    IDE_Reg_Data = 0,
    IDE_Reg_Error = 1,
    IDE_Reg_Status = 7,
    IDE_Reg_AltStatus = 0x0E,
    IDE_Reg_ActiveStatus = 0x0F,

    IDE_Reg_Feature = 1,
    IDE_Reg_SectorCount = 2,
    IDE_Reg_StartSector = 3,
    IDE_Reg_CylinderLow = 4,
    IDE_Reg_CylinderHigh = 5,
    IDE_Reg_HeadDevice = 6,
    IDE_Reg_Command = 7,
    IDE_Reg_IRQReset = 0x0E
};

#endif // IDEREGISTERS_H
