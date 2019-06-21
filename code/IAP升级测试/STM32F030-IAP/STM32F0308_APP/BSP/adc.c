#include "adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define 	ADC1_DR_Address      0x40012440

__IO uint32_t TempSensVoltmv = 0, VrefIntVoltmv = 0;
__IO uint16_t RegularConvData_Tab[2];

void ADC1_CH_DMA_Config(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;
  
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//ѡ��ת���ķֱ���
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//�޴���
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ָ��ADC�����ݶ��뷽ʽ�������
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;//ָ�����ĸ������ͨ������ɨ�������С�
  ADC_Init(ADC1, &ADC_InitStructure); 
 
  /*ת��ADC1���¶ȴ�������55.5����Ϊ����ʱ��*/ 
  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
  ADC_TempSensorCmd(ENABLE);//�¶ȴ�����ͨ������
  
  /*ת��ADC1 Vref��55.5����Ϊ����ʱ�� */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles); 
  ADC_VrefintCmd(ENABLE);//��׼��ѹͨ������
  
	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_55_5Cycles);
	
	
  /* ADCУ׼ */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* �ȴ�ADRDY falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
}


