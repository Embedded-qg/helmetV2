/**
  ******************************************************************************
  * @file    main.c
  * @author  Shengdui.Liang
  * @version V2.0
  * @date    2017-5-30
  * @brief   ��3.5.0�汾�⽨�����ܼ���ͷ������ (���Է������ݵ���������)
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "judge.h"
#include "MPU-6050.h"
#include "gpio.h"
#include "answer.h"
#include "beep.h"
#include "malloc.h"
#include "ov2640.h"
#include "sdfs_app.h"
#include "sd.h"
#include "stdlib.h"
#include "debug.h"
#include "ff.h" 
#include "gps.h"
#include "Bluetooth.h"
#include "bsp_usart3.h"
#include "sim900a.h"
#include "uart5.h"
#include "uart4.h"


#define PHOTO_NUM 3




extern u8* ov2640_framebuf;//�����֡����
extern u8 timecount;
extern u8 isAccident;
extern float Angular[3];
nmea_msg mse;
nmea_msg *gps = &mse;


void gpsData_send(void);
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

int main(void)
{
	u8 initCount,photoNum;
	char MPU_data[21];	
	char send_message[100];
	u8 *pname;					//��·�����ļ��� 
	RCC_Configuration();//RCCʱ�ӳ�ʼ��              
	delay_init();//��શ�ʱ����ʼ��
	NVIC_Configuration();//NVIC���ȼ���ʼ����RTC �� �ⲿ�жϣ�
	GPIO_Configuration();	
	EXTI_Configuration();//�ⲿ�жϳ�ʼ��   
loop:	
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//�رռ̵���
	GoToSleep();
	SystemInit();//ϵͳʱ�ӳ�ʼ��
	
	RCC_Configuration();//RCCʱ�ӳ�ʼ��              
	delay_init();//��શ�ʱ����ʼ��
	GPIO_Configuration();//�̵������ų�ʼ��PC14,PA11
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);//�򿪼̵���
	NVIC_Configuration();//NVIC���ȼ���ʼ����RTC �� �ⲿ�жϣ�                                                                                                                                                                                                                                                                                
	EXTI_Configuration();//�ⲿ�жϳ�ʼ��  
	USART_Config(9600);  //����1��ʼ�������ڵ���ʹ��
	USART3_Config(115200); //����3��ʼ��������gprs
	UART4_Init(9600);// ����4��ʼ��������GPS��������
	UART5_Init(9600);

	if(sdfs_app_mnt() != FR_OK)	printf("\r\nSD��ERROR\r\n");    //SD����ʼ��
	my_mem_init(SRAMIN);//��ʼ���ڲ��ڴ��
	ov2640_framebuf = mymalloc(SRAMIN,52*1024);//����֡����
	pname=mymalloc(SRAMIN,30);		//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
	if(!pname||!ov2640_framebuf)	printf("\r\n����ͷ�ڴ����ʧ��!\r\n");

	for(initCount = 0;initCount < 5 && OV2640_Init();initCount++)  //����ͷ��ʼ��
	{
		delay_ms(200);
	}	
	SystemInit();
	KEY_Init();
	BEEP_Init();
	mpu6050_init();
	while(1)
	{
		if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)  goto loop;
		DMA_ClearFlag(DMA1_FLAG_TC6);
		delay_ms(100);
		if(DMA_GetFlagStatus(DMA1_FLAG_TC6) != SET) continue;
		if(!IS_MPU_RUNNING)
		{
			SystemInit();
			mpu6050_init();
			continue;
		}
		printf("b");
		if(IsAccident() == FALSE) continue;
		SendFromMPUtoBluetooth(MPU_data);
		printf("MPU_data:%s\r\n",MPU_data);
		BEEP_Open();
    isAccident = timecount = 0;  //������ʱ���ͳ����жϱ��
		TIM4_Init(4999,7199);//500ms��ʱ
		while(isAccident == 0) if(KEY_Scan() == 1) isAccident = 1;//�ȴ���Ӧ     		
		BEEP_Close();
		TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��4
		if(isAccident == 1) continue;
		srand((unsigned)Angular[2]);
//		gpsData_send();   	
		USART3_printf(UART5,"aaa\r\n");
		for(initCount = 0;initCount < 5 && !gprs_init("123.207.124.49","6666");initCount++)//GPRS��ʼ��
		{
				printf("GPRS����״̬����\r\n");//�������³�ʼ��
		}
		for(photoNum = 0; photoNum < PHOTO_NUM;photoNum++ )
		{
			sprintf((char *)pname, "%d.jpg",rand() % 10000);
			ov2640_jpg_savephoto(pname);  
			delay_ms(100);  
		}	  
		gpsData_send();
		sim900a_gprs_send("HM+SMS\r\n3115005537.txt\r\n");
		sprintf(send_message,"�ҵ���������ʢ�ң����ھ���:%f%c,ά��%f%c�������˳������뼰ʱ��Ԯ��",(float)gps->longitude/100000,gps->ewhemi,(float)gps->latitude/100000,gps->nshemi);
		sim900a_gprs_send(send_message);
		sprintf(send_message,"�ҵ����������:�Ա�-%s,����-%d�꣬���-%dcm������-%dkg��Ѫ��-%c,������ʷ-%s\r\n","��",21,168,51,'A',"��");
		sim900a_gprs_send(send_message);
		for(photoNum = 0; photoNum < PHOTO_NUM;photoNum++ )
		{
			ov2640_jpg_sendphoto();  
			delay_ms(100);  
		}	
		sim900a_gprs_link_close();//�ر�����  
	}
}


void gpsData_send(void)
{
			while((UART4_RX_STA & 0x8000 ) == 0);       //�ȴ�GPS��Ϣ�������
			GPSMSG_Analysis(gps,UART4_RX_BUF);           //����GPS��Ϣ
			UART4_RX_STA = 0;	   										
}

/*********************************************END OF FILE**********************/
