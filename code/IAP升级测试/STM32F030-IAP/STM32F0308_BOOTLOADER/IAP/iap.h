#ifndef __IAP_H__
#define __IAP_H__

#include<stm32f0xx.h>

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
#define FLASH_APP1_ADDR		 ((uint32_t)0x08003000)  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
											//����0X08000000~0X0800FFFF�Ŀռ�ΪIAPʹ��
#define APP1_SIZE_ADDR		(FLASH_APP1_ADDR-4) 
#define APP1_SUCCESS_ADDR	(FLASH_APP1_ADDR-6)

void iap_load_app(u32 appxaddr);			//ִ��flash�����app����
void iap_load_appsram(u32 appxaddr);		//ִ��sram�����app����
u16 iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin
int iap_read_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);
void IAP_Set(void) __attribute((section(".ARM.__at_0x8001000")));

#endif


/*********************************************************************************************/
/*
�鿴FLASH��SRAM��ռ�ÿռ�������Ҫ���������䡣

Program Size: Code=29864 RO-data=123592 RW-data=60 ZI-data=3900
*/
/*********************************************************************************************/
/*
���� Ҫ����ע�����Code��RO-data��RW-data��ZI-data���������˼
Code����ʾ������ռ�� FLASH �Ĵ�С�� FLASH����
RO-data���� Read Only-data�� ��ʾ������ĳ������� const ���ͣ� FLASH����
RW-data���� Read Write-data�� ��ʾ�ѱ���ʼ����ȫ�ֱ����� SRAM��

ZI-data���� Zero Init-data�� ��ʾδ����ʼ����ȫ�ֱ���(SRAM)
*/
/*********************************************************************************************/
/*
��ôFLASH��SRAMռ������ļ��㷽ʽ������ʾ��

flash = Code + RO-data + RW-data = 29864 + 123592 + 60 = 153516 bytes

sram = RW-data + ZI-data = 60+ 3900= 3960bytes

����һ��Ҫ�ر�ע����ǳ���Ĵ�С����.hex�ļ��Ĵ�С�����Ǳ�������Code+RO-data��

�ڳ����ʼ����ʱ��RW-data���FLASH�п�����RAM�С�
*/



































