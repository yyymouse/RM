#include "stm32_flash.h"

uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_SECTOR_6;
    else if (addr < ADDR_FLASH_SECTOR_8)
        return FLASH_SECTOR_7;
    else if (addr < ADDR_FLASH_SECTOR_9)
        return FLASH_SECTOR_8;
    else if (addr < ADDR_FLASH_SECTOR_10)
        return FLASH_SECTOR_9;
    else if (addr < ADDR_FLASH_SECTOR_11)
        return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}

uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(__IO uint32_t *)faddr;
}

void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t Num)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t SectorError = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
        return; //�Ƿ���ַ

    HAL_FLASH_Unlock();            //����
    addrx = WriteAddr;             //д�����ʼ��ַ
    endaddr = WriteAddr + Num * 4; //д��Ľ�����ַ

    if (addrx < 0X080E1000)//�����ж�����д��ķ�Χ
    {
        while (addrx < endaddr)
        {
            if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF)
            {
                FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;     //�������ͣ���������
                FlashEraseInit.Sector = STMFLASH_GetFlashSector(addrx); //Ҫ����������
                FlashEraseInit.NbSectors = 1;                           //һ��ֻ����һ������
                FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;    //��ѹ��Χ��VCC=2.7~3.6V֮��!!
                if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
                {
                    break; //����������
                }
            }
            else
                addrx += 4;
            FLASH_WaitForLastOperation(FLASH_WAITETIME); //�ȴ��ϴβ������
        }
    }
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); //�ȴ��ϴβ������
    if (FlashStatus == HAL_OK)
    {
        while (WriteAddr < endaddr) //д����
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK) //д������
            {
                break; //д���쳣
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }
    HAL_FLASH_Lock(); //����
}

void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); //��ȡ4���ֽ�.
        ReadAddr += 4;                            //ƫ��4���ֽ�.
    }
}

//д����
void FlashWrite_flag(uint32_t StartAddress,uint32_t data)
{
	//�������һЩ StartAddress��ַ �Ƿ���Ч���ж�
    HAL_FLASH_Unlock();//����
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,StartAddress,data);
    HAL_FLASH_Lock();//����
}


