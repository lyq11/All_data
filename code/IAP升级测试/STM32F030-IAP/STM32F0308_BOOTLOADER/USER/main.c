//64K FLASH   8K RAM
/* Includes -------------------*/
#include "common.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "iap.h"
#include "crc8_16.h"
#include "softtimer.h"
#include "stmflash.h"
/****************************************************************************************************/
#define BOOT_Version  	0x0220
#define FLASH_ROM   	64
#define FILE_SIZE_A  	*(vu32*)APP1_SIZE_ADDR//���ڴ��app�Ĵ�С
#define SPLIT_PACK 		256

#define SUPPORT_SENDFILE 0

char* filename_ptr="app.bin";
char file_name[50];

u32 FILE_SIZE=0;
u32 file_size=0;
u32 packet_size=0;
u32 packets_all=0;
u32 packets_now=0;
u32 FLASH_SAVEADDR=FLASH_APP1_ADDR;

u8 machine_state=0;
ifs_stmr_handle statemachine_handle;
ifs_stmr_handle led_handle=-1;
ifs_stmr_handle IAP_handle=-1;

void Send_File(void);
void Run_App(void *arg);
void state_timeout(void *arg);
int Wait_Ack(u8 ack_ch);

int main(void)
{
	u8 count=0;	
	u8 successful=0;
	u16 calc_crc=0,crc_temp=0;
	
	Systick_Init();
	LED_Init();	
	USART1_Configuration(115200);
		//OptionByte_Write_A5A5();
	ifs_stmr_init();
	statemachine_handle=ifs_stmr_registered(5000,state_timeout,NULL,IFS_STMR_FLAG_ONESHOT);
//��ʱ�Ƿ����APP
	IAP_handle=ifs_stmr_registered(2000,Run_App,&successful,IFS_STMR_FLAG_ONESHOT);
	ifs_stmr_start(IAP_handle);

	printf("-Bootloader-�ܿռ�:%ld Byte ",FLASH_ROM*1024ul);
	uart_putu16(0xffff);
	while(1)
	{
		if(uart_recv.uart_recv_flag)
		{
			switch(machine_state)
			{
				case 0:
					if((uart_recv.recv_buf[0]==0xaa)&&(uart_recv.recv_buf[1]==0xbb)&&(uart_recv.recv_buf[2]==0xcc)&&(uart_recv.recv_buf[3]==0xdd))
					{//4+4+2+...
						crc_temp=(uart_recv.recv_buf[10]<<8)|(uart_recv.recv_buf[11]);
						calc_crc=Get_Crc16(&uart_recv.recv_buf[0]+4,6);
						calc_crc=(calc_crc<<8)|(calc_crc>>8);
						//printf("1.crc_temp=%x,calc_crc=%x \n",crc_temp,calc_crc);
						if(calc_crc==crc_temp)
						{
							file_size=(uart_recv.recv_buf[4]<<24);
							file_size|=(uart_recv.recv_buf[5]<<16);
							file_size|=(uart_recv.recv_buf[6]<<8);
							file_size|=(uart_recv.recv_buf[7]);
							
							FILE_SIZE=file_size;
							printf("file_size=%x \n",file_size);
							//(FLASH_APP1_ADDR-0X08000000)//APP����ʹ�õĿռ� ����ΪBOOTLOADERʹ��
							if(FILE_SIZE>(FLASH_ROM*1024-(FLASH_APP1_ADDR-0X08000000)))
							{
								uart_putu16(0xfff1);
								break;
							}
							packet_size=uart_recv.recv_buf[8]<<8|uart_recv.recv_buf[9];
							packets_all=file_size/packet_size+(file_size%packet_size?1:0);
							
							packets_now=0;
						#if SUPPORT_SENDFILE
							//�����ļ������ڴ���  �����ļ���С��flash
							{
								u16 temp_addr[2];
								temp_addr[0]=FILE_SIZE;
								temp_addr[1]=FILE_SIZE>>16;
								STMFLASH_Write(APP1_SIZE_ADDR,temp_addr,2);							
							}

						#endif
							strncpy(file_name,(const char*)(&uart_recv.recv_buf[0]+12),100);

							machine_state=1;
							successful=0;
							FLASH_SAVEADDR=FLASH_APP1_ADDR;
							if(OptionByte_Read()==0xa5a5)//׼������APP�����б�־
							{
								//u16 temp_addr=0xffff;
								//STMFLASH_Write(APP1_SUCCESS_ADDR,&temp_addr,1);
								OptionByte_Write(0xfff2);
							}
							ifs_stmr_start(statemachine_handle);
							ifs_stmr_stop(IAP_handle);
							//uart_putc(0x06);
							uart_putu16(0x6751);
						}
						else
						{
							uart_putc(0x5169);
						}
					}
					else if((uart_recv.recv_buf[0]==0xaa)&&(uart_recv.recv_buf[1]==0xbb)&&(uart_recv.recv_buf[2]==0xdd))
					{
						FILE_SIZE=FILE_SIZE_A;
						if((strlen(filename_ptr)>0)&&(FILE_SIZE>0))
						Send_File();
					}
					else
					{
						uart_putu16(0xfff3);
					}
					break;
				case 1:
				{

					ifs_stmr_start(statemachine_handle);
					packet_size=uart_recv.count-4;

					crc_temp=(uart_recv.recv_buf[uart_recv.count-2]<<8)|(uart_recv.recv_buf[uart_recv.count-1]);
					calc_crc=Get_Crc16(&uart_recv.recv_buf[0]+2,packet_size);
					calc_crc=(calc_crc<<8)|(calc_crc>>8);
					//printf("packets_now=%d.crc_temp=%x,calc_crc=%x \n",packets_now,crc_temp,calc_crc);
					for(int i=0;i<uart_recv.count;i++)
					{
						//printf("%2x ",uart_recv.recv_buf[i]);
					}
					if(calc_crc==crc_temp)
					{
						if(packets_now==((uart_recv.recv_buf[0]<<8)|uart_recv.recv_buf[1]))
						{
							packets_now++;
							//uart_putc(0x06);
							//uart_putc(0x51);
							uart_putu16(0x6751);
							//�洢���� ����Լ��Ĵ���
							if(iap_write_appbin(FLASH_SAVEADDR,&uart_recv.recv_buf[0]+2,packet_size))
							{
								uart_putu16(0xfff4);
								machine_state=0;
								delay_ms(1000);
								printf("\r\nflashд���󣬿���������д����");
								break;
							}
							FLASH_SAVEADDR+=(packet_size);
							//
							if(packets_now==packets_all)//USART_RX_BUF[0]<<8|USART_RX_BUF[1]
							{
								//���ݽ������;	
								machine_state=0;
								successful=1;
								OptionByte_Write_A5A5();
								Run_App(&successful);
							}
						}
						else//�ط���n������
						{
							uart_putc(0x08);
							uart_putc((u8)(packets_now>>8));
							uart_putc((u8)packets_now);
						}
					}
					else
					{
						//printf("2.crc_temp=%x,calc_crc=%x \n",crc_temp,calc_crc);
						if((uart_recv.recv_buf[0]==0xff)&&(uart_recv.recv_buf[1]==0xff))
						{
							machine_state=1;
							ifs_stmr_stop(statemachine_handle);
							break;
						}
						uart_putc(0x07);
					}
					break;	
				}

				default:
					break;
			}
			USART_Restart();
		}
		count++;
		ifs_stmr_task();
	}
}
void Run_App(void *arg)
{
	char* success_flag=(char*)arg;
		u16 OptionBytes=0;
		OptionBytes=OptionByte_Read();
		printf("OptionBytes=%d\n",OptionBytes);	
	if(*success_flag)
	{
		if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{
			iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
		}
	}
	else//��ʱ����
	{

		if(/*SUCCESSFUL_FLAG*/OptionBytes==0xa5a5)//APP�����б�־
		{
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}
			printf("\r\n���д���,����û��������APP");
		}
		else
			printf("\r\n���д���,����û��������APP(0x%04x)",OptionBytes);
	}
	uart_putc(0x0e);
	
}
void state_timeout(void *arg)
{
	ifs_stmr_start(IAP_handle);
	machine_state=0;
}

int Wait_Ack(u8 ack_ch)
{
	
	int ret=-1;
	u8 timeout=100;
	
	while(timeout--)
	{
		if(uart_recv.uart_recv_flag)
		{
			
			if(uart_recv.recv_buf[0]==ack_ch)
			{
				ret=0;
				break;
			}
			else if((uart_recv.recv_buf[0]==0x08)&&(uart_recv.count==3))
			{
				ret=(uart_recv.recv_buf[1]<<8)|uart_recv.recv_buf[2];
			}
			else if((uart_recv.recv_buf[0]==0xff)&&((uart_recv.recv_buf[1]==0xff)))
			{
				ret=0xffff;
			}
			break;	
		}
		delay_ms(10);
	} 
	
	USART_Restart();
	return ret;
}


void Send_File(void)
{
#if SUPPORT_SENDFILE
	
	u8 Rd_Buffer[512];
	u8 send_buf[128];
	u16 packets,packets_length;
	u16 cal_crc;
	u32 flash_addr=FLASH_APP1_ADDR;
	int i=0;
	int response=-1,retry=0;
	
	
	send_buf[0]=0xaa;
	send_buf[1]=0xbb;
	send_buf[2]=0xcc;
	send_buf[3]=0xdd;
	send_buf[4]=(u8)(FILE_SIZE>>24);
	send_buf[5]=(u8)(FILE_SIZE>>16);
	send_buf[6]=(u8)(FILE_SIZE>>8);
	send_buf[7]=(u8)(FILE_SIZE);
	send_buf[8]=(u8)(SPLIT_PACK>>8);
	send_buf[9]=(u8)(SPLIT_PACK);	
	
	cal_crc=Get_Crc16(send_buf+4,6);
	cal_crc=(cal_crc<<8)|(cal_crc>>8);
	send_buf[10]=(u8)(cal_crc>>8);
	send_buf[11]=(u8)(cal_crc);
	strncpy((char*)(send_buf+12),filename_ptr,100);
	
	USART_Restart();
	Usart_SendBytes(send_buf,12+strlen(filename_ptr));
	if(Wait_Ack(0x06)!=0)
	{
		__nop();
		return;
	}
	//FILE_SIZE=FILE_SIZE_A;
	if(FILE_SIZE>128*1024)//�̼�����128k
		return;
	packets=FILE_SIZE/SPLIT_PACK+(FILE_SIZE%SPLIT_PACK?1:0);
	for (i = 0; i < packets; i++)
	{
Retry_FileSeek:
		send_buf[0]=(u8)(i>>8);
		send_buf[1]=(u8)(i);
		if(i==packets-1)
		{
			if(FILE_SIZE<SPLIT_PACK)
				packets_length=FILE_SIZE%SPLIT_PACK;
			else
				packets_length=SPLIT_PACK;
		}
		else
		{
			packets_length=SPLIT_PACK;
		}
		iap_read_appbin(flash_addr,Rd_Buffer,packets_length);
		flash_addr+=packets_length;	
		
		cal_crc=Get_Crc16(Rd_Buffer,packets_length);
		cal_crc=(cal_crc<<8)|(cal_crc>>8);
		send_buf[2]=(u8)(cal_crc>>8);
		send_buf[3]=(u8)(cal_crc);
Retry_Send:
		USART_Restart();
		Usart_SendBytes(send_buf,2);
		Usart_SendBytes(Rd_Buffer,packets_length);
		Usart_SendBytes(send_buf+2,2);
		response=Wait_Ack(0x06);
		if(response!=0)
		{
			retry++;
			if(retry>5)
				break;;
			if(response>0)
			{
				if(response==0xffff)
				{
					return;
				}
				i=response;
				flash_addr=i*SPLIT_PACK;//���¶�λ��ַ
				goto Retry_FileSeek;
			}
			else
			{
				goto Retry_Send;
			}
		}
		
	}
	uart_putu32(0xaaaaaaaa);
	uart_putu32(0xaaaaaaaa);
#endif
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
