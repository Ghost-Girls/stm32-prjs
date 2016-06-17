/*
* STM32F1_USART1.c
*
*  Created on: 2013-11-13
*      Author: Administrator
*/



#include "../src/conf.h"

#define USE_REMAP		0//����1��ӳ�� 0:����ӳ��;1:��ӳ��
#define RX_BUF_SIZE  0x100
#define PRINTF_BUF_SIZE  0x200


static uint8_t USART1_RxBuffer[RX_BUF_SIZE];//����1�ڲ����ջ���
//static uint8_t USART1_TxBuffer[0x100];
static uint8_t print_buffer[PRINTF_BUF_SIZE];//��ӡ����

static volatile uint32_t USART1_ReadIndex = 0;//������
static volatile uint8_t sendDoneFlag = 1;//������ɱ�־
static volatile uint8_t recvDoneFlag = 0;//������ɱ�־

/**
* ���ڳ�ʼ��
* @param baud ������
*/
void USART1_Configuration(uint32_t baud)
{
    
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_USART1|
                               RCC_APB2Periph_AFIO,ENABLE);
    
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#if USE_REMAP
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
    
    
	DMA_InitTypeDef DMA_InitStructure;
    
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART1_RxBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(USART1_RxBuffer);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);
    
    
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART1_TxBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//	DMA_InitStructure.DMA_BufferSize = sizeof(USART1_TxBuffer);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}


/**
* ���ڷ��ͺ���
* @param buffer	��������
* @param length	���ͳ���
* @return
*/
uint16_t USART1_SendBuffer(const uint8_t* buffer, uint16_t length)
{
	if( (buffer==0) || (length==0) )
	{
		return 0;
	}
	sendDoneFlag = 0;
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA1_Channel4->CMAR = (uint32_t)buffer;
	DMA_SetCurrDataCounter(DMA1_Channel4, length);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	return length;
}

/**
* ������ɱ�־��ѯ
* @return	1:������� 	0:δ���
*/
uint8_t isUSART1SendDone(void)
{
	return sendDoneFlag;
}


/**
*Printf��ӡ����
* @param format	Printf��ʽ
*/
void Debug_Printf(const char *format, ...)
{
	uint32_t length;
	va_list args;
	va_start(args, format);
	length = vsnprintf((char*)print_buffer, sizeof(print_buffer), (char*)format, args);//��ʽ������
	va_end(args);
	USART1_SendBuffer(print_buffer,length);//����
	while(!sendDoneFlag);//�ȴ��������
}


/**
* ��ӡ16��������
* @param hex			����
* @param hex_length	���鳤��
*/
void Printf_Hex(const uint8_t* hex, uint16_t hex_length)
{
	const uint8_t char_table[] = "0123456789ABCDEF";
	uint16_t j=0;
	for(uint16_t i=0;(i<hex_length)&&j<sizeof(print_buffer);i++)
	{
		print_buffer[j++] = char_table[(hex[i]&0xF0)>>4];
		print_buffer[j++] = char_table[hex[i]&0x0F];
		print_buffer[j++] = ' ';
	}
	print_buffer[j++] = '\n';
	USART1_SendBuffer(print_buffer,j);//����
	while(!sendDoneFlag);//�ȴ��������
}

/**
* ��������ʣ������ѯ
* @return	 ʣ����
*/
uint32_t USART1_DataAvailable(void)
{
    uint32_t remain_length;
    uint32_t write_index;
    write_index = sizeof(USART1_RxBuffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
    
    if( USART1_ReadIndex > write_index )
    {
        remain_length = (sizeof(USART1_RxBuffer) - USART1_ReadIndex) + write_index;
    }
    else
    {
        remain_length = write_index - USART1_ReadIndex;
    }
    return remain_length;
    
}


/**
* ���ڽ��պ���
* @param buffer		���ջ���
* @param max_length	���ջ���Ĵ�С,��max_lengthС�����ݵĳ���,�������ɱ�־������0
* @return ���յ����ݵĳ���
*/
uint32_t USART1_RecvBuffer(uint8_t* buffer, uint32_t max_length)
{
    uint32_t recv_length;
    uint32_t write_idx;
    if((buffer==0) || (max_length==0))
    {
        return 0;
    }
    write_idx = sizeof(USART1_RxBuffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
    if(USART1_ReadIndex<write_idx)//������յ��ĳ���
    {
        recv_length = write_idx - USART1_ReadIndex;
    }
    else
    {
        recv_length = sizeof(USART1_RxBuffer) - USART1_ReadIndex + write_idx;
    }
    for(uint16_t i=0;(i<recv_length) && (i<max_length);i++)//��ȡ��������
    {
        if( (USART1_ReadIndex+i) == sizeof(USART1_RxBuffer))
        {
            USART1_ReadIndex = 0;
        }
        buffer[i] = USART1_RxBuffer[USART1_ReadIndex];
        USART1_ReadIndex++;
    }
    if(USART1_DataAvailable()==0)//�Ƿ��RxBuffer��ȡ������?
    {
        recvDoneFlag = 0;//�����Ѷ���,�������ɱ�־
    }
    return recv_length;
}


/**
* ������ɱ�־��ѯ
* @return	1:�������	0:δ���
*/
uint8_t isUSART1RecvDone(void)
{
	return recvDoneFlag;
}

/**
* ��մ��ڽ��ջ���
* @return	���ڽ��ջ���������ݵĳ���
*/
uint32_t USART1_Flush(void)
{
	uint32_t flush_length;
	uint32_t write_index;
    
	write_index = sizeof(USART1_RxBuffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
    
	if( USART1_ReadIndex > write_index )
	{
		flush_length = (sizeof(USART1_RxBuffer) - USART1_ReadIndex) + write_index;
	}
	else
	{
		flush_length = write_index - USART1_ReadIndex;
	}
    
	USART1_ReadIndex = write_index;
    recvDoneFlag = 0;//�������ɱ�־
	return flush_length;
}


/**
* �����ж�
*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);//����
		USART_ReceiveData(USART1);//�ն���״̬
		recvDoneFlag = 1;
	}
}


/**
* ����1DMA��������ж�
*/
void DMA1_Channel4_IRQHandler(void)
{
	if ( DMA_GetITStatus(DMA1_IT_TC4) )
	{
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		sendDoneFlag = 1;
	}
}
