#include "sd.h"
#include "usart.h"

u8 SD_Type = SD_TYPE_ERR;			/* ���SD�������� */
u8 buf[SECTOR_SIZE];	/* SD���ݻ����� */

/**
 * @brief  ��SPI����Ϊ����ģʽ
 * @param  
 * @retval None
 */
static void SD_SPI_SetLowSpeed(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_256);	//���õ�����ģʽ	
}


/**
 * @brief  ��SPI����Ϊ����ģʽ
 * @param  
 * @retval None
 */
static void SD_SPI_SetHighSpeed(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);	//���õ�����ģʽ	
}


/**
 * @brief		�ȴ�SD��׼����
 * @param  
 * @retval 	1 ��׼��
					  0 �ȴ���ʱ
 */
static u8 SD_WaitReady(void)
{
	u32 t = 0;
	while(++t < 0XFFFFFF)	//�ȴ� 
		if(SD_SPI_ReadWriteByte(SD_DUMMY_BYTE) == SD_DUMMY_BYTE)	return 1;//���߿���		  	
	return 0;
}


/**
 * @brief	ȡ��Ƭѡ��SD��
 * @param  
 * @retval 	None
 */
void SD_DisSelect(void)
{
	/* ȡ��Ƭѡ�豸 */
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	//�ṩ�����8��ʱ��
 	SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);	
}


/**
 * @brief	Ƭѡ��SD����ʹ�乤��
 * @param  
 * @retval 	1 ѡ�гɹ�
			0 ѡ��ʧ��
 */
u8 SD_Select(void)
{	
	/* Ƭѡ�豸 */
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	if(SD_WaitReady() == 1) return 1;
	SD_DisSelect();
	return 0;
}

	/**
 * @brief  	��SD������һ������
 * @param	  cmd ���� 
			      arg �������
			      crc crcУ��ֵ
 * @retval 	SD�����ص���Ӧ
 */
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
	u8 r1;	
	u8 retry = 0; 
	SD_DisSelect();		//ȡ���ϴ�Ƭѡ
	SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
	if(!SD_Select())	//����Ƭѡ
		return SD_DUMMY_BYTE;	//Ƭѡʧ��
	//�����������Լ�У���
	SD_SPI_ReadWriteByte(cmd | 0x40);	//8λ������λ��Ϊ1
  SD_SPI_ReadWriteByte(arg >> 24);	//��32λ�Ĳ�����ֳ�4�ֽڣ���һ����
  SD_SPI_ReadWriteByte(arg >> 16);
  SD_SPI_ReadWriteByte(arg >> 8);
  SD_SPI_ReadWriteByte(arg);	  
  SD_SPI_ReadWriteByte(crc); 			//8λУ���
	
	if(cmd == CMD12)	//����ֹͣ����ӷ���������
		SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);	
	
    //�ȴ���Ӧ����ʱ�˳�
	retry = 0X1F;
	do
	{
		r1 = SD_SPI_ReadWriteByte(0xFF);
	}while((r1 & 0X80) && retry--);	 //r1���λ��Ϊ1������Ӧ�������ȴ�
	return r1;
}


/**
 *  @name SD_GetResponse
 *	@description �ȴ�SD����Ӧ
 *	@param response Ҫ��õĻ�Ӧ
 *	@return	SD_RESPONSE_NO_ERROR �ɹ��õ���Ӧ
			SD_RESPONSE_FAILURE �ȴ���Ӧ��ʱʧ�ܻ��Ӧ��ƥ��
 *  @notice
 */
u8 SD_GetResponse(u8 response)
{
	u16 Count = 0xFFFF;//�ȴ�����	   						  
	while((SD_SPI_ReadWriteByte(SD_DUMMY_BYTE) != response) && Count)	Count--;	//�ȴ��õ�׼ȷ�Ļ�Ӧ  	  
	if(Count == 0)	return SD_RESPONSE_FAILURE;		//�õ���Ӧʧ��   
	else 		return SD_RESPONSE_NO_ERROR;	//��ȷ��Ӧ
}


/**
 *  @name 			 SD_RecvData
 *	@description ��sd����ȡһ�����ȵ����ݣ����浽������buf��
 *	@param 			 buf ���ݻ�����
								 len Ҫ��ȡ�����ݳ���
 *	@return 		 1 ��ȡ�ɹ�
								 0 ��ȡʧ��	
 *  @notice
 */
u8 SD_RecvData(u8 *buf,u16 len)
{	
	//�ȴ�SD������������ʼ����
	if(SD_GetResponse(SD_BEGIN_END_FLAG) != SD_RESPONSE_NO_ERROR)		return 0;
	//��ʼ��������
  while(len--)	
	{
		*buf++ = SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
//	  printf("%x",*buf++);
	}
		//������2��αCRC��dummy CRC��
  SD_SPI_ReadWriteByte(0xFF);
	SD_SPI_ReadWriteByte(0xFF);		
  return 1;//��ȡ�ɹ�
}



/**
 *  @name SD_SendBlock
 *	@description ��sd��д��һ�����ݰ������ݣ���512�ֽ�
 *	@param 			 buf ���ݻ�����
					 			 cmd ָ��
 *	@return       1 ���ͳɹ�
	           	  	0 ����ʧ��	
 *  @notice
 */
u8 SD_SendBlock(const u8 *buf, u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady() == 0)return 0;//�ȴ�׼��ʧЧ
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++) SPI1_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
	  SD_SPI_ReadWriteByte(0xFF);//����crc
	  SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
		if((t&0x1F)!=0x05)return 2;//��Ӧ����									  					    
	}						 									  					    
  return 1;//д��ɹ�
}

//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0������
//		 1���ɹ�													   
u8 SD_GetCID(u8 *cid_data)
{
	u8 r1;	   
  //��CMD10�����CID
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����	 
  }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)	return 0;
	else return 1;
}	

//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:1��NO_ERR
//		 0������														   
u8 SD_GetCSD(u8 *csd_data)
{
	u8 r1;	 
  r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
  if(r1==0)
	{
    r1=SD_RecvData(csd_data, 16);//����16���ֽڵ����� 
  }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)	return 0;
	else return 1;
}  

//��ȡSD����������������������   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.														  
u32 SD_GetSectorCount(void)
{
  u8 csd[16];
  u32 Capacity;  
  u8 n;
	u16 csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
  if(SD_GetCSD(csd)!=0) return 0;	    
	//���ΪSDHC�����������淽ʽ����
  if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
  {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//�õ�������	 		   
  }else//V1.XX�Ŀ�
  {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�õ�������   
   }
   return Capacity;
}

/**
 * @brief  	��ʼ��SD��ʹ����빤��ģʽ
 * @param  
 * @retval 	0 ������
 */
u8 SD_Init(void)
{
	int i;
	u8 r1;		//���������Ӧ
	u16 retry;	//��ų�ʱ����
	u8 buf[4];	//��ʱ������
	SPI1_Init();
	SD_SPI_SetLowSpeed();   /*SPI����Ϊ����ģʽ��CLKʱ������ܳ���400KHz */
	for(i = 0; i < 15; ++i)		SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);   /*�ϵ���ʱ����74��ʱ�����ڣ�������ʱ80��ʱ������*/
	retry = 500;
	do{
		r1 = SD_SendCmd(CMD0,0,0x95);
	}while(r1 != 0x01 && retry-- > 0);
	SD_Type = SD_TYPE_ERR; //Ĭ���޿�״̬
	if(r1 != 0x01)	return 1;
	r1 = SD_SendCmd(CMD8,0x1AA,0x87);    		//����SD��������ѹ����SD������
  if(r1 == 0x01)       //SD V2.0
	{
		for(i = 0;i < 4;i++)
				buf[i] = SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
		if(buf[2] == 0x01 && buf[3] == 0xAA)
		{
				retry = 0xFFFE;
				do{
					SD_SendCmd(CMD55,0,0x01); //����CMD55������SD������ָ������������
					r1 = SD_SendCmd(CMD41,0x40000000,0x01);
				}while(r1 && retry--);
				if(retry && SD_SendCmd(CMD58,0,0x01) == 0)
				{
						for(i = 0;i < 4; i++)
							buf[i] = SD_SPI_ReadWriteByte(0xFF); //�õ�OCRֵ
						if(buf[0] & 0x40)	 SD_Type = SD_TYPE_V2HC;  //���CCS
						else SD_Type = SD_TYPE_V2;
				}
		}
	}
	else{	//SD V1.x/ MMC	V3
		SD_SendCmd(CMD55, 0, 0X01);		//����CMD55
		r1 = SD_SendCmd(CMD41, 0, 0X01);	//����CMD41
		if(r1 <= 1)
		{		
			SD_Type = SD_TYPE_V1;
			retry = 0XFFFE;
			do{ //�ȴ��˳�IDLEģʽ
					SD_SendCmd(CMD55, 0, 0X01);	//����CMD55
					r1 = SD_SendCmd(CMD41, 0, 0X01);//����CMD41
			}while(r1 && retry--);
		}
		else{	//MMC����֧��CMD55+CMD41ʶ��
			SD_Type = SD_TYPE_MMC;//MMC V3
			retry = 0XFFFE;
			do{ //�ȴ��˳�IDLEģʽ
				r1 = SD_SendCmd(CMD1,0,0X01);//����CMD1
			}while(r1 && retry--);  
		}
		if(retry == 0 || SD_SendCmd(CMD16, 512, 0X01) != 0)		SD_Type = SD_TYPE_ERR;//����Ŀ�
	}
	SD_DisSelect();			//ȡ��Ƭѡ
	SD_SPI_SetHighSpeed();	//������������Ϊ����
	printf("SD_Type = %d\r\n",SD_Type);
	if(SD_Type)	return 0;
	if(r1)	return r1;
	return 0xaa;
}


/**
 *  @name SD_ReadDisk
 *	@description ��SD������
 *	@param		  	buf ���ݻ�����
								  sector ������
								  cnt Ҫ��ȡ��������Ŀ
 *	@return 			���������1 �ɹ���������ʧ��
 *  @notice
 */
u8 SD_ReadDisk(u8 *buf, u32 sector, u8 cnt)
{
	u8 r1;
	if(SD_Type != SD_TYPE_V2HC)	sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	if(cnt == 1)
	{
		r1 =25;
		r1 = SD_SendCmd(CMD17, sector, 0X01);//������  
		if(r1 == 0)  r1 = SD_RecvData(buf, 512);//����512����?
	}
	else
	{
		r1 = SD_SendCmd(CMD18, sector, 0X01);//����������
		do
		{
			r1 = SD_RecvData(buf, 512);//����512���ֽ�	 
			buf += 512;  
		}while(--cnt && r1 == 1); 	
		SD_SendCmd(CMD12, 0, 0X01);	//����ֹͣ����
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	
	return r1;
}


/**
 *  @name SD_WriteDisk
 *	@description дSD������
 *	@param 	buf ���ݻ�����
			sector ������
			cnt Ҫд���������Ŀ
 *	@return ���������0 �ɹ���������ʧ��
 *  @notice
 */
u8 SD_WriteDisk(const u8 *buf, u32 sector, u8 cnt)
{
	u8 r1;
	if(SD_Type != SD_TYPE_V2HC)
		sector *= 512;//ת��Ϊ�ֽڵ�ַ
	if(cnt == 1)
	{
		r1 = SD_SendCmd(CMD24,sector,0X01);//������
		if(r1 == 0)//ָ��ͳɹ�
		{
			r1 = SD_SendBlock(buf,0xFE);//д512���ֽ�	   
		}
	}
	else
	{
		if(SD_Type != SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//����ָ��	
		}
 		r1 = SD_SendCmd(CMD25,sector,0X01);//����������
		if(r1 == 0)
		{
			do
			{
				r1 = SD_SendBlock(buf,0xFC);//����512���ֽ�	 
				buf += 512;  
			}while(--cnt && r1 == 1);
			r1 = SD_SendBlock(0,0xFD);//����512���ֽ� 
		}
	}   
	SD_DisSelect();//ȡ��Ƭѡ
  return r1;
}	




/**
 * @brief 	����ʱ�˳���SD����ʼ������
 * @param 	
 * @return 	1 ��ʼ���ɹ�
			0 ��ʼ��ʧ��
 */
u8 SD_TryInit(void)
{
	int retry = 5;
	
	/* ��ʼ��SD�� */
	while(--retry > 0 && SD_Init())	//��ⲻ��SD��
	{
		SD_DEBUG("SD Card Error!Please check!\r\n");
		delay_ms(500);
	}
	if(retry)
	{
		SD_DEBUG("Init SD Card successfully!\r\n");
		return 1;
	}
	else
	{
		SD_DEBUG("failed to init SD Card .\r\n");
		return 0;
	}
}



/**
 * @brief 	�鿴ĳ������������
 * @param 	sector ������
 * @return 	None
 */
void SD_ViewSector(u8 sector)
{
	int i;

	if(!SD_ReadDisk(buf, sector, 1))
		SD_DEBUG("\r\nRead SD failed.\r\n");
	else
	{
		SD_DEBUG("\r\nView sector %d: \r\n", sector);
		for(i = 0; i < SECTOR_SIZE; ++i)
			SD_DEBUG("%x ", buf[i]);
		SD_DEBUG("\r\nSector %d END...\r\n", sector);
	}	
}

