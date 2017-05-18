/**
  ******************************************************************************
  * @file    exit.c
  * @author  Shengqiang.Zhang
  * @version V1.1
  * @date    2016-10-25
  * @brief   ���ܻ��ѵ��жϲ���
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "exti.h"


/**
  * @brief  Sys_Sleepy()
  * @param  ��
  * @retval ��
  */
void GoToSleep(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//�رռ̵���
/*	
	SCB->SCR |= 0X00;
	

	#if defined ( __CC_ARM   )                          
	__force_stores();
	#endif
	__WFI();//�ȴ��ж�
*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);//����ֹͣģʽ
}

/**
  * @brief  RCC_Configuration()
  * @param  ��
  * @retval ��
  */
void RCC_Configuration(void)
{
	//��RCC�Ĵ�����������ΪĬ��ֵ
	RCC_DeInit(); 
	//���ⲿ����ʱ�Ӿ���HSE  
	RCC_HSEConfig(RCC_HSE_ON);
	//�ȴ��ⲿ����ʱ�Ӿ����� 
	while( ERROR == RCC_WaitForHSEStartUp());
	//����AHBʱ��AHB clock = SYSCLK 
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	//���ø���AHBʱ�� APB2 clock = HCLK 
	RCC_PCLK2Config(RCC_HCLK_Div1);
	//���õ���AHBʱ�� APB1 clock = HCLK/2 
	RCC_PCLK1Config(RCC_HCLK_Div2);
	// ����PLL ʱ��ԴΪHSE ��Ƶϵ��Ϊ9  
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); 
	// ��PLL 
	RCC_PLLCmd(ENABLE);
	// �ȴ�PLL����
	while(RESET ==  RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );
	// ����ϵͳʱ��   RCC_SYSCLKSource_XX    ��ѡ( PLLCLK  HSI  HSE )  
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
	// �ж��Ƿ�PLL��ϵͳʱ�� 
	while(RCC_GetSYSCLKSource() != 0x08);

}

/**
  * @brief  to recovery the symtem clock
  * @param  ��
  * @retval ��
  */
void SYSCLKConfig(void)
{
	ErrorStatus HSEStartUpStatus;
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* �ȴ� HSE ׼������ */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {

    /* ʹ�� PLL */ 
    RCC_PLLCmd(ENABLE);

    /* �ȴ� PLL ׼������ */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* ѡ��PLL��Ϊϵͳʱ��Դ */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/**
  * @brief  RTC_Configuration()
  * @param  ��
  * @retval ��
  */
void RTC_Configuration (void)
{
	//ʹ��PWR��Դ����ʱ�Ӻ�BKP�󱸼Ĵ���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//����RTCʱ��Դ,�������BKP
	PWR_BackupAccessCmd(ENABLE); 
	//��λ������
	BKP_DeInit();
	//����LSE
	RCC_LSEConfig(RCC_LSE_ON); 
	//�ȴ�LSE��Ч
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET) ;
	//ѡ��LSE��ΪRTCʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
	//����RTCʱ��
	RCC_RTCCLKCmd(ENABLE);
	//����RTC,�ȴ�RTC APBͬ��
	RTC_WaitForSynchro();
	//Ԥ��ƵֵΪ1s
	RTC_SetPrescaler(32767);
	//�ȴ����һ��дָ�����
	RTC_WaitForLastTask();
	//����RTC�����ж�
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	//�ȴ����һ��дָ�����
	RTC_WaitForLastTask(); 
}

/**
  * @brief  EXTI_Configuration()
  * @param  ��
  * @retval ��
  */
void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	//ʹ���ⲿ�жϷ�ʽ
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd =ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/**
  * @brief  This function handles EXTI3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void) /*�жϻ���*/
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
