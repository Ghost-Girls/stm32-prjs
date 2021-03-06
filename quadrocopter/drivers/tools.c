#include "tools.h"
#include "pid.h"
#include "mpu6050.h"
#include "stm32f10x_usart.h"
#include "flow.h"
#include "stm32f10x.h"
#include "conf.h"
#include <stdarg.h>  

static uint8_t  fac_us=0;//us延时倍乘数
static uint16_t fac_ms=0;//ms延时倍乘数
float height = 0.0f;
va_list args;  
  
char sign[] = { '0','1','2','3','4','5',  
                '6','7','8','9','a','b',  
                'c','d','e','f'};  
 
void UserInit(void)
{
  RCC_Conf();
  GPIO_Conf();
  SPI_Conf();
  TIMER_Conf();
  PWM_Conf();
  USART_Conf();
  I2C_Conf();
  NVIC_Conf();

  DelayInit(72);
}
 
void State_Update(float dt)
{
  MPU6050_Read();
  MPU_GetAccValue();
  MPU_GetGyroRate();
  AHRSupdateIMU(gyro_x_rate*M_PI/180,gyro_y_rate*M_PI/180,gyro_z_rate*M_PI/180, acc_x_temp,acc_y_temp,acc_z_temp,dt);
  AHRS_GetRPY();
  /*ADNS3080_Read();*/
}

void Motor_Init(void)
{

  TIM4->CCR1=4000;
  TIM4->CCR2=4000;
  TIM4->CCR3=4000;
  TIM4->CCR4=4000;
  delay_ms(2500);
  TIM4->CCR1=2000;
  TIM4->CCR2=2000;
  TIM4->CCR3=2000;
  TIM4->CCR4=2000;
  delay_ms(2500);
}

void PID_Update(float dt)
{
  PIDx_Update(dt);
  PIDy_Update(dt);
  PIDz_Update(dt);
}

void print_int(int num, int mode, int flag)  
{  
  if(num == 0){  
    if(flag == 0){  
      UsartPut('0');  
      return;  
    }else{  
      return;  
    }  
  }  
  
  print_int(num/mode, mode, 1);  
  UsartPut(sign[ num%mode ]);  
}  
  
void print_str(char const *str)  
{  
  if(str == NULL)  
  {  
    return;  
  }  
  
  while(*str != '\0')  
  {  
    UsartPut(*str);  
    str++;  
  }  
}  
  
  
void print_float(float num)  
{  
  int part = (int)num/1;  
  
  print_int(part, 10, 0);  
  UsartPut('.');  
  part=num*1000000-part*1000000;  
  print_int(part, 10, 0);  
}  
 
// 向串口发送一个字节的数据，配合printf使用
uint8_t UsartPut(uint8_t ch)
{
	USART_SendData(USART2, (uint8_t) ch);
	//Loop until the end of transmission
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) {
	}
  return ch;
}

// 从串口接受数据
uint8_t UsartGet(void)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(USART2);
}

 
void check_type(char type)  
{  
  switch(type)
  {  
    case 'd':
    {  
      int num = va_arg(args, int);  
  
      if(num < 0){  
        UsartPut('-');  
        num = num * (-1);  
      }  
  
      print_int(num, 10, 0);  
      break;  
    }  
  
    case 'c':
    {  
      char ch = (char)va_arg(args, int);  
      UsartPut(ch);  
      break;  
    }  
    
    case 's':
    {  
      char *str = va_arg(args, char *);  
      print_str(str);  
      break;  
    }  
    
    case 'f':
    {  
      float num = (float)va_arg(args, double);  
    
      if(num < 0)  {  
        UsartPut('-');  
        num = num * (-1);  
      }  
    
      print_float(num);  
      break;  
    }  
    
    case 'p':
    {  
      int num = va_arg(args, int);  
      UsartPut('0');  
      UsartPut('x');  
      print_int(num, 16, 0);  
      break;  
    }  
    
    default:
    {  
      UsartPut('%');  
      UsartPut(type);  
    }  
  }  
}  
  
void myprintf(const char *format, ...)  
{  
  if(format == NULL){  
    return;  
  }  
    
  va_start(args, format);  
    
  while(*format != '\0'){  
    while(*format != '%' && *format != '\0'){  
      UsartPut(*format);  
      format++;         
    }  
  
    if(*format != '\0'){  
      format++;  
      if(*format != '\0'){  
        check_type(*format);  
        format++;  
      }else{  
        UsartPut(*(format-1));  
      }  
    }  
  }  
  
  va_end(args);  
}  

float HCSR04_Get(void)
{
  float length = 0.0f;

  GPIO_WriteBit(GPIOA,GPIO_Pin_1,1);
  delay_us(20);
  GPIO_WriteBit(GPIOA,GPIO_Pin_1,0);
  //计数器清0
  TIM2->CNT = 0;
  while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && TIM2->CNT<1000);
  TIM2->CNT = 0;
  tim2_count = 0;
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && tim2_count < 9);
  length = (tim2_count*0xffff+TIM2->CNT)/58.8;
  height = length;
  return length;
}

//初始化延迟函数
void DelayInit(u8 SYSCLK)
{ 
  fac_us=SYSCLK/8;		//不论是否使用ucos,fac_us都需要使用
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数
}

int get_tick_count(unsigned long *count)
{
  count[0] = SysTick->CTRL;
	return 0;
}

//延时nus
//nus为要延时的us数.
void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD=nus*fac_us; //时间加载
	SysTick->VAL=0x00;    //清空计数器
	SysTick->CTRL=0x01 ;    //开始倒数
	do {
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL=0x00;     //关闭计数器
	SysTick->VAL =0X00;     //清空计数器
}

//延时nms
void delay_ms(u16 nms)
{
	u32 temp;
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
  SysTick->VAL=0x00;    //清空计数器
	SysTick->CTRL=0x01 ;    //开始倒数
	do {
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL=0x00;     //关闭计数器
	SysTick->VAL =0x00;       //清空计数器
}
