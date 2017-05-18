/**
  ******************************************************************************
  * @file    main.c
  * @author  Jinji.Fang
  * @version V1.0
  * @date    2016-10-16
  * @brief   ��3.5.0�汾�⽨�����ܼ���ͷ������ (���Է������ݵ���������)
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "delay.h"
#include "exti.h"
#include "gps.h"
#include "uart4.h"
#include "gpio.h"
#include "usart.h"
#include "beep.h"
#include "judge.h"
#include "MPU-6050.h"
#include "answer.h"
#include "spi.h"
#include "malloc.h"
#include "ov2640.h"
#include "sdfs_app.h"
#include "sd.h"
#include "debug.h"
#include "ff.h"
#include "Bluetooth.h"
#include "bsp_usart3.h"
#include "sim900a.h"

#define PHOTO_NUM 2



u8 isAccident = 0;
extern u8* ov2640_framebuf;//�����֡����
extern u8 timecount;
void camera_init();

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

int main(void)
{
	bool sysWorking = TRUE;
	int count = 0;
	int count2 = 0;
	int i = 0;

	char MPU_data[21];		
  nmea_msg mse;     //GPS��Ϣ�ṹ��
	nmea_msg *gps = &mse;              
	u8 *pname;					//��·�����ļ��� 
	RCC_Configuration();//RCCʱ�ӳ�ʼ��              
	delay_init();//��શ�ʱ����ʼ��
	NVIC_Configuration();//NVIC���ȼ���ʼ����RTC �� �ⲿ�жϣ�
	GPIO_Configuration();//�̵������ų�ʼ��PC13,PD12
	EXTI_Configuration();//�ⲿ�жϳ�ʼ��,�ж�����PC14
	USART_Config(115200);//����1��ʼ��������������������
	USART3_Config(115200);//����3��ʼ��������GPRS
	UART4_Init(115200);
	delay_ms(1800);

	while(1)
	{	
		delay_ms(100);
		GoToSleep();//����˯��ģʽ���ȴ��жϻ���
		SystemInit();//����ϵͳʱ��
		delay_init();//��શ�ʱ����ʼ��

		GPIO_Configuration();//�̵������ų�ʼ��PC14,PA11
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);//�򿪼̵���
	
		NVIC_Configuration();//NVIC���ȼ���ʼ��                                                                                                                                                                                                                                                                                 
		EXTI_Configuration();//�ⲿ�жϳ�ʼ��   	
		BEEP_Init();
		KEY_Init(); 
//����1��ʼ��������������������	
		USART_Config(115200);
//����3��ʼ��������GPRS				
		USART3_Config(115200);
//����4��ʼ��������gps��������		
		UART4_Init(9600);
////SD����ʼ��
		if(sdfs_app_mnt() != FR_OK)	printf("\r\nSD��ERROR\r\n");
		else printf("sd ok\r\n");
////gprs��ʼ��		
		count2 = 0;
		//sim900a_tx_printf("ATE1\r");
		while((!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)) && gprs_init("123.207.124.49","6666") == 0)//GPRS��ʼ��
		{
			count2++;
			if(count2 >= 5)
			{
				printf("GPRS����״̬����\r\n");//�������³�ʼ��
				break ;
			}
		}
		my_mem_init(SRAMIN);//��ʼ���ڲ��ڴ��
		ov2640_framebuf = mymalloc(SRAMIN,52*1024);//����֡����
		pname=mymalloc(SRAMIN,30);		//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
		if(!pname||!ov2640_framebuf)	//�ڴ�������
		{		    
				printf("\r\n�ڴ����ʧ��!\r\n");
				delay_ms(200);				   
		} 
    i = 0;
		while(OV2640_Init())		//��ʼ��OV2640
		{
				i++;
				delay_ms(200);
				if(i == 5)	break;
		}
		
		while(1)
		{
			SystemInit();
			USART2_Config();//����2��ʼ��
			USART2_DMA_Config();//DMA��ʼ��
			
			DMA_ClearFlag(DMA1_FLAG_TC6);
			USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);		
			
			sysWorking = TRUE; //�ж�ϵͳ�Ƿ��ڹ���״̬
			while(sysWorking)	/* ϵͳ��ʼ���� */
			{					
				if(DMA_GetFlagStatus(DMA1_FLAG_TC6) == SET)
				{	
					if(!IS_MPU_RUNNING)					break;
					else
					{ 	
						printf("b");			

						if(TRUE == IsAccident())
						{ 

							SendFromMPUtoBluetooth(MPU_data);//������ٶ����ݸ�����
//						printf("Accident!%f %f %f %f\n", Axis[0], Axis[1], Axis[2], Square(Axis[0]) + Square(Axis[1]) + Square(Axis[2]));						
//							printf("%s\n", MPU_data);				
              			        BEEP_Open();
							
							timecount = 0;
							isAccident = 0;
							TIM4_Init(4999,7199);
							while(isAccident == 0)
							{
								if(KEY_Scan() == 1) isAccident = 1;      							
							}
							BEEP_Close();
							TIM_Cmd(TIM4, DISABLE);
							if(isAccident == 1) 
							{ 	
								printf("not accidet!\r\n");
								break;
							}
							while((UART4_RX_STA & 0x8000 ) == 0);       //�ȴ�GPS��Ϣ�������
							GPSMSG_Analysis(gps,UART4_RX_BUF);           //����GPS��Ϣ
							Send_NMEA_MSG(gps);                           //��ӡ��ص�GPS��Ϣ
							UART4_RX_STA = 0;	
//							/*������Ƭ*/	            
							for(count = 0; count < PHOTO_NUM; )
							{
												sprintf((char *)pname, "%d.jpg", count++);
												ov2640_jpg_photo(pname);  
												delay_ms(1000);  
							}			
							
						}
					}
				}
				DMA_ClearFlag(DMA1_FLAG_TC6);
	
				delay_ms(100);
				
				
				if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)//�ߵ�ƽ��ʾҪ����˯��
				{
					sysWorking = FALSE;
					delay_ms(100);
				}
			}
		  if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)//�ߵ�ƽ��ʾҪ����˯��
			{
				EXTI->IMR&=~(1<<0);//����line0�ϵ��жϣ���������ͷ�ⲿ�ж�ͨ��0		
				break;
			}
		}
	}		
}


/*********************************************END OF FILE**********************/
