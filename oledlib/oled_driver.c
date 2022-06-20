#include "oled_driver.h"
/***    本文件包含: 
                OLED引脚配置
                通信对象配置
                底层驱动函数            ***/
        
////////////////////////////////////////////////////////////////////////////
//OLED引脚配置

void OLED_GPIO_Init(void) {
    #if (OLED_MODE==MODE_I2C || OLED_MODE==MODE_I2C_7PIN)
    GPIO_InitTypeDef GPIO_InitStructure;                        //定义一个配置GPIO的结构体变量
    RCC_APB2PeriphClockCmd(OLED_SCL_CLK|OLED_SDA_CLK, ENABLE);  //初始化GPIO时钟
    GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_SCL_GPIO, &GPIO_InitStructure);              //调用配置GPIO工作方式的库函数
    GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_SDA_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OLED_SCL_GPIO, OLED_SCL_PIN);
    GPIO_SetBits(OLED_SDA_GPIO, OLED_SDA_PIN);
    #if (OLED_MODE == MODE_I2C_7PIN)
    //七脚oled使用I2C时还需复位RST(维持低电平200ms), 拉低CS, DC用于控制I2C地址
        #ifdef OLED_DC_CLK
    RCC_APB2PeriphClockCmd(OLED_DC_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_DC_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(OLED_DC_GPIO, OLED_DC_PIN);
        #endif
        #ifdef OLED_CS_CLK
    RCC_APB2PeriphClockCmd(OLED_CS_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_CS_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(OLED_CS_GPIO, OLED_CS_PIN);
        #endif
        #ifdef OLED_RST_CLK
    RCC_APB2PeriphClockCmd(OLED_RST_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_RST_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(OLED_RST_GPIO, OLED_RST_PIN);
        #endif
    #endif
    #else
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(OLED_SCLK_CLK|OLED_SDO_CLK|OLED_DC_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_SCLK_GPIO, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = OLED_SDO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_SDO_GPIO, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = OLED_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_DC_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OLED_SCLK_GPIO, OLED_SCLK_PIN);
    GPIO_SetBits(OLED_SDO_GPIO, OLED_SDO_PIN);
    GPIO_SetBits(OLED_DC_GPIO, OLED_DC_PIN);
        #ifdef OLED_RST_CLK
    RCC_APB2PeriphClockCmd(OLED_RST_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_RST_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(OLED_RST_GPIO, OLED_RST_PIN);
        #endif
        #ifdef OLED_CS_CLK
    RCC_APB2PeriphClockCmd(OLED_CS_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_CS_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OLED_CS_GPIO, OLED_CS_PIN);
        #endif
    #endif
}


////////////////////////////////////////////////////////////////////////////
//通信对象配置

#if (OLED_MODE==MODE_I2C || OLED_MODE==MODE_I2C_7PIN)
I2c OLED;
void OLED_SCLSet(int8_t dir) {}
void OLED_SDASet(int8_t dir) {
    if(dir == IN) {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(OLED_SDA_GPIO, &GPIO_InitStructure);
    }else {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(OLED_SDA_GPIO, &GPIO_InitStructure);
    }
}
void OLED_SCLOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_SetBits(OLED_SCL_GPIO, OLED_SCL_PIN);
    }else {
        GPIO_ResetBits(OLED_SCL_GPIO, OLED_SCL_PIN);
    }
}
void OLED_SDAOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_SetBits(OLED_SDA_GPIO, OLED_SDA_PIN);
    }else {
        GPIO_ResetBits(OLED_SDA_GPIO, OLED_SDA_PIN);
    }
}
uint8_t OLED_SDAIn(void) {
    return GPIO_ReadInputDataBit(OLED_SDA_GPIO, OLED_SDA_PIN);
}
void OLED_Delayus(int16_t nus) {
    Delay_us(nus);
}

void OLED_Wire_Init(void) {
    OLED.ADDRESS = OLED_ADDRESS;
    OLED.i2cSCLSet = OLED_SCLSet;
    OLED.i2cSDASet = OLED_SDASet;
    OLED.i2cSCLOut = OLED_SCLOut;
    OLED.i2cSDAOut = OLED_SDAOut;
    OLED.i2cSDAIn = OLED_SDAIn;
    OLED.delayus = OLED_Delayus;
    OLED.i2cSCLOut(HIGH);
    OLED.i2cSDAOut(HIGH);
}
#else
Spi OLED;
void OLED_CSOut(uint8_t bit) {
    #ifdef OLED_CS_CLK
    if(bit == HIGH) {
        GPIO_SetBits(OLED_CS_GPIO, OLED_CS_PIN);
    }else {
        GPIO_ResetBits(OLED_CS_GPIO, OLED_CS_PIN);
    }
    #endif
}
void OLED_SCLKOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_SetBits(OLED_SCLK_GPIO, OLED_SCLK_PIN);
    }else {
        GPIO_ResetBits(OLED_SCLK_GPIO, OLED_SCLK_PIN);
    }
}
void OLED_SDOOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_SetBits(OLED_SDO_GPIO, OLED_SDO_PIN);
    }else {
        GPIO_ResetBits(OLED_SDO_GPIO, OLED_SDO_PIN);
    }
}
void OLED_Delayus(int16_t nus) {
    Delay_us(nus);
}

void OLED_Wire_Init(void) {
    OLED.spiCSOut = OLED_CSOut;
    OLED.spiSCLKOut = OLED_SCLKOut;
    OLED.spiSDOOut = OLED_SDOOut;
    OLED.delayus = OLED_Delayus;
    OLED.spiSCLKOut(HIGH);
    OLED.spiSDOOut(HIGH);
}
#endif
void OLED_WriteByte(uint8_t data, uint8_t address) {
    #if (OLED_MODE==MODE_I2C || OLED_MODE==MODE_I2C_7PIN)
    MODULAR_I2CWriteByte(OLED, I2CHIGH, 0, data, address);
    #else
    if(address == 0X00) {
        GPIO_ResetBits(OLED_DC_GPIO, OLED_DC_PIN);
    }else {
        GPIO_SetBits(OLED_DC_GPIO, OLED_DC_PIN);
    }
    MODULAR_SPIWriteByte(OLED, data);
    GPIO_SetBits(OLED_DC_GPIO, OLED_DC_PIN);
    #endif
}
////////////////////////////////////////////////////////////////////////////
//底层驱动函数

void OLED_Reset(void) {
    #ifdef OLED_RST_GPIO
    GPIO_ResetBits(OLED_RST_GPIO, OLED_RST_PIN);
    #endif
    OLED_Delayms(200);
    #ifdef OLED_RST_GPIO
    GPIO_SetBits(OLED_RST_GPIO, OLED_RST_PIN);
    #endif
}
void OLED_On(void) {
    OLED_WriteByte(0X8D, 0X00);     //设置电荷泵
    OLED_WriteByte(0X14, 0X00);     //开启电荷泵
    OLED_WriteByte(0XAF, 0X00);     //开启OLED显示
}
void OLED_Off(void) {
    OLED_WriteByte(0XAE, 0X00);     //关闭OLED显示
    OLED_WriteByte(0X8D, 0X00);     //设置电荷泵
    OLED_WriteByte(0X10, 0X00);     //关闭电荷泵
}
void OLED_SetCursor(uint8_t page, uint8_t col) {
    OLED_WriteByte(0xB0+page, 0X00);            //设置显示起始页地址（0~7）
    #if (OLED_CHIP == SSD1306)
    OLED_WriteByte(0x00+(col&0x0F), 0X00);      //设置显示起始列地址低4位
    #elif (OLED_CHIP == SH1106)
    OLED_WriteByte(0x02+(col&0x0F), 0X00);      //设置显示起始列地址低4位
    #endif
    OLED_WriteByte(0x10+((col>>4)&0x0F), 0X00); //设置显示起始列地址高4位
}
void OLED_Clear(void) {     //清屏 全部发送0x00
    uint8_t i = 0, j = 0;
    for(i=0; i<8; i++) {
        OLED_SetCursor(i, 0);
        for(j=0; j<128; j++) {
            OLED_WriteByte(0x00, 0X40);
        }
    }
}
void OLED_Fill(uint8_t Buffer[]) {
    uint8_t i = 0, j = 0;
    uint8_t *p = Buffer;
    for(i=0; i<8; i++) {
        OLED_SetCursor(i, 0);
        for(j=0; j<128; j++) {
            OLED_WriteByte(*p++, 0X40);
        }
    }
}
void OLED_Init(void) {      //初始化  initial settings configuration
    //1. 基本命令
    OLED_WriteByte(0xAE, 0X00);     //设置显示关(默认)/开: 0xAE显示关闭(睡眠模式),0xAF显示正常开启  Set Display OFF(RESET)/ON: 0xAE Display OFF(sleep mode),0xAF Display ON in normal mode
    OLED_WriteByte(0xA4, 0X00);     //设置从内存(默认)/完全显示: 0xA4从内存中显示,0xA5完全显示  Entire Display OFF(RESET)/ON: 0xA4 Output follows RAM content,0xA5 Output ignores RAM content
    OLED_WriteByte(0xA6, 0X00);     //设置正常(默认)/反向显示: 0xA6内存中0关闭显示1开启显示,0xA7内存中1关闭显示0开启显示  Set Normal/Inverse Display: 0xA6 Normal display,0xA7 Inverse display
    OLED_WriteByte(0x81, 0X00);     //设置对比度(默认0x7F)  Set Contrast Control
    OLED_WriteByte(0xFF, 0X00);     //对比度范围为0~255(0x00~0xFF)
    //2. 滚动命令
    //3. 寻址设置命令
    OLED_WriteByte(0x20, 0X00);     //设置内存寻址模式(默认0x10) Set Memory Addressing Mode
        OLED_WriteByte(0x02, 0X00); //0x00水平寻址模式,0x01垂直寻址模式,0x02页面寻址模式(默认),其他无效  0x00 Horizontal Addressing Mode,0x01 Vertical Addressing Mode,0x02 Page Addressing Mode,others Invalid
    OLED_WriteByte(0xB0, 0X00);     //设置页起始地址(0xB0~0xB7,仅页寻址模式有效)  Set Page Start Address for Page Addressing Mode
    #if (OLED_CHIP == SSD1306)
    OLED_WriteByte(0x00, 0X00);     //设置列起始地址低4位(默认0x00,0x00~0x0F,仅页寻址模式有效)  Set Lower Column Start Address for Page Addressing Mode
    #elif (OLED_CHIP == SH1106)
    OLED_WriteByte(0x02, 0X00);     //设置列起始地址低4位(默认0x00,0x00~0x0F,仅页寻址模式有效)  Set Lower Column Start Address for Page Addressing Mode
    #endif
    OLED_WriteByte(0x10, 0X00);     //设置列起始地址高4位(默认0x10,0x10~0x1F,仅页寻址模式有效)  Set Higher Column Start Address for Page Addressing Mode
                                    //命令0x21,0x22仅水平寻址和垂直寻址模式有效
    //4. 硬件配置命令(面板分辨率和布局相关)
    OLED_WriteByte(0x40, 0X00);     //设置显示起始行地址(默认0x40,0x40~0x7F)  Set Display Start Line ADDRESS
    OLED_WriteByte(0xA8, 0X00);     //设置复用比(默认0x3F)  Set Multiplex Ratio
        OLED_WriteByte(0x3F, 0X00); //复用比的范围为16MUX~64MUX,等于设置的数值+1(0x0F~0x3F)
    OLED_WriteByte(0xD3, 0X00);     //设置显示偏移  Set Display Offset
        OLED_WriteByte(0x00, 0X00); //通过COM设置从0d到63d的垂直位移(默认0x00)
    OLED_WriteByte(0xDA, 0X00);     //设置COM引脚硬件配置  Set COM Pins Hardware Configuration
        OLED_WriteByte(0x12, 0X00); //第5位0/1: 禁用(默认)/启用COM左右重新映射, 第4位0/1: 顺序COM引脚/可选COM引脚配置(默认)
    OLED_WriteByte(0xA1, 0X00);     //(画面水平翻转控制,0xA1正常0xA0翻转)   设置段重映射: 0xA0列地址从0映射至SEG0,0xA1列地址从127映射至SEG0  Set Segment Re-map
    OLED_WriteByte(0xC8, 0X00);     //(画面垂直翻转控制,0xC8正常0xC0翻转)   设置COM输出扫描方向: 0xC0普通模式,从COM0扫描至COM[N–1]; 0xC8重映射模式,从COM[N-1]扫描到COM0(其中N为复用比)  Set COM Output Scan Direction
    //5. 定时驱动方案设置命令
    OLED_WriteByte(0xD5, 0X00);     //设置振荡器频率(默认0x8)和显示时钟分频比(默认0x0,即分频比为1)  Set Oscillator Frequency & Display Clock Divide Ratio
        OLED_WriteByte(0xF0, 0X00); //高4位: 设置晶振频率,FOSC频率随高4位数值增减而增减(0x0~0xF); 低4位: 设置DCLK的分频比,分频比=高4位数值+1设置Pre-charge间隔
    OLED_WriteByte(0xD9, 0X00);     //设置Pre-charge间隔(默认0x22)  Set Pre-charge Period
        OLED_WriteByte(0xF1, 0X00); //高4位： 第2阶段间隔(0x1~0xF), 高4位： 第1阶段间隔时钟周期(0x1~0xF)
    OLED_WriteByte(0xDB, 0X00);     //Set VCOMH(默认0x20)
        OLED_WriteByte(0x30, 0X00); //0x00 0.65xVcc, 0x20 0.77xVCC, 0x30 0.83xVCC
        
    OLED_WriteByte(0x8D, 0X00);     //设置电荷泵  Set Charge Pump ENABLE/DISABLE
        OLED_WriteByte(0x14, 0X00); //使能电荷泵
    OLED_WriteByte(0xAF, 0X00);     //设置显示关(默认)/开: 0xAE显示关闭(睡眠模式),0xAF显示正常开启  Set Display OFF(RESET)/ON: 0xAE Display OFF(sleep mode),0xAF Display ON in normal mode
}
void OLED_Confi(void) {     //初始化引脚, 配置oled并清屏
    OLED_GPIO_Init();
    OLED_Wire_Init();
    OLED_Reset();                   //延时200ms等待OLED电源稳定
    OLED_Init();
    OLED_Clear();
}
void OLED_Delayms(uint16_t ms) {    //图形库普通的延时函数 需要用户自己配置
    delay_ms(ms);
}
