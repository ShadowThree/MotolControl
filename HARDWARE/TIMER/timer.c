#include "timer.h"
#include "led.h"
#include "usart.h"
#include "global.h"

#define KEY1 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

extern keySta keySta1;
extern uint8_t cntKeyLow;

volatile u16 cntTimer = 0;

// ���ڰ���״̬���
void TIM2_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		// ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1; 			// Ԥ��Ƶֵ����TIM2ʱ��Ϊ��72M/1440 = 50K��
	TIM_TimeBaseStructure.TIM_Period = 5000u - 1; 				// �Զ���װ��ֵ	 ������5000Ϊ 100ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 					// ʹ��ָ����TIM2�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			// TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  	// ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  		// �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  		// ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

//	TIM_Cmd(TIM2, ENABLE);  		// ʹ��TIM2����
}

// ��ʱ������ʱ�䣺100ms
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		cntTimer++;
		if(cntTimer >= 3 && KEY1 == 1)		// �����һ�ΰ����Ѿ�300ms, �����Ѿ��ͷţ��Ҷ�ʱ��δֹͣ --> ��������
		{
			keySta1 = SingleClick;
			TIM_Cmd(TIM2, DISABLE);
			cntTimer = 0;
			cntKeyLow = 0;
		}
		if(cntTimer == 10 && KEY1 == 0)		// �����¼����� 1s, �ж�Ϊ����
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		//ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1; 				// Ԥ��Ƶֵ����TIM3ʱ��Ϊ��72M/1440 = 50K��
	TIM_TimeBaseStructure.TIM_Period = 50000u - 1; 					// �Զ���װ��ֵ	 ������50000Ϊ 1s
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 					//ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  			// �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  	//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  		//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  				//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

//	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
//	TIM_Cmd(TIM3, ENABLE);  				//ʹ��TIM3����
}

extern void DrawFont_GBK24B(u16 x,u16 y, u16 color, u8*str);
extern uint8_t motorStat;
extern uint16_t duty;
extern uint8_t timeClockRotate;
extern uint8_t timeAntiClockRotate;
uint8_t cntTimer3 = 0;

// ���ڿ��Ƶ������תʱ��
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		LED = ~LED;
		cntTimer3++;
		if(cntTimer3 == 1)		// ��ת
		{
			TIM_SetCompare3(TIM5,0);
			TIM_SetCompare2(TIM5,duty);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"��ת��ʼ");
		}
		else if(cntTimer3 == timeClockRotate + 1)		// �м���ͣ 1s
		{
			TIM_SetCompare3(TIM5,0);
			TIM_SetCompare2(TIM5,0);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"��ת����");
		}
		else if(cntTimer3 == timeClockRotate + 2)		// ���ֹͣ���ٷ�ת
		{
			TIM_SetCompare2(TIM5,0);
			TIM_SetCompare3(TIM5,duty);
			DrawFont_GBK24B(0,216,0x0000,(u8*)"��ת��ʼ");
		}
		else if(cntTimer3 == timeClockRotate + timeAntiClockRotate + 2)		// ��ת
		{
			TIM_SetCompare2(TIM5,0);
			TIM_SetCompare3(TIM5,0);
			cntTimer3 = 0;
			DrawFont_GBK24B(0,216,0x0000,(u8*)"��ת����");
		}	
	}
}

// ���ڲ��� PWM �������
void TIM5_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	// ʹ�ܶ�ʱ��5ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  	// ʹ��GPIO�����AFIO���ù���ģ��ʱ��
 
	// ���ø�����Ϊ�����������,���TIM5 CH2,CH3��PWM���岨��	PA1,PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  // TIM_CH2 & TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
 
	TIM_TimeBaseStructure.TIM_Period = arr; 						// �Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 						// Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	// ��ʼ�� TIM5 Channel-2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		// �������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  						// ����ָ��������ʼ������TIM5 OC1
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  				// ʹ��TIM5��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_Cmd(TIM5, ENABLE);  // ʹ��TIM5
}
