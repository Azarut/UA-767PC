/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "init.h" 
#include "Board_LED.h"  

#define CRC16_INITIAL_REMAINDER 0x0000
#define CRC16_FINAL_XOR_VALUE   0x0000
#define CRC16_WIDTH (8 * sizeof(uint16_t))
#define Green_On LED_On(7)	
#define Green_Off LED_Off(7)	
#define Red_On LED_On(5)	
#define Red_Off LED_Off(5)

uint16_t Systolic = 0;
uint16_t Dyatolic = 0;
uint16_t Pulse = 0;

uint8_t Led_State = 0;

int16_t  Version = 0x0003;
int32_t  TransmitterID = 123456789;
int16_t  MessageType = 3;
int16_t  DataType = 3082;
uint32_t DeltaTime = 1000;
uint16_t SystolicPressure = 190;
uint16_t DiastolicPressure = 100;
uint16_t PulseCnt = 140;
uint8_t aTCP_Buffer[22];

uint8_t need_to_resend = 0;
DMA_InitTypeDef dma_1;
DMA_InitTypeDef dma_3;
uint16_t CRC_calc = 0;
void send_str(uint8_t string[], uint8_t lenghth);

/*
 * main: initialize and start the system
 */
/*    АТ-комманды SIM800       */
uint8_t ECHO[5] = "ATE0\r"; 
uint8_t SET_SMS_OP_1[10] = "AT+CMGF=1\r"; 
uint8_t SET_SMS_OP_2[15] = "AT+CSCS= \"GSM\"\r"; 
uint8_t READ_SMS[10] = "AT+CMGR=1\r"; 
uint8_t DEL_SMS[19] = "AT+CMGDA=\"DEL ALL\"\r";
uint8_t APN[19] = "AT+CSTT=\"internet\"\r"; 
uint8_t CHK_CONNECT[9] =  "AT+CIICR\r";
uint8_t CHK_IP[9] = "AT+CIFSR\r";
uint8_t SRV_CONNECT[43] = "AT+CIPSTART=\"TCP\",\"054.203.229.250\",\"8081\"\r";
uint8_t SEND_DATA[11] = "AT+CIPSEND\r";
uint8_t CHK_GPRS[10] = "AT+CGATT?\r";
uint8_t GPRS_CONNECT[11] = "AT+CGATT=1\r";
uint8_t END_LINE[1] = {0x1A};
/*******************************/

const uint16_t crcTable[256] =
    { 0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
      0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
      0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
      0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
      0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
      0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
      0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
      0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
      0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
      0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
      0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
      0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
      0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
      0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
      0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
      0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
      0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
      0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
      0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
      0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
      0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
      0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
      0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
      0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
      0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
      0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
      0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
      0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
      0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
      0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
      0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
      0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202 
		};

uint16_t crc16(const void * const message, const uint16_t nBytes) 
{
    uint8_t  data;
    uint16_t byte, remainder = CRC16_INITIAL_REMAINDER;

    for (byte = 0; byte < nBytes; ++byte) {
        data = ((const uint8_t * const) message)[byte] ^ (remainder >> (CRC16_WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }

    return (remainder ^ CRC16_FINAL_XOR_VALUE);
}
void Rx_Blink (void const *argument)
{
while(1)
{
	switch(Led_State)
	{
		case 1: 
			Green_On;
			osDelay(33);
		  Green_Off;     // 
		  osDelay(300);
			break;
		case 2: 
			Red_On;
			osDelay(33);
		  Red_Off;
		  osDelay(1000);
			break;
		case 3: 
			Red_On;
			osDelay(33);
			Red_On;
			osDelay(33);
			Red_On;
			osDelay(33);
			Red_On;
			osDelay(33);
			Red_On;
			osDelay(33);
		  Red_Off;
		  osDelay(500);
			break;
		case 4: 
			Green_On;
			osDelay(1000);
		  Green_Off;
			Led_State = 0;
			break;
		case 5: 
			Red_On;
			osDelay(1000);
		  Red_Off;
			Led_State = 0;
		break;
		default:
			 Green_Off;
			 Red_Off;
			break;		
	}
}
}	
void RX_Clear(void)
{
	DMA_InitTypeDef dma_2;
	RX_Buffer[0] = 0;
	DMA_DeInit(DMA1_Channel6);
	DMA_StructInit(&dma_2);
	dma_2.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->RDR);
	dma_2.DMA_MemoryBaseAddr = (uint32_t)RX_Buffer;
	dma_2.DMA_BufferSize = 48;
	dma_2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_2.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_2.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init(DMA1_Channel6, &dma_2);	
	DMA_Cmd(DMA1_Channel6, ENABLE);	
}

uint8_t CheckCRC(uint8_t CheckBuf[], uint8_t size)
{ uint8_t good = 1; uint16_t tempCRC = 0;
  
	tempCRC = (CheckBuf[size-2] <<0x08) + CheckBuf[size-1];
	CRC_calc = crc16((uint32_t*)CheckBuf, size-2);
	if(CRC_calc == tempCRC) 
		good = 1;
	else
		good = 0;
	if(good) 
		return(1);
	else
		return(0);
}


void CheckError(void)
{
	if(stringtoreceive[2] == 'E')
			need_to_resend = 1;
}
void Init_SIM800(void)
{
					
			osDelay(5000);
		  send_str(ECHO, 5);
		  osDelay(1000);
		  send_str(SET_SMS_OP_1, 10);
		  osDelay(1000);
		  send_str(SET_SMS_OP_2, 15);;
		  osDelay(1000);
		  send_str(DEL_SMS, 19);
		  osDelay(1000);
		  send_str(CHK_GPRS, 10);
		  osDelay(1000);
	    send_str(GPRS_CONNECT, 11);
		  osDelay(1000);
      send_str(APN, 19);
		  osDelay(1000);
	    send_str(CHK_CONNECT, 9);
		  osDelay(1000);
	    send_str(CHK_IP, 9);
		  osDelay(1000);
	    send_str(SRV_CONNECT, 43);
		  osDelay(5000);
}
void UART2_DMA_SEND(uint8_t string[], uint8_t lenghth)
{
	DMA_DeInit(DMA1_Channel7);

	dma_1.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->TDR);
	dma_1.DMA_MemoryBaseAddr = (uint32_t)string;
	dma_1.DMA_DIR = DMA_DIR_PeripheralDST;
	dma_1.DMA_BufferSize = lenghth;
	dma_1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init(DMA1_Channel7, &dma_1);
	DMA_Cmd(DMA1_Channel7, ENABLE);
}
void send_str(uint8_t string[], uint8_t lenghth) 
{
	DMA_DeInit(DMA1_Channel4);

	dma_1.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->TDR);
	dma_1.DMA_MemoryBaseAddr = (uint32_t)string;
	dma_1.DMA_DIR = DMA_DIR_PeripheralDST;
	dma_1.DMA_BufferSize = lenghth;
	dma_1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init(DMA1_Channel4, &dma_1);
	DMA_Cmd(DMA1_Channel4, ENABLE);

	DMA_DeInit(DMA1_Channel5);
	DMA_StructInit(&dma_3);
	dma_3.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->RDR);
	dma_3.DMA_MemoryBaseAddr = (uint32_t)stringtoreceive;
	dma_3.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma_3.DMA_BufferSize = 32;
	dma_3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_3.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_3.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init(DMA1_Channel5, &dma_3);	
	DMA_Cmd(DMA1_Channel5, ENABLE);		
} 

void ReadADTask (void const *argument)
{ uint8_t i = 3;
	while(1)
	{
		if(RX_Buffer[1] == 0x30)
		{
			for(i = 2; i<10; i++)
			{
				if(RX_Buffer[i] <= '9')
					RX_Buffer[i] -= 0x30;
				else
					RX_Buffer[i] -= 0x37;
			}
			Systolic = (RX_Buffer[2] << 4) + RX_Buffer[3];
			Dyatolic = (RX_Buffer[4] << 4) + RX_Buffer[5];
			Pulse = (RX_Buffer[6] << 4) + RX_Buffer[7];
			Systolic += Dyatolic;
			aTCP_Buffer[15] = (uint8_t)Systolic;
			aTCP_Buffer[14] = Systolic >> 8;
			aTCP_Buffer[17] = (uint8_t)Dyatolic;
			aTCP_Buffer[16] = Dyatolic >> 8;
			aTCP_Buffer[19] = (uint8_t)Pulse;
			aTCP_Buffer[18] = Pulse >> 8;
			CRC_calc = crc16((uint32_t*)aTCP_Buffer, 20);
			aTCP_Buffer[21] = (uint8_t)CRC_calc;
			aTCP_Buffer[20] =  CRC_calc >> 8;
			Led_State = 1;
			send_str(SEND_DATA, 11);
		  osDelay(1000);
	    send_str(aTCP_Buffer, 22);
		  osDelay(1000);
	    send_str(END_LINE, 1);
			Led_State = 0;
			osDelay(1000);
			if(stringtoreceive[2] == 'S') Led_State = 4;
			if(stringtoreceive[2] == 'E') Led_State = 5;
			RX_Clear();
		}
		else if(RX_Buffer[1] != '1') RX_Clear();
		osDelay(1000);
	}
}

void MainTask (void const *argument)
{
Init_SIM800();
aTCP_Buffer[1] = (uint8_t)Version;
aTCP_Buffer[0] = Version >> 8;
aTCP_Buffer[5] = (uint8_t)TransmitterID;
aTCP_Buffer[4] = TransmitterID >> 8;
aTCP_Buffer[3] = TransmitterID >> 16;
aTCP_Buffer[2] = TransmitterID >> 24;
aTCP_Buffer[7] = (uint8_t)MessageType;
aTCP_Buffer[6] =  MessageType >> 8;
aTCP_Buffer[9] = (uint8_t)DataType;
aTCP_Buffer[8] =  DataType >> 8;
aTCP_Buffer[13] = (uint8_t)DeltaTime;
aTCP_Buffer[12] = DeltaTime >> 8;
aTCP_Buffer[11] = DeltaTime >> 16;
aTCP_Buffer[10] = DeltaTime >> 24;

//	aTCP_Buffer[20] = (uint8_t)Version >> 8;
//	aTCP_Buffer[21] = (uint8_t)Version >> 8;
tid_ReadADTask = osThreadCreate (osThread(ReadADTask), NULL);
tid_Rx_Blink = osThreadCreate (osThread(Rx_Blink), NULL);
while(1)
{
	//send_str("Hello world\n", 12);
}
}





int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  SystemInit();
	LED_Initialize ();
	Configure_GPIO_USART1();
	Configure_GPIO_USART2();
	Configure_DMA1();
	Configure_USART1();	
	Configure_USART2();
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	
	tid_MainTask = osThreadCreate (osThread(MainTask), NULL);
	
	osKernelStart ();                         // start thread execution 
}
