#ifndef __OLED_COLOR_H
#define __OLED_COLOR_H
#include "oledlib.h"

//������ɫ���򵥻���or���ӻ���
#define  COLOR_CHOOSE_DRAW   (SIMPLE_DRAW)   




//���ý�Ҫ���Ƶ����ص�ɫ
void SetDrawColor(Type_color value);
//��ȡ��ǰ���õ����ص�ɫ
Type_color GetDrawColor(void);

void SetFillcolor(Type_color value);
Type_color GetFillColor(void);

//�õ�ĳ�������ɫ
Type_color GetPixel(int x,int y);
#endif
