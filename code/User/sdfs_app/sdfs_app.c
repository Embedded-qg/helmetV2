/**
  ******************************************************************************
  * @file    sdfs_app.c
  * @author  Shengdui.Liang
  * @version V1.1
  * @date    2016-10-25
  * @brief   �ļ�ϵͳ
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "sdfs_app.h"
#include "stm32f10x.h"
#include "sd.h"
#include "ff.h"

/* �����ļ�ϵͳ������HT���ջ�ռ��С�������Ҫ��myfat����Ϊȫ�ֵ� */
FATFS myfat;

/* Ĭ�ϵ�Ŀ¼ */
const char *root = "";


#if SDFS_APP_DEBUG_ON
/**
 * @brief 	�����ļ�ϵͳ�Ƿ���������
 * @param  
 * @retval 	None
 */
void sdfs_app_test(void)
{
	/* �ļ����� */
    FIL fil;
 
	/* ����SD�� */
	if(f_mount(&myfat, root, 0) == FR_OK)
	{
		printf("Mount SD successfully\r\n");
		if (f_open(&fil, "testfile.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
		{
			printf("Open file successfully.\r\n");
			if (f_printf(&fil, "Hello,World!\n") > 0)
				printf("Puts file successfully.\r\n");
			else
				printf("Puts file failed.\r\n");
		}
		else
			printf("Open file failed.\r\n");
		f_close(&fil);
	}
	else
		SDFS_APP_DEBUG("Mount failed\r\n");
}
#endif


/**
 * @brief 	����SD��
 * @param	
 * @retval 	1 �����ɹ�
			0 ����ʧ��
 */
int sdfs_app_mnt(void)
{
	/* ע��һ�����������߼���Ϊ1 */
	printf("sd start!!!!\n");
	if(f_mount(&myfat, root, 1) != FR_OK)
	{
		SDFS_APP_DEBUG("Mount SD failed.\r\n");
		return 1;
	}
	return 0;
}

