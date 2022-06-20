#ifndef __OLED_FONT_H
#define __OLED_FONT_H
#include "oledlib.h"


struct Cn16CharTypeDef                	// 汉字字模数据结构 
{
	unsigned char  Index[2];            // 汉字内码索引,一个汉字占两个字节	
	unsigned char  Msk[32];             // 点阵码数据(16*16有32个数据) 
};


extern const unsigned char font5x7[];
extern const unsigned char F8X16[];
extern struct Cn16CharTypeDef const CN16CHAR[];

//设置背景模式，0为透明，1为正常。
void SetTextBkMode(Type_textbk);
//获取当前背景模式
Type_textbk GetTextBkMode(void);
void SetFontSize(unsigned char);
unsigned char GetFontSize(void);

#endif
