#ifndef __OLED_FONT_H
#define __OLED_FONT_H
#include "oledlib.h"


struct Cn16CharTypeDef                	// ������ģ���ݽṹ 
{
	unsigned char  Index[2];            // ������������,һ������ռ�����ֽ�	
	unsigned char  Msk[32];             // ����������(16*16��32������) 
};


extern const unsigned char font5x7[];
extern const unsigned char F8X16[];
extern struct Cn16CharTypeDef const CN16CHAR[];

//���ñ���ģʽ��0Ϊ͸����1Ϊ������
void SetTextBkMode(Type_textbk);
//��ȡ��ǰ����ģʽ
Type_textbk GetTextBkMode(void);
void SetFontSize(unsigned char);
unsigned char GetFontSize(void);

#endif
