
#include "stm32f10x.h"
#include "SysTick.h"
#include "assignment1.h"
#include "assignment2.h"
#include "assignment3.h"

extern float Angle;//�Ƕ�

int main(void)
{	

	SysTick_Init();//��ϵͳ�δ�ʱ�ӳ�ʼ��	
		 
	I2C_MPU6050_Init();//�������������ӿڳ�ʼ��
	
	InitMPU6050();//�۳�ʼ��MPU6050

//	LED_Config();//����ʾLED��ʼ��

  TIM1_Init();//�ݶ�ʱ��tim1��tim8��ʼ��

	ADC1_Init();//��ADC��ʼ��

	USART2_Config();//�߳�ʼ������2

	Direction_Init();//���ʼ��ת��

	while(1)
	{
	}
}



/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
