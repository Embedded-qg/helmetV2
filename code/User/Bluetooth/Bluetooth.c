/**
  ******************************************************************************
  * @file    Bluetooth.c
  * @author  Xiaorong.Zhang  Jinji.Fang
  * @version V1.1
  * @date    2016-10-25
  * @brief   ����ͨ��Э��
  ******************************************************************************
  * @attention  
  *
  ******************************************************************************
  */
	
#include "Bluetooth.h"
#include "ov7725.h"

float Axis[3];
static char num[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};//�������ַ���ʾ

/**
 * @brief  �����źŴ�����
 * @param  
 * @retval 1��ʾ����������0��ʾ�󴥷�
 */
int CrashFunction(void)
{
	u8 recSignal[3] = {0};
	u8 ch;
	u8 flag;	//�жϳ�������ʱ���ƶ����Ƿ��������
	while(1)
	{
		printf("aaa\n");//���ͳ��������ź�

		/*�����ƶ��˷����ġ�ok���ź�*/
		while('o' != (ch = getchar()));
		
		printf("%c",ch);
		recSignal[0] = ch;
		recSignal[1] = getchar();
		
		printf("%c",recSignal[1]);
	//	BLUETOOTH_DEBUG("%c,%c\n", recSignal[0], recSignal[1]);   
		
		if(recSignal[1] == 'k')
		{
			//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//�رշ�����
			//������յ�ac�����������ķ��������յ�wt������Ǵ��󴥷�����
			recSignal[0] = getchar();
			printf("%c",recSignal[0]);
			recSignal[1] = getchar();
			printf("%c",recSignal[1]);
			recSignal[2] = '\0';
			
			if(recSignal[0] == 'a' && recSignal[1] == 'c')
			{
				//�ƶ����������
				flag = getchar();
				printf("%c",flag);
				if(flag != '0')//ȷ�ϳ���ʱ��app�����ַ�'0'
					continue;
				printf("success!\r\n");
			//	BLUETOOTH_DEBUG("ac success!\n");
				//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//�رշ�����
				return 1;
			}
			else if(recSignal[0] == 'w' && recSignal[1] == 't')
			{
				//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//�رշ�����
				BLUETOOTH_DEBUG("wt success! %s\n", recSignal);
				return 0;
			}
			else{
				BLUETOOTH_DEBUG("ac wt error! %s\n", recSignal);
				continue;//�ƶ��˳�����
			}
		}
		else{
			BLUETOOTH_DEBUG("ok error %s\n", recSignal);
		}
	}
}
/**
 * @brief  		������ٶ������Է��͸��ֻ�����ʾ
 * @param  		char *data:�洢�����͵�����
 * @retval 		void
 */
void SendFromMPUtoBluetooth(char *data)
{
	int i, j;
	
	for(i = 0; i < 3; ++i)
		*data++ = 'b';//ǰ�����ַ�Ϊ'b',��data������ǰ�ƶ�һλ
	
	for(i = 0; i < 3; ++i)//xyz����ѭ��
	{
		if(Axis[i] < 0)
		{
			Axis[i] = -Axis[i];
			*(data + i * 6) = num[1];//��������λΪ1
		}
		else
			*(data + i * 6) = num[0];
		
		Axis[i] *= 1000;
		for(j = 5; j >= 1; --j)
		{
			*(data + i * 6 + j) = num[(int)Axis[i] % 10];
			Axis[i] /= 10.0;
		}
	}
}


static u8 BT_SendByte(u8 b) 		
{						
	USART_SendData(USART2, b);	
	while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{ }
	return b;
}	



