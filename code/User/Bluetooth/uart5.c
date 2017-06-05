#include "delay.h"
#include "uart5.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
 	
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//���ջ���,���UART5_MAX_RECV_LEN���ֽ�.
u16 UART5_RX_STA=0;  

/*
@name: UART5_IRQHandler
@param: none
@return: none
@description: UART4�жϴ��������������ݣ�
*/
void UART5_IRQHandler(void)
{
	u8 res;	    

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�����
	{	 
    res = USART_ReceiveData(UART5);		  	
		if(UART5_RX_STA < UART5_MAX_RECV_LEN)		//�����Խ�������
		{ 

			TIM_SetCounter(TIM2,0);//���������        				 
			if(UART5_RX_STA==0)   TIM2_Set(1);	 	//ʹ�ܶ�ʱ��3���ж� 
			UART5_RX_BUF[UART5_RX_STA++]=res;		//��¼���յ���ֵ	 
			UART5_RX_BUF[UART5_RX_STA] = '\0';
		}
		else  UART5_RX_STA |= 1<<15;					//ǿ�Ʊ�ǽ������
	}  											 
}   

/*
@name:UART5_Init();
@param: bound--����4�Ĳ�����
@return: none
@description: ��ʼ������5
*/
void UART5_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	// GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);   //USART2ʱ��

  /*����PC12����UART5 Tx*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC10
  /*����PD2����UART5 Rx*/    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PC11
	
	/*����UART5����*/
	USART_InitStructure.USART_BaudRate = bound;//����������Ϊbound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������4
  
	USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�   
	/*UART4�ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM2_Init(99,7199);		//10ms�ж�
	UART5_RX_STA=0;		//����
	TIM2_Set(0);			//�رն�ʱ��3	 	
}

/*
@name:TIM2_IRQHandler();
@param:none
@return:none
@description:��ʱ��3�жϺ��������ô���2�Ľ�����ɱ��
*/
void TIM2_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET) return ;//�Ǹ����ж�
 	UART5_RX_STA |= 0x8000;	//��ǽ������
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־    
	TIM2_Set(0);			//�ر�TIM3   
}

/*
@name�� TIM2_Set();
@param:  sta--��ʱ��״̬��1Ϊ�򿪼�ʱ����0Ϊ�رռ�ʱ��
@return�� none
@description:��ʱ��3��ʹ����ʧ��
*/
void TIM2_Set(u8 sta)
{
	if(sta == 0)  
	{
		TIM_Cmd(TIM2, DISABLE);//�رն�ʱ��3
		return ;
	}
    TIM_SetCounter(TIM2,0);//���������
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx	 	   
}


/*
@name:TIM2_Init(u16 arr,u16 psc);
@param:arr--�Զ���װֵ,psc--ʱ��Ԥ��Ƶ��	
@return: none
@desription:ͨ�ö�ʱ���жϳ�ʼ��
*/
void TIM2_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��//TIM3ʱ��ʹ��    
	/*��ʱ��TIM3��ʼ��*/
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	/*����TIM3�ж�*/ 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

