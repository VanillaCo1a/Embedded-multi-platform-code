#ifndef __DRAW_API_H
#define __DRAW_API_H
#include "oledlib.h"

//***********************************************************************
//*		 Ϊ�����������ͼ�ο⣬�Ҳο��˺ܶ��������㷨,���˴���ʱ��ȥ����ֲ
//*�Ż������вο�������github�е�arduboy��Ϸ�����󲿷�ͼ������ֲ���ģ�
//*����������ĳ���汾,���кܶ�ط���Ҫ�Ż��Ľ�������Ҫ�����ͼ�ο�����
//*��������������Ȥ�Ķ�������ģ�飬���붼������򵥵ķ�ʽ��д�ģ����ǿ�Դ�ġ�
//*����Ҳ�����ע��˵�����ø������Ե�Ƭ����oled��������Ӳ����̣������
//ʹ���˸ÿ⣬�����������bվ��עoximeterg���ɻ�ȡ�ÿ�����°汾����Ϣ��
//ע�⣺��������ʵ�ĵ�ͼ�Σ�Ӱ��ˢ��Ч�ʣ����Ż��У�
//*oled��ͼ�ο�0.0.0.1        							���ߣ�maoyongjie     
//https://github.com/hello-myj/stm32_oled/
//************************************************************************

//-------------------------------------------------------------------------
/**************************************************************************/
														//����˵��
/*														 x(0~127)
												 ------------------>
												|
												|
												|y(0~63)
												|
												|
												v
*/
//--------------------------------------------------------------------------
/**************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	TEXT_BK_NULL=0,
	TEXT_BK_NOT_NULL,
}Type_textbk;
typedef enum
{
	pix_black,
	pix_white,
	//............
}Type_color;
typedef struct COORDINATE 
{
	int x;
	int y;
}TypeXY;
typedef struct ROATE
{
	TypeXY center;
	float angle;
	int direct;
}TypeRoate;


////////////////////////////////////////////////////////////////////////////////////////////////
//����:��ʼ��ͼ�ο�
extern void InitGraph(void);
//����:����Ļ���ݸ��µ��豸��
extern void UpdateScreen(void);
//����:����
extern void ClearScreen(void);
//�뽫�˺�������1ms�ж��Ϊͼ���ṩʱ��
extern void OledTimeMsFunc(void);
//ͼ�ο�test.c����ʱ����
extern void DelayMs(u16);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled_font�еĺ���
//����:���������С  0 1 2 3
extern void SetFontSize(unsigned char value);
//����:��ȡ�����С
extern unsigned char GetFontSize(void);
//����:��������ַ�ʱ�ı�����ɫ 0Ϊ͸����1Ϊ������
extern void SetTextBkMode(Type_textbk value);
//����:��ȡ����ַ�ʱ�ı�����ɫ 0Ϊ͸����1Ϊ������
extern Type_textbk GetTextBkMode(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled_color�еĺ���
//����:���û��Ƶ���ɫ  pix_black:��ɫ��pix_white:��ɫ
extern void SetDrawColor(Type_color value);
//����:��ȡ��ǰ���õĻ�����ɫ  pix_black:��ɫ��pix_white:��ɫ
extern Type_color GetDrawColor(void);
//����:����������ɫ  pix_black:��ɫ��pix_white:��ɫ
extern void SetFillcolor(Type_color value);
//����:��ȡ��ǰ���õ������ɫ  pix_black:��ɫ��pix_white:��ɫ
extern Type_color GetFillColor(void);
//���ܣ���ȡһ�����ص����ɫ
extern Type_color GetPixel(int x,int y);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled.draw��ֱ�ӹ��û����õĺ���
extern void MoveTo(int x,int y);
//����:���Ի���,�������������ƶ�����һ������(�ڻ��������б�����)
extern void LineTo(int x,int y);
//����:��ȡ��ǰ����Ƶ��X����
extern int GetX(void);
//����:��ȡ��ǰ����Ƶ��y����
extern int GetY(void);
//����:��ȡ��ǰ����Ƶ��X��Y����
extern TypeXY GetXY(void);

//���ܣ�����һ����
extern void DrawPixel(int x,int y);

//����:����һ��������
extern void DrawLine(int x1,int y1,int x2,int y2);
//����:���ٻ���һ��ˮƽ�� wΪ����
extern void DrawFastHLine(int x, int y, unsigned char w);
//����:���ٻ���һ����ֱ��
extern void DrawFastVLine(int x, int y, unsigned char h);
//����:�������� ���� numΪ�۵����
void DrawPolyLineTo(const TypeXY *points,int num);

//����:ʹ�öԽǵ���ƾ���
extern void DrawRect1(int left,int top,int right,int bottom);
//����:ʹ�öԽǵ�������
extern void DrawFillRect1(int left,int top,int right,int bottom);
//����:ʹ�ö��� ��߻��ƾ���
extern void DrawRect2(int left,int top,int width,int height);
//����:ʹ�ö��� ���������
extern void DrawFillRect2(int left,int top,int width,int height);

//����:����һ��Բ
extern void DrawCircle( int usX_Center, int usY_Center, int usRadius);
//����:����һ��ʵ��Բ
extern void DrawFillCircle( int usX_Center, int usY_Center, int usRadius);
//����:4��֮һԲҪ����һ�ݻ��ļ���
extern void DrawCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername);
//����://���2���ķ�֮һԲ���м�ľ��� (ר�ú��� ������ע��)
extern void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta);

//����:����һ��Բ��(���Ż�)
extern void DrawArc(int x,int y,unsigned char r,int angle_start,int angle_end);
//����:���һ��Բ��(������)
extern void DrawFillArc(int x,int y,unsigned char r,int angle_start,int angle_end);

//����:����һ��Բ�Ǿ���
extern void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
//����:����һ�����Բ�Ǿ���
extern void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);

//����:����һ����Բ
extern void DrawEllipse(int x0,int y0,int a,int b);
//����:����һ�������Բ
extern void DrawFillEllipse(int x0,int y0,int a,int b);
//����:����һ������������Բ
extern void DrawEllipseRect( int x0, int y0, int x1, int y1);

//����:����һ��������
extern void DrawTriangle(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
//����:����һ�����������
extern void DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

//����:����һ��ͼƬ bitmapΪͼƬ���ݣ���ͨ��ȡģ��ȡ��
extern void DrawBitmap(int x, int y, const unsigned char *bitmap, unsigned char w, unsigned char h);

//����:����һ���ַ� �����:��size=0ʱ xΪ0-7���� yΪ0-127��
extern void DrawChar(int x, int y, unsigned char c);
//����:�����ַ��� �����:��size=0ʱ xΪ0-7���� yΪ0-127��
extern void DrawString(int x, int y,char *str);
//����:�������� lenΪ���ֳ��� �����:��size=0ʱ xΪ0-7���� yΪ0-127��
extern void DrawNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len);
//��ʾ����
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char *cn);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled.basic��ֱ�ӹ��û����õĺ���
//����:���������һ�����ͼ��(���������Բ���зǳ��ʺ�ʹ��)
extern void FloodFill2(unsigned char x,unsigned char y,int oldcolor,int newcolor);
//����:�ƶ���������

//����:���ýǶȡ���ת������ת����
extern void SetRotateValue(int x,int y,float angle,int direct);
//����:������ת���ĵ�
extern void SetRotateCenter(int x0,int y0);
//����:������ת�Ƕ�
extern void SetAngle(float angle);
//����:������ת����
extern void SetAnggleDir(int direction);
//����:��һ��������תһ���Ƕ�
extern TypeXY GetRotateXY(int x,int y);

//����:�̶�֡ˢ��
extern unsigned char FrameRateUpdateScreen(int value);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled.buffer�еĺ���
//��Ļ���ݻ���
extern void SetScreenBuffer(void);
//����:�����µĻ�����Ϊ��Ļ���ݣ�
extern void SetTempBuffer(void);
//����:��ȡ��ǰ�Ļ���buffer��ϵ�� ���ػ���ϵ��SCREEN_BUFFER��TEMP_BUFFER
extern unsigned char GetSelectedBuffer(void);
//����:�����Ĭ�ϵ���Ļ��������
extern void ClearScreenBuffer(unsigned char val);
//����:�������ʱ����Ļ��������
extern void ClearTempBuffer(void);
//����:��ʱ����Ĺ���
extern void TempBufferFunc(int func);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//���º���Ϊoled.debug�еĺ���
//����:���Դ�ӡ����bug�����ܺ�printfһ�£�����ʹ��ClearScreen����Ҳ�ɴ�ӡ������
extern int OledPrintf(const char *str,...);

#endif
