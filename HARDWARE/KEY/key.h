#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)		// 读取按键K1

void KEY_Init(void);	// IO初始化
u8 KEY_Scan(u8);  		// 按键扫描函数					    
#endif
