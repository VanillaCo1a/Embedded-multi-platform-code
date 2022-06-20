/*
 * 本文件包含:
 * 1. OLED配置结构体初始化
 * 2. OLED的I2C/SPI通信IO初始化
 * 3. OLED的I2C/SPI通信驱动
 * 4. OLED配置初始化
 */
#include "oled_driver.h"

/////////////////////////    OLED配置结构体初始化    /////////////////////////
//    OLED设备结构体
DEVS_TypeDef oleds = {.type = OLED, .size = OLED_NUM, .pool = (devpool_size)-1, .stream = 0};
//    OLED参数配置
OLED_TypeDef oled_parameter[OLED_NUM] = {
    {.oledchip = OLED_SH1106},
};
//    OLED通信配置
DEVCMNI_TypeDef oled_cmni[OLED_NUM] = {
    {.type = SPI, .dev = HARDWARE, .handle = {.hai2c = {OLED_I2CADDR1, &ai2c}, .haspi = {&aspi}, .hi2c = NULL, .hspi = &hspi2}},
};
//    OLEDIO配置
OLED_IOTypeDef oled_io[OLED_NUM] = {
#if MCU_COMPILER == MCU_STM32HAL
    {{.SCL_SCLK = {OLED0_SCL_SCLK_GPIO_Port, OLED0_SCL_SCLK_Pin},
      .SDA_SDO_OWIO = {OLED0_SDA_SDO_GPIO_Port, OLED0_SDA_SDO_Pin},
      .CS = {OLED0_CS_GPIO_Port, OLED0_CS_Pin}},
     .DC = {OLED0_DC_GPIO_Port, OLED0_DC_Pin},
     .RST = {NULL, 0x00}},
// {{.SCL_SCLK = {OLED1_SCL_SCLK_GPIO_Port, OLED1_SCL_SCLK_Pin},
//   .SDA_SDO_OWIO = {OLED1_SDA_SDO_GPIO_Port, OLED1_SDA_SDO_Pin},
//   .CS = {NULL, 0x00}},
//   .DC = {NULL, 0x00},
//  .RST = {NULL, 0x00}},
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    {{.SCL_SCLK = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_12},
      .SDA_SDO_OWIO = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_13},
      .CS = {0x00, NULL, 0x00}},
     .DC = {0x00, NULL, 0x00},
     .RST = {0x00, NULL, 0x00}},
#endif
};


/////////////////////////    OLED的I2C/SPI通信IO初始化    /////////////////////////
//可复用于I2C和SPI通信的OLED引脚定义(*为必须接IO由芯片控制): SCL_SCLK*,SDA_SDO_OWIO*,CS,DC(*SPI),RST
//I2C/SPI通信的基础通信引脚(在device.c中初始化): SCL*,SDA*/SCLK*,SDO*,CS
//OLED的其他控制引脚,非必须引脚可据需择芯片控制/电路控制('为不建议的接线方法): CS(I2C),DC(*SPI),RST
//[I2C]CS:片选(接IO/接GND), DC:I2C地址选择(接IO/接VCCGND), RST:复位(接IO/接主控芯片RST/接VCC')
//[SPI]DC*:命令/数据选择(接IO), RST:复位(接IO/接主控芯片RST/接VCC')
void OLED_IOInit(void) {
    DEV_TypeDef *oled = DEV_getdev(&oleds);
    OLED_IOTypeDef *oledio = (OLED_IOTypeDef *)oled->io;
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
#if MCU_COMPILER == MCU_STM32HAL
    //HAL库的IO初始化在main函数中完成
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    GPIO_InitTypeDef GPIO_InitStructure;
    if(oled->cmni->type == I2C) {
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化CS
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
        if(oledio->DC.GPIOx != NULL) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化DC
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&oledio->DC);
        }
    } else if(oled->cmni->type == SPI) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化DC
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        DEV_CLK_GPIO_PIN_CONFI(&oledio->DC);
    }
    if(oledio->RST.GPIOx != NULL) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化RST
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        DEV_CLK_GPIO_PIN_CONFI(&oledio->RST);
    }
#endif
    //IO电位配置
    if(oled->cmni->type == I2C) {
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
        if(oledio->DC.GPIOx != NULL) {
            if(oled->cmni->handle.hai2c.addr == OLED_I2CADDR1) {
                DEVIO_WritePin_RESET(&oledio->DC);
            } else {
                DEVIO_WritePin_SET(&oledio->DC);
            }
        }
    } else if(oled->cmni->type == SPI) {
        DEVIO_WritePin_SET(&oledio->DC);
    }
    if(oledio->RST.GPIOx != NULL) {
        DEVIO_WritePin_RESET(&oledio->RST);
    }
}


/////////////////////////   OLED的 I2C/SPI通信驱动    /////////////////////////
//[四脚OLED]只能进行I2C通信, 没有其他控制引脚
//[七脚OLED][I2C]CS:片选,默认置高,通信时置低, DC:I2C地址选择,在通信时根据当前OLEDI2C地址配置电位
//[七脚OLED][SPI]DC*:命令/数据选择,在通信时根据写往OLED内命令/数据寄存器配置电位
//    OLED模拟通信写字节函数
//对于不同的通信方式的OLED, 区别主要在于此处的IO操作, 上层的操作基本相同
void OLED_WriteByte(uint8_t data, uint8_t address) {
    DEV_TypeDef *oled = DEV_getdev(&oleds);
    OLED_IOTypeDef *oledio = (OLED_IOTypeDef *)oled->io;
    if(oled->cmni->type == I2C) {
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
        if(oled->cmni->handle.hai2c.addr == OLED_I2CADDR1) {
            DEVIO_WritePin_RESET(&oledio->DC);
        } else if(oled->cmni->handle.hai2c.addr == OLED_I2CADDR2) {
            DEVIO_WritePin_SET(&oledio->DC);
        }
        DEVCMNI_WriteByte(data, address);
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
    } else if(oled->cmni->type == SPI) {
        if(address == 0X00) {
            DEVIO_WritePin_RESET(&oledio->DC);
        } else if(address == 0X40) {
            DEVIO_WritePin_SET(&oledio->DC);
        }
        DEVCMNI_WriteByte(data, address);
        DEVIO_WritePin_SET(&oledio->DC);
    }
}
//    OLED模拟通信连续写多字节函数
void OLED_WriteConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    DEV_TypeDef *oled = DEV_getdev(&oleds);
    OLED_IOTypeDef *oledio = (OLED_IOTypeDef *)oled->io;
    if(oled->cmni->type == I2C) {
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
        if(oled->cmni->handle.hai2c.addr == OLED_I2CADDR1) {
            DEVIO_WritePin_RESET(&oledio->DC);
        } else {
            DEVIO_WritePin_SET(&oledio->DC);
        }
        DEVCMNI_WriteConti(pdata, size, address);
        if(((DEVCMNI_IOTypeDef *)oledio)->CS.GPIOx != NULL) {
            DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)oledio)->CS);
        }
    } else if(oled->cmni->type == SPI) {
        if(address == 0X00) {
            DEVIO_WritePin_RESET(&oledio->DC);
        } else if(address == 0X40) {
            DEVIO_WritePin_SET(&oledio->DC);
        }
        DEVCMNI_WriteConti(pdata, size, address);
        DEVIO_WritePin_SET(&oledio->DC);
    }
}

/////////////////////////    OLED配置初始化    /////////////////////////
//    图形库普通的延时函数 需要用户自己配置
void OLED_delayms(uint16_t ms) {
#if MCU_COMPILER == MCU_STM32HAL
#ifdef __TIMER_H
    delayms_timer(ms);
#else
    HAL_Delay(ms);
#endif
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    delayms_timer(ms);
#endif
}
//    OLED的驱动函数, 将前面的配置IO口,通信等操作封装集成为OLED驱动, 供外部调用
void OLED_Reset(void) {
    if(((OLED_IOTypeDef *)DEV_getdev(&oleds)->io)->RST.GPIOx != 0x00) {
        DEVIO_WritePin_RESET(&((OLED_IOTypeDef *)DEV_getdev(&oleds)->io)->RST);
    }
    OLED_delayms(200);
    if(((OLED_IOTypeDef *)DEV_getdev(&oleds)->io)->RST.GPIOx != 0x00) {
        DEVIO_WritePin_SET(&((OLED_IOTypeDef *)DEV_getdev(&oleds)->io)->RST);
    }
}
void OLED_On(void) {               //OLED唤醒
    OLED_WriteByte(0X8D, 0X00);    //设置电荷泵
    OLED_WriteByte(0X14, 0X00);    //开启电荷泵
    OLED_WriteByte(0XAF, 0X00);    //开启OLED显示
}
void OLED_Off(void) {              //OLED休眠
    OLED_WriteByte(0XAE, 0X00);    //关闭OLED显示
    OLED_WriteByte(0X8D, 0X00);    //设置电荷泵
    OLED_WriteByte(0X10, 0X00);    //关闭电荷泵
}
void OLED_Flip(int8_t horizontal, int8_t vertical) {
    if(horizontal == 0) {
        OLED_WriteByte(0xA1, 0X00);
    } else {
        OLED_WriteByte(0xA0, 0X00);
    }
    if(vertical == 0) {
        OLED_WriteByte(0xC8, 0X00);
    } else {
        OLED_WriteByte(0xC0, 0X00);
    }
}
//    设置器件内部光标, 格式为(页,列)
void OLED_Cursor(uint8_t page, uint8_t col) {
    OLED_WriteByte(0xB0 + page, 0X00);    //设置显示起始页地址（0~7）
    if(((OLED_TypeDef *)DEV_getdev(&oleds)->parameter)->oledchip == OLED_SSD1306) {
        OLED_WriteByte(0x00 + (col & 0x0F), 0X00);    //设置显示起始列地址低4位
    } else if(((OLED_TypeDef *)DEV_getdev(&oleds)->parameter)->oledchip == OLED_SH1106) {
        OLED_WriteByte(0x02 + (col & 0x0F), 0X00);    //设置显示起始列地址低4位
    }
    OLED_WriteByte(0x10 + ((col >> 4) & 0x0F), 0X00);    //设置显示起始列地址高4位
}
//    在指定坐标处直接刷新一个字节(页中的某一列,上低下高), 由于要重设光标,非常地缓慢
void OLED_directByte(uint8_t page, uint8_t col, uint8_t data) {
    OLED_Cursor(page, col);
    OLED_WriteByte(data, 0X40);
}
//    使用I2C/SPI从缓存区读取对应位置数据, 用连续页写入的方式刷新整个屏幕
void OLED_FillScreen(uint8_t Buffer[]) {
    int8_t i = 0;
    uint8_t *pdata = Buffer;
    for(i = 0; i < SCREEN_PAGE; i++) {
        OLED_Cursor(i, 0);
        OLED_WriteConti(pdata, SCREEN_COLUMN, 0X40);
        pdata += SCREEN_COLUMN;
    }
}
//    清屏(先清空缓存区, 再更新到屏幕)
void OLED_ClearScreen(void) {
    OLED_clearBuffer();
    OLED_updateScreen(DEV_getistm(&oleds));
}
//    配置器件内部寄存器  initial settings configuration
void OLED_Init(void) {
    //1. 基本命令
    OLED_WriteByte(0xAE, 0X00);    //设置显示关(默认)/开: 0xAE显示关闭(睡眠模式),0xAF显示正常开启  Set Display OFF(RESET)/ON: 0xAE Display OFF(sleep mode),0xAF Display ON in normal mode
    OLED_WriteByte(0xA4, 0X00);    //设置从内存(默认)/完全显示: 0xA4从内存中显示,0xA5完全显示  Entire Display OFF(RESET)/ON: 0xA4 Output follows RAM content,0xA5 Output ignores RAM content
    OLED_WriteByte(0xA6, 0X00);    //设置正常(默认)/反向显示: 0xA6内存中0关闭显示1开启显示,0xA7内存中1关闭显示0开启显示  Set Normal/Inverse Display: 0xA6 Normal display,0xA7 Inverse display
    OLED_WriteByte(0x81, 0X00);    //设置对比度(默认0x7F)  Set Contrast Control
    OLED_WriteByte(0xFF, 0X00);    //对比度范围为0~255(0x00~0xFF)
    //2. 滚动命令
    //...
    //3. 寻址设置命令
    OLED_WriteByte(0x20, 0X00);    //设置内存寻址模式(默认0x10) Set Memory Addressing Mode
    OLED_WriteByte(0x02, 0X00);    //0x00水平寻址模式,0x01垂直寻址模式,0x02页面寻址模式(默认),其他无效  0x00 Horizontal Addressing Mode,0x01 Vertical Addressing Mode,0x02 Page Addressing Mode,others Invalid
    OLED_WriteByte(0xB0, 0X00);    //设置页起始地址(0xB0~0xB7,仅页寻址模式有效)  Set Page Start Address for Page Addressing Mode
    if(((OLED_TypeDef *)DEV_getdev(&oleds)->parameter)->oledchip == OLED_SSD1306) {
        OLED_WriteByte(0x00, 0X00);    //设置显示起始列地址低4位
    } else if(((OLED_TypeDef *)DEV_getdev(&oleds)->parameter)->oledchip == OLED_SH1106) {
        OLED_WriteByte(0x02, 0X00);    //设置显示起始列地址低4位
    }
    OLED_WriteByte(0x10, 0X00);    //设置列起始地址高4位(默认0x10,0x10~0x1F,仅页寻址模式有效)  Set Higher Column Start Address for Page Addressing Mode
                                   //命令0x21,0x22仅水平寻址和垂直寻址模式有效
    //4. 硬件配置命令(面板分辨率和布局相关)
    OLED_WriteByte(0x40, 0X00);    //设置显示起始行地址(默认0x40,0x40~0x7F)  Set Display Start Line ADDRESS
    OLED_WriteByte(0xA8, 0X00);    //设置复用比(默认0x3F)  Set Multiplex Ratio
    OLED_WriteByte(0x3F, 0X00);    //复用比的范围为16MUX~64MUX,等于设置的数值+1(0x0F~0x3F)
    OLED_WriteByte(0xD3, 0X00);    //设置显示偏移  Set Display Offset
    OLED_WriteByte(0x00, 0X00);    //通过COM设置从0d到63d的垂直位移(默认0x00)
    OLED_WriteByte(0xDA, 0X00);    //设置COM引脚硬件配置  Set COM Pins Hardware Configuration
    OLED_WriteByte(0x12, 0X00);    //第5位0/1: 禁用(默认)/启用COM左右重新映射, 第4位0/1: 顺序COM引脚/可选COM引脚配置(默认)
    OLED_WriteByte(0xA1, 0X00);    //(画面水平翻转控制,0xA1正常0xA0翻转)   设置段重映射: 0xA0列地址从0映射至SEG0,0xA1列地址从127映射至SEG0  Set Segment Re-map
    OLED_WriteByte(0xC8, 0X00);    //(画面垂直翻转控制,0xC8正常0xC0翻转)   设置COM输出扫描方向: 0xC0普通模式,从COM0扫描至COM[N–1]; 0xC8重映射模式,从COM[N-1]扫描到COM0(其中N为复用比)  Set COM Output Scan Direction
    //5. 定时驱动方案设置命令
    OLED_WriteByte(0xD5, 0X00);    //设置振荡器频率(默认0x8)和显示时钟分频比(默认0x0,即分频比为1)  Set Oscillator Frequency & Display Clock Divide Ratio
    OLED_WriteByte(0xF0, 0X00);    //高4位: 设置晶振频率,FOSC频率随高4位数值增减而增减(0x0~0xF); 低4位: 设置DCLK的分频比,分频比=高4位数值+1设置Pre-charge间隔
    OLED_WriteByte(0xD9, 0X00);    //设置Pre-charge间隔(默认0x22)  Set Pre-charge Period
    OLED_WriteByte(0xF1, 0X00);    //高4位： 第2阶段间隔(0x1~0xF), 高4位： 第1阶段间隔时钟周期(0x1~0xF)
    OLED_WriteByte(0xDB, 0X00);    //Set VCOMH(默认0x20)
    OLED_WriteByte(0x30, 0X00);    //0x00 0.65xVcc, 0x20 0.77xVCC, 0x30 0.83xVCC
}
void OLED_Confi(void) {
    //初始化OLED类设备, 将参数绑定到设备池中, 并初始化通信引脚
    DEV_Confi(&oleds, oled_parameter, oled_io, oled_cmni);
    //初始化其他引脚, 复位定义RST脚的OLED
    DEV_Do(&oleds, OLED_IOInit);
    DEV_Do(&oleds, OLED_Reset);    //延时200ms等待OLED电源稳定
    //对选中的OLED进行内部寄存器配置和清屏
    if(DEV_setistm(&oleds, 0) == 1) {    //配置第1个OLED
        DEV_Error();
    }
    OLED_Init();
    OLED_Cursor(0, 0);    //设置光标和屏幕方向, 在初始化函数中已经配置过, 单独分离出来方便修改
    OLED_Flip(0, 0);
    OLED_ClearScreen();    //清空显存和屏幕
    OLED_On();             //开启屏幕显示
    //...
}
