#ifndef __OLED_CONFIG_H
#define __OLED_CONFIG_H
#include "oledlib.h"

//定义屏幕基本状况
//保持了可拓展性 如果换用其他类型屏
//因注意其他配套c文件中用了这些宏定义的部分
#define OLED_12864                                    //屏幕类型
#define SCREEN_PART (1)                            //定义几个屏幕缓冲区, 每个缓冲区均为1个屏幕大小
#define SCREEN_PAGE (8)                            //总行数 (大行)
#define SCREEN_ROW (64)                            //总行数
#define SCREEN_COLUMN (128)                    //总列数

//绘制颜色，简单绘制or复杂绘制
#define COLOR_CHOOSE_DRAW (SIMPLE_DRAW)
//设置/获取画线的像素点颜色
void setLineColor(Type_color value);
Type_color getLineColor(void);
//设置/获取填充图形的像素点颜色
void setFillcolor(Type_color value);
Type_color getFillColor(void);
//得到某个点的颜色
Type_color getPixel(int x,int y);
//设置/获取背景模式, 0为透明,1为正常
void SetTextBkMode(Type_textbk);
Type_textbk GetTextBkMode(void);
//设置/获取字体格式
void SetFontSize(unsigned char);
unsigned char GetFontSize(void);

void OledTimeMsFunc(void);
#endif
