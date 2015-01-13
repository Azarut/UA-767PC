#include "init.h"

uint8_t stringtosend[] = "DMA\n";
uint8_t stringtoreceive[32] = {0};
uint8_t RX_Buffer[48] = {0};
uint8_t TX_Buffer[904] = {0};

void Configure_GPIO_LED(void)
{
//  /* (1) Enable the peripheral clock of GPIOA and GPIOB */
//  /* (2) Select output mode (01) on GPIOA pin 5 */
//  /* (3) Select output mode (01) on GPIOB pin 4 */
//  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN; /* (1) */  
//  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) 
//               | (GPIO_MODER_MODE5_0); /* (2) */  
//  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE4)) 
//               | (GPIO_MODER_MODE4_0); /* (3) */  
}

void Configure_GPIO_USART2(void)
{GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
}

void Configure_GPIO_USART1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/*Включаем тактирование USART1*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/*Настраиваем порты USART1*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_7);
}

void Configure_USART1(void)
{USART_InitTypeDef UART_Config;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/*Настраиваем параметры USART1 и запускаем*/
	UART_Config.USART_BaudRate = 115200;
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	UART_Config.USART_Parity = USART_Parity_No;
	UART_Config.USART_StopBits = USART_StopBits_1;
	UART_Config.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &UART_Config);
	USART_Cmd(USART1, ENABLE);
	
}

void Configure_USART2(void)
{USART_InitTypeDef UART_Config;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	UART_Config.USART_BaudRate = 9600;
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	UART_Config.USART_Parity = USART_Parity_No;
	UART_Config.USART_StopBits = USART_StopBits_2;
	UART_Config.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &UART_Config);
	USART_Cmd(USART2, ENABLE);
}

void Configure_DMA1(void)
{DMA_InitTypeDef dma;
	
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
 
DMA_StructInit(&dma);
dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->TDR);
dma.DMA_MemoryBaseAddr = (uint32_t)stringtosend;
dma.DMA_DIR = DMA_DIR_PeripheralDST;
dma.DMA_BufferSize = 15;
dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_Init(DMA1_Channel4, &dma);	
//DMA_Cmd(DMA1_Channel4, ENABLE);	
	
DMA_StructInit(&dma);
dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->RDR);
dma.DMA_MemoryBaseAddr = (uint32_t)stringtoreceive;
dma.DMA_DIR = DMA_DIR_PeripheralSRC;
dma.DMA_BufferSize = sizeof(stringtoreceive);
dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_Init(DMA1_Channel5, &dma);	
DMA_Cmd(DMA1_Channel5, ENABLE);	

DMA_StructInit(&dma);
dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->RDR);
dma.DMA_MemoryBaseAddr = (uint32_t)RX_Buffer;
dma.DMA_DIR = DMA_DIR_PeripheralSRC;
dma.DMA_BufferSize = 48;
dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_Init(DMA1_Channel6, &dma);	
DMA_Cmd(DMA1_Channel6, ENABLE);	

DMA_StructInit(&dma);
dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->TDR);
dma.DMA_MemoryBaseAddr = (uint32_t)TX_Buffer;
dma.DMA_DIR = DMA_DIR_PeripheralDST;
dma.DMA_BufferSize = 10;
dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_Init(DMA1_Channel7, &dma);	
//DMA_Cmd(DMA1_Channel7, ENABLE);

}

//void DMA1_Channel2_3_IRQHandler(void)
//{
////  if((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2)
////  {
////    DMA1->IFCR = DMA_IFCR_CTCIF2;/* Clear TC flag */
////  }
////  else if((DMA1->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3)
////  {
////    DMA1->IFCR = DMA_IFCR_CTCIF3;/* Clear TC flag */
////    DMA1_Channel3->CCR &=~ DMA_CCR_EN;
////    DMA1_Channel3->CNDTR = sizeof(stringtoreceive);/* Data size */
////    DMA1_Channel3->CCR |= DMA_CCR_EN;
////  }
////  else
////  {
////    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);/* Disable DMA1_Channel2_3_IRQn */
////  }
//}

//void DMA1_Channel4_5_6_7_IRQHandler(void)
//{
////	if((DMA1->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4)
////  {
////    DMA1->IFCR = DMA_IFCR_CTCIF4;/* Clear TC flag */
////  }
////	else if((DMA1->ISR & DMA_ISR_TCIF5) == DMA_ISR_TCIF5)
////  {
////    DMA1->IFCR = DMA_IFCR_CTCIF5;/* Clear TC flag */
////    DMA1_Channel5->CCR &=~ DMA_CCR_EN;
////    DMA1_Channel5->CNDTR = sizeof(RX_Buffer);/* Data size */
////    DMA1_Channel5->CCR |= DMA_CCR_EN;
////  }
////  else
////  {
////    NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);/* Disable DMA1_Channel2_3_IRQn */
////  }
//}
