#ifndef  _assignment1_H_
#define  _assignment1_H_

#include "stm32f10x.h"

/* ����MPU6050�ڲ���ַ*/
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)

#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

#define	ACCEL_XOUT_H	0x3B//���ٶȼƲ���ֵ�Ĵ�����ַ
#define	ACCEL_XOUT_L	0x3C

#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E

#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
/********************/
#define	TEMP_OUT_H		0x41//�¶Ȳ���ֵ�Ĵ�����ַ
#define	TEMP_OUT_L		0x42
/*********************/
#define	GYRO_XOUT_H		0x43//�����ǲ���ֵ�Ĵ�����ַ
#define	GYRO_XOUT_L		0x44
	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46

#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
/**********************/
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)

#define	MPU_ADRESS 		0xD0   //IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
#define I2C_Speed 		400000//IICͨѶ�ٶ�400khz

void I2C_MPU6050_Init(void);//�����������ӿڳ�ʼ��
void InitMPU6050(void);     //��ʼ��MPU6050

//void assignment1(void);//�ó�����Ҫ��pwm

void Angle_Calcu(void);//�Ƕȼ���
void PWM_Calcu(void);//pwm�������

void LED_Config(void);

void Delay_ms(uint32_t xms);
				
#endif
