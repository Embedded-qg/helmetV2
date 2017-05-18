#include "bsp_usart3.h"
#include "sim900a.h"

void gprs_init(void)
{
		if(sdfs_app_mnt() == FR_OK) 
			printf("ok\r\n");//SDFS��ʼ��
		else printf("ERROR\r\n");
		
		while(sim900a_cmd("AT\r","OK",1000) != 1)//���ģ����Ӧ�Ƿ�����
		{
			printf("ģ����Ӧ���Բ�����������������\r\n");
		}
		printf("GPRSģ����Ӧ����\r\n");

		if (gprs_reg_status() == 0)
			sim900a_gprs_init();//��ʼ��GPRS
		
		if (sim900a_gprs_status() == 0)//�������״̬
		{
			if (1 == sim900a_gprs_tcp_link("123.207.124.49", "6666"))
			{
				printf("TCP�������ӳɹ���\r\n\r\n");
				if(sim900a_gprs_send("����һ�£�123456") == 1)//��������
				{
					printf("���ͳɹ���\r\n\r\n");
					sim900a_gprs_link_close();//�ر�����
					delay_ms(1000);
				}
				else 
					printf("����ʧ�ܣ�\r\n\r\n");

				SIM900A_CLEAN_RX();//�������3���ص�����
			}			
		}
}