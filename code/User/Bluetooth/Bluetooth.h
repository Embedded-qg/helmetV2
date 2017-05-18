#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "usart.h"
#include <stdio.h>
#include "stm32f10x.h"



#define BLUETOOTH_DEBUG_ON 0

#if BLUETOOTH_DEBUG_ON
	#define BLUETOOTH_DEBUG(fmt,args...) printf (fmt ,##args)
#else
	#define BLUETOOTH_DEBUG(fmt,args...)
#endif

/**
 * @brief  �����źŴ�����
 * @param  
 * @retval 1��ʾ����������0��ʾ�󴥷�
 */
int CrashFunction(void);


void SendFromMPUtoBluetooth(char *data);


static void SendImageSliceToAPP(u16 *dat, u32 size, u8 hasSliced);
	
#endif
