#include "main.h"

// PCR参数保存地址(地址必须为偶数)
#define ADDR_timeClockRotate  		0x08030000
#define ADDR_timeAntiClockRotate  0x08030004

uint8_t motorStat = 0;
uint16_t duty = 0;
extern uint8_t cntTimer3;

int main(void)
{
	myInit();		// 一些模块和IO口初始化
	while(1)
	{		
		if(keySta1 != NoKeyClick)
		{
			if(keySta1 == SingleClick)
			{
				if(++motorStat == 4)
					motorStat = 0;		// 改变状态
				if(motorStat == 1)		// 慢速
				{
					duty = 1400;		// 通过改变 PWM 占空比控制电机转速
					DrawFont_GBK32B(36,136,BLACK,(u8*)"转速: 低速");
					TIM_Cmd(TIM3, ENABLE);
				}			
				else if(motorStat == 2)
				{
					duty = 1550;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"转速: 中速");
				}
				else if(motorStat == 3)
				{
					duty = 1700;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"转速: 高速");
				}
				else
				{
					TIM_SetCompare3(TIM5,0);
					TIM_SetCompare2(TIM5,0);
					TIM_Cmd(TIM3, DISABLE);
					cntTimer3 = 0;
					DrawFont_GBK32B(36,136,BLACK,(u8*)"转速: 停止");
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
		
		// 实现串口的收发（命令的接收与执行）
		if(USART_RX_STA&0x8000)		// 如果收到了数据
		{
			size_t t;
			lenOfTx = USART_RX_STA&0x3fff;			//得到此次接收到的数据长度

			for(t = 0; t < lenOfTx; t++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);	//等待发送结束
				USART_SendData(USART1, USART_RX_BUF[t]);		//向串口1发送数据(自发自收)
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}
	} 
}

void myInit(void)
{
	//RCC_Configuration();		// 在startup_stm32f10x_hd.s文件中已经调用了SystemInit函数对时钟进行初始化。
								// the clock already setting in startup_stm32f10x_hd.s, so not need set again.
	delay_init();	    	    //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	     	//串口初始化为115200
 	LED_Init();			      	// GPIO初始化
	EXTIX_Init();		 		//外部中断初始化
	LCD_Init();           		//初始化LCD SPI 接口
	TIM5_PWM_Init(PWM_COUNT, 4-1);	 		// 4分频。PWM频率=72000/1800/4=10Khz
	TIM2_Int_Init();			// 检测按键状态功能定时器
	TIM3_Int_Init();			// 电机转动时间控制定时器
	
#if	KEEP_PCR_PARAM_IN_FLASH			// 保存参数到FLASH
	STMFLASH_Write( ADDR_timeClockRotate,     (u16*)&timeClockRotate,     sizeof(uint8_t) );
	STMFLASH_Write( ADDR_timeAntiClockRotate, (u16*)&timeAntiClockRotate, sizeof(uint8_t) );
#endif
	// 从FLASH中读取参数
	STMFLASH_Read( ADDR_timeClockRotate,     (u16*)&timeClockRotate,     sizeof(uint8_t) );
	STMFLASH_Read( ADDR_timeAntiClockRotate, (u16*)&timeAntiClockRotate, sizeof(uint8_t) );

	// LCD显示初始化
	DrawFont_GBK32B(56,0,RED,(u8*)"电机控制");
	DrawFont_GBK32B(36,72,BLACK,(u8*)"正反转");
	DrawFont_GBK24B(142,59,BLACK,(u8*)"正:   s");
	itoa(text_buffer, timeClockRotate);					// 可以用 sprintf() 函数代替
	DrawFont_GBK24B(190,59,BLACK,(u8*)text_buffer);
	DrawFont_GBK24B(142,93,BLACK,(u8*)"反:   s");
	itoa(text_buffer, timeAntiClockRotate);
	DrawFont_GBK24B(190,93,BLACK,(u8*)text_buffer);
	DrawFont_GBK32B(36,136,BLACK,(u8*)"转速: 停止");

	// 串口发送
	printf("\t电机控制\r\n");
	printf("\t\t20201225\r\n");
	Motor_Stop();
}
void RCC_Configuration(void)
{
	//----------使用外部RC晶振-----------
	RCC_DeInit();			//初始化为缺省值
	RCC_HSEConfig(RCC_HSE_ON);	//使能外部的高速时钟 
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);	//等待外部高速时钟使能就绪
	
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
	
	//---------打开相应外设时钟--------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//使能APB2外设的GPIOA的时钟
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
