/*
	原作者哔哩哔哩:							MjGame 		https://space.bilibili.com/38673747
	同GifHub:								maoyongjie 	https://github.com/hello-myj/stm32_oled/
	代码整理注释删减增加优化等 哔哩哔哩:	一只程序缘	https://space.bilibili.com/237304109
	整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作
	
	此c文件用于对字体进行操作 在oled.draw中被DrawChar()等函数被调用
	主要有设置字体背景颜色 
	(字体颜色同划线线条颜色GetDrawColor())
	字体尺寸
	基础字符字库(取模方式见字库顶部)
	汉字字库(取模方式见字库顶部)
*/

#include "oled_font.h"

static Type_textbk _TextBk=TEXT_BK_NULL;
static unsigned char _FontSize=0;			//默认字体尺寸 为8x6

/////////////////////////////////////////////////////////////////////////////////
//设置字体填充的背景颜色
//1白0黑 
//TEXT_BK_NULL：无背景，TEXT_BK_NOT_NULL：有背景
void SetTextBkMode(Type_textbk value)
{
	_TextBk=value;
}
Type_textbk GetTextBkMode(void)
{
	return _TextBk;
}

//在显示字体前先设置需要显示的字体的尺寸
//尺寸有 0 1 2 3
//对应像素是 0 8 16 24
//0是默认字体 		也就是F8X16[]		大小8x16
//1是原作者字体 	也就是font5x7[]		大小6x8
//2是1的2倍放大							大小12x16
//3是1的3倍放大							大小18x24
void SetFontSize(unsigned char value)
{
	_FontSize=value;
}
unsigned char GetFontSize(void)
{
	return _FontSize;
}


//基础图形英文字符字库
//若要显示更大的字符按照此字符比例放大即可
//详情见DrawChar()
const unsigned char font5x7[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
    0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
    0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
    0x18, 0x3C, 0x7E, 0x3C, 0x18,
    0x1C, 0x57, 0x7D, 0x57, 0x1C,
    0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
    0x00, 0x18, 0x3C, 0x18, 0x00,
    0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
    0x00, 0x18, 0x24, 0x18, 0x00,
    0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
    0x30, 0x48, 0x3A, 0x06, 0x0E,
    0x26, 0x29, 0x79, 0x29, 0x26,
    0x40, 0x7F, 0x05, 0x05, 0x07,
    0x40, 0x7F, 0x05, 0x25, 0x3F,
    0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
    0x7F, 0x3E, 0x1C, 0x1C, 0x08,
    0x08, 0x1C, 0x1C, 0x3E, 0x7F,
    0x14, 0x22, 0x7F, 0x22, 0x14,
    0x5F, 0x5F, 0x00, 0x5F, 0x5F,
    0x06, 0x09, 0x7F, 0x01, 0x7F,
    0x00, 0x66, 0x89, 0x95, 0x6A,
    0x60, 0x60, 0x60, 0x60, 0x60,
    0x94, 0xA2, 0xFF, 0xA2, 0x94,
    0x08, 0x04, 0x7E, 0x04, 0x08,
    0x10, 0x20, 0x7E, 0x20, 0x10,
    0x08, 0x08, 0x2A, 0x1C, 0x08,
    0x08, 0x1C, 0x2A, 0x08, 0x08,
    0x1E, 0x10, 0x10, 0x10, 0x10,
    0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
    0x30, 0x38, 0x3E, 0x38, 0x30,
    0x06, 0x0E, 0x3E, 0x0E, 0x06,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x5F, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x07, 0x00,
    0x14, 0x7F, 0x14, 0x7F, 0x14,
    0x24, 0x2A, 0x7F, 0x2A, 0x12,
    0x23, 0x13, 0x08, 0x64, 0x62,
    0x36, 0x49, 0x56, 0x20, 0x50,
    0x00, 0x08, 0x07, 0x03, 0x00,
    0x00, 0x1C, 0x22, 0x41, 0x00,
    0x00, 0x41, 0x22, 0x1C, 0x00,
    0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
    0x08, 0x08, 0x3E, 0x08, 0x08,
    0x00, 0x80, 0x70, 0x30, 0x00,
    0x08, 0x08, 0x08, 0x08, 0x08,
    0x00, 0x00, 0x60, 0x60, 0x00,
    0x20, 0x10, 0x08, 0x04, 0x02,
    0x3E, 0x51, 0x49, 0x45, 0x3E,
    0x00, 0x42, 0x7F, 0x40, 0x00,
    0x72, 0x49, 0x49, 0x49, 0x46,
    0x21, 0x41, 0x49, 0x4D, 0x33,
    0x18, 0x14, 0x12, 0x7F, 0x10,
    0x27, 0x45, 0x45, 0x45, 0x39,
    0x3C, 0x4A, 0x49, 0x49, 0x31,
    0x41, 0x21, 0x11, 0x09, 0x07,
    0x36, 0x49, 0x49, 0x49, 0x36,
    0x46, 0x49, 0x49, 0x29, 0x1E,
    0x00, 0x00, 0x14, 0x00, 0x00,
    0x00, 0x40, 0x34, 0x00, 0x00,
    0x00, 0x08, 0x14, 0x22, 0x41,
    0x14, 0x14, 0x14, 0x14, 0x14,
    0x00, 0x41, 0x22, 0x14, 0x08,
    0x02, 0x01, 0x59, 0x09, 0x06,
    0x3E, 0x41, 0x5D, 0x59, 0x4E,
    0x7C, 0x12, 0x11, 0x12, 0x7C,
    0x7F, 0x49, 0x49, 0x49, 0x36,
    0x3E, 0x41, 0x41, 0x41, 0x22,
    0x7F, 0x41, 0x41, 0x41, 0x3E,
    0x7F, 0x49, 0x49, 0x49, 0x41,
    0x7F, 0x09, 0x09, 0x09, 0x01,
    0x3E, 0x41, 0x41, 0x51, 0x73,
    0x7F, 0x08, 0x08, 0x08, 0x7F,
    0x00, 0x41, 0x7F, 0x41, 0x00,
    0x20, 0x40, 0x41, 0x3F, 0x01,
    0x7F, 0x08, 0x14, 0x22, 0x41,
    0x7F, 0x40, 0x40, 0x40, 0x40,
    0x7F, 0x02, 0x1C, 0x02, 0x7F,
    0x7F, 0x04, 0x08, 0x10, 0x7F,
    0x3E, 0x41, 0x41, 0x41, 0x3E,
    0x7F, 0x09, 0x09, 0x09, 0x06,
    0x3E, 0x41, 0x51, 0x21, 0x5E,
    0x7F, 0x09, 0x19, 0x29, 0x46,
    0x26, 0x49, 0x49, 0x49, 0x32,
    0x03, 0x01, 0x7F, 0x01, 0x03,
    0x3F, 0x40, 0x40, 0x40, 0x3F,
    0x1F, 0x20, 0x40, 0x20, 0x1F,
    0x3F, 0x40, 0x38, 0x40, 0x3F,
    0x63, 0x14, 0x08, 0x14, 0x63,
    0x03, 0x04, 0x78, 0x04, 0x03,
    0x61, 0x59, 0x49, 0x4D, 0x43,
    0x00, 0x7F, 0x41, 0x41, 0x41,
    0x02, 0x04, 0x08, 0x10, 0x20,
    0x00, 0x41, 0x41, 0x41, 0x7F,
    0x04, 0x02, 0x01, 0x02, 0x04,
    0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x03, 0x07, 0x08, 0x00,
    0x20, 0x54, 0x54, 0x78, 0x40,
    0x7F, 0x28, 0x44, 0x44, 0x38,
    0x38, 0x44, 0x44, 0x44, 0x28,
    0x38, 0x44, 0x44, 0x28, 0x7F,
    0x38, 0x54, 0x54, 0x54, 0x18,
    0x00, 0x08, 0x7E, 0x09, 0x02,
    0x18, 0xA4, 0xA4, 0x9C, 0x78,
    0x7F, 0x08, 0x04, 0x04, 0x78,
    0x00, 0x44, 0x7D, 0x40, 0x00,
    0x20, 0x40, 0x40, 0x3D, 0x00,
    0x7F, 0x10, 0x28, 0x44, 0x00,
    0x00, 0x41, 0x7F, 0x40, 0x00,
    0x7C, 0x04, 0x78, 0x04, 0x78,
    0x7C, 0x08, 0x04, 0x04, 0x78,
    0x38, 0x44, 0x44, 0x44, 0x38,
    0xFC, 0x18, 0x24, 0x24, 0x18,
    0x18, 0x24, 0x24, 0x18, 0xFC,
    0x7C, 0x08, 0x04, 0x04, 0x08,
    0x48, 0x54, 0x54, 0x54, 0x24,
    0x04, 0x04, 0x3F, 0x44, 0x24,
    0x3C, 0x40, 0x40, 0x20, 0x7C,
    0x1C, 0x20, 0x40, 0x20, 0x1C,
    0x3C, 0x40, 0x30, 0x40, 0x3C,
    0x44, 0x28, 0x10, 0x28, 0x44,
    0x4C, 0x90, 0x90, 0x90, 0x7C,
    0x44, 0x64, 0x54, 0x4C, 0x44,
    0x00, 0x08, 0x36, 0x41, 0x00,
    0x00, 0x00, 0x77, 0x00, 0x00,
    0x00, 0x41, 0x36, 0x08, 0x00,
    0x02, 0x01, 0x02, 0x04, 0x02,
    0x3C, 0x26, 0x23, 0x26, 0x3C,
    0x1E, 0xA1, 0xA1, 0x61, 0x12,
    0x3A, 0x40, 0x40, 0x20, 0x7A,
    0x38, 0x54, 0x54, 0x55, 0x59,
    0x21, 0x55, 0x55, 0x79, 0x41,
    0x21, 0x54, 0x54, 0x78, 0x41,
    0x21, 0x55, 0x54, 0x78, 0x40,
    0x20, 0x54, 0x55, 0x79, 0x40,
    0x0C, 0x1E, 0x52, 0x72, 0x12,
    0x39, 0x55, 0x55, 0x55, 0x59,
    0x39, 0x54, 0x54, 0x54, 0x59,
    0x39, 0x55, 0x54, 0x54, 0x58,
    0x00, 0x00, 0x45, 0x7C, 0x41,
    0x00, 0x02, 0x45, 0x7D, 0x42,
    0x00, 0x01, 0x45, 0x7C, 0x40,
    0xF0, 0x29, 0x24, 0x29, 0xF0,
    0xF0, 0x28, 0x25, 0x28, 0xF0,
    0x7C, 0x54, 0x55, 0x45, 0x00,
    0x20, 0x54, 0x54, 0x7C, 0x54,
    0x7C, 0x0A, 0x09, 0x7F, 0x49,
    0x32, 0x49, 0x49, 0x49, 0x32,
    0x32, 0x48, 0x48, 0x48, 0x32,
    0x32, 0x4A, 0x48, 0x48, 0x30,
    0x3A, 0x41, 0x41, 0x21, 0x7A,
    0x3A, 0x42, 0x40, 0x20, 0x78,
    0x00, 0x9D, 0xA0, 0xA0, 0x7D,
    0x39, 0x44, 0x44, 0x44, 0x39,
    0x3D, 0x40, 0x40, 0x40, 0x3D,
    0x3C, 0x24, 0xFF, 0x24, 0x24,
    0x48, 0x7E, 0x49, 0x43, 0x66,
    0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
    0xFF, 0x09, 0x29, 0xF6, 0x20,
    0xC0, 0x88, 0x7E, 0x09, 0x03,
    0x20, 0x54, 0x54, 0x79, 0x41,
    0x00, 0x00, 0x44, 0x7D, 0x41,
    0x30, 0x48, 0x48, 0x4A, 0x32,
    0x38, 0x40, 0x40, 0x22, 0x7A,
    0x00, 0x7A, 0x0A, 0x0A, 0x72,
    0x7D, 0x0D, 0x19, 0x31, 0x7D,
    0x26, 0x29, 0x29, 0x2F, 0x28,
    0x26, 0x29, 0x29, 0x29, 0x26,
    0x30, 0x48, 0x4D, 0x40, 0x20,
    0x38, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x38,
    0x2F, 0x10, 0xC8, 0xAC, 0xBA,
    0x2F, 0x10, 0x28, 0x34, 0xFA,
    0x00, 0x00, 0x7B, 0x00, 0x00,
    0x08, 0x14, 0x2A, 0x14, 0x22,
    0x22, 0x14, 0x2A, 0x14, 0x08,
    0x95, 0x00, 0x22, 0x00, 0x95,
    0xAA, 0x00, 0x55, 0x00, 0xAA,
    0xAA, 0x55, 0xAA, 0x55, 0xAA,
    0x00, 0x00, 0x00, 0xFF, 0x00,
    0x10, 0x10, 0x10, 0xFF, 0x00,
    0x14, 0x14, 0x14, 0xFF, 0x00,
    0x10, 0x10, 0xFF, 0x00, 0xFF,
    0x10, 0x10, 0xF0, 0x10, 0xF0,
    0x14, 0x14, 0x14, 0xFC, 0x00,
    0x14, 0x14, 0xF7, 0x00, 0xFF,
    0x00, 0x00, 0xFF, 0x00, 0xFF,
    0x14, 0x14, 0xF4, 0x04, 0xFC,
    0x14, 0x14, 0x17, 0x10, 0x1F,
    0x10, 0x10, 0x1F, 0x10, 0x1F,
    0x14, 0x14, 0x14, 0x1F, 0x00,
    0x10, 0x10, 0x10, 0xF0, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x10,
    0x10, 0x10, 0x10, 0x1F, 0x10,
    0x10, 0x10, 0x10, 0xF0, 0x10,
    0x00, 0x00, 0x00, 0xFF, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xFF, 0x10,
    0x00, 0x00, 0x00, 0xFF, 0x14,
    0x00, 0x00, 0xFF, 0x00, 0xFF,
    0x00, 0x00, 0x1F, 0x10, 0x17,
    0x00, 0x00, 0xFC, 0x04, 0xF4,
    0x14, 0x14, 0x17, 0x10, 0x17,
    0x14, 0x14, 0xF4, 0x04, 0xF4,
    0x00, 0x00, 0xFF, 0x00, 0xF7,
    0x14, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0xF7, 0x00, 0xF7,
    0x14, 0x14, 0x14, 0x17, 0x14,
    0x10, 0x10, 0x1F, 0x10, 0x1F,
    0x14, 0x14, 0x14, 0xF4, 0x14,
    0x10, 0x10, 0xF0, 0x10, 0xF0,
    0x00, 0x00, 0x1F, 0x10, 0x1F,
    0x00, 0x00, 0x00, 0x1F, 0x14,
    0x00, 0x00, 0x00, 0xFC, 0x14,
    0x00, 0x00, 0xF0, 0x10, 0xF0,
    0x10, 0x10, 0xFF, 0x10, 0xFF,
    0x14, 0x14, 0x14, 0xFF, 0x14,
    0x10, 0x10, 0x10, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0xF0, 0x10,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xFF, 0xFF,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x38, 0x44, 0x44, 0x38, 0x44,
    0x7C, 0x2A, 0x2A, 0x3E, 0x14,
    0x7E, 0x02, 0x02, 0x06, 0x06,
    0x02, 0x7E, 0x02, 0x7E, 0x02,
    0x63, 0x55, 0x49, 0x41, 0x63,
    0x38, 0x44, 0x44, 0x3C, 0x04,
    0x40, 0x7E, 0x20, 0x1E, 0x20,
    0x06, 0x02, 0x7E, 0x02, 0x02,
    0x99, 0xA5, 0xE7, 0xA5, 0x99,
    0x1C, 0x2A, 0x49, 0x2A, 0x1C,
    0x4C, 0x72, 0x01, 0x72, 0x4C,
    0x30, 0x4A, 0x4D, 0x4D, 0x30,
    0x30, 0x48, 0x78, 0x48, 0x30,
    0xBC, 0x62, 0x5A, 0x46, 0x3D,
    0x3E, 0x49, 0x49, 0x49, 0x00,
    0x7E, 0x01, 0x01, 0x01, 0x7E,
    0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
    0x44, 0x44, 0x5F, 0x44, 0x44,
    0x40, 0x51, 0x4A, 0x44, 0x40,
    0x40, 0x44, 0x4A, 0x51, 0x40,
    0x00, 0x00, 0xFF, 0x01, 0x03,
    0xE0, 0x80, 0xFF, 0x00, 0x00,
    0x08, 0x08, 0x6B, 0x6B, 0x08,
    0x36, 0x12, 0x36, 0x24, 0x36,
    0x06, 0x0F, 0x09, 0x0F, 0x06,
    0x00, 0x00, 0x18, 0x18, 0x00,
    0x00, 0x00, 0x10, 0x10, 0x00,
    0x30, 0x40, 0xFF, 0x01, 0x01,
    0x00, 0x1F, 0x01, 0x01, 0x1E,
    0x00, 0x19, 0x1D, 0x17, 0x12,
    0x00, 0x3C, 0x3C, 0x3C, 0x3C,
    0x00, 0x00, 0x00, 0x00, 0x00,
};


/****************************************8*16的点阵************************************/
const unsigned char F8X16[]=	  
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//! 1
  0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//" 2
  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//# 3
  0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$ 4
  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//% 5
  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//& 6
  0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//' 7
  0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//( 8
  0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//) 9
  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//* 10
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+ 11
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//, 12
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//- 13
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//. 14
  0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,/// 15
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0 16
  0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1 17
  0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2 18
  0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3 19
  0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4 20
  0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5 21
  0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6 22
  0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7 23
  0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8 24
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9 25
  0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//: 26
  0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//; 27
  0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//< 28
  0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//= 29
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//> 30
  0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//? 31
  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@ 32
  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A 33
  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B 34
  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C 35
  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D 36
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E 37
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F 38
  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G 39
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H 40
  0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I 41
  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J 42
  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K 43
  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L 44
  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M 45
  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N 46
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O 47
  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P 48
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q 49
  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R 50
  0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S 51
  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T 52
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U 53
  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V 54
  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W 55
  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X 56
  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y 57
  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z 58
  0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[ 59
  0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\ 60
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//] 61
  0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^ 62
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_ 63
  0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//` 64
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a 65
  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b 66
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c 67
  0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d 68
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e 69
  0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f 70
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g 71
  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h 72
  0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i 73
  0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j 74
  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k 75
  0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l 76
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m 77
  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n 78
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o 79
  0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p 80
  0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q 81
  0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r 82
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s 83
  0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t 84
  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u 85
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v 86
  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w 87
  0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x 88
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y 89
  0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z 90
  0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{ 91
  0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//| 92
  0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//} 93
  0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~ 94
	0x00,0x08,0x06,0xFF,0x06,0x08,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,//↑8*16,95
	0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x10,0x60,0xFF,0x60,0x10,0x00,0x00,//↓8*16 96
};
//使用常用取模软件 PCtoLCD2002
//汉字采用 16x16 阴码 列行式 逆向
//中文字库
struct Cn16CharTypeDef const CN16CHAR[] = {
"<",0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xBC,0x8C,0x84,0x80,0x80,0x80,0x80,0x80,0x00,0x00,
0x00,0x01,0x03,0x07,0x0F,0x1F,0x3D,0x31,0x21,0x01,0x01,0x01,0x01,0x01,0x00,0x00,
">",0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x88,0xB8,0xF8,0xF0,0xE0,0xC0,0x80,0x00,
0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x11,0x1D,0x1F,0x0F,0x07,0x03,0x01,0x00,
"←",0x80,0x80,0xC0,0xC0,0xE0,0xA0,0x90,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,
0x00,0x00,0x01,0x01,0x03,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
"→",0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,0xA0,0xE0,0xC0,0xC0,0x80,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x02,0x03,0x01,0x01,0x00,0x00,0x00,
"↑",0x00,0x00,0x00,0x00,0x40,0x30,0x1C,0xFF,0x1C,0x30,0x40,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
"↓",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x02,0x0C,0x38,0xFF,0x38,0x0C,0x02,0x00,0x00,0x00,0x00,0x00,
"°",0x00,0x00,0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
"〇",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

"风",0x00,0x00,0xFE,0x02,0x12,0x22,0xC2,0x02,0xC2,0x32,0x02,0xFE,0x00,0x00,0x00,0x00,
0x80,0x60,0x1F,0x00,0x20,0x10,0x0C,0x03,0x0C,0x30,0x00,0x0F,0x30,0x40,0xF8,0x00,
"板",0x10,0x10,0xD0,0xFF,0x90,0x10,0x00,0xFC,0x24,0xE4,0x24,0x22,0x23,0xE2,0x00,0x00,
0x04,0x03,0x00,0xFF,0x00,0x83,0x60,0x1F,0x80,0x41,0x26,0x18,0x26,0x41,0x80,0x00,
"控",0x10,0x10,0x10,0xFF,0x90,0x20,0x98,0x48,0x28,0x09,0x0E,0x28,0x48,0xA8,0x18,0x00,
0x02,0x42,0x81,0x7F,0x00,0x40,0x40,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x40,0x00,
"制",0x40,0x50,0x4E,0x48,0x48,0xFF,0x48,0x48,0x48,0x40,0xF8,0x00,0x00,0xFF,0x00,0x00,
0x00,0x00,0x3E,0x02,0x02,0xFF,0x12,0x22,0x1E,0x00,0x0F,0x40,0x80,0x7F,0x00,0x00,
"系",0x00,0x00,0x22,0x32,0x2A,0xA6,0xA2,0x62,0x21,0x11,0x09,0x81,0x01,0x00,0x00,0x00,
0x00,0x42,0x22,0x13,0x0B,0x42,0x82,0x7E,0x02,0x02,0x0A,0x12,0x23,0x46,0x00,0x00,
"统",0x20,0x30,0xAC,0x63,0x30,0x00,0x88,0xC8,0xA8,0x99,0x8E,0x88,0xA8,0xC8,0x88,0x00,
0x22,0x67,0x22,0x12,0x12,0x80,0x40,0x30,0x0F,0x00,0x00,0x3F,0x40,0x40,0x71,0x00,

"角",0x20,0x10,0xE8,0x24,0x27,0x24,0x24,0xE4,0x24,0x34,0x2C,0x20,0xE0,0x00,0x00,0x00,
0x80,0x60,0x1F,0x09,0x09,0x09,0x09,0x7F,0x09,0x09,0x49,0x89,0x7F,0x00,0x00,0x00,
"度",0x00,0x00,0xFC,0x24,0x24,0x24,0xFC,0x25,0x26,0x24,0xFC,0x24,0x24,0x24,0x04,0x00,
0x40,0x30,0x8F,0x80,0x84,0x4C,0x55,0x25,0x25,0x25,0x55,0x4C,0x80,0x80,0x80,0x00,
"测",0x10,0x60,0x02,0x8C,0x00,0xFE,0x02,0xF2,0x02,0xFE,0x00,0xF8,0x00,0xFF,0x00,0x00,
0x04,0x04,0x7E,0x01,0x80,0x47,0x30,0x0F,0x10,0x27,0x00,0x47,0x80,0x7F,0x00,0x00,
"量",0x20,0x20,0x20,0xBE,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xBE,0x20,0x20,0x20,0x00,
0x00,0x80,0x80,0xAF,0xAA,0xAA,0xAA,0xFF,0xAA,0xAA,0xAA,0xAF,0x80,0x80,0x00,0x00,

"力",0x00,0x10,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,
0x00,0x80,0x40,0x20,0x18,0x06,0x01,0x00,0x20,0x40,0x80,0x40,0x3F,0x00,0x00,0x00,

"动",0x40,0x44,0xC4,0x44,0x44,0x44,0x40,0x10,0x10,0xFF,0x10,0x10,0x10,0xF0,0x00,0x00,
0x10,0x3C,0x13,0x10,0x14,0xB8,0x40,0x30,0x0E,0x01,0x40,0x80,0x40,0x3F,0x00,0x00,
"态",0x00,0x04,0x84,0x84,0x44,0x24,0x54,0x8F,0x14,0x24,0x44,0x84,0x84,0x04,0x00,0x00,
0x41,0x39,0x00,0x00,0x3C,0x40,0x40,0x42,0x4C,0x40,0x40,0x70,0x04,0x09,0x31,0x00,

"扇",0x00,0x00,0xFC,0x24,0x24,0x24,0x25,0x26,0x24,0x24,0x24,0x24,0x24,0x3C,0x00,0x00,
0x40,0x30,0x0F,0x21,0x15,0x49,0x81,0x7F,0x00,0x21,0x15,0x49,0x81,0x7F,0x00,0x00,

"当",0x00,0x40,0x42,0x44,0x58,0x40,0x40,0x7F,0x40,0x40,0x50,0x48,0xC6,0x00,0x00,0x00,
0x00,0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFF,0x00,0x00,0x00,
"前",0x08,0x08,0xE8,0x29,0x2E,0x28,0xE8,0x08,0x08,0xC8,0x0C,0x0B,0xE8,0x08,0x08,0x00,
0x00,0x00,0xFF,0x09,0x49,0x89,0x7F,0x00,0x00,0x0F,0x40,0x80,0x7F,0x00,0x00,0x00,

"时",0x00,0xFC,0x84,0x84,0x84,0xFC,0x00,0x10,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x00,
0x00,0x3F,0x10,0x10,0x10,0x3F,0x00,0x00,0x01,0x06,0x40,0x80,0x7F,0x00,0x00,0x00,
"间",0x00,0xF8,0x01,0x06,0x00,0xF0,0x12,0x12,0x12,0xF2,0x02,0x02,0x02,0xFE,0x00,0x00,
0x00,0xFF,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0x1F,0x00,0x40,0x80,0x7F,0x00,0x00,
"设",0x40,0x40,0x42,0xCC,0x00,0x40,0xA0,0x9E,0x82,0x82,0x82,0x9E,0xA0,0x20,0x20,0x00,
0x00,0x00,0x00,0x3F,0x90,0x88,0x40,0x43,0x2C,0x10,0x28,0x46,0x41,0x80,0x80,0x00,
"定",0x10,0x0C,0x44,0x44,0x44,0x44,0x45,0xC6,0x44,0x44,0x44,0x44,0x44,0x14,0x0C,0x00,
0x80,0x40,0x20,0x1E,0x20,0x40,0x40,0x7F,0x44,0x44,0x44,0x44,0x44,0x40,0x40,0x00,

"距",0x00,0x3E,0x22,0xE2,0x22,0x3E,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0xE2,0x02,0x00,
0x20,0x3F,0x20,0x1F,0x11,0x11,0x00,0x7F,0x44,0x44,0x44,0x44,0x44,0x47,0x40,0x00,
"离",0x04,0x04,0x04,0xF4,0x84,0xD4,0xA5,0xA6,0xA4,0xD4,0x84,0xF4,0x04,0x04,0x04,0x00,
0x00,0xFE,0x02,0x02,0x12,0x3A,0x16,0x13,0x12,0x1A,0x32,0x42,0x82,0x7E,0x00,0x00,

"开",0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x00,
0x00,0x80,0x40,0x30,0x0F,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
"始",0x10,0x10,0xF0,0x1F,0x10,0xF0,0x00,0x40,0xE0,0x58,0x47,0x40,0x50,0x60,0xC0,0x00,
0x40,0x22,0x15,0x08,0x16,0x21,0x00,0x00,0xFE,0x42,0x42,0x42,0x42,0xFE,0x00,0x00,

"停",0x80,0x60,0xF8,0x07,0x00,0x04,0x74,0x54,0x55,0x56,0x54,0x54,0x74,0x04,0x00,0x00,
0x00,0x00,0xFF,0x00,0x03,0x01,0x05,0x45,0x85,0x7D,0x05,0x05,0x05,0x01,0x03,0x00,
"止",0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0xFF,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x00,
0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,

"返",0x40,0x40,0x42,0xCC,0x00,0x00,0xFC,0x24,0xA4,0x24,0x22,0x22,0xA3,0x62,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x58,0x47,0x50,0x48,0x45,0x42,0x45,0x48,0x50,0x40,0x00,
"回",0x00,0x00,0xFE,0x02,0x02,0xF2,0x12,0x12,0x12,0xF2,0x02,0x02,0xFE,0x00,0x00,0x00,
0x00,0x00,0x7F,0x20,0x20,0x27,0x24,0x24,0x24,0x27,0x20,0x20,0x7F,0x00,0x00,0x00,

"结",0x20,0x30,0xAC,0x63,0x20,0x18,0x08,0x48,0x48,0x48,0x7F,0x48,0x48,0x48,0x08,0x00,
0x22,0x67,0x22,0x12,0x12,0x12,0x00,0xFE,0x42,0x42,0x42,0x42,0x42,0xFE,0x00,0x00,
"构",0x10,0x10,0xD0,0xFF,0x90,0x10,0x40,0x30,0x0F,0xC8,0x08,0x08,0x08,0xF8,0x00,0x00,
0x04,0x03,0x00,0xFF,0x00,0x03,0x00,0x0C,0x0B,0x08,0x4A,0x9C,0x40,0x3F,0x00,0x00,
"体",0x00,0x80,0x60,0xF8,0x07,0x10,0x10,0x10,0xD0,0xFF,0xD0,0x10,0x10,0x10,0x00,0x00,
0x01,0x00,0x00,0xFF,0x10,0x08,0x04,0x0B,0x08,0xFF,0x08,0x0B,0x04,0x08,0x10,0x00,
"指",0x10,0x10,0x10,0xFF,0x10,0x90,0x00,0x3F,0x48,0x48,0x44,0x44,0x44,0x42,0x70,0x00,
0x04,0x44,0x82,0x7F,0x01,0x00,0x00,0xFF,0x49,0x49,0x49,0x49,0x49,0xFF,0x00,0x00,
"针",0x40,0x20,0x38,0xE7,0x24,0x24,0x44,0x40,0x40,0x40,0xFF,0x40,0x40,0x40,0x40,0x00,
0x01,0x01,0x01,0x7F,0x21,0x11,0x09,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
"菜",0x04,0x04,0x44,0xC4,0x4F,0x44,0x44,0xC4,0x24,0x24,0x2F,0xB4,0x24,0x04,0x04,0x00,
0x40,0x44,0x24,0x24,0x15,0x0C,0x04,0xFE,0x04,0x0C,0x15,0x24,0x24,0x44,0x40,0x00,
"单",0x00,0x00,0xF8,0x49,0x4A,0x4C,0x48,0xF8,0x48,0x4C,0x4A,0x49,0xF8,0x00,0x00,0x00,
	
0x10,0x10,0x13,0x12,0x12,0x12,0x12,0xFF,0x12,0x12,0x12,0x12,0x13,0x10,0x10,0x00,
"题",0x80,0x80,0xBE,0xAA,0xAA,0xAA,0xBE,0x80,0x02,0xF2,0x1A,0xD6,0x12,0xF2,0x02,0x00, 
0x80,0x60,0x1C,0x20,0x7F,0x44,0x44,0x44,0x50,0x4B,0x44,0x43,0x44,0x4B,0x50,0x00,
"目",0x00,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0xFE,0x00,0x00,0x00,
0x00,0x00,0xFF,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0xFF,0x00,0x00,0x00,
"功",0x08,0x08,0x08,0xF8,0x08,0x08,0x08,0x10,0x10,0xFF,0x10,0x10,0x10,0xF0,0x00,0x00,
0x10,0x30,0x10,0x1F,0x08,0x88,0x48,0x30,0x0E,0x01,0x40,0x80,0x40,0x3F,0x00,0x00,
"能",0x08,0xCC,0x4A,0x49,0x48,0x4A,0xCC,0x18,0x00,0x7F,0x88,0x88,0x84,0x82,0xE0,0x00,
0x00,0xFF,0x12,0x12,0x52,0x92,0x7F,0x00,0x00,0x7E,0x88,0x88,0x84,0x82,0xE0,0x00,

"参",0x00,0x20,0x20,0xA8,0x6C,0x2A,0x39,0x28,0xA8,0x2A,0x6C,0xA8,0x20,0x20,0x00,0x00,
0x02,0x82,0x81,0x90,0x92,0x4A,0x49,0x45,0x24,0x22,0x10,0x08,0x01,0x02,0x02,0x00,
"数",0x90,0x52,0x34,0x10,0xFF,0x10,0x34,0x52,0x80,0x70,0x8F,0x08,0x08,0xF8,0x08,0x00,
0x82,0x9A,0x56,0x63,0x22,0x52,0x8E,0x00,0x80,0x40,0x33,0x0C,0x33,0x40,0x80,0x00,
"设",0x40,0x40,0x42,0xCC,0x00,0x40,0xA0,0x9E,0x82,0x82,0x82,0x9E,0xA0,0x20,0x20,0x00,
0x00,0x00,0x00,0x3F,0x90,0x88,0x40,0x43,0x2C,0x10,0x28,0x46,0x41,0x80,0x80,0x00,
"置",0x00,0x17,0x15,0xD5,0x55,0x57,0x55,0x7D,0x55,0x57,0x55,0xD5,0x15,0x17,0x00,0x00,
0x40,0x40,0x40,0x7F,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x7F,0x40,0x40,0x40,0x00,

"监",0x00,0x00,0x7E,0x00,0x00,0xFF,0x00,0x40,0x30,0x0F,0x04,0x14,0x64,0x04,0x00,0x00,
0x40,0x40,0x7E,0x42,0x42,0x7E,0x42,0x42,0x42,0x7E,0x42,0x42,0x7E,0x40,0x40,0x00,
"控",0x10,0x10,0x10,0xFF,0x90,0x20,0x98,0x48,0x28,0x09,0x0E,0x28,0x48,0xA8,0x18,0x00,
0x02,0x42,0x81,0x7F,0x00,0x40,0x40,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x40,0x00,
"显",0x00,0x00,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,0x00, 
0x40,0x42,0x44,0x58,0x40,0x7F,0x40,0x40,0x40,0x7F,0x40,0x50,0x48,0x46,0x40,0x00,
"示",0x40,0x40,0x42,0x42,0x42,0x42,0x42,0xC2,0x42,0x42,0x42,0x42,0x42,0x40,0x40,0x00,
0x20,0x10,0x08,0x06,0x00,0x40,0x80,0x7F,0x00,0x00,0x00,0x02,0x04,0x08,0x30,0x00,

"调",0x40,0x42,0xCC,0x00,0x00,0xFE,0x82,0x92,0x92,0xFE,0x92,0x92,0x82,0xFE,0x00,0x00,
0x00,0x00,0x3F,0x10,0x88,0x7F,0x00,0x1E,0x12,0x12,0x12,0x5E,0x80,0x7F,0x00,0x00,
"试",0x40,0x40,0x42,0xCC,0x00,0x90,0x90,0x90,0x90,0x90,0xFF,0x10,0x11,0x16,0x10,0x00,
0x00,0x00,0x00,0x3F,0x10,0x28,0x60,0x3F,0x10,0x10,0x01,0x0E,0x30,0x40,0xF0,0x00,
"辅",0xC8,0xB8,0x8F,0xE8,0x88,0x88,0x04,0xE4,0x24,0x24,0xFF,0x24,0x25,0xE6,0x04,0x00,
0x08,0x18,0x08,0xFF,0x04,0x04,0x00,0xFF,0x09,0x09,0x7F,0x09,0x89,0xFF,0x00,0x00,
"助",0x00,0xFE,0x22,0x22,0x22,0xFE,0x00,0x00,0x10,0x10,0xFF,0x10,0x10,0xF0,0x00,0x00,
0x20,0x3F,0x21,0x21,0x11,0x1F,0x90,0x40,0x20,0x18,0x47,0x80,0x40,0x3F,0x00,0x00,

"基",0x00,0x04,0x04,0x04,0xFF,0x54,0x54,0x54,0x54,0x54,0xFF,0x04,0x04,0x04,0x00,0x00,
0x11,0x11,0x89,0x85,0x93,0x91,0x91,0xFD,0x91,0x91,0x93,0x85,0x89,0x11,0x11,0x00,
"础",0x04,0x84,0xE4,0x5C,0x44,0xC4,0x00,0x7C,0x40,0x40,0xFF,0x40,0x40,0x7C,0x00,0x00,
0x02,0x01,0x7F,0x10,0x10,0x3F,0x00,0x7E,0x40,0x40,0x7F,0x40,0x40,0xFE,0x00,0x00,
"部",0x40,0x44,0x54,0x65,0x46,0x44,0x64,0x54,0x44,0x40,0xFE,0x02,0x22,0xDA,0x06,0x00,
0x00,0x00,0x7E,0x22,0x22,0x22,0x22,0x7E,0x00,0x00,0xFF,0x08,0x10,0x08,0x07,0x00,
"分",0x80,0x40,0x20,0x90,0x88,0x86,0x80,0x80,0x80,0x83,0x8C,0x10,0x20,0x40,0x80,0x00,
0x00,0x80,0x40,0x20,0x18,0x07,0x00,0x40,0x80,0x40,0x3F,0x00,0x00,0x00,0x00,0x00,

"发",0x00,0x00,0x18,0x16,0x10,0xD0,0xB8,0x97,0x90,0x90,0x90,0x92,0x94,0x10,0x00,0x00,
0x00,0x20,0x10,0x8C,0x83,0x80,0x41,0x46,0x28,0x10,0x28,0x44,0x43,0x80,0x80,0x00,
"挥",0x10,0x10,0x10,0xFF,0x10,0x98,0x26,0x22,0xE2,0x3A,0xA2,0x22,0x22,0x2A,0x06,0x00,
0x04,0x44,0x82,0x7F,0x01,0x10,0x10,0x13,0x12,0x12,0xFF,0x12,0x12,0x12,0x10,0x00,

"制",0x40,0x50,0x4E,0x48,0x48,0xFF,0x48,0x48,0x48,0x40,0xF8,0x00,0x00,0xFF,0x00,0x00,
0x00,0x00,0x3E,0x02,0x02,0xFF,0x12,0x22,0x1E,0x00,0x0F,0x40,0x80,0x7F,0x00,0x00,
"灵",0x00,0x00,0x82,0x92,0x92,0x92,0x92,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,
0x80,0x80,0x48,0x46,0x20,0x10,0x08,0x07,0x08,0x10,0x28,0x46,0x40,0x80,0x80,0x00,
"敏",0x08,0xF7,0x14,0x54,0x94,0x14,0xF4,0x04,0x80,0x70,0x8F,0x08,0x08,0xF8,0x08,0x00,
0x1D,0x13,0x11,0x55,0x99,0x51,0x3F,0x91,0x41,0x20,0x13,0x0C,0x33,0x40,0x80,0x00,
"度",0x00,0x00,0xFC,0x24,0x24,0x24,0xFC,0x25,0x26,0x24,0xFC,0x24,0x24,0x24,0x04,0x00,
0x40,0x30,0x8F,0x80,0x84,0x4C,0x55,0x25,0x25,0x25,0x55,0x4C,0x80,0x80,0x80,0x00,

"模",0x10,0x10,0xD0,0xFF,0x90,0x14,0xE4,0xAF,0xA4,0xA4,0xA4,0xAF,0xE4,0x04,0x00,0x00,
0x04,0x03,0x00,0xFF,0x00,0x89,0x4B,0x2A,0x1A,0x0E,0x1A,0x2A,0x4B,0x88,0x80,0x00,
"式",0x10,0x10,0x90,0x90,0x90,0x90,0x90,0x10,0x10,0xFF,0x10,0x10,0x11,0x16,0x10,0x00,
0x00,0x20,0x60,0x20,0x3F,0x10,0x10,0x10,0x00,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00,

"串",0x00,0x00,0x3C,0x24,0x24,0x24,0x24,0xFF,0x24,0x24,0x24,0x24,0x3C,0x00,0x00,0x00,
0x00,0x1F,0x09,0x09,0x09,0x09,0x09,0xFF,0x09,0x09,0x09,0x09,0x09,0x1F,0x00,0x00,
"口",0x00,0x00,0xFC,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xFC,0x00,0x00,0x00,
0x00,0x00,0x7F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7F,0x00,0x00,0x00,

"传",0x00,0x80,0x60,0xF8,0x07,0x40,0x48,0x48,0xE8,0x5F,0x48,0x48,0x48,0x40,0x40,0x00,
0x01,0x00,0x00,0xFF,0x00,0x00,0x02,0x0B,0x12,0x22,0xD2,0x0A,0x06,0x00,0x00,0x00,
"感",0x00,0x00,0xFC,0x04,0xD4,0x54,0x54,0x54,0xD4,0x04,0x7F,0x84,0xC5,0x36,0x04,0x00,
0x88,0x66,0x01,0x00,0x63,0x82,0x82,0x92,0xAB,0x84,0x82,0xC1,0x02,0x24,0xCF,0x00,
"器",0x80,0x80,0x9E,0x92,0x92,0x92,0x9E,0xE0,0x80,0x9E,0xB2,0xD2,0x92,0x9E,0x80,0x00,
0x08,0x08,0xF4,0x94,0x92,0x92,0xF1,0x00,0x01,0xF2,0x92,0x94,0x94,0xF8,0x08,0x00,

"运",0x40,0x40,0x42,0xCC,0x00,0x20,0x22,0x22,0xA2,0x62,0x22,0x22,0x22,0x20,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x44,0x4E,0x45,0x44,0x44,0x44,0x45,0x46,0x4C,0x40,0x00,
"行",0x00,0x10,0x88,0xC4,0x33,0x00,0x40,0x42,0x42,0x42,0xC2,0x42,0x42,0x42,0x40,0x00,
0x02,0x01,0x00,0xFF,0x00,0x00,0x00,0x00,0x40,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,
"状",0x00,0x08,0x30,0x00,0xFF,0x20,0x20,0x20,0x20,0xFF,0x20,0x20,0x22,0x2C,0x20,0x00,
0x04,0x04,0x02,0x01,0xFF,0x80,0x40,0x30,0x0E,0x01,0x06,0x18,0x20,0x40,0x80,0x00,
"态",0x00,0x04,0x84,0x84,0x44,0x24,0x54,0x8F,0x14,0x24,0x44,0x84,0x84,0x04,0x00,0x00,
0x41,0x39,0x00,0x00,0x3C,0x40,0x40,0x42,0x4C,0x40,0x40,0x70,0x04,0x09,0x31,0x00,

"测",0x10,0x60,0x02,0x8C,0x00,0xFE,0x02,0xF2,0x02,0xFE,0x00,0xF8,0x00,0xFF,0x00,0x00,
0x04,0x04,0x7E,0x01,0x80,0x47,0x30,0x0F,0x10,0x27,0x00,0x47,0x80,0x7F,0x00,0x00,
"据",0x10,0x10,0xFF,0x10,0x90,0x00,0xFE,0x92,0x92,0x92,0xF2,0x92,0x92,0x9E,0x80,0x00,
0x42,0x82,0x7F,0x01,0x80,0x60,0x1F,0x00,0xFC,0x44,0x47,0x44,0x44,0xFC,0x00,0x00,

"遥",0x40,0x40,0x42,0xCC,0x00,0x00,0x4A,0x32,0x22,0x2A,0xF1,0x21,0x31,0x2D,0x00,0x00,
0x00,0x80,0x40,0x3F,0x40,0x80,0x81,0xBD,0xA1,0xA1,0xBF,0xA1,0xA1,0xBD,0x81,0x00,

"更",0x00,0xC0,0xF0,0x78,0x1C,0x0C,0x0E,0x06,0x06,0x0E,0x0C,0x1C,0x78,0xF0,0xC0,0x00,
0x00,0x03,0x0F,0x1E,0x38,0x30,0x30,0x00,0x00,0x3F,0x3E,0x3C,0x3E,0x37,0x23,0x00,

"选",0x40,0x40,0x42,0xCC,0x00,0x50,0x4E,0xC8,0x48,0x7F,0xC8,0x48,0x48,0x40,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x50,0x4C,0x43,0x40,0x40,0x4F,0x50,0x50,0x5C,0x40,0x00,
"择",0x10,0x10,0xFF,0x10,0x00,0x82,0x86,0x4A,0x52,0xA2,0x52,0x4A,0x86,0x80,0x80,0x00,
0x42,0x82,0x7F,0x01,0x00,0x10,0x12,0x12,0x12,0xFF,0x12,0x12,0x12,0x10,0x00,0x00,

"整",0x04,0x74,0xD4,0x54,0xFF,0x54,0xD4,0x74,0x14,0x08,0x77,0x84,0x44,0x3C,0x04,0x00,
0x82,0x89,0x88,0xE8,0x8B,0x88,0x88,0xF9,0xA8,0xAA,0xA9,0xA8,0xA9,0x8A,0x82,0x00,

"切",0x40,0x40,0x40,0xFF,0x20,0x20,0x20,0x04,0x04,0xFC,0x04,0x04,0x04,0xFC,0x00,0x00,
0x00,0x00,0x00,0x1F,0x08,0x84,0x42,0x20,0x18,0x07,0x40,0x80,0x40,0x3F,0x00,0x00,
"换",0x10,0x10,0x10,0xFF,0x90,0x20,0x10,0xE8,0x27,0x24,0xE4,0x34,0x2C,0xE0,0x00,0x00,
0x02,0x42,0x81,0x7F,0x00,0x84,0x84,0x47,0x24,0x1C,0x07,0x1C,0x24,0x47,0x84,0x00,

"离",0x04,0x04,0x04,0xF4,0x84,0xD4,0xA5,0xA6,0xA4,0xD4,0x84,0xF4,0x04,0x04,0x04,0x00,
0x00,0xFE,0x02,0x02,0x12,0x3A,0x16,0x13,0x12,0x1A,0x32,0x42,0x82,0x7E,0x00,0x00,
"开",0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x00,
0x00,0x80,0x40,0x30,0x0F,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,

"提",0x10,0x10,0x10,0xFF,0x10,0x90,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00, 
0x04,0x44,0x82,0x7F,0x01,0x80,0x42,0x3A,0x42,0x82,0xFE,0x92,0x92,0x92,0x82,0x00,

"将",0x00,0x08,0x30,0x00,0xFF,0x00,0x88,0x84,0x53,0x22,0x12,0xCA,0x06,0x00,0x00,0x00,
0x04,0x04,0x02,0x01,0xFF,0x00,0x02,0x0A,0x12,0x42,0x82,0x7F,0x02,0x02,0x02,0x00,
"会",0x40,0x40,0x20,0x20,0x50,0x48,0x44,0x43,0x44,0x48,0x50,0x20,0x20,0x40,0x40,0x00,
0x00,0x02,0x42,0xE2,0x52,0x4A,0x46,0x42,0x42,0x42,0x52,0x62,0xC2,0x02,0x00,0x00,

"主",0x00,0x08,0x08,0x08,0x08,0x08,0x09,0xFA,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,
0x40,0x40,0x41,0x41,0x41,0x41,0x41,0x7F,0x41,0x41,0x41,0x41,0x41,0x40,0x40,0x00,
"界",0x00,0x00,0x00,0xFE,0x92,0x92,0x92,0xFE,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,0x00,
0x08,0x08,0x04,0x84,0x62,0x1E,0x01,0x00,0x01,0xFE,0x02,0x04,0x04,0x08,0x08,0x00,
"面",0x02,0x02,0xE2,0x22,0x22,0xF2,0x2E,0x22,0x22,0xE2,0x22,0x22,0xE2,0x02,0x02,0x00,
0x00,0x00,0xFF,0x40,0x40,0x7F,0x49,0x49,0x49,0x7F,0x40,0x40,0xFF,0x00,0x00,0x00,



"你",0x00,0x80,0x60,0xF8,0x07,0x40,0x20,0x18,0x0F,0x08,0xC8,0x08,0x08,0x28,0x18,0x00,
0x01,0x00,0x00,0xFF,0x00,0x10,0x0C,0x03,0x40,0x80,0x7F,0x00,0x01,0x06,0x18,0x00,
"确",0x04,0x84,0xE4,0x5C,0x44,0xC4,0x20,0x10,0xE8,0x27,0x24,0xE4,0x34,0x2C,0xE0,0x00,
0x02,0x01,0x7F,0x10,0x10,0x3F,0x80,0x60,0x1F,0x09,0x09,0x3F,0x49,0x89,0x7F,0x00,
"定",0x10,0x0C,0x44,0x44,0x44,0x44,0x45,0xC6,0x44,0x44,0x44,0x44,0x44,0x14,0x0C,0x00,
0x80,0x40,0x20,0x1E,0x20,0x40,0x40,0x7F,0x44,0x44,0x44,0x44,0x44,0x40,0x40,0x00,
"要",0x00,0x02,0xF2,0x92,0x92,0xFE,0x92,0x92,0x92,0xFE,0x92,0x92,0xF2,0x02,0x00,0x00,
0x00,0x02,0x82,0x92,0x9A,0x56,0x63,0x22,0x32,0x4A,0x46,0x42,0x82,0x02,0x00,0x00,

"吗",0x00,0xFC,0x04,0x04,0xFC,0x00,0x02,0xF2,0x82,0x82,0x82,0xFE,0x80,0x80,0x00,0x00,
0x00,0x0F,0x04,0x04,0x0F,0x00,0x08,0x08,0x08,0x08,0x48,0x88,0x40,0x3F,0x00,0x00,

"返",0x40,0x40,0x42,0xCC,0x00,0x00,0xFC,0x24,0xA4,0x24,0x22,0x22,0xA3,0x62,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x58,0x47,0x50,0x48,0x45,0x42,0x45,0x48,0x50,0x40,0x00,
"回",0x00,0x00,0xFE,0x02,0x02,0xF2,0x12,0x12,0x12,0xF2,0x02,0x02,0xFE,0x00,0x00,0x00,
0x00,0x00,0x7F,0x20,0x20,0x27,0x24,0x24,0x24,0x27,0x20,0x20,0x7F,0x00,0x00,0x00,

"三",0x00,0x04,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x04,0x00,0x00,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,
"角",0x20,0x10,0xE8,0x24,0x27,0x24,0x24,0xE4,0x24,0x34,0x2C,0x20,0xE0,0x00,0x00,0x00, 
0x80,0x60,0x1F,0x09,0x09,0x09,0x09,0x7F,0x09,0x09,0x49,0x89,0x7F,0x00,0x00,0x00,
"形",0x80,0x82,0x82,0xFE,0x82,0x82,0xFE,0x82,0x82,0x80,0x20,0x10,0x08,0x86,0x60,0x00,
0x80,0x40,0x30,0x0F,0x00,0x00,0xFF,0x00,0x00,0x80,0x84,0x42,0x21,0x10,0x0C,0x00,

"正",0x00,0x02,0x02,0xC2,0x02,0x02,0x02,0xFE,0x82,0x82,0x82,0x82,0x82,0x02,0x00,0x00,
0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,
"方",0x08,0x08,0x08,0x08,0x08,0xF8,0x89,0x8E,0x88,0x88,0x88,0x88,0x08,0x08,0x08,0x00,
0x00,0x80,0x40,0x20,0x18,0x07,0x00,0x00,0x40,0x80,0x40,0x3F,0x00,0x00,0x00,0x00,

"圆",0x00,0xFE,0x02,0x82,0xBA,0xAA,0xAA,0xAA,0xAA,0xAA,0xBA,0x82,0x02,0xFE,0x00,0x00,
0x00,0xFF,0x40,0x4F,0x60,0x50,0x48,0x46,0x48,0x50,0x60,0x4F,0x40,0xFF,0x00,0x00,

"半",0x00,0x00,0x42,0x44,0x58,0x40,0x40,0xFF,0x40,0x40,0x50,0x48,0x46,0x00,0x00,0x00,
0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xFF,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,
"径",0x00,0x10,0x88,0xC4,0x33,0x80,0x82,0x42,0x22,0x12,0x1A,0x26,0x22,0x40,0x80,0x00,
0x02,0x01,0x00,0xFF,0x00,0x40,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0x40,0x00,

"边",0x40,0x40,0x42,0xCC,0x00,0x00,0x08,0x08,0x88,0x7F,0x08,0x08,0x08,0xF8,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x50,0x48,0x46,0x41,0x40,0x48,0x50,0x48,0x47,0x40,0x00,
"长",0x80,0x80,0x80,0x80,0xFF,0x80,0x80,0xA0,0x90,0x88,0x84,0x82,0x80,0x80,0x80,0x00,
0x00,0x00,0x00,0x00,0xFF,0x40,0x21,0x12,0x04,0x08,0x10,0x20,0x20,0x40,0x40,0x00,

"距",0x00,0x3E,0x22,0xE2,0x22,0x3E,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0xE2,0x02,0x00,
0x20,0x3F,0x20,0x1F,0x11,0x11,0x00,0x7F,0x44,0x44,0x44,0x44,0x44,0x47,0x40,0x00,

"查",0x40,0x44,0x24,0xA4,0x94,0x8C,0x84,0xFF,0x84,0x8C,0x94,0xA4,0x24,0x44,0x40,0x00,
0x40,0x40,0x40,0x5F,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x5F,0x40,0x40,0x40,0x00,
"看",0x20,0x22,0x2A,0x2A,0xAA,0x6A,0x3A,0x2E,0x29,0x29,0x29,0x29,0x29,0x20,0x20,0x00,
0x08,0x04,0x02,0x01,0xFF,0x55,0x55,0x55,0x55,0x55,0x55,0xFF,0x00,0x00,0x00,0x00,	//117


};
