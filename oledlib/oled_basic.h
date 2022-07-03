#ifndef __OLED_BASIC_H
#define __OLED_BASIC_H
#include "oledlib.h"

#define point TypeXY 
#define RADIAN(angle)  ((angle==0)?0:(3.14159*angle/180))
#define MAX(x,y)  		((x)>(y)? (x):(y))
#define MIN(x,y)  		((x)<(y)? (x):(y))
#define SWAP(x, y) \
	(y) = (x) + (y); \
	(x) = (y) - (x); \
	(y) = (y) - (x);

#define HW_IIC    (0)
#define SW_IIC    (1)
#define HW_SPI		(2)
#define SW_SPI		(3)
#define HW_8080   (4)

#define IIC_1     (10)
#define IIC_2     (11)


#define SPI_1     (20)
#define SPI_2	    (21)	


//���ñ���ģʽ��0Ϊ͸����1Ϊ������
extern void SetTextBkMode(unsigned char value);
//��ȡ��ǰ����ģʽ
extern unsigned char GetTextBkMode(void);
void SetPointBuffer(int x,int y,int value);
unsigned char GetPointBuffer(int x,int y);
void OLED_Graph_Init(void);
void ClearScreen(void);
void UpdateScreen(void);

void FloodFill(unsigned char x,unsigned char y,int oldcolor,int newcolor);
void FillRect(int x,int y,int width,int height);
void FloodFill2(unsigned char x,unsigned char y,int oldcolor,int newcolor);
void FillVerticalLine(int x,int y,int height,int value);
void FillByte(int page,int x,unsigned  char byte);
void SetRotateValue(int x,int y,float angle,int direct);
void SetRotateCenter(int x0,int y0);
void SetAngle(float angle);
void SetAnggleDir(int direction);
TypeXY GetRotateXY(int x,int y);

unsigned char pgm_read_byte(const unsigned char * addr);
unsigned int oled_pow(unsigned char m,unsigned char n);
unsigned char FrameRateUpdateScreen(int value);
void WaitTimeMs(unsigned int time);
#endif

