#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "global.h"

keySta keySta1 = NoKeyClick;
uint8_t cntKeyLow = 0;
extern volatile u16 cntTimer;
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    KEY_Init();	 //	按键端口初始化

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟


	// PA0 中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		// 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				// 使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	// 抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;			// 子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

// 外部中断0服务程序 
void EXTI0_IRQHandler(void)  
{
	delay_ms(20);	// 消抖
	if(KEY1 == 0 && cntKeyLow == 0)	 	// 按键第一次按下
	{			
		TIM_Cmd(TIM2, ENABLE);  		//使能TIM2外设
		cntKeyLow++;
	}
	else if(KEY1 == 0 && cntKeyLow == 1 && cntTimer < 3)	// 按键在300ms内第二次按下 --> 按键双击
	{
		TIM_Cmd(TIM2, DISABLE);
		cntTimer = 0;
		cntKeyLow = 0;
		keySta1 = DoubleClick;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0); 	// 清除LINE0上的中断标志位  
}
