#include "MPU-6050.h"

/**************************************************************
*        Global Value Define Section
**************************************************************/
uint8_t MPU_Data [33] = {0};

float Angle[3];
float Angular[3];
float FinalBaseAxis[3];

/**
  * @brief  USART2 GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void USART2_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		
		/* config USART2 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		
		/* USART2 GPIO config */
		/* Configure USART2 Tx (PA.02) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART2 Rx (PA.3) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		/* USART2 mode config */
		USART_InitStructure.USART_BaudRate = 115200 ;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_Cmd(USART2, ENABLE);
}

/**
  * @brief  USART2 RX DMA ���ã����赽�ڴ�(USART2->DR)
  * @param  ��
  * @retval ��
  */
void USART2_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*����DMAʱ��*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
	
		/*����DMAԴ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;	   

		/*�ڴ��ַ(Ҫ�������ݵı�����ָ��)*/
		DMA_InitStructure.DMA_MemoryBaseAddr =  (u32)&MPU_Data;

		/*���򣺴������*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*�����С*/	
		DMA_InitStructure.DMA_BufferSize = 33;

		/*�����ַ����*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*�ڴ��ַ����*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*�������ݵ�λ 8bit*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*�ڴ����ݵ�λ 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMAģʽ��ѭ��ģʽ*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*���ȼ�����*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;  

		/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*����DMA1��6ͨ��*/		   
		DMA_Init(DMA1_Channel6, &DMA_InitStructure); 	   
		
		/*ʹ��DMA*/
		DMA_Cmd (DMA1_Channel6,ENABLE);					
}

//��ʼ��mpu6050
void mpu6050_init(void)
{
	USART2_Config();  //���ô���2
	USART2_DMA_Config();   //���ô���2��DMA����
	DMA_ClearFlag(DMA1_FLAG_TC6);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  //ʹ�ܴ���2��DMA����
}
	

/**
 * @brief  �ó�����������ļ��ٶ�
 * @param  
 * @retval None
 */
void Axis_DataTransfrom(void)
{
	int i, j;
	for(i = 0 ; i < 3; i++)
	{			
		if(MPU_Data[i * 11 + 1] == 0x51)
		{
			for(j = 0; j < 3; ++j)
				Axis[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 16;
				
			return;
				
//			Axis[0] = ((short)(MPU_Data[3 + i * 11] << 8 | MPU_Data[2 + i * 11])) / 32768.0 * 16;		//X����ٶ�	 
//			Axis[1] = ((short)(MPU_Data[5 + i * 11] << 8 | MPU_Data[4 + i * 11])) / 32768.0 * 16;    	//Y����ٶ�
//			Axis[2] = ((short)(MPU_Data[7 + i * 11] << 8 | MPU_Data[6 + i * 11])) / 32768.0 * 16;    	//Z����ٶ�		
		}
	}
}

/**
 * @brief  �ó�����������Ľ��ٶ�
 * @param  
 * @retval None
 */
void Angular_DataTransFrom(void)
{
	int i, j;
	for(i = 0 ; i < 3; i++)
	{			
		if(MPU_Data[i * 11 + 1] == 0x52)
		{
			for(j = 0; j < 3; ++j)
				Angular[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 2000;
					
		return;
					
//			Angular[0] = ((short)(MPU_Data[3 + i * 11] << 8 | MPU_Data[2 + i * 11])) / 32768.0 * 2000;		//X����ٶ�	 
//			Angular[1] = ((short)(MPU_Data[5 + i * 11] << 8 | MPU_Data[4 + i * 11])) / 32768.0 * 2000;		//Y����ٶ�
//			Angular[2] = ((short)(MPU_Data[7 + i * 11] << 8 | MPU_Data[6 + i * 11])) / 32768.0 * 2000;		//Z����ٶ�		
		}
	}
}

/**
 * @brief  �������������ĽǶ�
 * @param  
 * @retval None
 */
void Angle_DataTransfrom(void)
{
	int i, j;
	for(i = 0; i < 3; i++)
	{
		if(MPU_Data[i * 11 + 1] == 0x53)
		{
			for(j = 0; j < 3; ++j)
				Angle[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 180;
					
			return;
//			Angle[0] = ((short)(MPU_Data[3 + i * 11] << 8) | MPU_Data[2 + i * 11]) / 32768.0 * 180;	//X��Ƕ�
//			Angle[1] = ((short)(MPU_Data[5 + i * 11] << 8) | MPU_Data[4 + i * 11]) / 32768.0 * 180;	//Y��Ƕ�
//			Angle[2] = ((short)(MPU_Data[7 + i * 11] << 8) | MPU_Data[6 + i * 11]) / 32768.0 * 180;	//Z��Ƕ�
		}
	}
}

/**
 * @brief  �����õļ��ٶȣ�ȥ����׼ֵ
 * @param  
 * @retval None
 */
void Axis_GetFinalData(void)
{
	/*�ֱ��ý��ٶȺͼ��ٶȻ�׼ֵ*/
	Axis_DataTransfrom();
	Angle_DataTransfrom();
	Angular_DataTransFrom();
	
	/*������ٶȵĻ�׼ֵ*/
	DealWithRotation();
}

/**
 * @brief  		ͨ�������ٶȵľ��ұ仯���ж��Ƿ�������
 * @param  		void
 * @retval 		TRUE����ʾ���������ˣ�FALSE����ʾ��û�з�������
 */
bool IsAccident(void)
{
	double a;
	Axis_GetFinalData();	//�ռ�����
	if(fabs(Axis[0]) > 1.0 || fabs(Axis[1]) > 1.0 || fabs(Axis[2]) > 1.0)
	{
		if((a = (Square(Axis[0]) + Square(Axis[1]) + Square(Axis[2]))) > 2)
		{
				printf("a = %f", a);	
				return TRUE;
		}
	}
	else if(fabs(Angular[2]) > 800.0)
	{
		return TRUE;
	}
	
	return FALSE;
}

/**
 * @brief  		����X��Y��Z����ת��õ���ת����
 * @param  		matrix����ת���󣬴洢��ת�������
 * 				angleIndegree���Ƕȣ�����֪�Ķ�����ʽ��ʾ����Ҫת��Ϊ�� / n
 *				axis��	��ʾ�����ĸ���ת
 */
#define MATRIX( row, col ) matrix[ row * 3 + col ]
static void Rotate(float matrix[], float angleInDegree, float x, float y, float z)
{
	float alpha = angleInDegree / 180.0 * PI;	//���Ƕ�ת��Ϊ�� / n�ĸ�ʽ
	float Sin = sin(alpha);						
	float Cos = cos(alpha);						
	float translateValue = 1.0 - Cos;			//����ϵ��ֵ

	MATRIX(0, 0) = translateValue * x * x + Cos;
	MATRIX(0, 1) = translateValue * x * y + Sin * z;
	MATRIX(0, 2) = translateValue * x * z - Sin * y;
	MATRIX(1, 0) = translateValue * x * y - Sin * z;
	MATRIX(1, 1) = translateValue * y * y + Cos;
	MATRIX(1, 2) = translateValue * y * z + Sin * x;
	MATRIX(2, 0) = translateValue * x * z + Sin * y;
	MATRIX(2, 1) = translateValue * y * z - Sin * x;
	MATRIX(2, 2) = translateValue * z * z + Cos;
}

/**
 * @brief  		���վ���˷����о����ҳ˻�ø�������ת���λ��
 * @param  		matrix����ת���󣬴洢��ת�������
 *				coord��������󣬳�ʼʱ��λ������
 */
static void GetAcceleration(float matrix[9], float originCoord[3], float finalCoord[3])
{
	int i, j;
	for(i = 0; i < 3; ++i)
	{
		finalCoord[i] = 0.0;
		for(j = 0; j < 3; ++j)//�����ҳ�
		{
			finalCoord[i] += originCoord[j] * MATRIX(j, i);
		}
	}
}

/**
 * @brief  		������ת����ٶȵ�����ƽ������
 * @param  		void
 */
void DealWithRotation(void)
{
	float matrix[9] = {0.0};
	float originCoord[3];
	int i, j;
	
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
			originCoord[j] = 0.0;
		
		originCoord[i] = 1.0;
		
		Rotate(matrix, Angle[0], 1.0, 0.0, 0.0);
		GetAcceleration(matrix, originCoord, FinalBaseAxis);	//���ν���õľ����е�ֵ�����ҳ�
		Rotate(matrix, Angle[1], 0.0, 1.0, 0.0);
		GetAcceleration(matrix, FinalBaseAxis, originCoord);	//������ԭ���Ƿ���洢���޸ģ���Ϊÿһ�ο��Խ���õ���Ϊ��һ�εĲ���
		Rotate(matrix, Angle[2], 0.0, 0.0, 1.0);
		GetAcceleration(matrix, originCoord, FinalBaseAxis);
		
		Axis[i] -= FinalBaseAxis[2];	//��ȥZ���(��Ϊ����������ļ��ٶ�)
	}
}
