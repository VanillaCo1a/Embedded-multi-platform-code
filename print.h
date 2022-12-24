#ifndef __PRINT_H
#define __PRINT_H
#include "device.h"

void PRINT_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize, char *buf, size_t bufSize);
void PRINT_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize);
void PRINT_DevInit(void);
bool PRINT_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *length, DEV_StatusTypeDef wait);
bool PRINT_ScanString(int8_t num, char *str, size_t size, DEV_StatusTypeDef wait);
bool PRINT_PrintArray(int8_t num, const uint8_t arr[], size_t size, DEV_StatusTypeDef wait);
bool PRINT_PrintString(int8_t num, const char *str, DEV_StatusTypeDef wait);
bool PRINT_Printf(int8_t num, char *str, DEV_StatusTypeDef wait, ...);

#endif
