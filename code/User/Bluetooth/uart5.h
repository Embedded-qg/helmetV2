#ifndef __UART5_H
#define __UART5_H	 
#include "sys.h"  

#define UART5_MAX_RECV_LEN		100			//�����ջ����ֽ���
extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//���ջ���,���UART5_MAX_RECV_LEN�ֽ�
extern u16 UART5_RX_STA;   						//��������״̬

void UART5_Init(u32 bound);				//����5��ʼ�� 
void TIM2_Set(u8 sta);
void TIM2_Init(u16 arr,u16 psc);

#endif
