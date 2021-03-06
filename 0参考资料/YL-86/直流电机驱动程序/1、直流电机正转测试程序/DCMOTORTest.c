/*************************************************************************************************************
*   测试说明
*1、MCU:STC89C52RC
*2、测试频率:11.0592Mhz
*2、编程语言:C51
*2、编译环境:Keil C
*2、功能:全速正转
*************************************************************************************************************/

#include <reg52.h>

//L298输入控制端定义
sbit IN1 = P1^0;
sbit IN2 = P1^1;	
sbit IN3 = P1^2;	
sbit IN4 = P1^3;
sbit ENA = P1^4;	
sbit ENB = P1^5;

/********************************/
/*      主函数		             */
/********************************/
main()
{
	IN1=1;			
	IN2=0;			
	ENA=1;	
	IN3=1;			
	IN4=0;			
	ENB=1;	

	while(1);
}
