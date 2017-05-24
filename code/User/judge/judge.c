
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "judge.h"

extern u8 isAccident;
u8 timecount = 0;
	
//����������ʼ�������ڰ����ļ��
void KEY_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
}

//����ɨ�躯��
u8 KEY_Scan(void)
{
	static u8 key_up = 1;
	if(key_up && KEY == 0)
	{
		delay_ms(10);
		if(KEY == 0) return 1;
	}
	else if(KEY == 1) key_up = 1;
	return 0;//�ް�������
		
}

//��ʱ��4�жϣ������ж�����ʱ��
void TIM4_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //TIM4ʱ��ʹ��    
	/*��ʱ��TIM3��ʼ��*/
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�
	/*����TIM3�ж�*/ 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���		
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx
}


	
//��ʱ��4�жϺ���
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == RESET) return ;
	printf("count == %d\r\n",timecount);
	if(++timecount == 10)
	{
		isAccident = 2;
		TIM_Cmd(TIM4, DISABLE);  //ʧ��TIMx
		timecount = 0;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}