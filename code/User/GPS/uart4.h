#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"  

#define UART4_MAX_RECV_LEN		300			//�����ջ����ֽ���
extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���UART4_MAX_RECV_LEN�ֽ�
extern u16 UART4_RX_STA;   						//��������״̬

void UART4_Init(u32 bound);				//����4��ʼ�� 
void TIM3_Set(u8 sta);
void TIM3_Init(u16 arr,u16 psc);

#endif
