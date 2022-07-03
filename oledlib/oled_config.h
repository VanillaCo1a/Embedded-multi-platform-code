#ifndef __OLED_CONFIG_H
#define __OLED_CONFIG_H
#include "oledlib.h"

//������Ļ����״��
//�����˿���չ�� �����������������
//��ע����������c�ļ���������Щ�궨��Ĳ���

#define   	OLED_12864						//��Ļ����
#define 	SCREEN_PAGE_NUM			(8)		//������ (����)
#define  	SCREEN_PAGEDATA_NUM   	(128)	//ÿ���е�����
#define  	SCREEN_ROW 				(64)	//������
#define 	SCREEN_COLUMN 			(128)	//������

void Driver_Init(void);
void UpdateScreenBuffer(void);
void UpdateTempBuffer(void);
void OledTimeMsFunc(void);
void DelayMs(u16);

#endif
