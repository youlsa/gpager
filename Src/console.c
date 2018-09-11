/*
 * console.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw_usb.h"
#include "usbd_cdc_if.h"
#include "hw.h"

//*****************************************************************************
// @BUFFER
//*****************************************************************************

#define UART_RX_BUFFER_SIZE     256

static bool g_bDisableEcho = false;
static unsigned char g_pcUARTRxBuffer[UART_RX_BUFFER_SIZE];
static volatile uint32_t g_ui32UARTRxWriteIndex = 0;
static volatile uint32_t g_ui32UARTRxReadIndex = 0;

#define RX_BUFFER_USED              (GetBufferCount(&g_ui32UARTRxReadIndex, &g_ui32UARTRxWriteIndex, UART_RX_BUFFER_SIZE))
#define RX_BUFFER_FREE              (UART_RX_BUFFER_SIZE - RX_BUFFER_USED)
#define RX_BUFFER_EMPTY             (IsBufferEmpty(&g_ui32UARTRxReadIndex, &g_ui32UARTRxWriteIndex))
#define RX_BUFFER_FULL              (IsBufferFull(&g_ui32UARTRxReadIndex, &g_ui32UARTRxWriteIndex, UART_RX_BUFFER_SIZE))
#define ADVANCE_RX_BUFFER_INDEX(n)  (n) = ((n) + 1) % UART_RX_BUFFER_SIZE

static bool IsBufferFull(volatile uint32_t *pui32Read, volatile uint32_t *pui32Write, uint32_t ui32Size)
{
	return (((*pui32Write + 1) % ui32Size) == *pui32Read);
}

static bool IsBufferEmpty(volatile uint32_t *pui32Read, volatile uint32_t *pui32Write)
{
	return (*pui32Write == *pui32Read);
}

static uint32_t GetBufferCount(volatile uint32_t *pui32Read, volatile uint32_t *pui32Write, uint32_t ui32Size)
{
	return((*pui32Write >= *pui32Read) ? (*pui32Write - *pui32Read) : (ui32Size - (*pui32Read - *pui32Write)));
}

#ifdef UART_LOG_ENABLE
extern UART_HandleTypeDef huart1;
void uputs(char *msg, uint16_t len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, len, 300);
}

#else
void uputs(char *msg, uint16_t len)
{
	if(global.usb_connected)
	{
		uint8_t result = CDC_Transmit_FS((uint8_t *)msg, len);
		if(result == USBD_BUSY)
		{
			DelayMs(10);
			CDC_Transmit_FS((uint8_t *)msg, len);
		}
	}
}
#endif

int upeek(unsigned char ucChar)
{
	int iCount;
	int iAvail;
	uint32_t ui32ReadIndex;

	//
	// How many characters are there in the receive buffer?
	//
	iAvail = (int)RX_BUFFER_USED;
	ui32ReadIndex = g_ui32UARTRxReadIndex;

	//
	// Check all the unread characters looking for the one passed.
	//
	for(iCount = 0; iCount < iAvail; iCount++)
	{
		if(g_pcUARTRxBuffer[ui32ReadIndex] == ucChar)
		{
			//
			// We found it so return the index
			//
			return(iCount);
		}
		else
		{
			//
			// This one didn't match so move on to the next character.
			//
			ADVANCE_RX_BUFFER_INDEX(ui32ReadIndex);
		}
	}

	//
	// If we drop out of the loop, we didn't find the character in the receive
	// buffer.
	//
	return(-1);
}

int ugets(char *pcBuf, uint32_t ui32Len)
{
	uint32_t ui32Count = 0;
	int8_t cChar;

	//
	// Adjust the length back by 1 to leave space for the trailing
	// null terminator.
	//
	ui32Len--;

	//
	// Process characters until a newline is received.
	//
	while(1)
	{
		//
		// Read the next character from the receive buffer.
		//
		if(!RX_BUFFER_EMPTY)
		{
			cChar = g_pcUARTRxBuffer[g_ui32UARTRxReadIndex];
			ADVANCE_RX_BUFFER_INDEX(g_ui32UARTRxReadIndex);

			//
			// See if a newline or escape character was received.
			//
			if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
			{
				//
				// Stop processing the input and end the line.
				//
				break;
			}

			//
			// Process the received character as long as we are not at the end
			// of the buffer.  If the end of the buffer has been reached then
			// all additional characters are ignored until a newline is
			// received.
			//
			if(ui32Count < ui32Len)
			{
				//
				// Store the character in the caller supplied buffer.
				//
				pcBuf[ui32Count] = cChar;

				//
				// Increment the count of characters received.
				//
				ui32Count++;
			}
		}
	}

	//
	// Add a null termination to the string.
	//
	pcBuf[ui32Count] = 0;

	//
	// Return the count of int8_ts in the buffer, not counting the trailing 0.
	//
	return(ui32Count);
}

void uread(char *msg, uint16_t len)
{
	int8_t cChar;
	int32_t i32Char;
	static bool bLastWasCR = false;

	//
	// Get all the available characters from the UART.
	//
	for(uint32_t i = 0; i < len; i++)
	{
		//
		// Read a character
		//
		i32Char = msg[i];
		cChar = (unsigned char)(i32Char & 0xFF);

		//
		// If echo is disabled, we skip the various text filtering
		// operations that would typically be required when supporting a
		// command line.
		//
		if(!g_bDisableEcho)
		{
			//
			// Handle backspace by erasing the last character in the
			// buffer.
			//
			if(cChar == '\b')
			{
				//
				// If there are any characters already in the buffer, then
				// delete the last.
				//
				if(!RX_BUFFER_EMPTY)
				{
					//
					// Rub out the previous character on the users
					// terminal.
					//
					uputs("\b \b", 3);

					//
					// Decrement the number of characters in the buffer.
					//
					if(g_ui32UARTRxWriteIndex == 0)
					{
						g_ui32UARTRxWriteIndex = UART_RX_BUFFER_SIZE - 1;
					}
					else
					{
						g_ui32UARTRxWriteIndex--;
					}
				}

				//
				// Skip ahead to read the next character.
				//
				continue;
			}

			//
			// If this character is LF and last was CR, then just gobble up
			// the character since we already echoed the previous CR and we
			// don't want to store 2 characters in the buffer if we don't
			// need to.
			//
			if((cChar == '\n') && bLastWasCR)
			{
				bLastWasCR = false;
				continue;
			}

			//
			// See if a newline or escape character was received.
			//
			if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
			{
				//
				// If the character is a CR, then it may be followed by an
				// LF which should be paired with the CR.  So remember that
				// a CR was received.
				//
				if(cChar == '\r')
				{
					bLastWasCR = 1;
				}

				//
				// Regardless of the line termination character received,
				// put a CR in the receive buffer as a marker telling
				// UARTgets() where the line ends.  We also send an
				// additional LF to ensure that the local terminal echo
				// receives both CR and LF.
				//
				cChar = '\r';
				uputs("\n", 1);
			}
		}

		//
		// If there is space in the receive buffer, put the character
		// there, otherwise throw it away.
		//
		if(!RX_BUFFER_FULL)
		{
			//
			// Store the new character in the receive buffer
			//
			g_pcUARTRxBuffer[g_ui32UARTRxWriteIndex] = (unsigned char)(i32Char & 0xFF);
			ADVANCE_RX_BUFFER_INDEX(g_ui32UARTRxWriteIndex);

			//
			// If echo is enabled, write the character to the transmit
			// buffer so that the user gets some immediate feedback.
			//
			if(!g_bDisableEcho)
			{
				uputs((char *)&cChar, 1);
			}
		}
	}
}

uint32_t uprintf(char *fmt, ...)
{
	char buffer[256];
	uint16_t iw;

	va_list ap;
	va_start(ap, fmt);

	iw = vsprintf(buffer, fmt, ap);

	uputs(buffer, iw);

	va_end(ap);

	return iw;
}

static void uindent(int num)
{
  for (int i = 0 ; i < num ; i++)
  {
    uputs("  ", 2);
  }
}

void uoutput(uint8_t * buffer, int length, int indent)
{
	if (length == 0)
	{
		uputs("\n", 1);
	}

	int i = 0;
	while (i < length)
	{
		uint8_t array[16];
		int j;

		uindent(indent);
		memcpy(array, buffer+i, 16);
		for (j = 0 ; j < 16 && i+j < length; j++)
		{
			uprintf("%02X ", array[j]);
			if (j%4 == 3) uputs(" ", 1);
		}

		if (length > 16)
		{
			while (j < 16)
			{
				uputs("  ", 2);
				if (j%4 == 3) uputs(" ", 1);
				j++;
			}
		}
		uputs(" ", 1);

		for (j = 0 ; j < 16 && i+j < length; j++)
		{
			if (isprint(array[j]))
				uputs((char *)&array[j], 1);
			else
				uputs(".", 1);
		}
		uputs("\n", 1);
		i += 16;
	}
}

//*****************************************************************************
// @QUEUE
//*****************************************************************************
typedef struct
{
	uint8_t  front;
	uint8_t  rear;
	uint16_t len;
	uint8_t  data[UART_QUEUE_NUM][UART_QUEUE_BUFFER];
} queue_t;

static queue_t queue;

static void uart_queue_init(queue_t *theQueue)
{
	int i;

	theQueue->front = 0;
	theQueue->rear = 0;
	theQueue->len = 0;

	for(i=0; i<UART_QUEUE_NUM; i++)
	{
		memset(&theQueue->data[i], 0, UART_QUEUE_BUFFER);
	}
}

static uint8_t uart_queue_put(queue_t *theQueue, uint8_t *theItemValue, uint16_t *theSize)
{
	if((theQueue->rear + 1) % UART_QUEUE_NUM == theQueue->front)
	{
		return (0);
	}

	theQueue->len = *theSize;

	memset(&theQueue->data[theQueue->rear], 0, UART_QUEUE_BUFFER);
	memcpy(&theQueue->data[theQueue->rear], theItemValue, theQueue->len);

	theQueue->rear = (theQueue->rear + 1) % UART_QUEUE_NUM;

	return (1);
}

static uint8_t uart_queue_get(queue_t *theQueue, uint8_t *theItemValue, uint16_t *theSize)
{
	if(theQueue->front == theQueue->rear)
	{
		//DEBUG("The serial queue is empty\r\n");
		return (0);
	}

	*theSize = theQueue->len;

	memset(theItemValue, 0, *theSize);
	memcpy(theItemValue, &theQueue->data[theQueue->front], *theSize);

	theQueue->front = (theQueue->front +1) % UART_QUEUE_NUM;

	return (1);
}

void serial_init(void)
{
	uart_queue_init(&queue);
}

uint8_t serial_put(uint8_t *data, uint16_t *size)
{
	return uart_queue_put(&queue, data, size);
}

uint8_t serial_get(uint8_t *data, uint16_t *size)
{
	return uart_queue_get(&queue, data, size);
}
