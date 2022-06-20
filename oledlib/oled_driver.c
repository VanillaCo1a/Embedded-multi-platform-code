/*
	原作者哔哩哔哩:							MjGame 		https://space.bilibili.com/38673747
	同GifHub:								maoyongjie 	https://github.com/hello-myj/stm32_oled/
	代码整理注释删减增加优化等 哔哩哔哩:	一只程序缘	https://space.bilibili.com/237304109
	整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作
	
	图形库原理:其实就是对一个数组进行操作,数组操作完成之后,直接将整个
	数组刷新到屏幕上
	因此此c文件用于配置oled底层 用于单片机与oled的直接且唯一通信
	
	移植此图形库主要改变以下内容
	OLED_GPIO_Init_function()	配置通信引脚
	OLED_Write_Byte()			写命令&数据
*/

#include "oled_driver.h"

void OLED_Write_Byte(unsigned char oled_word, unsigned char oled_mode) {
	#if OLED_Communication_MODE == 0
	OLED_I2C_Init();
	OLED_I2C_Start();
	OLED_I2C_Write_Byte((OLED_ADDRESS<<1)|0x00);
	OLED_I2C_Write_Wait();	
	if(oled_mode == OLED_MODE_DATA) {
		OLED_I2C_Write_Byte(0x40);			//数据寄存器
	}else if (oled_mode == OLED_MODE_CMD){
		OLED_I2C_Write_Byte(0x00);			//命令寄存器
	}
	OLED_I2C_Write_Wait();	
	OLED_I2C_Write_Byte(oled_word);
	OLED_I2C_Write_Wait();	
	OLED_I2C_Stop();
	#else
	OLED_SPI_Init();
	if(oled_mode == OLED_MODE_DATA) {
		GPIO_SetBits(OLED_DC_GPIO_S, OLED_DC_PIN);
	}else {
		GPIO_ResetBits(OLED_DC_GPIO_S, OLED_DC_PIN);
	}
	OLED_SPI_Write(oled_word);
	GPIO_SetBits(OLED_DC_GPIO_S, OLED_DC_PIN);
	#endif
}
void OLED_Init(void)
{
	OLED_Write_Byte(0xAE, OLED_MODE_CMD); //display off
	OLED_Write_Byte(0x20, OLED_MODE_CMD);	//Set Memory Addressing Mode	
	OLED_Write_Byte(0x10, OLED_MODE_CMD);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	OLED_Write_Byte(0xb0, OLED_MODE_CMD);	//Set Page Start Address for Page Addressing Mode,0-7
	OLED_Write_Byte(0xc8, OLED_MODE_CMD);	//Set COM Output Scan Direction
	OLED_Write_Byte(0x00, OLED_MODE_CMD); //---set low column address
	OLED_Write_Byte(0x10, OLED_MODE_CMD); //---set high column address
	OLED_Write_Byte(0x40, OLED_MODE_CMD); //--set start line address
	OLED_Write_Byte(0x81, OLED_MODE_CMD); //--set contrast control register
	OLED_Write_Byte(0xff, OLED_MODE_CMD); //亮度调节 0x00~0xff
	OLED_Write_Byte(0xa1, OLED_MODE_CMD); //--set segment re-map 0 to 127
	OLED_Write_Byte(0xa6, OLED_MODE_CMD); //--set normal display
	OLED_Write_Byte(0xa8, OLED_MODE_CMD); //--set multiplex ratio(1 to 64)
	OLED_Write_Byte(0x3F, OLED_MODE_CMD); //
	OLED_Write_Byte(0xa4, OLED_MODE_CMD); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_Write_Byte(0xd3, OLED_MODE_CMD); //-set display offset
	OLED_Write_Byte(0x00, OLED_MODE_CMD); //-not offset
	OLED_Write_Byte(0xd5, OLED_MODE_CMD); //--set display clock divide ratio/oscillator frequency
	OLED_Write_Byte(0xf0, OLED_MODE_CMD); //--set divide ratio
	OLED_Write_Byte(0xd9, OLED_MODE_CMD); //--set pre-charge period
	OLED_Write_Byte(0x22, OLED_MODE_CMD); //
	OLED_Write_Byte(0xda, OLED_MODE_CMD); //--set com pins hardware configuration
	OLED_Write_Byte(0x12, OLED_MODE_CMD);
	OLED_Write_Byte(0xdb, OLED_MODE_CMD); //--set vcomh
	OLED_Write_Byte(0x20, OLED_MODE_CMD); //0x20,0.77xVcc
	OLED_Write_Byte(0x8d, OLED_MODE_CMD); //--set DC-DC enable
	OLED_Write_Byte(0x14, OLED_MODE_CMD); //
	OLED_Write_Byte(0xaf, OLED_MODE_CMD); //--turn on oled panel
	OLED_Clear();
}
void OLED_Clear(void) {		//清屏 全部发送0x00
	unsigned char i = 0, j = 0;
	for(i=0; i<8; i++) {
		OLED_Write_Byte(0xb0+i, OLED_MODE_CMD);		//page0-page1
		OLED_Write_Byte(0x00, OLED_MODE_CMD);			//low  column start address
		OLED_Write_Byte(0x10, OLED_MODE_CMD);			//high column start address
		for(j=0; j<128; j++) {
			OLED_Write_Byte(0x00, OLED_MODE_DATA);
		}
	}
}
void OLED_On(void) {
	OLED_Write_Byte(0X8D, OLED_MODE_CMD);  //设置电荷泵
	OLED_Write_Byte(0X14, OLED_MODE_CMD);  //开启电荷泵
	OLED_Write_Byte(0XAF, OLED_MODE_CMD);  //OLED唤醒
}
void OLED_Off(void) {
	OLED_Write_Byte(0X8D, OLED_MODE_CMD);  //设置电荷泵
	OLED_Write_Byte(0X10, OLED_MODE_CMD);  //关闭电荷泵
	OLED_Write_Byte(0XAE, OLED_MODE_CMD);  //OLED休眠
}
void OLED_Fill(unsigned char BMP[]) {
	u8 i = 0, j = 0;
	unsigned char *p = BMP;

	for(i=0; i<8; i++) {
		OLED_Write_Byte(0xb0+i, OLED_MODE_CMD);		//page0-page1
		OLED_Write_Byte(0x02, OLED_MODE_CMD);			//low column start address
		OLED_Write_Byte(0x10, OLED_MODE_CMD);	
		for(j=0;j<128;j++) {
			OLED_Write_Byte(*p++, OLED_MODE_DATA);
		}
	}
}

/*****************************************************	库函数方式初始化GPIO	****************************************************/
void OLED_GPIO_Init_function(void) {
	#if OLED_Communication_MODE == 0
	//0.定义一个配置GPIO的结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;
	//1.初始化GPIO时钟
	RCC_APB2PeriphClockCmd(OLED_I2C_GPIO_L, ENABLE);	//设置需要开启的APB2时钟;
	//2.1调用配置GPIO工作方式的库函数
	GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN | OLED_SDA_PIN;	//初始化所需引脚;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//设置GPIO的工作模式; 此处设置为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置GPIO的工作频率; 此处设置为50Mhz
	//2.2将配置好GPIO设置的结构体变量传入初始化函数
	GPIO_Init(OLED_I2C_GPIO_S, &GPIO_InitStructure);	//设置要配置的GPIO;
	
	GPIO_SetBits(OLED_I2C_GPIO_S, OLED_SCL_PIN | OLED_SDA_PIN);
	
	/**
	//七脚oled使用I2C时还需拉高RST, 拉低DC和CS
	RCC_APB2PeriphClockCmd(OLED_SPI_GPIO_L, ENABLE);	//设置需要开启的APB2时钟;
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN | OLED_DC_PIN | OLED_CS_PIN;	//初始化所需引脚;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//设置GPIO的工作模式; 此处设置为推挽输出
	GPIO_Init(OLED_I2C_GPIO_S, &GPIO_InitStructure);	//设置要配置的GPIO;
	GPIO_SetBits(OLED_SPI_GPIO_S, OLED_RST_PIN);
	GPIO_ResetBits(OLED_SPI_GPIO_S, OLED_DC_PIN | OLED_CS_PIN);		**/
	#else
		//0.定义一个配置GPIO的结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;
	//1.初始化GPIO时钟
	RCC_APB2PeriphClockCmd(OLED_SPI_GPIO_L, ENABLE);	//设置需要开启的APB2时钟;
	//2.1调用配置GPIO工作方式的库函数
	GPIO_InitStructure.GPIO_Pin = OLED_SCLK_PIN | OLED_SDO_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_CS_PIN;	//初始化所需引脚;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//设置GPIO的工作模式; 此处设置为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置GPIO的工作频率; 此处设置为50Mhz
	//2.2将配置好GPIO设置的结构体变量传入初始化函数
	GPIO_Init(OLED_SPI_GPIO_S, &GPIO_InitStructure);	//设置要配置的GPIO;
	
	GPIO_SetBits(OLED_SPI_GPIO_S, OLED_SCLK_PIN | OLED_SDO_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_CS_PIN);
	#endif

}
/****************************	函数效果: 开启GPIOC口的时钟, 使GPIOC.13引脚在推挽输出,50Mhz的模式下工作	***************************/
