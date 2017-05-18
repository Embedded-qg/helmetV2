/***************************************************************************************
*        File Name              :	spi.h
*        CopyRight              :	Zoisite
*        ModuleName             :	sccb
*
*        CPU                    :	Intel i5-4210u
*        RTOS                   :	Windows 10 pro
*
*        Create Data            :	
*        Author/Corportation    : Shengdui/Zoisite
*
*        Abstract Description   :	�ṩSPI�ӿ�
*
*--------------------------------Revision History--------------------------------------
*       No      version     Data        Revised By      Item        Description
*       1       v1.0        2016/10/18   Shengdui                   Create this file
*
***************************************************************************************/
/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef _SPI_H
#define _SPI_H

/**************************************************************
*        Include File Section
**************************************************************/
#include"stm32f10x.h"

/**************************************************************
*        Macro Define Section
**************************************************************/

/**************************************************************
*        Prototype Declare Section
**************************************************************/

/**
 * @brief  ��ʼ��SPI0
 * @param  
 * @retval None
 */
void SPI1_Init(void);

/**
 * @brief  ͨ��SPI���ͣ���ȡһ���ֽ�
 * @param  txData Ҫ���͵��ֽ�
 * @retval ��ȡ���ֽ�
 */
u8 SPI1_ReadWriteByte(u8 txData);

/**
 * @brief  ����SPI1��ʱ�ӷ�Ƶ
 * @param  SpeedSet ��Ƶֵ
				SPI_BaudRatePrescaler����2   2   	(SPI 36M@sys 72M)
				SPI_BaudRatePrescaler_8   8  	(SPI 9M@sys 72M)
				SPI_BaudRatePrescaler_16  16 	(SPI 4.5M@sys 72M)
				SPI_BaudRatePrescaler_256 256 	(SPI 281.25K@sys 72M)
 * @retval None
 */
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);

#endif
