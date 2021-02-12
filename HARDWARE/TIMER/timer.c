#include "timer.h"
#include "led.h"
#include "usart.h"
#include "global.h"

#define KEY1 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

extern keySta keySta1;
extern uint8_t cntKeyLow;

volatile u16 cntTimer = 0;

// 用于按键状态检测
void TIM2_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		// 时钟使能

	TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1; 			// 预分频值（即TIM2时钟为：72M/1440 = 50K）
	TIM_TimeBaseStructure.TIM_Period = 5000u - 1; 				// 自动重装载值	 计数到5000为 100ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 					// 使能指定的TIM2中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			// TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  	// 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  		// 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  		// 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	TIM_Cmd(TIM2, ENABLE);  		// 使能TIM2外设
}

// 定时器触发时间：100ms
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		cntTimer++;
		if(cntTimer >= 3 && KEY1 == 1)		// 距离第一次按下已经300ms, 按键已经释放，且定时器未停止 --> 单机按键
		{
			keySta1 = SingleClick;
			TIM_Cmd(TIM2, DISABLE);
			cntTimer = 0;
			cntKeyLow = 0;
		}
		if(cntTimer == 10 && KEY1 == 0)		// 按键事件超过 1s, 判定为长按
		{
			keySta1 = LongClick;
			TIM_Cmd(TIM2, DISABLE);
			cntTimer = 0;
			cntKeyLow = 0;
		}
	}
}

void TIM3_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		//时钟使能

	TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1; 				// 预分频值（即TIM3时钟为：72M/1440 = 50K）
	TIM_TimeBaseStructure.TIM_Period = 50000u - 1; 					// 自动重装载值	 计数到50000为 1s
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 					//使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  			// 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  	//先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  		//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  				//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
//	TIM_Cmd(TIM3, ENABLE);  				//使能TIM3外设
}

extern void DrawFont_GBK24B(u16 x,u16 y, u16 color, u8*str);
extern uint8_t motorStat;
extern uint16_t duty;
extern uint8_t timeClockRotate;
extern uint8_t timeAntiClockRotate;
uint8_t cntTimer3 = 0;

// 用于控制电机正反转时间
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		LED = ~LED;
		cntTimer3++;
		if(cntTimer3 == 1)		// 正转
		{
			TIM_SetCompare3(TIM5,0);
			TIM_SetCompare2(TIM5,duty);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"正转开始");
		}
		else if(cntTimer3 == timeClockRotate + 1)		// 中间暂停 1s
		{
			TIM_SetCompare3(TIM5,0);
			TIM_SetCompare2(TIM5,0);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"正转结束");
		}
		else if(cntTimer3 == timeClockRotate + 2)		// 电机停止后再反转
		{
			TIM_SetCompare2(TIM5,0);
			TIM_SetCompare3(TIM5,duty);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"反转开始");
		}
		else if(cntTimer3 == timeClockRotate + timeAntiClockRotate + 2)		// 反转
		{
			TIM_SetCompare2(TIM5,0);
			TIM_SetCompare3(TIM5,0);
			cntTimer3 = 0;
			DrawFont_GBK24B(0,216,0x0000,(u8*)"反转结束");
		}	
	}
}

// 用于产生 PWM 驱动电机
void TIM5_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	// 使能定时器5时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  	// 使能GPIO外设和AFIO复用功能模块时钟
 
	// 设置该引脚为复用输出功能,输出TIM5 CH2,CH3的PWM脉冲波形	PA1,PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  // TIM_CH2 & TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
 
	TIM_TimeBaseStructure.TIM_Period = arr; 						// 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 						// 预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	// 初始化 TIM5 Channel-2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		// 输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  						// 根据指定参数初始化外设TIM5 OC1
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  				// 使能TIM5在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_Cmd(TIM5, ENABLE);  // 使能TIM5
}
