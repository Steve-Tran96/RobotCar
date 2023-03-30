#include "include.h"
static bool fUartRxComplete = FALSE;
static uint8 UartRxBuffer[50];
uint8 Uart1RxBuffer[50];
static bool UartBusy = FALSE;
uint8 frameIndexSumSum[256];

void InitUart1(void)
{
  
  bitSet(UCSR0B,RXCIE0);//
  bitSet(UCSR0B,RXEN0); //			
  bitSet(UCSR0B,TXEN0); //
  bitSet(UCSR0C,UCSZ01);//
  bitSet(UCSR0C,UCSZ00);//
  UBRR0=(F_CPU/16/9600-1);//
}

void Uart1SendData(BYTE dat)
{
	loop_until_bit_is_set(UCSR0A,UDRE0);//
	UDR0=dat;
}

void UART1SendDataPacket(uint8 dat[],uint8 count)
{
	uint8 i;
	for(i = 0; i < count; i++)
	{
		Uart1SendData(dat[i]);
	}
}

static bool UartRxOK(void)
{
	if(fUartRxComplete)
	{
		fUartRxComplete = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}





