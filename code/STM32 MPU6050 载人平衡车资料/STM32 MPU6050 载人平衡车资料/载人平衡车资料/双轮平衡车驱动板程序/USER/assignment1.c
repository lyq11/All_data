/*�ļ����ƣ�����1
1���ɼ�mpu6050���ݡ�������Ӳ��I2C1
2������Ƕȣ����ٶȡ��������ɵ�λ����
3�������ںϡ�������������˲��㷨
4�������ٶ���λ�ơ������ϴε�PWM����
5��PID�����㷨
6���õ���Ҫ��PWMD��ֵ

��Ƭ���ͺţ�STM32F103RCT6 32KB 256KB

��Ҫ�õ����ⲿ��Դ

I2C1��SCL-PB6;SDA-PB7

��汾  ��ST3.5.0

*/


#include "assignment1.h"
#include "assignment2.h"
#include "assignment3.h"
#include "SysTick.h"
#include "math.h"//��ѧ��

uint32_t I2C_MPU_ACCE_Read[2];//���ٶȼ���ʱ����
uint32_t I2C_MPU_GYRO_Read[2];//��������ʱ����

void Kalman_Filter(float Accel,float Gyro);

/*����ȫ�ֱ���*/
float Accel_z;	     //z����ٶ�ֵ�ݴ�
float Accel_x;	     //X����ٶ�ֵ�ݴ�
float Accel_y;	     //y����ٶ�ֵ�ݴ�

float Gyro_y;        //Y�������������ݴ�

float Angle_gy;      //�ɽ��ٶȼ������б�Ƕ�
float Angle_az;      //�ɼ��ٶȼ������б�Ƕ�

float Angle;         //С��������б�Ƕ�

float PWM;//���յõ���PWM����ֵ

int   speed_mr;		 //�ҵ��ת��
int   speed_ml;		 //����ת��
int int_PWM;				//����PWMֵ

float PWM;           //�ۺ�PWM����
float PWMI;			 //PWM����ֵ

/*�������*/
float speed;        //���ת���˲�
float position;	    //λ��
float speed_filter; //ת����
float integral;//�ٶȻ��ְٷֱ�


float x;
float y;
float z;

/*��ʱ����*/

/**us��ʱ����**/
void Delay_us(uint32_t xus)
{
	uint8_t i;
	while(xus--)
	{
		i=7;
		while(i--);
	}
}

/**ms��ʱ����**/
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}



/***************************************************��1�����ɼ�mpu6050����******************************************************/

/*��ʼ��I2C1�������*/
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd  (RCC_APB2Periph_GPIOB, ENABLE );//ʹ���� I2C1 �йص�ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PB6-I2C1_SCL��PB7-I2C1_SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

/*Ӳ������I2C1*/
static void I2C_Mode_Config(void)
{
	I2C_InitTypeDef I2C_InitStructure;  
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; //I2C ���� 
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
	I2C_InitStructure.I2C_OwnAddress1 = MPU_ADRESS; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; 		
	I2C_Cmd(I2C1,ENABLE);//ʹ�� I2C1 
	I2C_Init(I2C1, &I2C_InitStructure);//I2C1 ��ʼ��	   
	I2C_AcknowledgeConfig(I2C1, ENABLE);//����Ӧ��ģʽ  
}

/*
 * ��������I2C_MPU6050_ByteWrite
 * ����  ��дһ���ֽڵ�I2C MPU6050�Ĵ�����
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�MPU6050�Ĵ����ĵ�ַ 
 * ���  ����
 * ����  ����
 * ����  ���ڲ�����
 */	
static void I2C_MPU_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));/*wait until I2C bus is not busy*///�ȴ�iic�����Ƿ�æµ
  I2C_GenerateSTART(I2C1, ENABLE);/* Send START condition *///���Ϳ�ʼ����
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/* Test on EV5 and clear it *///�¼� 
  I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Transmitter);/* Send MMA address for write *///�����豸��
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));/* Test on EV6 and clear it *///�¼�    
  I2C_SendData(I2C1, WriteAddr);/* Send the MMA's Register address to write to *///�����豸�Ĵ�����ַ 
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/* Test on EV8 and clear it *///�¼�   
  I2C_SendData(I2C1, pBuffer);/* Send the byte to be written *///����д���豸�Ĵ��������� 
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/* Test on EV8 and clear it *///�¼�    
  I2C_GenerateSTOP(I2C1, ENABLE);/* Send STOP condition *///ֹͣ�ź�
}


/*
 * ��������I2C_MMA_ByteRead
 * ����  ����MMA�Ĵ��������ȡһ�����ݡ� 
 * ����  �� -ReadAddr �������ݵ�MMA�Ĵ����ĵ�ַ��        
 * ���  ����
 * ����  ����ȡ���ļĴ�������
 * ����  ���ڲ�����
 */	

void I2C_MPU_ByteRead(uint32_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)//��һ��������ָ�����������һ����ַ���1��
																				   //�ڶ��������ǼĴ�����ַ��ֻ��Ҫ�����һ����ַ����																					//�ڶ��ڶ�����ַ�������������Ƕ��ٸ�8λ��ָ����ۼ�
{   
   while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//���æµ   
  I2C_GenerateSTART(I2C1, ENABLE);//��ʼ�ź�  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//Ӧ��
  I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Transmitter);//�����豸��ַ
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//Ӧ�� 
  I2C_Cmd(I2C1, ENABLE);//ʹ��iic
  I2C_SendData(I2C1, ReadAddr);//�����豸�Ĵ�����ַ  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//Ӧ��    
  I2C_GenerateSTART(I2C1, ENABLE);//��ʼ�ź�  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//Ӧ��  
  I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Receiver);//�����豸��ַ��+1�����ź�  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//Ӧ��  
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)//����Ͷ�һ��һ��
    {
      I2C_AcknowledgeConfig(I2C1, DISABLE);//��Ӧ��      
      I2C_GenerateSTOP(I2C1, ENABLE);//ֹͣ�ź�
    }
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))//Ӧ��  
    {      
      *pBuffer = I2C_ReceiveData(I2C1);//�������ݴ���ָ�����
      pBuffer++;//ָ�������ַ+1       
      NumByteToRead--;//Ҫ���ĵ�ַ��������ݸ���-1        
    }   
  }
  I2C_AcknowledgeConfig(I2C1, ENABLE);//��Ӧ��
}

/*
 * ��������I2C_MPU6050_Init
 * ����  ��I2C ����(MPU6050)��ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void I2C_MPU6050_Init(void)
{	   
 	I2C_GPIO_Config();
	I2C_Mode_Config();
}

/*д���ʼ��MPU6050*/
void InitMPU6050()
{
	I2C_MPU_ByteWrite(0x00,PWR_MGMT_1);	//�������״̬
	I2C_MPU_ByteWrite(0x07,SMPLRT_DIV);//IICд��ʱ�ĵ�ַ�ֽ�����
	I2C_MPU_ByteWrite(0x06,CONFIG);//��ͨ�˲�Ƶ��
	I2C_MPU_ByteWrite(0x18,GYRO_CONFIG);//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
	I2C_MPU_ByteWrite(0x01,ACCEL_CONFIG);//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
}

/*��ȡ���ݣ��ϳ�����*/
uint32_t ACCE_Data_Z(uint8_t ZOUT_H)//z�� ���ٶ�
{
	uint32_t ACCEL_H;
	uint8_t ACCEL_L;
	I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,ZOUT_H,2);
	
	ACCEL_H=I2C_MPU_ACCE_Read[0];
	ACCEL_L=I2C_MPU_ACCE_Read[1];
	
	return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

uint32_t ACCE_Data_X(uint8_t XOUT_H)//x�� ���ٶ�
{
	uint32_t ACCEL_H;
	uint8_t ACCEL_L;
	I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,XOUT_H,2);
	
	ACCEL_H=I2C_MPU_ACCE_Read[0];
	ACCEL_L=I2C_MPU_ACCE_Read[1];
	
	return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

uint32_t ACCE_Data_Y(uint8_t YOUT_H)//y�� ���ٶ�
{
	uint32_t ACCEL_H;
	uint8_t ACCEL_L;
	I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,YOUT_H,2);
	
	ACCEL_H=I2C_MPU_ACCE_Read[0];
	ACCEL_L=I2C_MPU_ACCE_Read[1];
	
	return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

uint32_t GYRO_Data_Y(uint8_t YOUT_H)//y�� ������
{
	uint32_t GYRO_H;
	uint8_t GYRO_L;

	I2C_MPU_ByteRead(I2C_MPU_GYRO_Read,YOUT_H,2);

	GYRO_H=I2C_MPU_GYRO_Read[0];//Y�������Ǹ�λ
	GYRO_L=I2C_MPU_GYRO_Read[1];

	return (GYRO_H<<8)+GYRO_L;   //�ϳ�����
}

/*********************************************************��2��������Ƕȣ����ٶ�**********************************************/

/*�Ƕȼ���*/
void Angle_Calcu(void)	 
{

//��ΧΪ2gʱ�������ϵ��16384 LSB/g
//deg = rad*180/3.14

/*����z���ϵ��������ٶȼ������оƬ��ˮƽ��ļн�*/
		 	
   	Accel_x=ACCE_Data_X(ACCEL_XOUT_H);//x��
//	Accel_y=ACCE_Data_Y(ACCEL_YOUT_H);//y��
	Accel_z=ACCE_Data_Z(ACCEL_ZOUT_H);//z��

	Gyro_y = GYRO_Data_Y(GYRO_YOUT_H);//GetData�ϳɵ�ԭʼ����


	if(Accel_x<32764)
	{
		x=Accel_x/16384;
	}
	else
	{
		x=1-(Accel_x-49152)/16384;
	}

//	if(Accel_y<32764)
//	{
//		y=Accel_y/16384;
//	}
//	else
//	{
//		y=1-(Accel_y-49152)/16384;
//	}

	if(Accel_z<32764)
	{
		z=Accel_z/16384;
	}
	else
	{
		z=(Accel_z-49152)/16384;
	}

	Angle_az=(atan(x/z))*180/3.14;

/*�Ƕȵ�������*/											
	if(Accel_x<32764)
	{
	Angle_az=+Angle_az;
	}
	if(Accel_x>32764)
	{
	Angle_az=-Angle_az;
	}

    /*���ٶ�*/
	if(Gyro_y<32768)//��ǰ�˶�
	{
	Gyro_y=-(Gyro_y/16.4);//��ΧΪ1000deg/sʱ�������ϵ��16.4 LSB/(deg/s)
	}
	if(Gyro_y>32768)//����˶�
	{
	Gyro_y=+(65535-Gyro_y)/16.4;
	}
	
	//Angle_gy = Angle_gy + Gyro_y*0.025;  //���ٶȻ��ֵõ���б�Ƕ�.Խ����ֳ����ĽǶ�Խ��	
	 	
	//-------�������˲��ں�-----------------------

	Kalman_Filter(Angle_az,Gyro_y);       //�������˲��������
															  
} 



/**********************************************************��3��������������*************************************************/
		
float Q_angle=0.001;//0.001  
float Q_gyro=0.003;//0.03
float R_angle=0.5;//0.5
float dt=0.01;//0.1	                  //dtΪkalman�˲�������ʱ��;
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

/*����������*/
void Kalman_Filter(float Accel,float Gyro)		
{
	Angle+=(Gyro - Q_bias) * dt; //�������

	
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]=- PP[1][1];
	Pdot[2]=- PP[1][1];
	Pdot[3]=Q_gyro;
	
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - Angle;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	Angle	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	Gyro_y   = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�

}


/****************************************************��4 �������ת�ٺ�λ��ֵ����******************************************/


//void Psn_Calcu(void)	 
//{
//	
//	speed_r_l =PWM*0.5;
//	speed *= 0.7;		                  //�����ٶ��˲�
//	speed += speed_r_l*0.3;	
//	position += speed;	                  //���ֵõ�λ��
//}

/******************************************************��5�������PID�㷨**********************************************/
extern float Kp;
static float Kd =12.0;	    //2.6PID����
// static float Kpn = 0.0;    //0.01PID����
// static float Ksp = 0.0;	    //2.0PID����

void PWM_Calcu()	 
{
	
// 	speed=PWM*0.04;//��PWM�ź�����ٶ��ź�	
// 	
// 	speed_filter*=0.9;		
// 	speed_filter+=speed*0.1;//�ٶ��ź��˲�	
// 	
// 	position+=speed_filter;//�ٶ�ת����λ���ź�
// 	
// 		
// 	PWM  = Kp*Angle+Kd*Gyro_y+Kpn*position + Ksp*speed_filter;          //PID��PWM= kp*��б��+kd*���ٶ� + kpn*λ���� +ksp*���ת���˲����ֵ		
//	speed=PWM*0.1;
	
	
	PWM=1000*((Angle/90)*Kp)+Kd*Gyro_y;
//	PWM  = 12*Kp*Angle+Kd*Gyro_y;
	
	
	int_PWM=(int)PWM;//ǿ��ת��������PWM
}


















