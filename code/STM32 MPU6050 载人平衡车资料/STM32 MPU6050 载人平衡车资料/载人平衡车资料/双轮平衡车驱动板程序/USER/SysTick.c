/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��SysTick.c
 * ����    ��SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
 *           ���õ��� 1us 10us 1ms �жϡ�         

 * ��汾  ��ST3.5.0
*/

#include "SysTick.h"
#include "assignment1.h"
#include "assignment2.h"
#include "assignment3.h"

extern float Angle;//�Ƕ�
extern float Gyro_y;//������

extern float Accel_x;//X����ٶ�ֵ�ݴ�
extern float Accel_y;//Y����ٶ�ֵ�ݴ�
extern float Accel_z;//���ٶȼ�ֵ�ݴ�

extern float Angle_az;//���ٶȼ�������Ƕ�	   
extern float Angle_gy;//�����Ǽ�������

extern float PWM;//������PEMֵ

extern int PWM_R;//����PWMֵ����
extern int PWM_L; //����PWMֵ����

extern int ADC_Direction;
extern int Direction_Difference;

extern float x;
extern float y;
extern float z;

extern int int_PWM;//����PWMֵ

extern float integral;//�ٶȻ��ְٷֱ�

/*����ϵͳ�δ�ʱ�� SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	 SysTick_Config(SystemCoreClock /200);//200hzϵͳƵ��

}

/*��ʱ���жϷ�����������ϵͳ��������*/
float a;
float Kp;

void SysTick_Handler()
{	
	
	a++;
	Kp=a/40;
	if(Kp>12)
	{
	Kp=12;	
	}//����kp�Ĳ���������㷨���ڻ�������pid����
		
	
	Angle_Calcu();//�Ƕȼ���
	PWM_Calcu();//pwm�������
	Direction_Dispose();//�����㷨��pwm���
//	printf("%f\n",Kp);

}




