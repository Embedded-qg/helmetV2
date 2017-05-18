#include"spi.h"
#include"usart.h"
#include"delay.h"
void SPI1_Init(void)
{
	SPI_InitTypeDef	SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;                   
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PA4 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	GPIO_SetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����˫��ȫ˫��SPI����ģʽ
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����Ϊ��SPI
	SPI_InitStructure.SPI_CPHA  = SPI_CPHA_2Edge;//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ʱ�ӵ���̬��ʱ�����ո�
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//���ͽ���8λ֡����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1,&SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);//ʹ��SPI1����
}

u8 SPI1_ReadWriteByte(u8 txData)
{
		while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET) 
			;//���ͻ���ձ�־λ
		SPI_I2S_SendData(SPI1,txData);//ͨ��SPI����һ������
		while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET)
			  ;//���ջ���ǿձ�־λ
	  return SPI_I2S_ReceiveData(SPI1);
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); 
} 
