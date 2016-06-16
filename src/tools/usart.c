#include "../main.h"

extern u16 timer_counter;
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
  USART_SendData(USARTx, (u16) ch);
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {
  }
	return ch;
}
#endif 

int HCSR04_Get(void)
{
  int length;
  
  printf("Send sound.\r\n",1);
  GPIO_WriteBit(GPIOA,GPIO_Pin_1,1);
  delay_us(20);
  GPIO_WriteBit(GPIOA,GPIO_Pin_1,0);
  //��������0
  timer_counter = 0;
  while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
  TIM_Cmd(TIM2, ENABLE);// TIM2 enable counter [����tim2����]
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && timer_counter <10000){
  }
  TIM_Cmd(TIM2, DISABLE);
  
  length=timer_counter/58.8;
  printf("trick: %d \r\nDistance:%d cm\r\n-----------------------\r\n",timer_counter,length);
  return length;
}

void USART_Conf(void)
{
  USART_InitTypeDef USART_InitStructure;
  //�������ã� ������ 115200 ����λ 8 ֹͣλ 1  ��żλ NONE  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity =  USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //��ʼ������
  USART_Init(USARTx, &USART_InitStructure);
  //��������
  USART_Cmd(USARTx, ENABLE);
}

