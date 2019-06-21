

#include "iwdg.h"
#include "stm32f0xx_iwdg.h"
 /**
  * @file   IWDG_Config
  * @brief  ��ʼ���������Ź�
  * @param  ��
  * @retval ��
  */
void IWDG_Config(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	IWDG_SetPrescaler(IWDG_Prescaler_32);		  //����IWDGԤ��Ƶֵ��256��Ƶ���
	IWDG_SetReload(40000/64);	   //����IWDG����װ��ֵ	:��Χ0~0x0FFF
	IWDG_ReloadCounter();	   //ι��������װ�ؼ�����
	IWDG_Enable();			   //ʹ��IWDG��ʱ��
}

  /* 
�趨IWDG�������ʱ�� 500ms
     Counter Reload Value = 500ms/IWDG counter clock period
                          = 500ms / (LSI/32)
                          = 0.5s / (LsiFreq/32)
                          = LsiFreq/(32 * 2)
                          = LsiFreq/64
  */
