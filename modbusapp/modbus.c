
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mb.h"
#include "mbport.h"

//*


#define TXDE	BIT(24)
#define RXDE	BIT(23)

#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 2000
#define REG_HOLDING_NREGS 130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];


xTaskHandle hdl_modbus;

/*
void init_serial3() {
	PCONP |= BIT(25);
	
	// PCLK UART3, PCLK = CCLK //
	PCLKSEL1 &= ~(BIT(18) | BIT(19));
	PCLKSEL1 |= BIT(18);
	
	// init TX3, RX3 //
	PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));
	PINSEL1 |= (BIT(18) | BIT(19));
	PINSEL1 |= (BIT(20) | BIT(21));
	PINSEL1 &= ~(BIT(16) | BIT(17));
	// TXDE di highkan //
	FIO0DIR |= TXDE;
	//FIO0SET = TXDE;		// on	---> bisa kirim
	//FIO0SET &= ~TXDE;		// off	---> gak bisa kirim
	//FIO0CLR = TXDE;
	FIO0SET = TXDE;
	
	FIO0DIR |= RXDE;
	FIO0SET  = RXDE;
	
	serial3_init( 115200, (1 * configMINIMAL_STACK_SIZE) );
}
//*/

static portTASK_FUNCTION(task_modbus, pvParameters )
{
	//init_serial3();
	vTaskDelay(500);
	if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )    {
       printf("tidak bisa init Modbus TCP !!!\r\n");
    }
    vTaskDelay(500);
	
	for (;;)
	{
		//togle_led_utama();
		vTaskDelay(500);
	}
}
void init_task_modbus(void) {
	xTaskCreate(task_modbus, ( signed portCHAR * ) "modbus",  (configMINIMAL_STACK_SIZE * 20) ,\
		 NULL, tskIDLE_PRIORITY + 2, ( xTaskHandle * ) &hdl_modbus );
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode ) {
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNRegs;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )	{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )	{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
