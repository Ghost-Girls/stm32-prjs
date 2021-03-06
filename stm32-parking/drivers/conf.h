/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_adc.h"
//#include "stm32f10x_bkp.h"
//#include "stm32f10x_can.h"
//#include "stm32f10x_cec.h"
//#include "stm32f10x_crc.h"
//#include "stm32f10x_dac.h"
//#include "stm32f10x_dbgmcu.h"
//#include "stm32f10x_fsmc.h"
//#include "stm32f10x_iwdg.h"
//#include "stm32f10x_pwr.h"
//#include "stm32f10x_rtc.h"
//#include "stm32f10x_sdio.h"
#include "stm32f10x_spi.h"
//#include "stm32f10x_wwdg.h"
//#include "stm32f10x_dma.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#include "nvic.h"

//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

/* Constant ------------------------------------------------------------------*/
//红外接口 GPIOA
#define TRIG1_Pin               GPIO_Pin_10
#define TRIG2_Pin               GPIO_Pin_11
#define TRIG3_Pin               GPIO_Pin_12
//串口通讯接口 GPIOA
#define GPIO_RxPin              GPIO_Pin_3              
#define GPIO_TxPin              GPIO_Pin_2              
//PWM接口 GPIOB
#define BUZZER_Pin              GPIO_Pin_6
#define SERVO_Pin               GPIO_Pin_7
//陀螺仪IIC接口 GPIOB
#define SDA_Pin                 GPIO_Pin_11
#define SCL_Pin                 GPIO_Pin_10
#define MPU_I2Cx                I2C2
#define I2C_Speed               100000

#define TRUE                    1
#define FALSE                   0

#define STOP_PWM                1600
#define MIN_PWM                 2000
#define MAX_PWM                 3600
#define PWM_RANGE               2000

#define M_PI  3.14159265358979323846  /* pi */
#define SYSCLK_FREQ_72MHz
#define _DLIB_PRINTF_SPECIFIER_FLOAT

/* functions -----------------------------------------------------------------*/
void USART_Conf(void);
void I2C_Conf(void);
void RCC_Conf(void);
void TIMER_Conf(void);
void GPIO_Conf(void);
void NVIC_Conf(void);
void PWM_Conf(void);
void SPI_Conf(void);
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


#endif /* __CONF_H */

