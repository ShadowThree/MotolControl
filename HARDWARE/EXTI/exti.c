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
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    KEY_Init();	 //	�����˿ڳ�ʼ��

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��


	// PA0 �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		// ����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				// ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	// ��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;			// �����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}

// �ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)  
{
	delay_ms(20);	// ����
	if(KEY1 == 0 && cntKeyLow == 0)	 	// ������һ�ΰ���
	{			
		TIM_Cmd(TIM2, ENABLE);  		//ʹ��TIM2����
		cntKeyLow++;
	}
	else if(KEY1 == 0 && cntKeyLow == 1 && cntTimer < 3)	// ������300ms�ڵڶ��ΰ��� --> ����˫��
	{
		TIM_Cmd(TIM2, DISABLE);
		cntTimer = 0;
		cntKeyLow = 0;
		keySta1 = DoubleClick;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0); 	// ���LINE0�ϵ��жϱ�־λ  
}
