#ifndef __OLED_COLOR_H
#define __OLED_COLOR_H
#include "oledlib.h"

//绘制颜色，简单绘制or复杂绘制
#define  COLOR_CHOOSE_DRAW   (SIMPLE_DRAW)   




//设置将要绘制的像素点色
void SetDrawColor(Type_color value);
//获取当前设置的像素点色
Type_color GetDrawColor(void);

void SetFillcolor(Type_color value);
Type_color GetFillColor(void);

//得到某个点的颜色
Type_color GetPixel(int x,int y);
#endif
