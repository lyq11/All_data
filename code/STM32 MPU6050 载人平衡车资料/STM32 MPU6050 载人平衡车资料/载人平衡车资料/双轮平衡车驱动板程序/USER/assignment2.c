/*�ļ����ƣ�����2
1�����òɼ������źš�������Ӳ��ADC
2�����ö�ʱ�����á���tim1��tim8
3�������㷨���ٶȺϳ�
4��PWM���

��Ƭ���ͺţ�STM32F103RCT6 32KB 256KB

��Ҫ�õ�����Դ
PB13 34 TIM1_CH1N	   PB14 35 TIM1_CH2N
PA8  41 TIM1_CH1	   PA9  42 TIM1_CH2

PA7 23 TIM8_CH1N	   PB0 26 TIM8_CH1N
PC6 37 TIM8_CH1		   PC7 38 TIM8_CH2

��汾  ��ST3.5.0
*/

#include "assignment2.h"

#include "stm32f10x.h"
/**************************************************��1���������źŲɼ����������õ�ADC***************************************/

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue[4];//adֵ�ݴ�����
int ADC_ConvertedValueLocal; //��ص�ѹֵ
int ADC_Direction;//����adֵԭʼֵ���ϵ�ʱ����
int Direction_Difference;//ת���

extern float PWM;//��ǰ���ļ���ȡȫ�ֱ���
extern int int_PWM;//����PWMֵ

int PWM_R;         //����PWMֵ����
int PWM_L;         //����PWMֵ����
extern float Angle;//�Ƕ�

/*
 * ��������ADC1_GPIO_Config */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Configure PC.01  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				// PC1,����ʱ������������
}


/* ��������ADC1_Mode_Config*/
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;//����4������ռ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����ʹ��
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//���� 16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 //��ֹɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ������ͣ�ؽ���ADCת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 4;	 	//Ҫת����ͨ����Ŀ1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*ADCx,ͨ����ţ�ɨ��˳�򣬲������� */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);//��ص�ѹ
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_1Cycles5);//ת��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_1Cycles5);//��������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_1Cycles5);//�ҵ������
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*��λУ׼�Ĵ��� */   
	ADC_ResetCalibration(ADC1);
	/*�ȴ�У׼�Ĵ�����λ��� */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADCУ׼ */
	ADC_StartCalibration(ADC1);
	/* �ȴ�У׼���*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* ����û�в����ⲿ����������ʹ���������ADCת�� */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*��������ADC1_Init */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}


/****************************************************��2�������ö�ʱ��***************************************/
uint16_t Channel1Pulse = 0, Channel2Pulse = 0;

void TIM1_Mode_Init(void)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

/*��ʱ����ʼ��*/
	Channel1Pulse = 0;//��ʼ����û���κ�pwm���
	Channel2Pulse = 0;

  TIM_TimeBaseStructure.TIM_Prescaler = 12;         //����������ΪTIM ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //������ģʽ  /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_Period = 999;        //�Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 //ʱ�ӷָ� ����Ƶ
//  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;        //���� ���� ����ֵ
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

/*TIM1 ����*/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           //�����ȵ���ģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;         //ʹ������Ƚ�״̬
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //ʹ��  ���� ���״̬

  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;                //��1ͨ�������ʼֵ
  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse; //��2ͨ�������ʼֵ

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;        //����Ƚϼ��Ե�
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//���� ������Ե�
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;   //MOE=0 ���� TIM1����ȽϿ���״̬
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;//MOE=0 ���� TIM1����ȽϿ���״̬

//  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse; //��2ͨ�������ʼֵ

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);//�趨1ͨ���õĲ��� ��ʼ��TIM
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);//�趨1ͨ���õĲ��� ��ʼ��TIM
  
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);//��ʼ��
  TIM_OC2Init(TIM8, &TIM_OCInitStructure);//��ʼ��
		   
/*����ɲ������  ����ʱ��  ����ƽ OSSI OSSR ״̬ AOE(�Զ����ʹ��)*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;  //����������ģʽ�� �ǹ���״̬ѡ�� OSSR
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;  //����������ģʽ�� �ǹ���״̬ѡ�� OSSI
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;  //ʹ������ƽ1
  TIM_BDTRInitStructure.TIM_DeadTime = 80;		//ָ��������ʹ� �ر�״̬֮�����ʱ Լ 400ns  
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//�Զ��������ʹ��

  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);	//��ʼ���趨�õĲ���
  TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);	//��ʼ���趨�õĲ���

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
  TIM_Cmd(TIM8, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM8, ENABLE);

}

/*��ʼ��ʱ��*/
void RCC_Configuration(void)
{
  /* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_TIM1|
  						 RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|
						 RCC_APB2Periph_AFIO, ENABLE);
}
/*�������õ��Ŀ���*/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_13 | GPIO_Pin_14 ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*��ʼ����ʱ��1����Ҫ����Դ*/
void TIM1_Init(void)
{
	RCC_Configuration();//����ʱ��
	
	GPIO_Configuration();//���ÿ���
	
	TIM1_Mode_Init();//���ö�ʱ��

}
/******************************************************��3���������㷨���ٶȺϳ�*********************************/
/*�����ʼ������*/
void Direction_Init(void)//Direction������
{
	ADC_Direction=ADC_ConvertedValue[1];//���ͷ���adֵ	
}

/*�����㷨���ٶȺϳ�*/
void Direction_Dispose(void)//Dispose:����
{
	int Direction_Read;

  Direction_Read=ADC_ConvertedValue[1];//��ȡ����
	Direction_Difference=(ADC_Direction-Direction_Read)*0.1;//��=ԭʼ����-��������


/*pwm��ѹ����������ص�ѹ�仯pwm����*/

// ����������/4096��*3.3*21������=ʵ�ʵ�ѹ ԭʼpwm*��32/������ѹ��=���pwm
//	ADC_ConvertedValueLocal =(float) ADC_ConvertedValue[2]/4096*3.28*21;

//	P=32/ADC_ConvertedValueLocal;

	if(int_PWM>0)//�ж���ǰ������������ǰ
	{
		if(Direction_Difference>0)//�ж�ת�������������Ǹ��������֤��
			{
			PWM_R=int_PWM-(int_PWM*Direction_Difference/100);//��ǰ��PWM����-ת��ֵ=��//��ȡPWM�ź�*����ȡ��ת��/100������˼���ǲ����ź���ת����PWM�İٷֱ�
			PWM_L=int_PWM+(int_PWM*Direction_Difference/100);//���ת��ֵ
			}																	 //��������ת
		else
			{
			PWM_R=int_PWM-(int_PWM*Direction_Difference/100);//�ұ�ת��ֵ
			PWM_L=int_PWM+(int_PWM*Direction_Difference/100);//���ת��ֵ
			}																	//��������ת
	TIM_SetCompare1(TIM1,PWM_L);//������������һͨ�����趨��ǰ�˶�
	TIM_SetCompare1(TIM8,PWM_R);//�����ҵ������һͨ�����趨��ǰ�˶�
	}
	
	else
	{	
		if(Direction_Difference>0)
			{
			PWM_R=(-int_PWM)-(int_PWM*Direction_Difference/100);//��ǰ��PWM����-ת��ֵ=�ұ�ת��ֵ
			PWM_L=(-int_PWM)+(int_PWM*Direction_Difference/100);//���ת��ֵ		
			}																	 //��������ת
		else
			{
			PWM_R=(-int_PWM)-(int_PWM*Direction_Difference/100);//�ұ�ת��ֵ
			PWM_L=(-int_PWM)+(int_PWM*Direction_Difference/100);//���ת��ֵ
			} 																 //��������ת		
	TIM_SetCompare2(TIM1,PWM_L);//�����������ڶ�ͨ�����趨�������˶�
	TIM_SetCompare2(TIM8,PWM_R);//�����������ڶ�ͨ�����趨�������˶�
	}

}	
	
	
	
	
	
	