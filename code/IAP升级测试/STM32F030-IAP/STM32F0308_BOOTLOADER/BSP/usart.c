#include "systick.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{  
	while(!((USART1->ISR)&(1<<7))){}
  USART1->TDR=ch;
  return (ch);
}
#endif 



static uint8_t USART_RX_BUF[UART_RX_LEN]; 
 _UART_RECEIVE uart_recv;


void USART1_IRQHandler(void) 
{
    uint16_t Len = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
    {  
        USART1->ICR |= 1<<4; //����ж�   
        
        DMA_Cmd(DMA1_Channel3,DISABLE);  
        Len = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);  
		
        if((Len >0)&&(Len < UART_RX_LEN)&&(uart_recv.uart_recv_flag==0))
		{
			memcpy(uart_recv.recv_buf, USART_RX_BUF, Len);
			uart_recv.count = Len;
			uart_recv.recv_buf[uart_recv.count]=0;
			uart_recv.uart_recv_flag = 1;
        }
	
	//���ô������ݳ���  
	DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
	//��DMA  
	DMA_Cmd(DMA1_Channel3,ENABLE);
    }
}

static void USART1_DMA_Config()
{
    DMA_InitTypeDef DMA_InitStructure;  
  
    //������DMA����    
    //����DMAʱ��  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1ͨ��5����  
    DMA_DeInit(DMA1_Channel3);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->RDR);  
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART_RX_BUF;  
    //dma���䷽����  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //����DMA�ڴ���ʱ�������ĳ���  
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;  
    //����DMA���������ģʽ��һ������  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA_Mode_Circular DMA_Mode_Normal
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
    //����DMA��2��memory�еı����������  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel3,&DMA_InitStructure);  
  
    //ʹ��ͨ��3  
    DMA_Cmd(DMA1_Channel3,ENABLE);
}

//��ʼ��IO ����1
void USART1_Configuration(uint32_t bound)
{  	 
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//USART_WordLength_8b;//�˴�������żУ��ʱ��Ҫ9������λ������żУ��ʱ8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;//USART_Parity_No;	//żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);//��ʼ������


	USART1_DMA_Config();

	//TXE�����ж�,TC��������ж�,RXNE�����ж�,PE��ż�����ж�,�����Ƕ��       
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);    
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART1->ICR |= 1<<4;     //���������IDLE�жϣ������һֱ��IDLE�ж�
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	//����DMA��ʽ����  
	//USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 

	USART_Cmd(USART1, ENABLE);
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=0 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	uart_recv.uart_recv_flag = 0;
}
void USART_Restart(void)
{
	uart_recv.uart_recv_flag = 0;

}
int uart_putc(int ch)
{
while(!((USART1->ISR)&(1<<7))){}
	USART1->TDR=ch;
	return ch;
}
void uart_putu16(uint16_t ww) {
  uart_putc(ww >>  0 & 0x000000ff);
  uart_putc(ww >>  8 & 0x000000ff);
}
void uart_putu32(uint32_t ww) {
  uart_putc(ww >>  0 & 0x000000ff);
  uart_putc(ww >>  8 & 0x000000ff);
  uart_putc(ww >> 16 & 0x000000ff);
  uart_putc(ww >> 24 & 0x000000ff);
}
void Send_String(uint8_t *pSTR)
{				
	while(*pSTR)
	{
		while(!((USART1->ISR)&(1<<7))){}
		USART1->TDR=*pSTR++;
	}
}
void Usart_SendBytes(u8 array[],u16 len)
{
	u8 i=0;
	while(len)
	{
		while((USART1->ISR&0X80)==0){}
			USART1->TDR = array[i++]; 
		len--;
	}		
}
void Send_Byte(uint8_t ch)
{				
	while(!((USART1->ISR)&(1<<7))){}
	USART1->TDR=ch;
}


