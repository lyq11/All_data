#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stm32f0xx.h"

///////////////////64k flash, 8k sram///////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//PAGE63  0x0800 7C00
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
u16 STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
u16 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
u16 OptionByte_Read(void) __attribute((section(".ARM.__at_0x8001030")));
void OptionByte_Write(u16 data);// __attribute((section(".ARM.__at_0x8001060")));
void OptionByte_Write_A5A5(void) __attribute((section(".ARM.__at_0x8001060")));
//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);
#endif

















