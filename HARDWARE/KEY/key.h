#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)		// ��ȡ����K1

void KEY_Init(void);	// IO��ʼ��
u8 KEY_Scan(u8);  		// ����ɨ�躯��					    
#endif
