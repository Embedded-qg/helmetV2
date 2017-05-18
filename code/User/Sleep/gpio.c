/**
  ******************************************************************************
  * @file    gpio.c
  * @author  Shengqiang.Zhang
  * @version V1.1
  * @date    2016-10-25
  * @brief   ���ܻ������ų�ʼ��
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "gpio.h"

/**
  * @brief  GPIO_Configuration()
  * @param  ��
  * @retval ��
  */
 void GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;/*����Թܽ����ź�*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;/*�̵���*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//��©���,�����õļ̵�����5V�����ģ�����ȷ��I/O���ܹ�����5V���������Ӳ��
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//�ߵ�ƽ���رռ̵���
}
