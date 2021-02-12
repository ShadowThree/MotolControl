#include "main.h"

// PCR���������ַ(��ַ����Ϊż��)
#define ADDR_timeClockRotate  		0x08030000
#define ADDR_timeAntiClockRotate  0x08030004

uint8_t motorStat = 0;
uint16_t duty = 0;
extern uint8_t cntTimer3;

int main(void)
{
	myInit();		// һЩģ���IO�ڳ�ʼ��
	while(1)
	{		
		if(keySta1 != NoKeyClick)
		{
			if(keySta1 == SingleClick)
			{
				if(++motorStat == 4)
					motorStat = 0;		// �ı�״̬
				if(motorStat == 1)		// ����
				{
					duty = 1400;		// ͨ���ı� PWM ռ�ձȿ��Ƶ��ת��
					DrawFont_GBK32B(36,136,BLACK,(u8*)"ת��: ����");
					TIM_Cmd(TIM3, ENABLE);
				}			
				else if(motorStat == 2)
				{
					duty = 1550;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"ת��: ����");
				}
				else if(motorStat == 3)
				{
					duty = 1700;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"ת��: ����");
				}
				else
				{
					TIM_SetCompare3(TIM5,0);
					TIM_SetCompare2(TIM5,0);
					TIM_Cmd(TIM3, DISABLE);
					cntTimer3 = 0;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"ת��: ֹͣ");
					DrawFont_GBK24B(0,216,0x0000,(u8*)"        ");
				}
		
				printf("keyStal = SingleClik\r\n");
				DrawFont_GBK24B(0,0,BLACK,(u8*)"S");
				keySta1 = NoKeyClick;
			}
			else if(keySta1 == DoubleClick)
			{
				printf("keyStal = DoubleClick\r\n");
				DrawFont_GBK24B(0,0,BLACK,(u8*)"D");
				keySta1 = NoKeyClick;
			}
			else if(keySta1 == LongClick)
			{
				printf("keyStal = LongClick\r\n");
				DrawFont_GBK24B(0,0,BLACK,(u8*)"L");
				keySta1 = NoKeyClick;
			}
		}
		
		// ʵ�ִ��ڵ��շ�������Ľ�����ִ�У�
		if(USART_RX_STA&0x8000)		// ����յ�������
		{
			size_t t;
			lenOfTx = USART_RX_STA&0x3fff;			//�õ��˴ν��յ������ݳ���

			for(t = 0; t < lenOfTx; t++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
				USART_SendData(USART1, USART_RX_BUF[t]);		//�򴮿�1��������(�Է�����)
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}
	} 
}

void myInit(void)
{
	//RCC_Configuration();		// ��startup_stm32f10x_hd.s�ļ����Ѿ�������SystemInit������ʱ�ӽ��г�ʼ����
								// the clock already setting in startup_stm32f10x_hd.s, so not need set again.
	delay_init();	    	    //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	     	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			      	// GPIO��ʼ��
	EXTIX_Init();		 		//�ⲿ�жϳ�ʼ��
	LCD_Init();           		//��ʼ��LCD SPI �ӿ�
	TIM5_PWM_Init(PWM_COUNT, 4-1);	 		// 4��Ƶ��PWMƵ��=72000/1800/4=10Khz
	TIM2_Int_Init();			// ��ⰴ��״̬���ܶ�ʱ��
	TIM3_Int_Init();			// ���ת��ʱ����ƶ�ʱ��
	
#if	KEEP_PCR_PARAM_IN_FLASH			// ���������FLASH
	STMFLASH_Write( ADDR_timeClockRotate,     (u16*)&timeClockRotate,     sizeof(uint8_t) );
	STMFLASH_Write( ADDR_timeAntiClockRotate, (u16*)&timeAntiClockRotate, sizeof(uint8_t) );
#endif
	// ��FLASH�ж�ȡ����
	STMFLASH_Read( ADDR_timeClockRotate,     (u16*)&timeClockRotate,     sizeof(uint8_t) );
	STMFLASH_Read( ADDR_timeAntiClockRotate, (u16*)&timeAntiClockRotate, sizeof(uint8_t) );

	// LCD��ʾ��ʼ��
	DrawFont_GBK32B(56,0,RED,(u8*)"�������");
	DrawFont_GBK32B(36,72,BLACK,(u8*)"����ת");
	DrawFont_GBK24B(142,59,BLACK,(u8*)"��:   s");
	itoa(text_buffer, timeClockRotate);					// ������ sprintf() ��������
	DrawFont_GBK24B(190,59,BLACK,(u8*)text_buffer);
	DrawFont_GBK24B(142,93,BLACK,(u8*)"��:   s");
	itoa(text_buffer, timeAntiClockRotate);
	DrawFont_GBK24B(190,93,BLACK,(u8*)text_buffer);
	DrawFont_GBK32B(36,136,BLACK,(u8*)"ת��: ֹͣ");

	// ���ڷ���
	printf("\t�������\r\n");
	printf("\t\t20201225\r\n");
	Motor_Stop();
}
void RCC_Configuration(void)
{
	//----------ʹ���ⲿRC����-----------
	RCC_DeInit();			//��ʼ��Ϊȱʡֵ
	RCC_HSEConfig(RCC_HSE_ON);	//ʹ���ⲿ�ĸ���ʱ�� 
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);	//�ȴ��ⲿ����ʱ��ʹ�ܾ���
	
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//Enable Prefetch Buffer
	FLASH_SetLatency(FLASH_Latency_2);		//Flash 2 wait state
	
	RCC_HCLKConfig(RCC_SYSCLK_Div1);		//HCLK = SYSCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);			//PCLK2 =  HCLK
	RCC_PCLK1Config(RCC_HCLK_Div2);			//PCLK1 = HCLK/2
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);	//PLLCLK = 8MHZ * 9 =72MHZ
	RCC_PLLCmd(ENABLE);			//Enable PLLCLK
 
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}		//Wait till PLLCLK is ready
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);	//Select PLL as system clock
	while(RCC_GetSYSCLKSource()!=0x08);		//Wait till PLL is used as system clock source
	
	//---------����Ӧ����ʱ��--------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//ʹ��APB2�����GPIOA��ʱ��
}

void itoa(char* buf, int i)
{
	buf[0] = i/10 + 0x30;
	buf[1] =  i % 10 + 0x30;
	buf[2] = 0;
	
	if(buf[0] == 0)
	{
		buf[0] = buf[1];
		buf[1] = buf[2];
	}
}
