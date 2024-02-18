#ifndef __CAW_CONFIG_H__
#define __CAW_CONFIG_H__

#define USE_STM32_UART  // 使能STM32 UART进行通讯（不用请注释掉此行）
#define USE_IT          // 使能中断

#ifdef USE_STM32_UART
#define UART_RECV_BUF_SIZE 256
#include <usart.h>
#define UART_HANDLE &huart1  // 配置UART
#endif

#endif