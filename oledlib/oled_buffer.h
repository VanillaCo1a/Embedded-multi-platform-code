#ifndef __OLED_BUFFER_H
#define __OLED_BUFFER_H
#include "oledlib.h"
#define SCREEN_BUFFER    	1
#define TEMP_BUFFER			0

typedef enum
{
	TEMPBUFF_COPY_TO_SCREEN,
	TEMPBUFF_CLEAN,
	TEMPBUFF_COVER_L,
	TEMPBUFF_COVER_H
	
}Type_tempbuff;

void SetScreenBuffer(void);
void SetTempBuffer(void);
unsigned char GetSelectedBuffer(void);
void ClearScreenBuffer(unsigned char val);
void ClearTempBuffer(void);
void UpdateScreenDisplay(void);
unsigned char GetPointBuffer(int x,int y);
void SetPointBuffer(int x,int y,int value);

void WriteByteBuffer(int page,int x,unsigned char byte);
unsigned char ReadByteBuffer(int page,int x);
void TempBufferFunc(int func);
#endif

