#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//���ջ���,���UART4_MAX_RECV_LEN���ֽ�.
u16 UART4_RX_STA=0;  

/*
@name: UART4_IRQHandler
@param: none
@return: none
@description: UART4�жϴ��������������ݣ�
*/
void UART4_IRQHandler(void)
{
	u8 res;	    

	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 
    res = USART_ReceiveData(UART4);		  	
		if(UART4_RX_STA < UART4_MAX_RECV_LEN)		//�����Խ�������
		{ 

			TIM_SetCounter(TIM3,0);//���������        				 
			if(UART4_RX_STA==0)   TIM3_Set(1);	 	//ʹ�ܶ�ʱ��3���ж� 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//��¼���յ���ֵ	 
			UART4_RX_BUF[UART4_RX_STA] = '\0';
		}
		else  UART4_RX_STA |= 1<<15;					//ǿ�Ʊ�ǽ������
	}  											 
}   

/*
@name:UART4_Init();
@param: bound--����4�Ĳ�����
@return: none
@description: ��ʼ������4
*/
void UART4_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	// GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);   //USART2ʱ��

  /*����PC10����UART4 Tx*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PA2
  /*����PC11����UART4 Rx*/    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC11
	
	/*����UART4����*/
	USART_InitStructure.USART_BaudRate = bound;//����������Ϊbound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������4
  
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�   
	/*UART4�ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM3_Init(99,7199);		//10ms�ж�
	UART4_RX_STA=0;		//����
	TIM3_Set(0);			//�رն�ʱ��3	 	
}

/*
@name:TIM3_IRQHandler();
@param:none
@return:none
@description:��ʱ��3�жϺ��������ô���2�Ľ�����ɱ��
*/
void TIM3_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == RESET) return ;//�Ǹ����ж�
 	UART4_RX_STA |= 0x8000;	//��ǽ������
//	printf("UART4%d",UART4_RX_STA);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־    
	TIM3_Set(0);			//�ر�TIM3   
}

/*
@name�� TIM3_Set();
@param:  sta--��ʱ��״̬��1Ϊ�򿪼�ʱ����0Ϊ�رռ�ʱ��
@return�� none
@description:��ʱ��3��ʹ����ʧ��
*/
void TIM3_Set(u8 sta)
{
	if(sta == 0)  
	{
		TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��3
		return ;
	}
    TIM_SetCounter(TIM3,0);//���������
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx	 	   
}


/*
@name:TIM3_Init(u16 arr,u16 psc);
@param:arr--�Զ���װֵ,psc--ʱ��Ԥ��Ƶ��	
@return: none
@desription:ͨ�ö�ʱ���жϳ�ʼ��
*/
void TIM3_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��//TIM3ʱ��ʹ��    
	/*��ʱ��TIM3��ʼ��*/
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	/*����TIM3�ж�*/ 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

