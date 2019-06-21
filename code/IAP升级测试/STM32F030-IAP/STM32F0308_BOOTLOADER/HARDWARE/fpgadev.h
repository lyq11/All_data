#ifndef __FPGADEV_H
#define	__FPGADEV_H

#include "stm32f0xx.h"
typedef struct
{
	u32 rep_video_addr;
	u32 video_page_num;
	u32 rep_audio_addr;
	u32 audio_page_num;
	u32 rep_dq1_file_len;
	u32 audio_valid_bytes;
	u32 audio_total_bytes;
    u32 rep_dq1_aempty_num;
}REPLAY_CONFIGURE_STRUCT;

void Test_ReadWrite(void* arg);
void nvme_init(void);
/***********************
���ò����ٶ�
**********************/
void _SetPlaySpeed(u8 SpeedLevel);
/***********************
���ò���/��ͣ/ֹͣ
**********************/
void _SetProgramStatus(u8 status);
/***********************
���ò�����һ��
**********************/
void _SetNextProgram(void);
/***********************
���ò�����һ��
**********************/
void _SetPrevProgram(void);
/***********************
���ò�������
**********************/
void _SetVolume(u8 value);
/***********************
���ò��ŵڼ����ļ�
**********************/
void _SetPlay_Index(u8 Index);
/***********************
��ȡ�ļ���֡��
**********************/
u32 _GetFileFrames(void);
/***********************
��ȡ�ļ���֡��
**********************/
u32 _GetNow_Frames(void);
/***********************
��ȡ�ļ�֡��
**********************/
u32 _GetFileFrameRate(void);
/***********************
��ȡ��ǰ����״̬
**********************/
u32 _GetPlayStatues(void);
/***********************
��ȡ��Ҫ���ŵ��ļ�����
**********************/
u32 _GetFileNums(void);
#endif /* __GPIO_H */
